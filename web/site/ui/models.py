from django.db import models
from django.db import transaction

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
