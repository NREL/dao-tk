from django.apps import AppConfig
from importlib import import_module
from django.conf import settings
SessionStore = import_module(settings.SESSION_ENGINE).SessionStore
from ui import mspt

s = SessionStore()

class UiConfig(AppConfig):
    name = 'ui'
    verbose_name = 'Dashboard App'
