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

def make_dataset(time_box):
    # Prepare data
    
    data = data_base[:time_box]

    cds = ColumnDataSource(data={
        'time': [datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data]
    })

    j = 0
    for col_name in data_labels[2:]:
        cds.data.update({
            col_name: [entry[col_name] for entry in data]
        })
        
        label_colors.update({
            col_name+'_color': [Spectral9[j]]
        })

        if '_plus' in col_name or '_minus' in col_name:
            value_name = re.split('(_minus|_plus)', col_name)

            value_arr = np.array(cds.data[value_name[0]])
            temp_arr = np.array(cds.data[col_name])

            postfix = '_lower' if value_name[1] == "_minus" else "_upper"
            add_or_sub = operator.sub if value_name[1] == "_minus" else operator.add
            cds.data[value_name[0] + postfix] = list(\
                add_or_sub(value_arr, np.multiply(value_arr, temp_arr/100))) # Divide by 100 for percentage (%)
            cds.data.pop(col_name)
        else:
            j += 1

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
        y_axis_label = "Forecast ($)",
        x_range=(time[0], time[-1]),
        title="Solar Forecast"
        )
    used_labels = set()
    for label in (label for label in data_labels[2:] if label not in used_labels):

        legend_label = col_to_title(label)
        
        if not re.search('(_minus|_plus)', label) is None:
            value_name = re.split('(_minus|_plus)', label)[0]
            band = Band(
                base='time',
                lower= value_name + '_lower',
                upper= value_name + '_upper',
                source=src,
                level = 'underlay',
                fill_alpha=1.0, 
                line_width=1, 
                line_color='black',
                name = label)
            used_labels.add(value_name + '_lower')
            used_labels.add(value_name + '_upper')
            plot.add_layout(band)
        else:
            color = label_colors[label+'_color'][0]
            plot.line( 
                x='time',
                y=label,
                line_color = color, 
                line_alpha = 0.7, 
                hover_line_color = color,
                hover_alpha = 1.0,
                line_width=2,
                legend_label = legend_label,
                source=src,
                name = label)
            used_labels.add(label)

    # styling
    plot = style(plot)
    plot.legend.click_policy = 'hide'

    return plot

# Convert the column name to a title (Only for this SQLite table)
def col_to_title(label):
    if '_' in label:
            legend_label = ' '.join([word.title() for word in label.split('_')])
    else:
        legend_label = label.upper()
    return legend_label

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
# Create Radio Button Group
radio_button_group = RadioButtonGroup(
    labels=["Next 12 Hours", "Next 24 Hours", "Next 48 Hours"], 
    active=1,
    sizing_mode = 'fixed',
    width = 300,
    height = 30)
radio_button_group.on_change('active', updateTime)

# Create Checkbox Select Group
plot_select = CheckboxGroup(
    labels = [col_to_title(label) for label in data_labels[2:] if re.search('_minus|_plus', label) is None],
    active = [0],
    sizing_mode = 'scale_both'
)

widgets = column(
    radio_button_group, 
    plot_select,
    sizing_mode = 'fixed',
    width = 310,
    height = 300)

layout = row(widgets, plot, sizing_mode='scale_width')

curdoc().add_root(layout)
curdoc().title = "Solar Plot"