import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import mpld3
import datetime
# from mpld3 import plugins

def _timestamp_parse(ts):
    return datetime.datetime.strptime(ts, "%Y:%m:%d-%H:%M:%S")

def daily_tracking(csvdata, name, units):
    """
    csvdata format:
        "
        timestamp,model,actual\n
        2019:01:01-00:30:00,0,1\n
        ...
        "
    """

    # generate df
    df = pd.read_csv(csvdata, index_col="timestamp", parse_dates=True, date_parser=_timestamp_parse, )

    # plot line + confidence interval
    fig, ax = plt.subplots()
    ax.grid(True, alpha=0.3)

    l, = ax.plot(df.actual.index, df.actual.values, label="actual")
    ax.fill_between(df.actual.index, df.actual.values*0, df.actual.values, color=l.get_color(), alpha=0.4)
    ax.fill_between(df.actual.index, df.model.values, df.actual.values, color='red', alpha=0.6)

    ax.set_xlabel("Time")
    ax.set_ylabel(units)
    ax.set_title(name, size=20)

    return mpld3.fig_to_html(fig)