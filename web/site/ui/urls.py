from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path(r'<path:path>', views.index),     #main url handler. Supplemental named urls follow below.
    path(r'dashboard', views.dashboard, name='dashboard'),
    path(r'outlook', views.outlook, name='outlook'),
    path(r'planning', views.planning, name='planning'),
    path(r'system', views.system, name='system'),
    path(r'configure/event_log', views.configure_event_log, name='event_log' ),
    path(r'configure/logging', views.configure_logging, name='logging' ),
    path(r'configure/model_settings', views.configure_model_settings, name='model_settings' ),
    path(r'configure/sources', views.configure_sources, name='sources' ),
    path(r'configure/ui_settings', views.configure_ui_settings, name='ui_settings' ),
    path(r'whatif/interruptions', views.whatif_interruptions, name='interruptions' ),
    path(r'whatif/resources', views.whatif_resources, name='resources' ),
    path(r'whatif/weather', views.whatif_weather, name='weather' ),
]