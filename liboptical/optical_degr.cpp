#include "structures.h"
#include "optical_degr.h"
#include <iostream>
#include <fstream>

void* optical_degradation::create_context()
{
    return static_cast<void*>( new Results );
}

void optical_degradation::run(void* context,
                    int n_hours_sim, 
                    int n_wash_crews, 
                    int n_heliostat,  
                    float degr_per_hour, 
                    float degr_accel_per_year, 
                    float degr_replace_limit, 
                    float soil_per_hour, 
                    float wash_units_per_hour, 
                    float max_hoursperweek_crew, 
                    float max_hoursperday_crew,
                    int seed
                   )
{
    //mean time to failure for a given heliostat [hours]
    settings S; 
    S.degr_accel_per_year = degr_accel_per_year;
    S.degr_loss_per_hr = degr_per_hour;
    S.hours_per_week = max_hoursperweek_crew;
    S.hours_per_day = max_hoursperday_crew;
    S.n_helio = n_heliostat;
    S.n_hr_sim = n_hours_sim;
    S.n_wash_crews = n_wash_crews;
    S.replacement_threshold = degr_replace_limit;
    S.soil_loss_per_hr = soil_per_hour;
    S.wash_units_per_hour = wash_units_per_hour;

    /*std::ofstream of;
    of.open("log.txt");
    of << S.mf << "\n" << S.rep_min << "\n" << S.rep_max << "\n" <<  S.n_helio << "\n" << S.n_om_staff << "\n" << S.hr_prod << "\n" << S.n_hr_sim;
    of.close();
    return;*/
    Results *R = static_cast<Results*>( context );

    simulate(S, *R, seed);

}

float* optical_degradation::get_soiling_schedule(void* context, int *length)
{
    Results *R = static_cast<Results*>( context );
    *length = R->n_schedule;

    return R->soil_schedule;
}

float* optical_degradation::get_degradation_schedule(void* context, int *length)
{
    Results *R = static_cast<Results*>( context );
    *length = R->n_schedule;

    return R->degr_schedule;
}

float* optical_degradation::get_replacement_schedule(void* context, int *length)
{
    Results *R = static_cast<Results*>( context );
    *length = R->n_schedule;

    return R->repl_schedule;
}

float* optical_degradation::get_replacement_totals(void* context, int *length)
{
    Results *R = static_cast<Results*>( context );
    *length = R->n_schedule;

    return R->repl_total;
}

int optical_degradation::get_number_repairs(void* context)
{
    Results *R = static_cast<Results*>( context );
    return R->n_replacements;
}
