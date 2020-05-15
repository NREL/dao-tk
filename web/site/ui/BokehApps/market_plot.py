from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Band
from bokeh.models.widgets import Button, CheckboxGroup, RadioButtonGroup, Div, Select
from bokeh.palettes import Category20
from bokeh.layouts import column, row, WidgetBox, Spacer
from bokeh.themes import built_in_themes

import pandas as pd
from bokeh.io import curdoc
import sqlite3
import datetime
import numpy as np
from scipy.signal import savgol_filter

TIME_BOXES = {'NEXT_6_HOURS': 6,
              'NEXT_12_HOURS': 12,
              'NEXT_24_HOURS': 24,
              'NEXT_48_HOURS': 48
              }
conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels = c.execute("pragma table_info('ui_forecastsmarketdata')").fetchall()
data_labels = [label[1] for label in data_labels]


def get_string_date(date):
    # Return date in string without 0 padding on date month and day
    string_date = date.strftime('%m/%d/%Y %H:%M')
    return string_date

current_datetime = datetime.datetime.now().replace(year=2010) # Eventually the year will be removed
delta_end = datetime.timedelta(hours=TIME_BOXES['NEXT_48_HOURS'])

data_base = c.execute("select * from ui_forecastsmarketdata where timestamp >:start and timestamp <=:end",
    {'start':get_string_date(current_datetime), 'end': get_string_date(current_datetime + delta_end)}).fetchall()

def make_dataset(distribution):
    # Prepare data

    # Market Forecast
    value = [entry[data_labels[2]] for entry in data_base]
    value_ar = np.array(value)
    # Get error percentages
    lower_ar = np.array([entry[data_labels[3]]/100 for entry in data_base])
    upper_ar = np.array([entry[data_labels[4]]/100 for entry in data_base])
   
    cds = ColumnDataSource(data=dict(
            time = [datetime.datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data_base],
            value = value,
            lower = list(- np.multiply(value_ar, lower_ar) + value_ar),
            upper = list(np.multiply(value_ar, upper_ar) + value_ar)
        ))

    if distribution == "Smoothed":
        window, order = 51, 3
        for label in ['lower','upper']:
            cds.data[label] = savgol_filter(cds.data[label], window, order)

    return cds

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

    plot = figure(
        tools="", # this gives us our tools
        x_axis_type="datetime",
        sizing_mode = 'scale_both',
        width_policy='max',
        plot_height=250,
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Forecast ($)",
        x_range=(current_datetime, 
            current_datetime + datetime.timedelta(
                hours=TIME_BOXES['NEXT_24_HOURS'])),
        y_range=(-0.2, max(src.data['upper']) + 0.2),
        output_backend='webgl'
        )

    plot.line( 
        x='time',
        y='value',
        line_color = 'green', 
        line_alpha = 0.7, 
        hover_line_color = 'green',
        hover_alpha = 1.0,
        line_width=3,
        source=src,
        )

    band = Band(
        base='time',
        lower='lower',
        upper='upper',
        source=src,
        level = 'underlay',
        fill_alpha=1.0, 
        line_width=1, 
        line_color='black',
        )

    plot.add_layout(band)
    # styling
    plot = style(plot)

    return plot

def update(attr, old, new):
    active_time_window = window.options.index(window.value)
    time_box = list(TIME_BOXES.keys())[active_time_window]
    plot.x_range.end = current_datetime \
        + datetime.timedelta(hours=TIME_BOXES[time_box])
    new_src = make_dataset(distribution_select.value)
    src.data.update(new_src.data)
    


# Create widget layout

distribution = 'Discrete'
distribution_select = Select(
    value=distribution, 
    options=['Discrete', 'Smoothed'],
    width=150)
distribution_select.on_change('value', update)

time_window = "Next 24 Hours"
window = Select(
    options=["Next 6 Hours", "Next 12 Hours", "Next 24 Hours", "Next 48 Hours"], 
    value=time_window,
    width=150)
window.on_change('value', update)

src = make_dataset(distribution)

plot = make_plot(src)

widgets = row(
    window,
    distribution_select,
    width_policy='min')

title = Div(text="""<h3>Market</h3>""")

layout = column(title, widgets, plot, width_policy='max')

curdoc().add_root(layout)
curdoc().theme = 'dark_minimal'
curdoc().title = "Market Forecast Plot"