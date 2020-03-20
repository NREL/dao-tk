from django.db import models
from django.db import transaction
import datetime

#--------- dashboard summary object --------------------
class DashboardSummaryItem(models.Model):
    name = models.CharField(max_length=100, default="")
    varname = models.CharField(max_length=50, default="")
    units = models.CharField(max_length=25, default="")
    icon = models.CharField(max_length=150, default="")  #path to icon in media folder
    group = models.CharField(max_length=50, default="")

    baseline_max = models.FloatField(default=None)
    actual = models.FloatField(default=None)
    model = models.FloatField(default=None)

    difference = models.FloatField(default=None)
    bar_total_width = models.SmallIntegerField(default=160)
    bar_left_width = models.SmallIntegerField(default=None)
    bar_mid_width = models.SmallIntegerField(default=None)
    bar_right_width = models.SmallIntegerField(default=None)
    
    exceeds_model = models.BooleanField()   #auto-calculated

    def save(self, *args, **kwds):
        #update any calculated or dependent fields on save
        if self.actual and self.model:
            self.difference = self.actual - self.model
            self.exceeds_model = self.difference > 0
        
        self.bar_left_width = int( min([self.actual, self.model])/self.baseline_max*self.bar_total_width )
        self.bar_mid_width = int( abs(self.difference)/self.baseline_max*self.bar_total_width ) 
        self.bar_right_width = self.bar_total_width - (self.bar_left_width + self.bar_mid_width)

        #call the built-in save function
        super(DashboardSummaryItem, self).save(*args, **kwds)
#----------------------------------------------------------

#----------------------------------------------------------
class TimeSeriesEntry(models.Model):
    timestamp = models.DateTimeField()
    data = models.CharField(max_length=200, default=None)

class TimeSeriesHighlight(models.Model):
    name = models.CharField(max_length=100, default="")
    varname = models.CharField(max_length=50, default="")
    has_secondary_axis = models.BooleanField(default=False)
    units = models.CharField(max_length=25, default="")
    xaxis_label = models.CharField(max_length=100, default="")
    yaxis_label = models.CharField(max_length=100, default="")

    units_secondary = models.CharField(max_length=25, default="")
    yaxis_label_secondary = models.CharField(max_length=100, default="")

    entries = models.ManyToManyField(TimeSeriesEntry, "entries")
#----------------------------------------------------------

# New plot tables:
#--------------Dashboard Data------------------------------
class DashboardDataRTO(models.Model):
    timestamp = models.DateTimeField(verbose_name="Timestamp")
    actual = models.FloatField(verbose_name="Actual [MWe]", default=None)
    optimal = models.FloatField(verbose_name="Optimal [MWe]",default=None)
    scheduled = models.FloatField(verbose_name="Scheduled [MWe]", default=None)
    field_operation_generated = models.FloatField(verbose_name="Field Operation Generated [MWt]", default=None)
    field_operation_available = models.FloatField(verbose_name="Field Operation Available [MWt]", default=None)

    def __str__(self):
        return str(self.timestamp)


#----------Forcasts Market Data----------------------------
class ForecastsMarketData(models.Model):
    timestamp = models.DateTimeField(verbose_name="Timestamp")
    market_forecast = models.FloatField(verbose_name="Market Forcast [-]", default=None)
    ci_plus = models.FloatField(verbose_name="CI+ [%]", default=None)
    ci_minus = models.FloatField(verbose_name="CI- [%]", default=None)

    def __str__(self):
        return str(self.timestamp)

#-----------Forcasts Solar Data----------------------------
class ForecastsSolarData(models.Model):
    timestamp = models.DateTimeField(verbose_name="Timestamp")
    clear_sky = models.FloatField(verbose_name="Clear Sky [W/m2]", default=None)
    nam = models.FloatField(verbose_name="NAM [W/m2]", default=None)
    nam_plus = models.FloatField(verbose_name="NAM+ [%]", default=None)
    nam_minus = models.FloatField(verbose_name="NAM- [%]", default=None)
    rap = models.FloatField(verbose_name="RAP [W/m2]", default=None)
    rap_plus = models.FloatField(verbose_name="RAP+ [%]", default=None)
    rap_minus = models.FloatField(verbose_name="RAP- [%]", default=None)
    hrrr = models.FloatField(verbose_name="HRRR [W/m2]", default=None)
    hrrr_plus = models.FloatField(verbose_name="HRRR+ [%]", default=None)
    hrrr_minus = models.FloatField(verbose_name="HRRR- [%]", default=None)
    gfs = models.FloatField(verbose_name="GFS [W/m2]", default=None)
    gfs_plus = models.FloatField(verbose_name="GFS+ [%]", default=None)
    gfs_minus = models.FloatField(verbose_name="GFS- [%]", default=None)
    ndfd = models.FloatField(verbose_name="NDFD [W/m2]", default=None)
    ndfd_plus = models.FloatField(verbose_name="NDFD+ [%]", default=None)
    ndfd_minus = models.FloatField(verbose_name="NDFD- [%]", default=None)

    def __str__(self):
        return str(self.timestamp)
