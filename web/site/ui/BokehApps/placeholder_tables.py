from datetime import date
import random

from bokeh.io import curdoc
from bokeh.layouts import column, row, WidgetBox, Spacer, layout
from bokeh.models import ColumnDataSource, DataTable, DateFormatter, TableColumn

probability_data = dict(
        probability_event=['Frost','Snow','Rain','Wind outage','Peak Demand','Unplanned outage'],
        next_day=[random.randrange(0, 1000)/10 for i in range(6)],
        next_3_days=[random.randrange(0, 1000)/10 for i in range(6)],
        next_7_days=[random.randrange(0, 1000)/10 for i in range(6)],
    )

estimates_data = dict(
        parameter=['Cycle startup cost','Receiver startup cost','Startup energy usage','Receiver startup time',\
            'Power correction factor','10\% capacity credit price cutoff','Storage terminal inventory incentive'],
        units=['$/start', '$/start','MWh', 'Min', 'MW/MW', '$/MWh','$MWh'],
        current=['{:,}'.format(random.randrange(0, 20000)) for i in range(2)]\
            + [random.randrange(0, 100) for i in range(2)]\
            + [random.randrange(0,1000)/100]\
            + [random.randrange(0, 100) for i in range(2)],
        last_7_days=['{:+d}%'.format(random.randrange(-20, 20)) for i in range(7)],
        last_6_months=['{:+d}%'.format(random.randrange(-30, 30)) for i in range(7)]
)
probability_src = ColumnDataSource(probability_data)
estimates_src = ColumnDataSource(estimates_data)

probability_columns = [
        TableColumn(field="probability_event", title="Probability Event", width=True),
        TableColumn(field="next_day", title="Next Day"),
        TableColumn(field="next_3_days", title="Next 3 Days"),
        TableColumn(field="next_7_days", title="Next 7 Days")
    ]
estimates_columns = [
        TableColumn(field="parameter", title="Parameter", fit_columns=True),
        TableColumn(field="units", title="Units"),
        TableColumn(field="current", title="Current"),
        TableColumn(field="last_7_days", title="Last 7 Days"),
        TableColumn(field="last_6_months", title="Last 6 Mo.")
    ]
probability_table = DataTable(source=probability_src, columns=probability_columns, width=400, height=280)
estimates_table = DataTable(source=estimates_src, columns=estimates_columns, width=400, height=280)


# Show to current document/page
tables = row(
    probability_table,
    estimates_table,
    width_policy='max'
)

curdoc().add_root(tables)
curdoc().title = "Forecast Tables"
