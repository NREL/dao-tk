from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Band
from bokeh.models.widgets import RadioButtonGroup, CheckboxGroup
from bokeh.palettes import Spectral9
from bokeh.layouts import column, row, WidgetBox
import pandas as pd
from bokeh.io import curdoc
import sqlite3
from datetime import datetime
import numpy as np
import re
import operator

TIME_BOXES = {'NEXT_12_HOURS': 720,
              'NEXT_24_HOURS': 720 * 2,
              'NEXT_48_HOURS': 720 * 4
              }
conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels = c.execute("pragma table_info('ui_forecastssolardata')").fetchall()
data_labels = [label[1] for label in data_labels]
data_base = c.execute("select * from ui_forecastssolardata order by id desc limit {}".format(TIME_BOXES['NEXT_48_HOURS'])).fetchall()
data_base.reverse()
label_colors = {}
lines = {}
bands = {}

def make_dataset(time_box):
    # Prepare data
    
    data = data_base[:time_box]

    cds = ColumnDataSource(data={
        'time': [datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data]
    })

    for i,col_name in enumerate([label for label in data_labels[2:] if re.search('_(minus|plus)', label) is None]):
        cds.data.update({
            col_name: [entry[col_name] for entry in data]
        })
        
        label_colors.update({
            col_name+'_color': Spectral9[i]
        })

        r = re.compile(col_name+'_(minus|plus)')

        if len(list(filter(r.search, data_labels))) == 2:

            value_arr = np.array(cds.data[col_name])
            value_minus_arr = np.array(
                [entry[col_name+'_minus']/100 for entry in data]) # Divide by 100 for percentage (%)
            value_plus_arr = np.array(
                [entry[col_name+'_plus']/100 for entry in data]) # Divide by 100 for percentage (%)

            cds.data[col_name+'_lower'] = list(\
                value_arr - np.multiply(value_arr, value_minus_arr))
            cds.data[col_name+'_upper'] = list(\
                value_arr + np.multiply(value_arr, value_plus_arr))
    
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
    time = src.data['time']
    plot = figure(
        tools="xpan", # this gives us our tools
        x_axis_type="datetime",
        plot_height=250,
        sizing_mode='scale_both',
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (W/m^2)",
        x_range=(time[0], time[-1]),
        title="Solar Forecast"
        )

    for label in [label for label in src.column_names[1:]]:

        legend_label = col_to_title(label)
        
        if not re.search('(_lower|_upper)', label) is None:
            value_name = re.split('(_lower|_upper)', label)[0]
            bands[label] = Band(
                base='time',
                lower= value_name + '_lower',
                upper= value_name + '_upper',
                source=src,
                level = 'underlay',
                fill_alpha=1.0, 
                line_width=1, 
                line_color='black',
                visible = label in [title_to_col(plot_select.labels[i]) for i in plot_select.active],
                name = label)
            plot.add_layout(bands[label])
        else:
            color = label_colors[label+'_color']
            lines[label] = plot.line( 
                x='time',
                y=label,
                line_color = color, 
                line_alpha = 0.7, 
                hover_line_color = color,
                hover_alpha = 1.0,
                line_width=2,
                legend_label = legend_label,
                source=src,
                visible = label in [title_to_col(plot_select.labels[i]) for i in plot_select.active],
                name = label)

    # styling
    plot = style(plot)

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
    time_box = list(TIME_BOXES.values())[radio_button_group.active]
    new_src = make_dataset(time_box)
    src.data.update(new_src.data)
    # Update ranges
    plot.x_range.start = min(src.data['time'])
    plot.x_range.end = max(src.data['time'])
    # Update visible plots
    for label in lines.keys():
        label_name = col_to_title(label)
        lines[label].visible = label_name in [plot_select.labels[i] for i in plot_select.active]
        if label in bands.keys():
            bands[label].visible = lines[label].visible


# Create widgets
# Create Radio Button Group Widget
radio_button_group = RadioButtonGroup(
    labels=["Next 12 Hours", "Next 24 Hours", "Next 48 Hours"], 
    active=1,
    sizing_mode = 'fixed',
    width = 300,
    height = 30)
radio_button_group.on_change('active', update)

# Create Checkbox Select Group Widget
plot_select = CheckboxGroup(
    labels = [col_to_title(label) for label in data_labels[2:] if re.search('_(minus|plus)', label) is None],
    active = [0],
    sizing_mode = 'scale_both'
)

plot_select.on_change('active', update)

# Set initial plot information
initial_plots = [title_to_col(plot_select.labels[i]) for i in plot_select.active]

src = make_dataset(TIME_BOXES['NEXT_24_HOURS'])

plot = make_plot(src)

# Setup Widget Layouts
widgets = column(
    radio_button_group, 
    plot_select,
    sizing_mode = 'fixed',
    width = 310,
    height = 300)

layout = row(widgets, plot, sizing_mode='scale_width')

# Show to current document/page
curdoc().add_root(layout)
curdoc().title = "Solar Plot"