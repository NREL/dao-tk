from datetime import date
import random

from bokeh.io import curdoc
from bokeh.layouts import column, row, WidgetBox, Spacer, layout
from bokeh.models.widgets import Div
from bokeh.models import ColumnDataSource, DataTable, DateFormatter, TableColumn
from bokeh.themes import built_in_themes

estimates_data = dict(
        parameter=['Cycle startup cost','Receiver startup cost','Startup energy usage','Receiver startup time',\
            'Power correction factor','10% capacity credit price cutoff','Storage terminal inventory incentive'],
        units=['$/start', '$/start','MWh', 'Min', 'MW/MW', '$/MWh','$MWh'],
        current=['{:,}'.format(random.randrange(0, 20000)) for i in range(2)]\
            + [random.randrange(0, 100) for i in range(2)]\
            + [random.randrange(0,1000)/100]\
            + [random.randrange(0, 100) for i in range(2)],
        last_7_days=['{:+d}%'.format(random.randrange(-20, 20)) for i in range(7)],
        last_6_months=['{:+d}%'.format(random.randrange(-30, 30)) for i in range(7)]
)
estimates_src = ColumnDataSource(estimates_data)

estimates_columns = [
        TableColumn(field="parameter", title="Parameter", width=900),
        TableColumn(field="units", title="Units", width=175),
        TableColumn(field="current", title="Current", width=200),
        TableColumn(field="last_7_days", title="Last 7 Days"),
        TableColumn(field="last_6_months", title="Last 6 Mo.")
    ]

estimates_table = DataTable(source=estimates_src, 
                            columns=estimates_columns,
                            fit_columns=True,
                            index_position=None,
                            width_policy='max',
                            height=285)

title = Div(text="""<h3>Estimates</h3>""")

# Show to current page
tables = column(
    title,
    estimates_table,
    max_height=275,
    width_policy='max'
)

curdoc().add_root(tables)
curdoc().title = "Estimates Tables"
