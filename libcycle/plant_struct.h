#ifndef _PLANT_CAPACITY_STRUCT_
#define _PLANT_CAPACITY_STRUCT_

#include <string>
#include <vector>
#include <unordered_map>

#include "component.h"

struct cycle_file_settings
{
	std::string component_in_state;
	std::string component_out_state;
	std::string plant_in_state;
	std::string plant_out_state;
	cycle_file_settings();
};


struct cycle_state
{
	double maintenance_interval;
	double maintenance_duration;
	double downtime_threshold;
	double hours_to_maintenance;
	double power_output;
	bool is_online;
	bool is_on_standby;
	double capacity;
	double temp_threshold;
	double time_online;
	double time_in_standby;
	double downtime;
	double hot_start_penalty;
	double warm_start_penalty;
	double cold_start_penalty;
	cycle_state();
	void print();
};

struct simulation_params
{
	int read_periods;
	int sim_length;
	double steplength;
	double epsilon;
	bool print_output;
	int num_scenarios;
	double hourly_labor_cost;
	bool stop_at_first_failure;
	simulation_params();
	void print();
};

struct cycle_results
{
	std::unordered_map < int, std::vector < double > > cycle_capacity;
	std::unordered_map < int, std::vector < double > > cycle_efficiency;
	std::unordered_map < int, double  > labor_costs;
	std::vector < double > avg_cycle_capacity;
	std::vector < double > avg_cycle_efficiency;
	double avg_labor_cost;
	std::unordered_map<int, std::unordered_map< std::string, ComponentStatus > > component_status;
	std::unordered_map<int, cycle_state >  plant_status;
	std::vector < std::string > failure_event_labels;
	std::unordered_map < std::string, failure_event > failure_events;
	std::unordered_map <int, int> period_of_first_failure;
	double turbine_efficiency;
	double turbine_capacity;
	cycle_results();
};

#endif