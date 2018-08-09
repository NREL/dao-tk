#include "metrics.h"
#include "solpos00.h"

#include <fstream>
#include <sstream>

clustering_metrics::clustering_metrics()
{

	inputs.nsimdays = inputs.nyears = std::numeric_limits<int>::quiet_NaN();
	inputs.stowlimit = std::numeric_limits<double>::quiet_NaN();
	inputs.cskymodel = MEINEL;
	inputs.is_price_files = false;

	std::string keys[] = { "dni", "dni_prev", "dni_next", "clearsky", "price", "price_prev", "price_next", "tdry", "wspd", "sfavail" };

	for (int i = 0; i < 9; i++)
	{
		std::string key = keys[i];
		inputs.features[key].weight = std::numeric_limits<double>::quiet_NaN();
		inputs.features[key].divisions = std::numeric_limits<int>::quiet_NaN();
	}

	inputs.features["dni"].daylight_only = true;
	inputs.features["dni_prev"].daylight_only = true;
	inputs.features["dni_next"].daylight_only = true;
	inputs.features["clearsky"].daylight_only = true;
	inputs.features["price"].daylight_only = false;
	inputs.features["price_prev"].daylight_only = false;
	inputs.features["price_next"].daylight_only = false;
	inputs.features["tdry"].daylight_only = false;
	inputs.features["wspd"].daylight_only = true;
	inputs.features["sfavail"].daylight_only = true;

	results.nobs = std::numeric_limits<int>::quiet_NaN();
	results.nfeatures = std::numeric_limits<int>::quiet_NaN();

}

void clustering_metrics::clear_results()
{
	results.nobs = results.nfeatures = 0;
	results.data.clear();
	results.data_firstday.clear();
	results.data_lastday.clear();
	results.daily_dni.clear();
	return;
}

void clustering_metrics::set_default_weights()
{
	inputs.features["dni"].weight = 1.0;
	inputs.features["dni_prev"].weight = 0.5;
	inputs.features["dni_next"].weight = 0.5;
	inputs.features["clearsky"].weight = 0.0;
	inputs.features["price"].weight = 1.0;
	inputs.features["price_prev"].weight = 0.5;
	inputs.features["price_next"].weight = 0.5;
	inputs.features["tdry"].weight = 0.25;
	inputs.features["wspd"].weight = 0.25;
	inputs.features["sfavail"].weight = 0.0;

	inputs.features["dni"].divisions = 8;
	inputs.features["dni_prev"].divisions = 4;
	inputs.features["dni_next"].divisions = 4;
	inputs.features["clearsky"].divisions = 1;
	inputs.features["price"].divisions = 8;
	inputs.features["price_prev"].divisions = 4;
	inputs.features["price_next"].divisions = 4;
	inputs.features["tdry"].divisions = 4;
	inputs.features["wspd"].divisions = 4;
	inputs.features["sfavail"].divisions = 1;

	return;
}

void clustering_metrics::set_default_inputs()
{
	inputs.nsimdays = 2;
	inputs.nyears = 1;
	inputs.is_remove_outliers = true;
	inputs.stowlimit = 15.0;
	inputs.cskymodel = MEINEL;

	set_default_weights();

	return;
}

std::vector<std::string> clustering_metrics::split(const std::string &line, char delim = ',')
{
	std::string cell;
	std::vector<std::string> result;
	std::stringstream lineStream(line);
	while (std::getline(lineStream, cell, delim))
		result.push_back(cell);
	return result;
}

bool clustering_metrics::read_csv(const std::string &csvfile, std::vector<double>&data)
{
	// Read single column csv file and store results in data
	std::fstream file;
	std::string line;
	data.clear();
	bool ok = false;
	file.open(csvfile, std::ios::in);
	if (file.is_open())
	{
		while (std::getline(file, line))
			data.push_back(std::stod(line));
		file.close();
		ok = true;
	}
	return ok;
}

