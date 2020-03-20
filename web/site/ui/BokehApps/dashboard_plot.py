from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem
from bokeh.models.widgets import Button, CheckboxGroup
from bokeh.palettes import Category20
from bokeh.layouts import column, row, WidgetBox
import pandas as pd
from bokeh.io import curdoc
import sqlite3
from datetime import datetime

TIME_BOXES = {'LAST_12_HOURS': 720,
              'LAST_24_HOURS': 720 * 2,
              'LAST_48_HOURS': 720 * 4
              }
conn = sqlite3.connect('../../db.sqlite3')
conn.row_factory = sqlite3.Row
c = conn.cursor()
data_labels = c.execute("pragma table_info('ui_dashboarddatarto')").fetchall()
data_labels = [label[1] for label in data_labels]
data_base = c.execute("select * from ui_dashboarddatarto order by id desc limit {}".format(TIME_BOXES['LAST_48_HOURS'])).fetchall()
data_base.reverse()

def make_dataset(time_box):
    # Prepare data
    
    data = data_base[-time_box:]
    
    # data = DashboardDataRTO.objects.all()[LAST_24_HOURS:]
    by_plot = pd.DataFrame(columns=['time','value', 'color', 'name'])
    # print([plot_name for plot_name in data_labels[2:]])

    return ColumnDataSource(data=dict(
        time = [[datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data],] * len(data_labels[2:]),
        value = [[entry[plot_name] for entry in data] for plot_name in data_labels[2:]],
        color = Category20[12][:len(data_labels[2:])],
        label = data_labels[2:]
    ))

    # for i, plot_name in enumerate(data_labels[2:]):
    #     dataset = pd.DataFrame()
    #     dataset['time'] = [entry['timestamp'] for entry in data]
    #     dataset['value'] = [entry[plot_name] for entry in data]
    #     dataset['color'] = Blues8[i]
    #     dataset['name'] = plot_name

    #     by_plot = by_plot.append(dataset)

    # return ColumnDataSource(by_plot)

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

    time = src.data['time'][0]
    plot = figure(
        tools="xpan", # this gives us our tools
        x_axis_type="datetime",
        sizing_mode = 'scale_both',
        plot_height=250,
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (MWe)",
        x_range=(time[0], time[-1]),
        )

    plot.extra_y_ranges = {"mwt": DataRange1d()}
    plot.add_layout(LinearAxis(y_range_name="mwt", axis_label="Power (MWt)"), 'right')
    legend = Legend(orientation='horizontal', location='top_center', spacing=10)
    # legend.orientation = 'horizontal'
    # legend.location = 'top_center'
    lines = {}
    for (x, y, label, color) in zip(src.data['time'], src.data['value'], src.data['label'], src.data['color']):
        legend_label = ' '.join(word.title() for word in label.split('_'))
        if 'field' in label:
            lines[label] = plot.line( 
                x=x,
                y=y,
                line_color = color, 
                line_alpha = 0.7, 
                hover_line_color = color,
                hover_alpha = 1.0,
                y_range_name='mwt',
                line_width=2,
                visible=False)
            legend_label = legend_label + " (MWt)"
            legend.items.append(LegendItem(label=legend_label , renderers=[lines[label]]))
            plot.extra_y_ranges['mwt'].renderers.append(lines[label])

        else:
            lines[label] = plot.line( 
                x=x,
                y=y,
                line_color = color, 
                line_alpha = 0.7, 
                hover_line_color = color,
                hover_alpha = 1.0,
                line_width=2,
                visible=False)
            legend_label = legend_label + " (MWe)"
            legend.items.append(LegendItem(label=legend_label, renderers=[lines[label]]))
            plot.y_range.renderers.append(lines[label])

    lines['actual'].visible = True
    plot.add_layout(legend, 'above')
    plot.legend.click_policy = 'hide'
    
    # styling
    plot = style(plot)

    return plot

# Update the plot based on selections
def update(attr, old, new):
    # 12, 24, 48 hour update

    # new_src = make_dataset(data_to_plot)
    # src.data.update(new_src.data)
    return

# Convert this to a button for time
# plot_selection.on_change('active', update)

src = make_dataset(TIME_BOXES['LAST_24_HOURS'])

plot = make_plot(src)

# Put Controls into single element
# controls = WidgetBox(plot_selection)

# Create row layout
layout = row(plot)

curdoc().add_root(layout)
curdoc().title = "Dashboard"