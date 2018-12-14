#ifndef _STRUCTURES_ 
#define _STRUCTURES_

#include <vector>
#include <string>

struct opt_settings
{
    int n_helio; 

    int n_wash_crews;

    double wash_units_per_hour;
    double hours_per_day;
    double hours_per_week;
    
	bool use_fixed_replacement_threshold;
	bool use_mean_replacement_threshold;
    double replacement_threshold;

    double soil_loss_per_hr;
    double degr_loss_per_hr;
    double degr_accel_per_year;

	double annual_profit_per_kwh;
	double heliostat_refurbish_cost;

    int n_hr_sim;
	int soil_sim_interval;
	int refl_sim_interval;

	int seed;
}; 

struct opt_heliostat
{
    double refl_base;
    double soil_loss;
	double refl_loss_rate;
	double soil_loss_rate;
	double replacement_threshold;

    int age_hours;

    double *soil_history;
    double *refl_history;

	opt_heliostat();
    ~opt_heliostat();
};

struct opt_crew
{
	int start_heliostat;
	int end_heliostat;

    int current_heliostat;
    double carryover_wash_time;

    int replacements_made;
    double hours_this_week;
    double hours_today;

	opt_crew();
}; 

struct opt_results
{
    
    float *soil_schedule;
    float *degr_schedule;
    float *repl_schedule;
    float *repl_total;

    int n_schedule;

    float n_replacements;

	float heliostat_refurbish_cost;
	float heliostat_refurbish_cost_y1;

	float avg_soil;
	float avg_degr;

	opt_results()
	{
        //null pointers
        soil_schedule = 0;
        degr_schedule = 0;
        repl_schedule = 0;
        repl_total = 0;
    };
    ~opt_results()
    {
        if( soil_schedule != 0 )
        {
            delete [] soil_schedule;
            delete [] degr_schedule;
            delete [] repl_schedule;
            delete [] repl_total;
        }
    };
};


#endif