
#include "clearsky.h"
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>


s_location::s_location()
{
	m_lat = m_lon = m_elev = std::numeric_limits<double>::quiet_NaN();
	m_tz = std::numeric_limits<int>::quiet_NaN();
}

s_location::s_location(double lat, double lon, double elev, int tz)
{
	m_lat = lat;
	m_lon = lon;
	m_elev = elev;
	m_tz = tz;
}

s_location::s_location(std::string weatherfile)
{
	m_lat = m_lon = m_elev = std::numeric_limits<double>::quiet_NaN();
	m_tz = std::numeric_limits<int>::quiet_NaN();

	// Read file header
	std::fstream file;
	std::string line, cell;
	std::vector<std::string> header;

	file.open(weatherfile, std::ios::in);
	if (file.is_open())
	{
		std::getline(file, line);
		std::getline(file, line);

		std::stringstream lineStream(line);
		while (std::getline(lineStream, cell, ','))
			header.push_back(cell);
		file.close();
	}

	if (header.size() >= 9)
	{
		m_lat = std::stod(header.at(5));
		m_lon = std::stod(header.at(6));
		m_tz = std::stoi(header.at(7));
		m_elev = std::stod(header.at(8));
	}
}





clearsky::clearsky()
{
	m_location.m_lat = m_location.m_lon = m_location.m_elev = std::numeric_limits<double>::quiet_NaN();
	m_location.m_tz = std::numeric_limits<int>::quiet_NaN();
	m_csky.clear();
	m_sunrise.clear();
	m_sunset.clear();
}

clearsky::clearsky(s_location & loc)
{
	m_location.m_lat = loc.m_lat;
	m_location.m_lon = loc.m_lon;
	m_location.m_elev = loc.m_elev;
	m_location.m_tz = loc.m_tz;
	m_csky.clear();
	m_sunrise.clear();
	m_sunset.clear();
}

void clearsky::calculate_clearsky(double tstephr, unsigned int model, int year, bool is_start)
{
	// Calculate approximate clear sky DNI
	m_csky.clear();

	int nperhr = int(1. / tstephr);
	double tstep_sec = tstephr * 3600.;
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
					pdat->latitude = float(m_location.m_lat);	//[deg] {float} North is positive
					pdat->longitude = float(m_location.m_lon);	//[deg] {float} Degrees east. West is negative
					pdat->timezone = float(m_location.m_tz);		//[hr] {float} Time zone, east pos. west negative. Mountain -7, Central -6, etc..
					pdat->year = year;				//[year] {int} 4-digit year
					pdat->month = m + 1;				//[mo] {int} (1-12)
					pdat->day = d + 1;				//[day] {int} Day of the month
					pdat->daynum = doy;				//[day] {int} Day of the year
					pdat->hour = h + 1;				//[hr] {int} 0-23
					pdat->minute = min;				//[min] {int} 0-59
					pdat->second = sec;				//[sec]	{int} 0-59
					pdat->interval = int(tstep_sec);	//[sec] {int} Measurement interval.  Note: input time is assumed to be at the end of the measurement interval, values are calculated at the midpoint of the interval

					if (is_start)  // TMY files: dni at beginning of interval; Single year: dni at midpoint of interval
						pdat->interval = 2 * int(tstep_sec);	// double the interval so that the midpoint is at the beginning of the TMY interval

					long retcode = 0;
					retcode = S_solpos(pdat);
					S_decode(retcode, pdat);

					double zen = sp.zenetr;   // [deg]

											  // Estimate clear sky DNI
					double cskypt, s0, beta, alt;
					beta = 2 * 3.14159*(doy / 365.);
					s0 = 1367 * (1.00011 + 0.034221*cos(beta) + 0.00128*sin(beta) + 0.000719*cos(2 * beta) + 0.000077*sin(2 * beta));
					alt = m_location.m_elev / 1000.;   // convert to km

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

					m_csky.push_back(cskypt);
				}
			}
			doy += 1;
		}

	}

	return;
}


void clearsky::calculate_sunrise_sunset(int year)
{
	// Calculate daily sunrise the sunset times
	m_sunrise.clear();
	m_sunset.clear();

	int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int doy = 1;
	for (int m = 0; m < 12; m++)
	{
		for (int d = 0; d < days[m]; d++)
		{
			struct posdata sp, *pdat;
			pdat = &sp;
			S_init(pdat);
			pdat->latitude = float(m_location.m_lat);	//[deg] {float} North is positive
			pdat->longitude = float(m_location.m_lon);	//[deg] {float} Degrees east. West is negative
			pdat->timezone = float(m_location.m_tz);		//[hr] {float} Time zone, east pos. west negative. Mountain -7, Central -6, etc..
			pdat->year = year;				//[year] {int} 4-digit year
			pdat->month = m + 1;			//[mo] {int} (1-12)
			pdat->day = d + 1;				//[day] {int} Day of the month
			pdat->daynum = doy;				//[day] {int} Day of the year
			pdat->hour = 0;				//[hr] {int} 0-23
			pdat->minute = 0;				//[min] {int} 0-59
			pdat->second = 0;				//[sec]	{int} 0-59
			pdat->interval = 3600;	//[sec] {int} Measurement interval.  Note: input time is assumed to be at the end of the measurement interval, values are calculated at the midpoint of the interval

			long retcode = 0;
			retcode = S_solpos(pdat);
			S_decode(retcode, pdat);

			m_sunrise.push_back(sp.sretr / 60.);
			m_sunset.push_back(sp.ssetr / 60.);

			doy += 1;
		}
	}

	return;

}
