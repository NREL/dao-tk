from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Span
from bokeh.models.widgets import Button, CheckboxButtonGroup, RadioButtonGroup, Div, DateSlider, Slider, Button
from bokeh.palettes import Category20
from bokeh.layouts import column, row, WidgetBox, Spacer
import pandas as pd
from bokeh.io import curdoc
import sqlite3
import datetime
import re

conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels = c.execute("pragma table_info('ui_dashboarddatarto')").fetchall()
data_labels = list(map(lambda x: x['name'], data_labels ))
current_datetime = datetime.datetime.now().replace(year=2010)

def get_string_date(date):
    # Return date in string without 0 padding on date month and day
    return date.strftime('%m/%d/%Y %H:%M')

label_colors = {}
for i, data_label in enumerate(data_labels[2:]):
    label_colors.update({
        data_label: Category20[12][i]
    })
lines = {}

def make_dataset(range_start, range_end):
    # Prepare data

    data = c.execute("select * from ui_dashboarddatarto where timestamp >:range_start and timestamp <=:range_end",
    {'range_start':get_string_date(range_start), 'range_end':get_string_date(range_end)}).fetchall()
    print(range_start)
    print(range_end)
    cds = ColumnDataSource(data={
            'time': [datetime.datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data]
        })

    current_cds = ColumnDataSource(data={
            'time': [datetime.datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data\
                if datetime.datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') <= current_datetime]
        })
    
    for i, plot_name in enumerate(data_labels[2:]):
        if re.match('(actual|field.*)', plot_name) is not None:
            current_cds.data.update({ 
                plot_name: [entry[plot_name] for entry in data if datetime.datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') <= current_datetime]
            })
        else:
            cds.data.update({ 
                plot_name: [entry[plot_name] for entry in data]
            })

    return cds, current_cds

# Styling for a plot
def style(p):
    # Title 
    p.title.align = 'center'
    p.title.text_font_size = '20pt'
    p.title.text_font = 'serif'

    # Axis titles
    p.xaxis.axis_label_text_font_size = '14pt'
    p.xaxis.axis_label_text_font_style = 'bold'
    p.yaxis.axis_label_text_font_size = '14pt'
    p.yaxis.axis_label_text_font_style = 'bold'

    # Tick labels
    p.xaxis.major_label_text_font_size = '12pt'
    p.yaxis.major_label_text_font_size = '12pt'

    return p

def make_plot(src, current_src): # Takes in a ColumnDataSource
    # Create the plot

    time = src.data['time']
    plot = figure(
        tools="", # this gives us our tools
        x_axis_type="datetime",
        sizing_mode = 'scale_both',
        width_policy='max',
        plot_height=250,
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (MWe)"
        )

    plot.extra_y_ranges = {"mwt": DataRange1d()}
    plot.add_layout(LinearAxis(y_range_name="mwt", axis_label="Power (MWt)"), 'right')
    legend = Legend(orientation='horizontal', location='top_center', spacing=10)
    
    # Add current time vertical line
    current_line = Span(
        location=current_datetime,
        dimension='height',
        line_color='black',
        line_dash='dashed',
        line_width=1
    )
    plot.add_layout(current_line)

    for label in data_labels[2:]:
        legend_label = col_to_title(label)
        if 'field' in label:
            lines[label] = plot.line( 
                x='time',
                y=label,
                line_color = label_colors[label], 
                line_alpha = 0.7, 
                hover_line_color = label_colors[label],
                hover_alpha = 1.0,
                y_range_name='mwt',
                level='underlay',
                source = current_src,
                line_width=2,
                visible=label in [title_to_col(plot_select.labels[i]) for i in plot_select.active])

            legend_item = LegendItem(label=legend_label, renderers=[lines[label]])
            legend.items.append(legend_item)
            plot.extra_y_ranges['mwt'].renderers.append(lines[label])

        else:
            lines[label] = plot.line( 
                x='time',
                y=label,
                line_color = label_colors[label], 
                line_alpha = 0.7, 
                hover_line_color = label_colors[label],
                hover_alpha = 1.0,
                source= current_src if label == 'actual' else src,
                level='glyph' if label == 'actual' else 'underlay',
                line_width=3 if label == 'actual' else 2,
                visible=label in [title_to_col(plot_select.labels[i]) for i in plot_select.active])

            legend_item = LegendItem(label=legend_label, renderers=[lines[label]])
            legend.items.append(legend_item)
            plot.y_range.renderers.append(lines[label])
    
    # styling
    plot = style(plot)

    plot.add_layout(legend, 'above')

    return plot

def col_to_title(label):
    # Convert column name to title

    legend_label = ' '.join([word.title() for word in label.split('_')])

    return legend_label

def title_to_col(title):
    # Convert title to a column name

    col_name = title.lower().replace(' ','_')
    return col_name

def updateRange():
    # Update range when sliders move and update button is clicked
    delta = datetime.timedelta(hours=date_span_slider.value)
    selected_date = datetime.datetime.combine(date_slider.value, datetime.datetime.min.time())
    range_start = selected_date - delta
    range_end = selected_date + delta
    [new_src, new_current_src] = make_dataset(range_start, range_end)
    src.data.update(new_src.data)
    current_src.data.update(new_current_src.data)

def update(attr, old, new):
    # Update plots when widgets change

    # Update visible plots
    for label in lines.keys():
        label_name = col_to_title(label)
        lines[label].visible = label_name in [plot_select.labels[i] for i in plot_select.active]

# Create widget layout
# Create Checkbox Select Group Widget
labels_list = [col_to_title(label) for label in data_labels[2:]]
plot_select = CheckboxButtonGroup(
    labels = labels_list,
    active = [0],
    width_policy='min'
)
plot_select.on_change('active', update)

# Create Date Slider
# Get start and end date in table
end_date = c.execute('select timestamp from ui_dashboarddatarto order by id desc limit 1').fetchall()
end_date = end_date[0]['timestamp']
start_date = c.execute('select timestamp from ui_dashboarddatarto order by id asc limit 1').fetchall()
start_date = start_date[0]['timestamp']
date_slider = DateSlider(title='Date', start=start_date, end=end_date, value=current_datetime, step=1, width=250)

# Create Date Range Slider
date_span_slider = Slider(title='Time Span (Hours)', start=4, end=120, value=24, step=4, width=150)

# Create Update Button
update_range_button = Button(label='Update', button_type='primary', width=100)
update_range_button.on_click(updateRange)

title = Div(text="""<h2>Historical Dashboard Data</h2>""")

# Set initial plot information
initial_plots = [title_to_col(plot_select.labels[i]) for i in plot_select.active]

delta_init = datetime.timedelta(days=1)
src, current_src = make_dataset(current_datetime - delta_init, current_datetime + delta_init)

plot = make_plot(src, current_src)

# Setup Widget Layouts

# Dates
date_sliders = row(date_slider, date_span_slider)
date_widgets = column(date_sliders, update_range_button)
widgets = row(
    date_widgets,
    Spacer(width_policy='max'),
    column(
        Spacer(width_policy='max'), 
        plot_select),
    width_policy='max'
)


layout = column(title, widgets, plot, width_policy='max')

curdoc().add_root(layout)
curdoc().title = "Historical Dashboard Plot"