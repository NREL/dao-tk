#django imports
from django.shortcuts import render
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.db import transaction
# Model imports
from ui.models import DashboardSummaryItem
# Other package imports
from datetime import datetime, timedelta
import matplotlib.pyplot as plt, mpld3
from io import StringIO
#plot files imports
from fig import timeseries
#global variables
PROGRESS_BAR_WIDTH = 160

# Create your views here.
#-------------------------------------------------------------
#-------------------------------------------------------------
def index(request, path=None):
    
    context = {
        'connection_status' : True,
        'model_status' : False,
        'last_refresh' : datetime.now(),
    }

    if not path:
        return HttpResponseRedirect("dashboard")

    #handle URL based on path
    pvals = path.split('/')

    if 'configure' in pvals[0]:
        if len(pvals) != 2:
            return Http404(request)
        
        if 'event_log' in pvals[1]:
            return configure_event_log(request, context)
        elif 'logging' in pvals[1]:
            return configure_logging(request, context)
        elif 'model_settings' in pvals[1]:
            return configure_model_settings(request, context)
        elif 'sources' in pvals[1]:
            return configure_sources(request, context)
        elif 'ui_settings' in pvals[1]:
            return configure_ui_settings(request, context)
        else:
            return Http404(request)

    elif 'whatif' in pvals[0]:
        if len(pvals) != 2:
            return Http404(request)
        
        if 'interruptions' in pvals[1]:
            return whatif_interruptions(request, context)
        elif 'resources' in pvals[1]:
            return whatif_resources(request, context)
        elif 'weather' in pvals[1]:
            return whatif_weather(request, context)
        else:
            return Http404(request)
    else:
        if 'dashboard' in pvals[0]:
            return dashboard(request, context)
        elif 'outlook' in pvals[0]:
            return outlook(request, context)
        elif 'planning' in pvals[0]:
            return planning(request, context)
        elif 'system' in pvals[0]:
            return system(request, context)
        else:
            return Http404(request)

    return 

#-------------------------------------------------------------
def dashboard(request, context={}):
    """
    main view for the dashboard
    """
    
    #>>>>> temporary code to create necessary database objects
    DashboardSummaryItem.objects.all().delete()
    
    data = []
                # name                      #units              #icon           #varname                #group      #max        #actual     #model
    data.append(["Daily production total",  "MWh<sub>e</sub>",  "flash.png",    "daily_production",     "summary",  110*14*.75, 110*14*.25, 110*14*.31   ])
    data.append(["Net power",               "MW<sub>e</sub>",   "tower.png",    "net_power",            "summary",  110,        95,         80          ])
    data.append(["Gross power",             "MW<sub>e</sub>",   "turbine.png",  "gross_power",          "summary",  120,        100,      111.9       ])
    data.append(["Thermal storage charge",  "MWh<sub>t</sub>",  "tank.png",     "tes_charge",           "summary",  110*12/.4,  110*6/.4,   110*7/.4    ])
    data.append(["Daily revenue",           "$",                "notes.png",    "daily_revenue",        "summary",  ] + [ d * 90 for d in data[0][-3:]])
    

    with transaction.atomic():
        for row in data:
            DSI = DashboardSummaryItem(
                name = row[0],
                units = row[1],
                icon = row[2],
                varname = row[3],
                group = row[4],
                baseline_max = row[5],
                actual = row[6],
                model = row[7],
            )

            DSI.save()


    csvraw = [line.strip("\n").split(",")[0:2] for line in open("C:/Users/mwagner/Documents/NREL/software/dao-tk/web/site/fig/tsdata.csv",'r').readlines()[1:]]
    #add the timestamp
    dtit = datetime(2019, 1, 1, 0, 30, 0)
    csvdat = ["timestamp,actual,model"]
    for line in csvraw[24*100 : 24*105]:
        csvdat.append( ",".join([dtit.strftime("%Y:%m:%d-%H:%M:%S")] + line) )
        dtit += timedelta(minutes=30)
    #<<<<<<<<< end temporary code

    context["dashboard_summary_items"] = DashboardSummaryItem.objects.all()
    context["timeseries_plot"] = timeseries.daily_tracking(StringIO("\n".join(csvdat)), "Test timeseries plot", "MWh")


    return render(request, "dashboard.html", context)

#-------------------------------------------------------------
def outlook(request, context={}):
    return render(request, "outlook.html", context)

#-------------------------------------------------------------
def planning(request, context={}):
    return render(request, "planning.html", context)

#-------------------------------------------------------------
def system(request, context={}):
    return render(request, "system.html", context)

#-------------------------------------------------------------
#-------------------------------------------------------------

def configure_sources(request, context={}):
    return render(request, "configure/sources.html", context)    

def configure_logging(request, context={}):
    return render(request, "configure/logging.html", context)    

def configure_model_settings(request, context={}):
    return render(request, "configure/model_settings.html", context)    

def configure_ui_settings(request, context={}):
    return render(request, "configure/ui_settings.html", context)    

def configure_event_log(request, context={}):
    return render(request, "configure/event_log.html", context)    

#-------------------------------------------------------------
#-------------------------------------------------------------

def whatif_interruptions(request, context={}):
    return render(request, "whatif/interruptions.html", context)

def whatif_resources(request, context={}):
    return render(request, "whatif/resources.html", context)

def whatif_weather(request, context={}):
    return render(request, "whatif/weather.html", context)