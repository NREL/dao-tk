from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Band
from bokeh.models.widgets import Button, CheckboxGroup, RadioButtonGroup
from bokeh.palettes import Category20
from bokeh.layouts import column, row, WidgetBox
import pandas as pd
from bokeh.io import curdoc
import sqlite3
from datetime import datetime
import numpy as np

TIME_BOXES = {'NEXT_12_HOURS': 720,
              'NEXT_24_HOURS': 720 * 2,
              'NEXT_48_HOURS': 720 * 4
              }
conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels = c.execute("pragma table_info('ui_forecastsmarketdata')").fetchall()
data_labels = [label[1] for label in data_labels]
data_base = c.execute("select * from ui_forecastsmarketdata order by id desc limit {}".format(TIME_BOXES['NEXT_48_HOURS'])).fetchall()
data_base.reverse()

def make_dataset(time_box):
    # Prepare data
    
    data = data_base[:time_box]
    value = [entry[data_labels[2]] for entry in data]
    value_ar = np.array(value)
    lower_ar = np.array([entry[data_labels[3]]/100 for entry in data])
    upper_ar = np.array([entry[data_labels[4]]/100 for entry in data])
    source = ColumnDataSource(data=dict(
            time = [datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data],
            value = value,
            lower = list(- np.multiply(value_ar, lower_ar) + value_ar),
            upper = list(np.multiply(value_ar, upper_ar) + value_ar)
        ))

    return source

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

def make_plot(src): # Takes in a ColumnDataSource
    # Create the plot

    time = src.data['time']
    plot = figure(
        tools="xpan", # this gives us our tools
        x_axis_type="datetime",
        sizing_mode = 'scale_both',
        plot_height=250,
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Forecast ($)",
        x_range=(time[0], time[-1]),
        y_range=(-0.2, max(src.data['upper']) + 0.2),
        title="Market Forecast"
        )

    plot.line( 
        x='time',
        y='value',
        line_color = 'green', 
        line_alpha = 0.7, 
        hover_line_color = 'green',
        hover_alpha = 1.0,
        line_width=2,
        source=src)

    band = Band(
        base='time',
        lower='lower',
        upper='upper',
        source=src,
        level = 'underlay',
        fill_alpha=1.0, 
        line_width=1, 
        line_color='black')

    plot.add_layout(band)
    # styling
    plot = style(plot)

    return plot

def updateTime(attr, old, new):
    new_src = make_dataset(list(TIME_BOXES.values())[new])
    src.data.update(new_src.data)
    plot.x_range.start = min(src.data['time'])
    plot.x_range.end = max(src.data['time'])

# Convert this to a button for time
# plot_selection.on_change('active', update)

src = make_dataset(TIME_BOXES['NEXT_24_HOURS'])

plot = make_plot(src)

# Create widget layout
radio_button_group = RadioButtonGroup(
    labels=["Next 12 Hours", "Next 24 Hours", "Next 48 Hours"], active=1)
radio_button_group.on_change('active', updateTime)
widgets = row(radio_button_group)

layout = column(widgets, plot, sizing_mode='stretch_both')

curdoc().add_root(layout)
curdoc().title = "Dashboard"