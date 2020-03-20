from bokeh.plotting import figure
from bokeh.models import ColumnDataSource
from bokeh.models.widgets import Button, CheckboxGroup
from bokeh.palettes import Blues4
from bokeh.layouts import column, row, WidgetBox
import pandas as pd
from models import DashboardDataRTO

LAST_24_HOURS=-86400
def make_dataset(plot_list):
    # Prepare data
    data = DashboardDataRTO.objects.all()[LAST_24_HOURS:]
    by_plot = pd.DataFrame(columns=['time','value', 'color', 'name'])

    for i, plot_name in enumerate(plot_list):
        dataset = pd.DataFrame()
        dataset['time'] = [entry.timestamp for entry in data]
        dataset['value'] = [getattr(entry, plot_name) for entry in data]
        dataset['color'] = 'red' #Blues4[i]
        dataset['name'] = plot_name

        by_plot = by_plot.append(dataset)

    return ColumnDataSource(by_plot)

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
        plot_height = 200,
        toolbar_location = None,
        x_axis_label = None,
        y_axis_label = "Power (MWe)",
        x_range=(time[0], time[4])
        )

    plot.line(source = src, 
        x='time',
        y='value',
        line_color = 'red', 
        line_alpha = 0.7, 
        hover_line_color = 'red', 
        legend = 'name',
        hover_alpha = 1.0)

    plot.legend.click_policy = 'hide'
    
    # styling
    plot = style(plot)

    return plot

# Update the plot based on selections
def update(attr, old, new):
    data_to_plot = [plot_selection.labels[i] for i in plot_selection.active]

    new_src = make_dataset(data_to_plot)
    src.data.update(new_src.data)

available_plots = [field.name for field in DashboardDataRTO._meta.fields[2:]]
plot_selection = CheckboxGroup(labels=available_plots,
                                active=[0])
plot_selection.on_change('active', update)

#initial plot displayed
initial_plots = [plot_selection.labels[i] for i in plot_selection.active]

src = make_dataset(initial_plots)

plot = make_plot(src)

# Put Controls into single element
controls = WidgetBox(plot_selection)

# Create row layout
layout = row(controls, plot)

curdoc().add_root(layout)
curdoc().title = "Dashboard"