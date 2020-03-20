from django.contrib import admin

# Register your models here.
from .models import DashboardDataRTO, ForecastsMarketData, ForecastsSolarData

admin.site.register(DashboardDataRTO)
admin.site.register(ForecastsMarketData)
admin.site.register(ForecastsSolarData)