bool clustering_metrics::read_weather(const std::string &weatherfile, double &lat, double &lon, double &elev, int &tz, int &year, bool &is_tmy, std::vector<double>&dni, std::vector<double>&wspd, std::vector<double>&tdry)
{
	// Read weather file and return, latitude, longitude, elevation, dni, wind speed and tdry
	// Weather files are assumed to follow either TMY2 or TMY3 format


	std::fstream file;
	std::string line;
	bool ok = false;
	std::vector<int> years;

	file.open(weatherfile, std::ios::in);
	if (file.is_open())
	{
		std::getline(file, line);

		// Read file header
		std::getline(file, line);
		std::vector<std::string> cols = split(line);
		if (cols.size() < 9)  // Not enough data in header
			return false;
		lat = std::stod(cols.at(5));
		lon = std::stod(cols.at(6));
		tz = std::stoi(cols.at(7));
		elev = std::stod(cols.at(8));

		// Read column headers
		std::getline(file, line);
		cols.clear();
		cols = split(line);
		int dnicol = -1;
		int tcol = -1;
		int wcol = -1;
		std::vector<std::string> tname = { "Temperature", "Tdry" };
		std::vector<std::string> wname = { "Wind Speed", "Wspd" };
		for (int i = 0; i < cols.size(); i++)
		{
			if (cols[i] == "DNI")
				dnicol = i;
			else
			{
				for (int j = 0; j < tname.size(); j++)
				{
					if (cols[i] == tname[j])
						tcol = i;
				}
				for (int j = 0; j < wname.size(); j++)
				{
					if (cols[i] == wname[j])
						wcol = i;
				}
			}
		}
		if (dnicol == -1 || tcol == -1 || wcol == -1)
			return false;

		// Read data (skipping 2/29 if present)
		std::vector<std::string> data;
		dni.clear();
		tdry.clear();
		wspd.clear();
		while (std::getline(file, line))
		{
			data = split(line);
			double month = std::stod(data[1]);
			double day = std::stod(data[2]);
			double hour = std::stod(data[3]);
			year = std::stoi(data[0]);

			if (day == 1 && hour == 0)
				years.push_back(year);

			if (month == 2 && day == 29)
				continue;
			else
			{
				dni.push_back(std::stod(data[dnicol]));
				tdry.push_back(std::stod(data[tcol]));
				wspd.push_back(std::stod(data[wcol]));
			}
		}

		is_tmy = false;
		int m = 1;
		while (m < years.size() && !is_tmy)
		{
			if (years.at(m) != years.at(m - 1))
				is_tmy = true;
			m += 1;
		}

		ok = true;
	}

	return ok;
}

void clustering_metrics::approximate_clearsky(int model, double lat, double lon, double elev, int year, int tz, bool is_tmy, double tstephr, std::vector<double>&csky)
{
	// Calculate approximate clear sky DNI

	int nperhr = int(1. / tstephr);
	double tstep_sec = tstephr * 3600.;
	csky.clear();
	int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	int doy = 1;
	for (int m = 0; m < 12; m++)
	{
		for (int d = 0; d < days[m]; d++)
		{
			for (int h = 0; h < 24; h++)
			{
				for (int t = 0; t < nperhr; t++)
				{

					// Calculate sun position
					double tsec = t * tstep_sec;
					int min = int(tsec / 60.);
					int sec = int(tsec - min * 60.);

					struct posdata sp, *pdat;
					pdat = &sp;
					S_init(pdat);
					pdat->latitude = float(lat);	//[deg] {float} North is positive
					pdat->longitude = float(lon);	//[deg] {float} Degrees east. West is negative
					pdat->timezone = float(tz);		//[hr] {float} Time zone, east pos. west negative. Mountain -7, Central -6, etc..
					pdat->year = year;				//[year] {int} 4-digit year
					pdat->month = m + 1;				//[mo] {int} (1-12)
					pdat->day = d + 1;				//[day] {int} Day of the month
					pdat->daynum = doy;				//[day] {int} Day of the year
					pdat->hour = h + 1;				//[hr] {int} 0-23
					pdat->minute = min;				//[min] {int} 0-59
					pdat->second = sec;				//[sec]	{int} 0-59
					pdat->interval = int(tstep_sec);	//[sec] {int} Measurement interval.  Note: input time is assumed to be at the end of the measurement interval, values are calculated at the midpoint of the interval

					if (is_tmy)  // TMY files: dni at beginning of interval; Single year: dni at midpoint of interval
						pdat->interval = 2 * int(tstep_sec);	// double the interval so that the midpoint is at the beginning of the TMY interval

					long retcode = 0;
					retcode = S_solpos(pdat);
					S_decode(retcode, pdat);

					double zen = sp.zenetr;   // [deg]

											  // Estimate clear sky DNI
					double cskypt, s0, beta, alt;
					beta = 2 * 3.14159*(doy / 365.);
					s0 = 1367 * (1.00011 + 0.034221*cos(beta) + 0.00128*sin(beta) + 0.000719*cos(2 * beta) + 0.000077*sin(2 * beta));
					alt = elev / 1000.;   // convert to km

					switch (model)
					{
					case MEINEL:
					{
						if (zen >= 90.0)
							cskypt = 0.0;
						else
							cskypt = s0 * ((1.0 - 0.14*alt)*exp(-0.347*pow((1. / cos(zen*3.14159 / 180.)), 0.678)) + 0.14*alt);
						break;
					}

					case HOTTEL:
					{
						double a = 0.4237 - 0.00821*pow((6.0 - alt), 2.0);
						double b = 0.5055 + 0.00595*pow((6.5 - alt), 2.0);
						double c = 0.2711 + 0.01858*pow((2.5 - alt), 2.0);
						if (zen >= 90.0)
							cskypt = 0.0;
						else
							cskypt = s0 * (a + b * exp(-c / cos(zen*3.14159 / 180.)));
						break;
					}
					}

					csky.push_back(cskypt);
				}
			}
			doy += 1;
		}

	}

	return;
}

