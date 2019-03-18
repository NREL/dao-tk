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

	std::vector<double> get_operating_hours();

	void simulate(bool (*callback)(float prg, const char *msg)=0, std::string *results_file_name = 0);

	double operating_hours(std::vector<double> operating_hours, double t_start, double t_end);
	
	void initialize_results();

};

#endif