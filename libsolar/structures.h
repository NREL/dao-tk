#ifndef _STRUCTURES_ 
#define _STRUCTURES_

#include <vector>
#include <string>

struct settings
{
    int mf;
    double rep_min;
    double rep_max;
    int n_helio;
    int n_helio_sim;
    int n_om_staff;
    double hr_prod;
    int n_hr_sim;
    int seed;
}; 

struct Heliostat
{
    bool operating;
    double repair_remain;
    int number_failure;

    Heliostat();
};

struct Staff
{

    //list of heliostats
    std::vector< Heliostat * > queue;
    //list of queue length over time
    std::vector< double > queue_length;

    int repairs_made;
    double hours_worked;
    double hours_this_week;
    double hours_today;

    Staff();
}; 

struct Results
{
    float *avail_schedule;
    int n_avail_schedule;

    float n_repairs;
    float staff_utilization;

    Results(){
        avail_schedule = 0;
    };
    ~Results()
    {
        if( avail_schedule != 0 )
            delete [] avail_schedule;
    };
};

extern void simulate(settings &s, Results &R,  std::string *results_file_name = 0);

#endif