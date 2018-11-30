#include "optical_structures.h"
#include "wash_opt_structure.h"

#ifndef _OPTICAL_DEGR
#define _OPTICAL_DEGR

class optical_degradation
{
	bool m_sim_available; 

public:
	optical_degradation();

	solar_field_data m_solar_data;
	wash_crew_opt_results m_wc_results;
	opt_settings m_settings;
	opt_results m_results;

	double get_replacement_threshold(double mirror_output, int num_mirrors);

	void simulate(bool(*callback)(float prg, const char *msg)=0, std::string *results_file_name = 0, std::string *trace_file_name = 0);

	float* get_soiling_schedule(int *length);
	float* get_degradation_schedule(int *length);
	float* get_replacement_schedule(int *length);
	float* get_replacement_totals(int *length);

};


#endif
