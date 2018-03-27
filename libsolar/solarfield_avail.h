#ifndef _SOLARFIELD_AVAIL
#define _SOLARFIELD_AVAIL

#include "solarfield_structures.h"

class solarfield_availability
{
	solarfield_availability() {};

public:
	void* create_context();

	void run(void* context, int mtf, float repair_min, float repair_max, int n_heliostat, int n_om_staff, float max_hoursperweek_staff, int n_hours_sim, int seed=123, int n_heliostat_sim=1000);

	float* get_avail_schedule(void* context, int *length);

	void get_summary_results(void* context, int* n_repairs, float* staff_utilization);
};

#endif