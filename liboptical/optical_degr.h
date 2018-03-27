#include "optical_structures.h"

#ifndef _OPTICAL_DEGR
#define _OPTICAL_DEGR

class optical_degradation
{

public:
	void* create_context();

	void run(void* context, 
		int n_hours_sim, int n_wash_crews, int n_heliostat,  
		float degr_per_hour, float degr_accel_per_year, float degr_replace_limit, 
		float soil_per_hour, float wash_units_per_hour, 
		float max_hoursperweek_crew, float max_hoursperday_crew, 
		int seed);

	float* get_soiling_schedule(void* context, int *length);
	float* get_degradation_schedule(void* context, int *length);
	float* get_replacement_schedule(void* context, int *length);
	float* get_replacement_totals(void* context, int *length);

	int get_number_repairs(void* context);
};


#endif
