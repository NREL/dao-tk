#ifndef _SOLARFIELD_AVAIL
#define _SOLARFIELD_AVAIL

#include "solarfield_structures.h"

class solarfield_availability
{

	bool m_sim_available;

public:
	solarfield_availability();

	solarfield_settings m_settings;
	solarfield_results m_results;

	void simulate(bool (*callback)(float prg, const char *msg)=0, std::string *results_file_name = 0);

	double operating_hours(double sunrise, double sunset, double day_start, double day_end, double hod_start, double hod_end);
	
	void initialize_results();

};

#endif