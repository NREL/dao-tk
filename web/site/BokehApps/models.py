from django.db import models

# Create your models here.
#--------------Dashboard Data------------------------------
class DashboardDataRTO(models.Model):
    timestamp = models.DateTimeField("Timestamp")
    actual = models.FloatField("Actual [MWe]", default=None)
    optimal = models.FloatField("Optimal [MWe]",default=None)
    scheduled = models.FloatField("Scheduled [MWe]", default=None)
    field_operation_generated = models.FloatField("Field Operation Generated [MWt]", default=None)
    field_operation_available = models.FloatField("Field Operation Available [MWt]", default=None)

    def __str__(self):
        return str(self.timestamp)


#----------Forcasts Market Data----------------------------
class ForecastsMarketData(models.Model):
    timestamp = models.DateTimeField("Timestamp")
    market_forecast = models.FloatField("Market Forcast [-]", default=None)
    ci_plus = models.FloatField("CI+ [%]", default=None)
    ci_minus = models.FloatField("CI- [%]", default=None)

    def __str__(self):
        return str(self.timestamp)

#-----------Forcasts Solar Data----------------------------
class ForecastsSolarData(models.Model):
    timestamp = models.DateTimeField("Timestamp")
    clear_sky = models.FloatField("Clear Sky [W/m2]", default=None)
    nam = models.FloatField("NAM [W/m2]", default=None)
    nam_plus = models.FloatField("NAM+ [%]", default=None)
    nam_minus = models.FloatField("NAM- [%]", default=None)
    rap = models.FloatField("RAP [W/m2]", default=None)
    rap_plus = models.FloatField("RAP+ [%]", default=None)
    rap_minus = models.FloatField("RAP- [%]", default=None)
    hrrr = models.FloatField("HRRR [W/m2]", default=None)
    hrrr_plus = models.FloatField("HRRR+ [%]", default=None)
    hrrr_minus = models.FloatField("HRRR- [%]", default=None)
    gfs = models.FloatField("GFS [W/m2]", default=None)
    gfs_plus = models.FloatField("GFS+ [%]", default=None)
    gfs_minus = models.FloatField("GFS- [%]", default=None)
    ndfd = models.FloatField("NDFD [W/m2]", default=None)
    ndfd_plus = models.FloatField("NDFD+ [%]", default=None)
    ndfd_minus = models.FloatField("NDFD- [%]", default=None)

    def __str__(self):
        return str(self.timestamp)
