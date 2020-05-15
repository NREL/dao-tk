from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem, Band, Range1d
from bokeh.models.widgets import CheckboxButtonGroup, RadioButtonGroup, Div, DateSlider, Slider, Button, Select, DatePicker
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

conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels_forecast_solar = c.execute("pragma table_info('ui_forecastssolardata')").fetchall()
data_labels_forecast_solar = list(map(lambda x: x['name'], data_labels_forecast_solar ))
current_datetime = datetime.datetime.now().replace(year=2010)

def get_string_date(date):
    # Return date in string without 0 padding on date month and day
    return date.strftime('%m/%d/%Y %H:%M')

label_colors = {}
lines = {}
bands = {}

yMax = 0

def make_dataset(range_start, range_end, distribution):
    # Prepare data
    

    data = c.execute("select * from ui_forecastssolardata where rowid % 30 = 0 and timestamp >:range_start and timestamp <=:range_end",
    {'range_start':get_string_date(range_start), 'range_end':get_string_date(range_end)}).fetchall()
 
    cds = ColumnDataSource(data={
        'time': [datetime.datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data]
    })

    for i,col_name in enumerate([label for label in data_labels_forecast_solar[2:] if re.search('_(minus|plus)', label) is None]):
        cds.data.update({
            col_name: [entry[col_name] for entry in data]
        })
        
        label_colors.update({
            col_name+'_color': i*2
        })

        r = re.compile(col_name+'_(minus|plus)')

        if len(list(filter(r.search, data_labels_forecast_solar))) == 2:

            value_arr = np.array(cds.data[col_name])
            value_minus_arr = np.array(
                [entry[col_name+'_minus']/100 for entry in data]) # Divide by 100 for percentage (%)
            value_plus_arr = np.array(
                [entry[col_name+'_plus']/100 for entry in data]) # Divide by 100 for percentage (%)

            cds.data[col_name+'_lower'] = list(\
                value_arr - np.multiply(value_arr, value_minus_arr))
            cds.data[col_name+'_upper'] = list(\
                value_arr + np.multiply(value_arr, value_plus_arr))
    y_max = int(max(reduce(lambda entry_a, entry_b: entry_a + entry_b, list(cds.data.values())[1:])))
    if distribution == "Smoothed":
        window, order = 5, 3
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
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (W/m^2)",
        width_policy='max',
        height_policy='max',
        y_range=(0,y_max),
        output_backend='webgl'
        )
    legend = Legend(orientation='horizontal', location='top_center', spacing=10)
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

    plot.add_layout(legend, 'below')

    return plot

def col_to_title_upper(label):
    # Convert column name to title

    legend_label = ' '.join([word.upper() for word in label.split('_')])

    return legend_label

def title_to_col(title):
    # Convert title to a column name

    col_name = title.lower().replace(' ','_')
    return col_name

def update():
    # Update range when sliders move and update button is clicked
    delta = datetime.timedelta(hours=date_span_slider.value)
    selected_date = datetime.datetime.combine(date_slider.value_as_datetime, datetime.datetime.min.time())
    range_start = range_end = selected_date
    if( datetime.timedelta(0) > delta):
        range_start += delta
    else:
        range_end += delta

    new_src = make_dataset(range_start, range_end, distribution_select.value)
    src.data.update(new_src.data)


def update_plots(attr, old, new):
    # Update plots when widgets change
 
    selected_labels = [plot_select.labels[i] for i in plot_select.active]

    yMax = 0

    # Update visible plots
    for label in lines.keys():
        label_name = col_to_title_upper(label)
        lines[label].visible = label_name in selected_labels
        if lines[label].visible and yMax < max(lines[label].data_source.data[label]):
            yMax = max(lines[label].data_source.data[label])



# Create widgets
# Create Checkbox Select Group Widget
labels_list = [col_to_title_upper(label) for label in data_labels_forecast_solar[2:] if re.search('_(minus|plus)', label) is None]
plot_select = CheckboxButtonGroup(
    labels = labels_list,
    active = [0],
    width_policy='min',
    css_classes=['bokeh_buttons']
)
plot_select.on_change('active', update_plots)

# Create Date Slider
# Get start and end date in table
end_date = c.execute('select timestamp from ui_forecastssolardata order by id desc limit 1').fetchall()
end_date = end_date[0]['timestamp']
start_date = c.execute('select timestamp from ui_forecastssolardata order by id asc limit 1').fetchall()
start_date = start_date[0]['timestamp']
date_slider = DateSlider(title='Date', start=start_date, end=end_date, value=current_datetime, step=1, width=150)
# date_picker = DatePicker(title='Date', min_date=start_date, max_date=end_date, value=current_datetime.date(), width=150)

# Create Date Range Slider
date_span_slider = Slider(title='Time Span (Hours)', start=-240, end=240, value=24, step=4, width=150)

# Create Update Button
update_range_button = Button(label='Update', button_type='primary', width=100)
update_range_button.on_click(update)

distribution_init = 'Discrete'
distribution_select = Select(
    value=distribution_init,
    options=['Discrete', 'Smoothed'],
    width=125)

title = Div(text="""<h3>Solar</h3>""")

# Set initial plot information
initial_plots = [title_to_col(plot_select.labels[i]) for i in plot_select.active]

delta_init = datetime.timedelta(hours=24)
src = make_dataset(current_datetime.date(), current_datetime.date() + delta_init, distribution_init)

plot = make_plot(src)

# Setup Widget Layouts
widgets = row(
    column(
        date_slider,
        date_span_slider),
    Spacer(width_policy='max'),
    column(
        Spacer(height_policy='max'),
        plot_select,
        row(
            Spacer(width_policy='max'),
            distribution_select,
            update_range_button
        )
    ),
    width_policy='max'
)

layout = column(title, widgets, plot, max_height=525, height_policy='max', width_policy='max')

# Show to current document/page
curdoc().add_root(layout)
curdoc().theme = 'dark_minimal'
curdoc().title = "Historical Solar Forecast Plot"