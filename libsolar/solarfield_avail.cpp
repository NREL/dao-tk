#include "solarfield_structures.h"
#include "solarfield_avail.h"
#include <iostream>
#include <fstream>

void* solarfield_availability::create_context()
{
    return static_cast<void*>( new Results );
}

void solarfield_availability::run(void* context, int mtf, float repair_min, float repair_max, int n_heliostat, int n_om_staff, float max_hoursperweek_staff, int n_hours_sim, int seed, int n_heliostat_sim )
{
    //mean time to failure for a given heliostat [hours]
    settings S; 
    S.mf = mtf; 
    S.rep_min = repair_min;
    S.rep_max = repair_max;
    S.n_helio = n_heliostat;
    S.n_helio_sim = n_heliostat_sim;
    S.n_om_staff = n_om_staff;
    S.hr_prod = max_hoursperweek_staff;
    S.n_hr_sim = n_hours_sim;
    S.seed = seed;
    
    /*std::ofstream of;
    of.open("log.txt");
    of << S.mf << "\n" << S.rep_min << "\n" << S.rep_max << "\n" <<  S.n_helio << "\n" << S.n_om_staff << "\n" << S.hr_prod << "\n" << S.n_hr_sim;
    of.close();
    return;*/
    Results *R = static_cast<Results*>( context );

    simulate(S, *R);

}

float* solarfield_availability::get_avail_schedule(void* context, int *length)
{
    Results *R = static_cast<Results*>( context );
    *length = R->n_avail_schedule;

    return R->avail_schedule;
}

void solarfield_availability::get_summary_results(void* context, int* n_repairs, float* staff_utilization)
{
    Results *R = static_cast<Results*>( context );
    *n_repairs = R->n_repairs;
    *staff_utilization = R->staff_utilization;
    return;
}
