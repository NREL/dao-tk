from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Span
from bokeh.models.widgets import Button, CheckboxButtonGroup, RadioButtonGroup
from bokeh.palettes import Category20
from bokeh.layouts import column, row, WidgetBox, Spacer
import pandas as pd
from bokeh.io import curdoc
import sqlite3
import datetime
import re

TIME_BOXES = {'TODAY': 360 * 4,
              'LAST_6_HOURS': 360,
              'LAST_12_HOURS': 360 * 2,
              'LAST_24_HOURS': 360 * 4,
              'LAST_48_HOURS': 360 * 8
              }
conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels = c.execute("pragma table_info('ui_dashboarddatarto')").fetchall()
data_labels = [label[1] for label in data_labels]

current_time = datetime.datetime.now().time().replace(second=0, microsecond=0)
current_time_data_rows = current_time.hour * 60 + current_time.minute
print(current_time_data_rows)
extra_time_data_rows = TIME_BOXES['LAST_24_HOURS'] - current_time_data_rows

data_base = c.execute("select * from ui_dashboarddatarto order by id desc limit {}"\
    .format(TIME_BOXES['LAST_48_HOURS'] + extra_time_data_rows)).fetchall()
data_base.reverse()
label_colors = {}
lines = {}
for i, data_label in enumerate(data_labels[2:]):
    label_colors.update({
        data_label: Category20[12][i]
    })

# Current Datetime information
current_date = max([datetime.datetime.strptime(
    entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data_base]).date()
current_datetime = datetime.datetime.combine(current_date, current_time)

def make_dataset(time_box):
    # Prepare data
    if time_box != 'TODAY':
        data = data_base[-(TIME_BOXES[time_box] + extra_time_data_rows):-extra_time_data_rows]
    else:
        # Minutes remaining in the day
        data = data_base[-TIME_BOXES['TODAY']:]

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
        tools="xpan", # this gives us our tools
        x_axis_type="datetime",
        sizing_mode = 'scale_both',
        width_policy='max',
        plot_height=250,
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (MWe)",
        x_range=(time[0], time[-1]),
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
                line_width=2,
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

    legend_label = ' '.join([word.upper() for word in label.split('_')])

    return legend_label

def title_to_col(title):
    # Convert title to a column name

    col_name = title.lower().replace(' ','_')
    return col_name

def update(attr, old, new):
    # Update plots when widgets change

    # Get updated time block information
    time_box = list(TIME_BOXES.keys())[radio_button_group.active]
    new_src, new_current_src = make_dataset(time_box)
    src.data.update(new_src.data)
    current_src.data.update(new_current_src.data)
    # Update ranges
    plot.x_range.start = min(src.data['time'])
    plot.x_range.end = max(src.data['time'])
    # Update visible plots
    for label in lines.keys():
        label_name = col_to_title(label)
        lines[label].visible = label_name in [plot_select.labels[i] for i in plot_select.active]

# Create widget layout
radio_button_group = RadioButtonGroup(
    labels=["Today", "Last 6 Hours", "Last 12 Hours", "Last 24 Hours", "Last 48 Hours"], 
    active=0,
    width_policy='min')
radio_button_group.on_change('active', update)

# Create Checkbox Select Group Widget
labels_list = [col_to_title(label) for label in data_labels[2:]]
plot_select = CheckboxButtonGroup(
    labels = labels_list,
    active = [0],
    width_policy='min'
)

plot_select.on_change('active', update)

# Set initial plot information
initial_plots = [title_to_col(plot_select.labels[i]) for i in plot_select.active]

[src, current_src] = make_dataset('TODAY')
plot = make_plot(src, current_src)

widgets = row(
    radio_button_group,
    Spacer(width_policy='max'),
    plot_select)
    # width_policy='max')

layout = column(widgets, plot, width_policy='max')

curdoc().add_root(layout)
curdoc().title = "Dashboard"