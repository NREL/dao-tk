#include <vector>
#include <string>
#include <unordered_map>

#include "plant_struct.h"

cycle_file_settings::cycle_file_settings()
{
	plant_in_state = "plant_state_in";
	plant_out_state = "plant_state_out";
	component_in_state = "component_state_in";
	component_out_state = "component_state_out";
}

cycle_state::cycle_state()
{
	maintenance_interval = 0;
	maintenance_duration = 0;
	downtime_threshold = 0;
	hours_to_maintenance = 0;
	is_online = false;
	is_on_standby = false;
	power_output = 0;
	capacity = 0;
	temp_threshold = 0;
	time_online = 0;
	time_in_standby = 0;
	downtime = 0;
	hot_start_penalty = 0;
	warm_start_penalty = 0;
	cold_start_penalty = 0;
}

simulation_params::simulation_params()
{
	read_periods = 0;
	sim_length = 48;
	steplength = 1.;
	epsilon = 1e-10;
	print_output = false;
	num_scenarios = 1;
	hourly_labor_cost = 50.;
}

cycle_results::cycle_results()
{
	cycle_capacity = {};
	cycle_efficiency = {};
	labor_costs = {};
	avg_cycle_capacity = {};
	avg_cycle_efficiency = {};
	avg_labor_cost = 0.;
	component_status = {};
	plant_status = {};
	failure_event_labels = {};
	failure_events = {};
	period_of_first_failure = 0;
	turbine_efficiency = 1.;
	turbine_capacity = 1.;
}