#include "optical_structures.h"

#ifndef _OPTICAL_DEGR
#define _OPTICAL_DEGR

class optical_degradation
{
	bool m_sim_available;

public:
	optical_degradation();

	opt_settings m_settings;
	opt_results m_results;

	void simulate(std::string *results_file_name = 0, std::string *trace_file_name = 0);

	float* get_soiling_schedule(int *length);
	float* get_degradation_schedule(int *length);
	float* get_replacement_schedule(int *length);
	float* get_replacement_totals(int *length);

	int get_number_repairs();
};


#endif
