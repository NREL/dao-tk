# Generated by Django 2.2.2 on 2019-06-18 17:32

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('ui', '0003_timeseriesentry_timeserieshighlight'),
    ]

    operations = [
        migrations.AlterField(
            model_name='timeserieshighlight',
            name='display_start',
            field=models.DateTimeField(null=True),
        ),
    ]