#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "plant_struct.h"

cycle_file_settings::cycle_file_settings()
{
	ampl_dispatch_file = "./dispatch"; //incoming ampl dispatch file
	policy_filename = "./thresholds";  //cap and eff thresholds for shutdown
	rng_state_filename = "./rng";   //current RNG state
	plant_comp_info = "./plantcomps"; //plant composition
	sim_params_filename = "./simparams"; //sim_params data structure info
	day_idx_filename = "./sdk_data";    //single input - int for day_idx
	plant_in_state = "./plant_state";   //cycle_state
	plant_out_state = "./plant_state";
	component_in_state = "./component_state";  //component_state vector
	component_out_state = "./component_state";
	ampl_param_file = "./MxSim";
	failure_file = "./failures";
	num_failures_file = "./num_failures";
	cap_eff_filename = "./capeff";
	output_ampl_file = false;
	output_to_files = false;
	day_idx = 0;
	additional_periods = 0;
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
	thermal_output = 0;
	capacity = 0;
	thermal_capacity = 0;
	temp_threshold = 0;
	time_online = 0;
	time_in_standby = 0;
	downtime = 0;
	hot_start_penalty = 0;
	warm_start_penalty = 0;
	cold_start_penalty = 0;
	salt_pump_switch_time = 0.0;
	boiler_pump_switch_time = 0.0;
	water_pump_switch_time = 0.0;
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
		<< "thermal_output: " << thermal_output << "\n"
		<< "capacity: " << capacity << "\n"
		<< "thermal_capacity: " << thermal_capacity << "\n"
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
	num_annual_starts = 50;
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
		<< "stop_at_first_failure: " << stop_at_first_failure << "\n"
		<< "num_annual_starts: " << num_annual_starts << "\n";
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
	period_of_last_repair = {};
	turbine_efficiency = {};
	turbine_capacity = {};
	avg_turbine_efficiency = 1.;
	avg_turbine_capacity = 1.;
	expected_time_to_failure = 0.;
	expected_starts_to_failure = 0.;
}

