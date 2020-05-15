from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Band
from bokeh.models.widgets import RadioButtonGroup, CheckboxButtonGroup, Div, Select
from bokeh.palettes import Category20
from bokeh.layouts import column, row, WidgetBox, Spacer
from bokeh.themes import built_in_themes

import pandas as pd
from bokeh.io import curdoc
import sqlite3
import datetime
import numpy as np
import re
import operator
from scipy.signal import savgol_filter
from functools import reduce

TIME_BOXES = {'NEXT_6_HOURS': 6,
              'NEXT_12_HOURS': 12,
              'NEXT_24_HOURS': 24,
              'NEXT_48_HOURS': 48
              }
conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels = c.execute("pragma table_info('ui_forecastssolardata')").fetchall()
data_labels = [label[1] for label in data_labels]

def get_string_date(date):

    string_date = date.strftime('%m/%d/%Y %H:%M')
    return string_date

current_datetime = datetime.datetime.now().replace(year=2010) # Eventually the year will be removed
delta_end = datetime.timedelta(hours=TIME_BOXES['NEXT_48_HOURS'])

data_base = c.execute("select * from ui_forecastssolardata where rowid % 30 = 0 and timestamp >:start and timestamp <=:end",
    {'start':get_string_date(current_datetime), 'end': get_string_date(current_datetime + delta_end)}).fetchall()
label_colors = {}
lines = {}
bands = {}

def make_dataset(distribution):
    # Prepare data
 
    cds = ColumnDataSource(data={
        'time': [datetime.datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data_base]
    })

    for i,col_name in enumerate([label for label in data_labels[2:] if re.search('_(minus|plus)', label) is None]):
        cds.data.update({
            col_name: [entry[col_name] for entry in data_base]
        })
        
        label_colors.update({
            col_name+'_color': i*2
        })

        r = re.compile(col_name+'_(minus|plus)')

        if len(list(filter(r.search, data_labels))) == 2:

            value_arr = np.array(cds.data[col_name])
            value_minus_arr = np.array(
                [entry[col_name+'_minus']/100 for entry in data_base]) # Divide by 100 for percentage (%)
            value_plus_arr = np.array(
                [entry[col_name+'_plus']/100 for entry in data_base]) # Divide by 100 for percentage (%)

            cds.data[col_name+'_lower'] = list(\
                value_arr - np.multiply(value_arr, value_minus_arr))
            cds.data[col_name+'_upper'] = list(\
                value_arr + np.multiply(value_arr, value_plus_arr))

    if distribution == "Smoothed":
        window, order = 7, 3
        for label in filter(lambda x: x != 'clear_sky', cds.column_names[1:]):
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
    time = src.data['time']
    y_max = int(max(reduce(lambda entry_a, entry_b: entry_a + entry_b, list(src.data.values())[1:])))
    plot = figure(
        tools="", # this gives us our tools
        x_axis_type="datetime",
        plot_height=250,
        sizing_mode='scale_both',
        width_policy='max',
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (W/m^2)",
        x_range=(current_datetime, 
            current_datetime + datetime.timedelta(
                hours=TIME_BOXES['NEXT_24_HOURS'])),
        y_range=(0, y_max + 150),
        output_backend='webgl'
        )
    legend = Legend(orientation='vertical', location='center_left', spacing=10)
    for label in [label for label in src.column_names[1:]]:

        legend_label = col_to_title_upper(label)
        
        if not re.search('(_lower|_upper)', label) is None:
            value_name = re.split('(_lower|_upper)', label)[0]
            bands[value_name] = Band(
                base='time',
                lower= value_name + '_lower',
                upper= value_name + '_upper',
                source=src,
                level = 'underlay',
                fill_alpha=1.0,
                fill_color=Category20[20][label_colors[value_name+'_color']+1],
                line_width=1, 
                line_color='black',
                visible = label in [title_to_col(plot_select.labels[i]) for i in plot_select.active],
                name = label,
                )
            plot.add_layout(bands[value_name])
        else:
            color = Category20[20][label_colors[label+'_color']]
            lines[label] = plot.line( 
                x='time',
                y=label,
                line_color = color, 
                line_alpha = 1.0,
                line_width=3,
                source=src,
                visible = label in [title_to_col(plot_select.labels[i]) for i in plot_select.active],
                name = label,
                
                )
            legend_item = LegendItem(label=legend_label, renderers=[lines[label]])
            legend.items.append(legend_item)

    # styling
    plot = style(plot)

    plot.add_layout(legend, 'left')

    return plot

def col_to_title_upper(label):
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
    active_time_window = window.options.index(window.value)
    time_box = list(TIME_BOXES.keys())[active_time_window]
    # Update ranges
    plot.x_range.start = current_datetime
    plot.x_range.end = current_datetime + datetime.timedelta(hours=TIME_BOXES[time_box])
    new_src = make_dataset(distribution_select.value)
    src.data.update(new_src.data)

    yMax = 0
    # Update visible plots
    selected_labels = [plot_select.labels[i] for i in plot_select.active]
    for label in lines.keys():
        label_name = col_to_title_upper(label)
        lines[label].visible = label_name in selected_labels
        if label in bands.keys():
            bands[label].visible = lines[label].visible

# Create widgets
# Create Radio Button Group Widget
time_window_init = "Next 24 Hours"
window = Select(
    options=["Next 6 Hours", "Next 12 Hours", "Next 24 Hours", "Next 48 Hours"], 
    value=time_window_init,
    width=135)
window.on_change('value', update)

# Create Checkbox Select Group Widget
labels_list = [col_to_title_upper(label) for label in data_labels[2:] if re.search('_(minus|plus)', label) is None]
plot_select = CheckboxButtonGroup(
    labels = labels_list,
    active = [0],
    width_policy='min'
)

plot_select.on_change('active', update)

distribution = 'Discrete'
distribution_select = Select(
    value=distribution,
    options=['Discrete', 'Smoothed'],
    width=150
    )
distribution_select.on_change('value', update)


title = Div(text="""<h3>Solar</h3>""")

# Set initial plot information
initial_plots = [title_to_col(plot_select.labels[i]) for i in plot_select.active]

src = make_dataset(distribution)

plot = make_plot(src)

# Setup Widget Layouts

widgets = column(
    row(
        title,
        Spacer(width_policy='max'),
        window,
        distribution_select),
    row(
        Spacer(width_policy='max'),
        plot_select),
    width_policy='max'
    
)

layout = column(row(widgets), plot, width_policy='max')

# Show to current document/page
curdoc().add_root(layout)
curdoc().theme = 'dark_minimal'
curdoc().title = "Solar Forecast Plot"