#ifndef _STRUCTURES_ 
#define _STRUCTURES_

#include <vector>
#include <string>

struct settings
{
    int n_helio;

    int n_wash_crews;

    double wash_units_per_hour;
    double hours_per_day;
    double hours_per_week;
    
    double replacement_threshold;

    double soil_loss_per_hr;
    double degr_loss_per_hr;
    double degr_accel_per_year;

    int n_hr_sim;
}; 

struct Heliostat
{
    double refl_base;
    double soil_loss;

    int age_hours;

    double *soil_history;
    double *refl_history;

    Heliostat();
    ~Heliostat();
};

struct Crew
{

    int current_heliostat;
    double carryover_wash_time;

    int replacements_made;
    //double hours_worked;
    double hours_this_week;
    double hours_today;

    Crew();
}; 

struct Results
{
    
    float *soil_schedule;
    float *degr_schedule;
    float *repl_schedule;
    float *repl_total;

    int n_schedule;

    int n_replacements;

    Results(){
        //null pointers
        soil_schedule = 0;
        degr_schedule = 0;
        repl_schedule = 0;
        repl_total = 0;
    };
    ~Results()
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

extern void simulate(settings &s, Results &R,  int seed, std::string *results_file_name = 0, std::string *trace_file_name = 0);

#endif