void clustering_metrics::calc_metrics()
{
	clear_results();

	int tz, year, npts, nptsday, nptshr, day;
	double lat, lon, elev, tstephr;
	bool is_tmy;
	std::vector<double> csky;
	unordered_map<std::string, std::vector<double>> timeseries;

	std::vector<int> visible_days; // Days "visible" within this simulation period
	for (int d = -1; d < inputs.nsimdays + 1; d++)
		visible_days.push_back(d);


	//--- Number of days associated with each metric and offset from first day in group
	struct specs
	{
		std::string timeseries;
		int ndays, offset;
		double maxval, minval;
		std::vector<double> max_visible, max_visible_first, max_visible_last;
	};

	unordered_map<std::string, specs> featuremap;

	std::vector<std::string> group = { "dni", "price", "clearsky", "tdry", "wspd", "sfavail" };
	for (int k = 0; k < group.size(); k++)
	{
		featuremap[group.at(k)].timeseries = group.at(k);
		featuremap[group.at(k)].ndays = inputs.nsimdays;
		featuremap[group.at(k)].offset = 0;
	}
	featuremap["clearsky"].timeseries = "cskydiff";

	std::vector<std::string> group_prev = { "dni_prev", "price_prev" };
	for (int k = 0; k < group_prev.size(); k++)
	{
		featuremap[group_prev.at(k)].ndays = 1;
		featuremap[group_prev.at(k)].offset = -1;
	}

	std::vector<std::string> group_next = { "dni_next", "price_next" };
	for (int k = 0; k < group_next.size(); k++)
	{
		featuremap[group_next.at(k)].ndays = 1;
		featuremap[group_next.at(k)].offset = inputs.nsimdays;
	}
	featuremap["dni_prev"].timeseries = "dni";
	featuremap["dni_next"].timeseries = "dni";
	featuremap["price_prev"].timeseries = "price";
	featuremap["price_next"].timeseries = "price";




	//--- Determine total size of data array for clustering and initialize results
	std::vector<std::string> order = { "dni", "dni_prev", "dni_next", "clearsky", "price", "price_prev", "price_next", "tdry", "wspd", "sfavail" };
	int ngroup = int(363. / inputs.nsimdays);   // Number of possible simulation groups per year
	results.nobs = ngroup * inputs.nyears;
	results.nfeatures = 0;
	for (int k = 0; k < order.size(); k++)
	{
		std::string key = order[k];
		if (inputs.features[key].weight > 0.0)
			results.nfeatures += featuremap[key].ndays * inputs.features[key].divisions;

		featuremap[key].minval = 1.e10;
		featuremap[key].maxval = -1.e10;
		featuremap[key].max_visible.assign(results.nobs, -1e10);
		featuremap[key].max_visible_first.assign(inputs.nyears, -1e10);
		featuremap[key].max_visible_last.assign(inputs.nyears, -1e10);

	}
	results.data.resize_fill(results.nobs, results.nfeatures, std::numeric_limits<double>::quiet_NaN());
	results.data_firstday.resize_fill(inputs.nyears, results.nfeatures, std::numeric_limits<double>::quiet_NaN());
	results.data_lastday.resize_fill(inputs.nyears, results.nfeatures, std::numeric_limits<double>::quiet_NaN());




	//--- Read first year to set resolution and daylight points
	read_weather(inputs.weather_files[0], lat, lon, elev, tz, year, is_tmy, timeseries["dni"], timeseries["wspd"], timeseries["tdry"]);
	npts = (int)timeseries["dni"].size();
	nptsday = npts / 365;
	nptshr = nptsday / 24;
	tstephr = 8760. / (double)npts;
	approximate_clearsky(inputs.cskymodel, lat, lon, elev, year, tz, is_tmy, tstephr, csky);

	day = 172;
	int summer_sunrise = 0;
	int summer_daylight_pts = 0;
	double daylight_cutoff = 50;
	bool found_sunrise = false;
	for (int i = 0; i<nptsday; i++)
	{
		if (csky[day*nptsday + i] >= daylight_cutoff)
		{
			summer_daylight_pts += 1;
			if (!found_sunrise)
				summer_sunrise = i;
			found_sunrise = true;
		}
	}
	results.summer_sunrise = summer_sunrise;
	results.n_daylight_pts = summer_daylight_pts;





	//--- Read data for each year and calculate metrics
	results.daily_dni.resize_fill(365, inputs.nyears, 0.0);
	for (int y = 0; y < inputs.nyears; y++)
	{
		//--- Read weather
		read_weather(inputs.weather_files[y], lat, lon, elev, tz, year, is_tmy, timeseries["dni"], timeseries["wspd"], timeseries["tdry"]);
		approximate_clearsky(inputs.cskymodel, lat, lon, elev, year, tz, is_tmy, tstephr, csky);

		//--- Replace DNI and wind speed above stow limit
		timeseries["cskydiff"] = csky;
		for (int i = 0; i < timeseries["dni"].size(); i++)
		{
			if (timeseries["wspd"][i] > inputs.stowlimit)
			{
				timeseries["dni"][i] = 0.0;
				timeseries["wspd"][i] = inputs.stowlimit;  // Reset wind speed when heliostats are stowed to avoid outliers
			}
			timeseries["cskydiff"][i] -= timeseries["dni"][i];

		}

		//--- Calculate daily dni for year y
		for (int d = 0; d < 365; d++)
		{
			for (int h = 0; h < nptsday; h++)
				results.daily_dni.at(d, y) += timeseries["dni"].at(d*nptsday + h) / 1000. / double(nptshr);
		}

		//--- Read price
		if (inputs.is_price_files)
			read_csv(inputs.price_files[y], timeseries["price"]);
		else
			timeseries["price"] = inputs.prices;

		//--- Scale extreme price outliers 
		if (inputs.is_remove_outliers)
		{
			double cutoffiq = 3.5;    // Cutoff to define outliers (number of interquartile ranges above q3)
			double maxiq = 4.0;		  // Maximum for scaled outliers (number of interquartile ranges above q3)

			matrix<double> pm(timeseries["price"]);
			std::vector<double> q = pm.quartiles();
			double iq = q[3] - q[1];
			double upper = q[3] + cutoffiq * iq;
			double lower = q[1] - cutoffiq * iq;
			double ymaxscaled = fmin(q[4], q[3] + maxiq * iq);
			double yminscaled = fmax(q[0], q[1] - maxiq * iq);
			double avgprice = 0.0;
			double avgpricenew = 0.0;

			for (int i = 0; i < timeseries["price"].size(); i++)
			{
				avgprice += timeseries["price"][i];    // Average price before scaling
				if (timeseries["price"][i] > upper)
					timeseries["price"][i] = upper + (timeseries["price"][i] - upper) / (q[4] - upper) * (ymaxscaled - upper);
				if (timeseries["price"][i] < lower)
					timeseries["price"][i] = lower - (lower - timeseries["price"][i]) / (lower - q[0]) * (lower - yminscaled);
				avgpricenew += timeseries["price"][i];   // Average price after scaling
			}
			avgprice /= (double)timeseries["price"].size();
			avgpricenew /= (double)timeseries["price"].size();

			// Renormalize price
			if (avgprice > 0.98 && avgprice < 1.02)
			{
				for (int i = 0; i < timeseries["price"].size(); i++)
					timeseries["price"][i] /= avgpricenew;
			}
		}


		//--- Read solar field availability
		if (inputs.features["sfavail"].weight > 0.0)
		{
			timeseries["sfavail"].assign(timeseries["dni"].size(), 1.0);
			if (inputs.sfavail.size() < timeseries["dni"].size())
				timeseries["sfavail"] = inputs.sfavail;
		}


		//--- Calculate classification metrics
		int feature = 0;
		for (int k = 0; k < order.size(); k++)   // Loop over metrics
		{
			std::string key = order[k];
			if (inputs.features[key].weight > 0.0)   // Only use metrics with nonzero weighting factor
			{
				int ndiv = inputs.features[key].divisions;  // Number of divisions per day for this metric
				int np = nptsday;			// Number of points per day included in this metric
				int p1 = 0;					 // First point for this metric
				if (inputs.features[key].daylight_only)
				{
					np = summer_daylight_pts;
					p1 = summer_sunrise;
				}

				//--- Calculate averaged values for each day
				matrix<double> daily_metrics(365, ndiv, 0.0);
				double nperdiv = (double)np / (double)ndiv; // Fractional number of time points per division
				for (int i = 0; i < ndiv; i++)  // Divisions per day
				{
					double pstart = i * nperdiv;
					double pend = (i + 1)*nperdiv;

					// Points included in average for this division
					std::vector<int>pts;
					std::vector<double>wts;
					for (int p = int(pstart); p <= int(pend); p++)
					{
						pts.push_back(p);
						wts.push_back(1. / nperdiv);
					}
					wts[0] = (1.0 - (pstart - int(pstart))) / nperdiv;
					wts.back() = (pend - int(pend)) / nperdiv;

					// Daily values for each averaging period
					for (int d = 0; d < 365; d++)  // Day of year
					{
						for (int p = 0; p < pts.size(); p++)  // Points included in average for this division
						{
							if (pts[p] < nptsday)
							{
								int j = d * nptsday + p1 + pts[p];
								std::string name = featuremap[key].timeseries;
								daily_metrics.at(d, i) += timeseries[name].at(j) * wts[p];
							}
						}
						featuremap[key].maxval = fmax(featuremap[key].maxval, daily_metrics.at(d, i));
						featuremap[key].minval = fmin(featuremap[key].minval, daily_metrics.at(d, i));
					}
				}


				//--- Calculate metrics for each simulation grouping
				int d1, m, f, nd;
				nd = featuremap[key].ndays;
				for (int g = 0; g < ngroup; g++)   // Simulation groupings in this year
				{
					m = y * ngroup + g;
					d1 = (g * inputs.nsimdays + 1) + featuremap[key].offset;	 // First day of this simulation grouping that applies to this metric
					for (int j = 0; j < nd; j++)
					{
						for (int i = 0; i < ndiv; i++)  // Divisions per day
						{
							f = j * ndiv + i;
							results.data.at(m, feature + f) = daily_metrics.at(d1 + j, i);	// Clustering metrics for group g in year y before application of scaling or weighting factors
						}
					}

					// Detemine maximum value visible within the optimization horizon for this group
					for (int vd = 0; vd < visible_days.size(); vd++)
					{
						int doy = (g * inputs.nsimdays + 1) + visible_days[vd];
						for (int i = 0; i < ndiv; i++)  // Divisions per day
							featuremap[key].max_visible.at(m) = fmax(featuremap[key].max_visible.at(m), daily_metrics.at(doy, i));
					}

				}


				//--- Calculate subset of metrics for first/last days of each year (not included in any simulation group)
				int d0[] = { 0,  ngroup * inputs.nsimdays + 1 };  // First day, last days
				for (int p = 0; p < 2; p++)
				{
					d1 = d0[p] + featuremap[key].offset;
					for (int j = 0; j < nd; j++)
					{
						for (int i = 0; i < ndiv; i++)  // Divisions per day
						{
							f = j * ndiv + i;
							if (d1 + j >= 0 && d1 + j < 365)
							{
								if (p == 0)
									results.data_firstday.at(y, feature + f) = daily_metrics.at(d1 + j, i);
								else
									results.data_lastday.at(y, feature + f) = daily_metrics.at(d1 + j, i);
							}
						}
					}

					// Detemine maximum value visible within the optimization horizon for this group
					for (int vd = 0; vd < visible_days.size(); vd++)
					{
						int doy = d0[p] + visible_days[vd];
						if (doy >= 0 && doy < 365)
						{
							for (int i = 0; i < ndiv; i++)  // Divisions per day
							{
								if (p == 0)
									featuremap[key].max_visible_first.at(y) = fmax(featuremap[key].max_visible_first.at(y), daily_metrics.at(doy, i));
								else
									featuremap[key].max_visible_last.at(y) = fmax(featuremap[key].max_visible_last.at(y), daily_metrics.at(doy, i));
							}
						}
					}
				}

				feature += ndiv * nd;	 // Add to count of total data features
			}
		}
	}



	//--- Apply normalization and scaling
	int feature = 0;
	for (int k = 0; k < order.size(); k++)   // Loop over metrics
	{
		std::string key = order[k];
		if (inputs.features[key].weight > 0.0)			 // Only use metrics with nonzero weighting factor
		{
			int ndiv = inputs.features[key].divisions;  // Number of divisions per day for this metric
			int nd = featuremap[key].ndays;  // Number of days associated with this metric

											 //--- Simulation groups
			for (int y = 0; y < inputs.nyears; y++)
			{
				for (int g = 0; g < ngroup; g++)
				{
					int m = y * ngroup + g;

					double norm_visible = 1.0;
					if (key == "price" || key == "price_prev" || key == "price_next")  //Adjust price metrics relative to the maximum value visible within the optimization time horizon
						norm_visible = (featuremap[key].max_visible.at(m) - featuremap[key].minval) / (featuremap[key].maxval - featuremap[key].minval);

					// Normalize and apply weighting factor
					for (int f = 0; f < nd*ndiv; f++)
					{
						results.data.at(m, feature + f) = (results.data.at(m, feature + f) - featuremap[key].minval) / (featuremap[key].maxval - featuremap[key].minval) / norm_visible;
						results.data.at(m, feature + f) *= inputs.features[key].weight;
					}
				}
			}

			//--- First / last days
			for (int y = 0; y < inputs.nyears; y++)
			{
				double norm_visible_first = 1.0;
				double norm_visible_last = 1.0;
				if (key == "price" || key == "price_prev" || key == "price_next")  //Adjust price metrics relative to the maximum value visible within the optimization time horizon
				{
					norm_visible_first = (featuremap[key].max_visible_first.at(y) - featuremap[key].minval) / (featuremap[key].maxval - featuremap[key].minval);
					norm_visible_last = (featuremap[key].max_visible_last.at(y) - featuremap[key].minval) / (featuremap[key].maxval - featuremap[key].minval);
				}

				// Normalize and apply weighting factor
				for (int f = 0; f < nd*ndiv; f++)
				{
					if (results.data_firstday.at(y, feature + f) == results.data_firstday.at(y, feature + f))  // Data point is defined
					{
						results.data_firstday.at(y, feature + f) = (results.data_firstday.at(y, feature + f) - featuremap[key].minval) / (featuremap[key].maxval - featuremap[key].minval) / norm_visible_first;
						results.data_firstday.at(y, feature + f) *= inputs.features[key].weight;
					}

					if (results.data_lastday.at(y, feature + f) == results.data_lastday.at(y, feature + f))
					{
						results.data_lastday.at(y, feature + f) = (results.data_lastday.at(y, feature + f) - featuremap[key].minval) / (featuremap[key].maxval - featuremap[key].minval) / norm_visible_last;
						results.data_lastday.at(y, feature + f) *= inputs.features[key].weight;
					}
				}
			}

			feature += nd * ndiv;
		}
	}

	return;

}