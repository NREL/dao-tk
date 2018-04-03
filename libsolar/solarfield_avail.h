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

	float* get_avail_schedule(int *length);

	void get_summary_results(int* n_repairs, float* staff_utilization);
	
	void simulate(bool (*callback)(float prg, const char *msg)=0, std::string *results_file_name = 0);

};

#endif