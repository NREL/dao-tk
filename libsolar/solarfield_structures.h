#ifndef _SF_STRUCTURES_ 
#define _SF_STRUCTURES_

#include <vector>
#include <string>

struct solarfield_settings
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

struct solarfield_heliostat
{
    bool operating;
    double repair_remain;
    int number_failure;

	solarfield_heliostat();
};

struct solarfield_staff
{

    //list of heliostats
    std::vector< solarfield_heliostat * > queue;
    //list of queue length over time
    std::vector< double > queue_length;

    int repairs_made;
    double hours_worked;
    double hours_this_week;
    double hours_today;

	solarfield_staff();
}; 

struct solarfield_results
{
    float *avail_schedule;
    int n_avail_schedule;

    float n_repairs;
    float staff_utilization;

	float heliostat_repair_cost_y1;
	float heliostat_repair_cost;

	solarfield_results(){
        avail_schedule = 0;
    };
    ~solarfield_results()
    {
        if( avail_schedule != 0 )
            delete [] avail_schedule;
    };
};


#endif