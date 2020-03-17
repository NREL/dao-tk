from .models import DashboardDataRTO, ForecastsMarketData, ForecastsSolarData
import csv

def loadDashboardData():
    with open("data/ui-dashboard_data_1-min.csv") as datafile:
        reader = csv.reader(datafile)
        next(reader)
        for row in reader:
            _, create = DashboardDataRTO.objects.get_or_create(
                timestamp = row[0],
                actual = row[1],
                optimal = row[2],
                scheduled = row[3],
                field_operation_generated = row[4],
                field_operation_available = row[5],
        )

def loadMarketData():
    with open("data/ui-forecasts_market_data_1-min.csv") as datafile:
        reader = csv.reader(datafile)
        next(reader)
        for row in reader:
            _, create = ForecastsMarketData.objects.get_or_create(
                timestamp = row[0],
                market_forecast = row[1],
                ci_plus = row[2],
                ci_minus = row[3],
            )

def loadSolarData():
    with open("data/ui-forecasts_solar_data_1-min.csv") as datafile:
        reader = csv.reader(datafile)
        next(reader)
        for row in reader:
            _, create = ForecastsSolarData.objects.get_or_create(
                timestamp = row[0],
                clear_sky = row[1],
                nam = row[2],
                nam_plus = row[3],
                nam_minus = row[4],
                rap = row[5],
                rap_plus = row[6],
                rap_minus = row[7],
                hrrr = row[8],
                hrrr_plus = row[9],
                hrrr_minus = row[10],
                gfs = row[11],
                gfs_plus = row[12],
                gfs_minus = row[13],
                ndfd = row[14],
                ndfd_plus = row[15],
                ndfd_minus = row[16],
            )