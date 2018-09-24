#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "plant_struct.h"

cycle_file_settings::cycle_file_settings()
{
	plant_in_state = "./plant_state_in";
	plant_out_state = "./plant_state_out";
	component_in_state = "./component_state_in";
	component_out_state = "./component_state_out";
	ampl_param_file = "./MxSim";
	output_ampl_file = false;
	day_idx = 0;
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
	hot_start_penalty = 0;
}

void cycle_state::print()
{
	std::cerr << "maintenance_interval: " << maintenance_interval << "\n"
		<< "maintenance_duration: " << maintenance_duration << "\n"
		<< "downtime_threshold: " << downtime_threshold << "\n"
		<< "hours_to_maintenance: " << hours_to_maintenance << "\n"
		<< "is_online: " << is_online << "\n"
		<< "is_on_standby: " << is_on_standby << "\n"
		<< "power_output: " << power_output << "\n"
		<< "capacity: " << capacity << "\n"
		<< "temp_threshold: " << temp_threshold << "\n"
		<< "time_online: " << time_online << "\n"
		<< "time_in_standby: " << time_in_standby << "\n"
		<< "downtime: " << downtime << "\n"
		<< "hot_start_penalty: " << hot_start_penalty << "\n"
		<< "warm_start_penalty: " << warm_start_penalty << "\n"
		<< "cold_start_penalty: " << cold_start_penalty << "\n";
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
	stop_at_first_repair = false;
	stop_at_first_failure = false;
}

void simulation_params::print()
{
	std::cerr << "read_periods: " << read_periods << "\n"
		<< "sim_length: " << sim_length << "\n"
		<< "steplength: " << steplength << "\n"
		<< "epsilon: " << epsilon << "\n"
		<< "print_output: " << print_output << "\n"
		<< "num_scenarios: " << num_scenarios << "\n"
		<< "hourly_labor_cost: " << hourly_labor_cost << "\n"
		<< "stop_at_first_repair: " << stop_at_first_repair << "\n"
		<< "stop_at_first_failure: " << stop_at_first_failure << "\n";
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
	period_of_last_failure = {};
	turbine_efficiency = {};
	turbine_capacity = {};
	avg_turbine_efficiency = 1.;
	avg_turbine_capacity = 1.;
	expected_time_to_failure = 0.;
	expected_starts_to_failure = 0.;
}

