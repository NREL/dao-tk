from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Span
from bokeh.models.widgets import CheckboxButtonGroup, RadioButtonGroup, Div, DateSlider, Slider, Button, DatePicker
from bokeh.palettes import Category20
from bokeh.layouts import column, row, WidgetBox, Spacer
from bokeh.themes import built_in_themes

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

    data = c.execute("select * from ui_dashboarddatarto \
        where ((rowid % 30 = 0) or (rowid > (select max(rowid) from ui_dashboarddatarto) -30)) \
        and timestamp >:range_start and timestamp <=:range_end",
        {'range_start':get_string_date(range_start), 'range_end':get_string_date(range_end)}).fetchall()

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
        width_policy='max',
        height_policy='max',
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (MWe)",
        output_backend='webgl'
        )

    plot.extra_y_ranges = {"mwt": DataRange1d()}
    plot.add_layout(LinearAxis(y_range_name="mwt", axis_label="Power (MWt)"), 'right')
    legend = Legend(orientation='horizontal', location='top_center', spacing=10)
    
    # Add current time vertical line
    current_line = Span(
        location=current_datetime,
        dimension='height',
        line_color='white',
        line_dash='dashed',
        line_width=2
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
                line_width=3,
                visible=label in [title_to_col(plot_select.labels[i]) for i in plot_select.active],
                )

            legend_item = LegendItem(label=legend_label.replace('Operation', 'Op.') + " [MWt]", renderers=[lines[label]])
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
                visible=label in [title_to_col(plot_select.labels[i]) for i in plot_select.active],
                )

            legend_item = LegendItem(label=legend_label + " [MWe]", renderers=[lines[label]])
            legend.items.append(legend_item)
            plot.y_range.renderers.append(lines[label])
    
    # styling
    plot = style(plot)
    legend.label_text_font_size = '11px'

    plot.add_layout(legend, 'below')

    return plot

def col_to_title(label):
    # Convert column name to title

    legend_label = ' '.join([word.title() for word in label.split('_')])

    return legend_label

def title_to_col(title):
    # Convert title to a column name

    col_name = title.lower().replace(' ','_')
    return col_name

yMax = 0
def updateRange():
    global yMax
    # Update range when sliders move and update button is clicked
    delta = datetime.timedelta(hours=date_span_slider.value)
    selected_date = datetime.datetime.combine(date_slider.value_as_datetime, datetime.datetime.min.time())
    range_start = range_end = selected_date
    if( datetime.timedelta(0) > delta):
        range_start += delta
    else:
        range_end += delta
    [new_src, new_current_src] = make_dataset(range_start, range_end)
    src.data.update(new_src.data)
    current_src.data.update(new_current_src.data)

    for label in lines.keys():
        if lines[label].visible and yMax < max(lines[label].data_source.data[label]):
            yMax = max(lines[label].data_source.data[label])
            plot.y_range.end = yMax*1.33 if yMax*1.33 > 500  else 500


def update(attr, old, new):
    global yMax
    # Update plots when widgets change

    # Update visible plots
    for label in lines.keys():
        label_name = col_to_title(label)
        plot_select_labels = list(map(lambda label: label.replace('Op.', 'Operation'), plot_select.labels))
        lines[label].visible = label_name in [plot_select_labels[i] for i in plot_select.active]


# Create widget layout
# Create Checkbox Select Group Widget
labels_list = [col_to_title(label) for label in data_labels[2:]]
labels_list = list(map(lambda label: label.replace('Operation', 'Op.'), labels_list))
plot_select = CheckboxButtonGroup(
    labels = labels_list,
    active = [0],
    width_policy='min',
    css_classes=['bokeh_buttons'],
    background='#15191c'

)
plot_select.on_change('active', update)

# Create Date Slider
# Get start and end date in table
end_date = c.execute('select timestamp from ui_dashboarddatarto order by id desc limit 1').fetchall()
end_date = end_date[0]['timestamp']
start_date = c.execute('select timestamp from ui_dashboarddatarto order by id asc limit 1').fetchall()
start_date = start_date[0]['timestamp']
date_slider = DateSlider(title='Date', start=start_date, end=end_date, value=current_datetime, step=1, width=150)
# date_picker = DatePicker(title='Date', min_date=start_date, max_date=end_date, value=current_datetime.date(), width=150)

# Create Date Range Slider
date_span_slider = Slider(title='Time Span (Hours)', start=-240, end=240, value=24, step=4, width=150)


# Create Update Button
update_range_button = Button(label='Update', button_type='primary', width=100)
update_range_button.on_click(updateRange)

title = Div(text="""<h3>Dashboard Data</h3>""")

# Set initial plot information
initial_plots = [title_to_col(plot_select.labels[i]) for i in plot_select.active]

delta_init = datetime.timedelta(hours=24)
src, current_src = make_dataset(current_datetime.date(), current_datetime.date() + delta_init)

plot = make_plot(src, current_src)

# Setup Widget Layouts

widgets = column(
    row(
        Spacer(width_policy='max'),
        plot_select,
        Spacer(width_policy='max'),
        width_policy='max'          
    ),
    row(
        date_slider,
        Spacer(width_policy='max'),
        date_span_slider,
        Spacer(width_policy='max'),
        update_range_button,
        width_policy='max'), 
    width_policy='max'
    )

layout = column(title, widgets, plot, max_height=525, height_policy='max', width_policy='max')

curdoc().add_root(layout)
curdoc().theme = 'dark_minimal'
curdoc().title = "Historical Dashboard Plot"