from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.

#-------------------------------------------------------------
def index(request):
    # return HttpResponse("Hello world!")
    context = {}
    return render(request, "base.html", context)

def dashboard(request):
    context = {}
    return render(request, "dashboard.html", context)

def outlook(request):
    context = {}
    return render(request, "outlook.html", context)

def planning(request):
    context = {}
    return render(request, "planning.html", context)

def system(request):
    context = {}
    return render(request, "system.html", context)

#-------------------------------------------------------------
#-------------------------------------------------------------

def configure_sources(request):
    context = {}
    return render(request, "configure/sources.html", context)    

def configure_logging(request):
    context = {}
    return render(request, "configure/logging.html", context)    

def configure_model_settings(request):
    context = {}
    return render(request, "configure/model_settings.html", context)    

def configure_ui_settings(request):
    context = {}
    return render(request, "configure/ui_settings.html", context)    

def configure_event_log(request):
    context = {}
    return render(request, "configure/event_log.html", context)    

#-------------------------------------------------------------
#-------------------------------------------------------------

def whatif_interruptions(request):
    context = {}
    return render(request, "whatif/interruptions.html", context)

def whatif_resources(request):
    context = {}
    return render(request, "whatif/resources.html", context)

def whatif_weather(request):
    context = {}
    return render(request, "whatif/weather.html", context)