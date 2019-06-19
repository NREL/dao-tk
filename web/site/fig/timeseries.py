from matplotlib import use as mpluse
mpluse("Agg")  #need to use this backend to avoid errors on shutdown routines
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import mpld3
import datetime
import dateutil.parser
# from mpld3 import plugins
from pandas.plotting import register_matplotlib_converters

def _timestamp_parse(ts):
    # return datetime.datetime.strptime(ts, "%Y-%m-%d %H:%M:%S.%f")
    return dateutil.parser.parse(ts)

def daily_tracking(csvdata, name, units):

    """
    csvdata format:
        "
        timestamp,model,actual\n
        2019-01-01 00:30:00.0,0,1\n
        ...
        "
    """
    plt.style.use('seaborn-deep')
    #['seaborn-ticks', 'ggplot', 'dark_background', 'bmh', 'seaborn-poster', 'seaborn-notebook', 'fast', 'seaborn', 'classic', 
    # 'Solarize_Light2', 'seaborn-dark', 'seaborn-pastel', 'seaborn-muted', '_classic_test', 'seaborn-paper', 'seaborn-colorblind', 
    # 'seaborn-bright', 'seaborn-talk', 'seaborn-dark-palette', 'tableau-colorblind10', 'seaborn-darkgrid', 'seaborn-whitegrid', 
    # 'fivethirtyeight', 'grayscale', 'seaborn-white', 'seaborn-deep']
    plt.rcParams.update({'font.size': 12})
    register_matplotlib_converters()


    # generate df
    df = pd.read_csv(csvdata, index_col="timestamp", parse_dates=True, date_parser=_timestamp_parse, )

    # plot line + confidence interval
    # fig, ax = plt.subplots()
    fig = plt.figure(figsize=(10,3))
    ax = plt.gca()
    ax.grid(True, alpha=0.3)

    l, = ax.plot(df.actual.index, df.actual.values, label="actual")
    ax.fill_between(df.actual.index, df.actual.values*0, df.actual.values, color=l.get_color(), alpha=0.3)
    ax.fill_between(df.actual.index, df.model.values, df.actual.values, alpha=0.7)

    ax.set_xlabel("Time")
    ax.set_ylabel(units.replace("<sub>", '').replace("</sub>", ''))
    plt.tight_layout()

    # labels = ['{:s}\nActual: {:.1f}\nModel:{:.1f}'.format(df.actual.index[i].strftime("%m/%d %H:%M"), df.actual.values[i], df.model.values[i]) for i in range(len(df.actual.index))]
    # tooltip = mpld3.plugins.PointHTMLTooltip(l, labels)
    # mpld3.plugins.connect(fig, tooltip)

    fig_html =  mpld3.fig_to_html(fig)

    plt.close(fig)

    return fig_html
