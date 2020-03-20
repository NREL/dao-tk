from bokeh.plotting import figure
from bokeh.models import ColumnDataSource, LinearAxis, DataRange1d, Legend, LegendItem
from bokeh.models.widgets import Button, CheckboxGroup, RadioButtonGroup
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
label_colors = {}
for i, data_label in enumerate(data_labels[2:]):
    label_colors.update({
        data_label: Category20[12][i]
    })

def make_dataset(time_box):
    # Prepare data
    print(make_dataset.__name__)
    data = data_base[-time_box:]
    
    # data = DashboardDataRTO.objects.all()[LAST_24_HOURS:]
    by_plot = pd.DataFrame(columns=['time','value', 'color', 'name'])
    # print([plot_name for plot_name in data_labels[2:]])

    # return ColumnDataSource(data={
    #         time = [datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data],
    #         value = [[entry[plot_name] for entry in data] for plot_name in data_labels[2:]],
    #         color = Category20[12][:len(data_labels[2:])],
    #         label = data_labels[2:]
    #     }
    cds = ColumnDataSource(data={
            'time': [datetime.strptime(entry['timestamp'], '%m/%d/%Y %H:%M') for entry in data]
        })
    
    for i, plot_name in enumerate(data_labels[2:]):
        cds.data.update({ 
            plot_name: [entry[plot_name] for entry in data]
        })

    return cds
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

    time = src.data['time']
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
    for label in data_labels[2:]:
        legend_label = ' '.join(word.title() for word in label.split('_'))
        if 'field' in label:
            lines[label] = plot.line( 
                x='time',
                y=label,
                line_color = label_colors[label], 
                line_alpha = 0.7, 
                hover_line_color = label_colors[label],
                hover_alpha = 1.0,
                y_range_name='mwt',
                source = src,
                line_width=2,
                visible=False)

            legend_label = legend_label + " (MWt)"
            legend.items.append(LegendItem(label=legend_label , renderers=[lines[label]]))
            plot.extra_y_ranges['mwt'].renderers.append(lines[label])

        else:
            lines[label] = plot.line( 
                x='time',
                y=label,
                line_color = label_colors[label], 
                line_alpha = 0.7, 
                hover_line_color = label_colors[label],
                hover_alpha = 1.0,
                source=src,
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

def updateTime(attr, old, new):
    new_src = make_dataset(list(TIME_BOXES.values())[new])
    src.data.update(new_src.data)
    plot.x_range.start = min(src.data['time'])

src = make_dataset(TIME_BOXES['LAST_24_HOURS'])
plot = make_plot(src)

# Create widget layout
radio_button_group = RadioButtonGroup(
    labels=["Last 12 Hours", "Last 24 Hours", "Last 48 Hours"], active=1)
radio_button_group.on_change('active', updateTime)
widgets = row(radio_button_group)

layout = column(widgets, plot, sizing_mode='stretch_both')

curdoc().add_root(layout)
curdoc().title = "Dashboard"