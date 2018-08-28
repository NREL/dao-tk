#ifndef _CSKYH_
#define _CSKYH_

#include <vector>

#include "solpos00.h"

enum CSKYMODEL { MEINEL, HOTTEL };

struct s_location
{
	double m_lat;		// Latitude
	double m_lon;		// Longitude
	double m_elev;		// Elevation
	int m_tz;			// Time zone correction

	s_location();
	s_location(double lat, double lon, double elev, int tz);
	s_location(std::string weatherfile);  // Location taken from TMY weather file header
};


class clearsky
{
public:

	s_location m_location;			// Location
	std::vector<double> m_csky;		// Approximate clear-sky DNI

	std::vector<double> m_sunrise;	// Time at sunrise for each day of the year (hr)
	std::vector<double> m_sunset;	// Time at sunset for each day of the year (hr)


	clearsky();
	clearsky(s_location &loc);

	void calculate_clearsky(double tstephr = 1.0, unsigned int model = MEINEL, int year = 2017, bool is_start = false);

	void calculate_sunrise_sunset(int year = 2017);

	
};


#endif
