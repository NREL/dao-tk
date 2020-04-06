from datetime import date
import random

from bokeh.io import curdoc
from bokeh.models.widgets import Div
from bokeh.layouts import column, row, WidgetBox, Spacer, layout
from bokeh.models import ColumnDataSource, DataTable, DateFormatter, TableColumn
from bokeh.themes import built_in_themes

probability_data = dict(
        probability_event=['Frost','Snow','Rain','Wind outage','Peak Demand','Unplanned outage'],
        next_day=[random.randrange(0, 1000)/10 for i in range(6)],
        next_3_days=[random.randrange(0, 1000)/10 for i in range(6)],
        next_7_days=[random.randrange(0, 1000)/10 for i in range(6)],
    )

probability_src = ColumnDataSource(probability_data)

probability_columns = [
        TableColumn(field="probability_event", title="Probability Event", width=400),
        TableColumn(field="next_day", title="Next Day"),
        TableColumn(field="next_3_days", title="Next 3 Days"),
        TableColumn(field="next_7_days", title="Next 7 Days")
    ]

probability_table = DataTable(source=probability_src, 
                            columns=probability_columns,
                            fit_columns=True,
                            index_position=None,
                            width_policy='max',
                            height=285)

title = Div(text="""<h3>Probability</h3>""")

# Show to current page
tables = column(
    title,
    probability_table,
    max_height=275,
    width_policy='max'
)

curdoc().add_root(tables)
curdoc().title = "Probability Tables"
