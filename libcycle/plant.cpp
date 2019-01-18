#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include "plant.h"


PowerCycle::PowerCycle()
{
	SetSimulationParameters();
	SetPlantAttributes();
	GeneratePlantComponents();
}

void PowerCycle::Initialize(double age, int scen_idx)
{
	/*
	Initializes the plant by generating random lifetimes/probabilities for all 
	components and failure modes, and ages the plant for a single scenario.
	*/
	m_life_gen->assignStates(3*scen_idx);
	m_repair_gen->assignStates(3*scen_idx+1);
	m_binary_gen->assignStates(3*scen_idx+2);
	InitializeCyclingDists();
	GeneratePlantCyclingPenalties();
	ResetPlant();
	m_cycle_capacity = 1.;
	m_cycle_efficiency = 1.;
	for (int i = 0; i < m_sim_params.num_scenarios; i++)
	{
		m_results.period_of_last_failure[i] = -1;
		m_results.period_of_last_repair[i] = -1;
		m_results.cycle_capacity[i] = std::vector<double>(m_sim_params.sim_length,1.);
		m_results.cycle_efficiency[i] = std::vector<double>(m_sim_params.sim_length, 1.);
	}
	AgePlant(age);
	StoreCycleState();
}

void PowerCycle::InitializeCyclingDists()
{
	/*
	Sets distributions from which cycling penalties are generated.
	*/
	m_hs_dist = BoundedJohnsonDist(1.044471, 0.256348, 2.0685E-3, 2.5815E-2, "BoundedJohnson");
	m_ws_dist = BoundedJohnsonDist(2.220435, 0.623145, 1.457E-3, 8.865E-2, "BoundedJohnson");
	m_cs_dist = BoundedJohnsonDist(0.469391, 0.581813, 1.8455E-3, 1.1845E-2, "BoundedJohnson");
}

void PowerCycle::AssignGenerators( 
	WELLFiveTwelve *gen1,
	WELLFiveTwelve *gen2, 
	WELLFiveTwelve *gen3
)
{
	/*
	Assigns an RNG object to the plant, which is used to generate component
	lifetimes, failure probabilities, tests for binary failures, and repair times.
	Here, we use the WELL512 implementation by Panneton et al. (2006).
	*/
    m_life_gen = gen1;
	m_repair_gen = gen2;
	m_binary_gen = gen3;
}

void PowerCycle::GeneratePlantCyclingPenalties()
{
	/*
	Generates perfectly correlated cycling penalties
	for hot, warm, and cold starts.  A single percentile
	is used for all three distributions to prevent the 
	simulation model from penalizing hot starts more than
	cold starts.
	*/
	double unif = m_life_gen->getVariate();
	SetHotStartPenalty(m_hs_dist.GetInverseCDF(unif));
	SetWarmStartPenalty(m_ws_dist.GetInverseCDF(unif));
	SetColdStartPenalty(m_cs_dist.GetInverseCDF(unif));
}

void PowerCycle::SetHotStartPenalty(double pen)
{
	/* mutator for plant's hot-start penalty. */
	m_current_cycle_state.hot_start_penalty = pen;
}

void PowerCycle::SetWarmStartPenalty(double pen)
{
	/* mutator for plant's warm-start penalty. */
	m_current_cycle_state.warm_start_penalty = pen;
}

void PowerCycle::SetColdStartPenalty(double pen)
{
	/* mutator for plant's cold-start penalty. */
	m_current_cycle_state.cold_start_penalty = pen;
}

void PowerCycle::SetSimulationParameters( 
	int read_periods,
	int sim_length,
	double steplength,
	double epsilon,
	bool print_output,
	int num_scenarios,
	double hourly_labor_cost,
	bool stop_at_first_repair,
	bool stop_at_first_failure
)
{   /*
	Sets all simulation parameters for the power cycle
	model.  This is done with default values at initialization,
	and may also be called in C() in the DAO-tk solution.
    
	read_periods -- number of read-only periods (obsolete)
	sim_length -- length of simulation, in time periods
	steplength -- length of single time period (hours)
	epsilon -- threshold for logical differences between floating-point numbers
	print_output -- true if outputting failure events to a log
	num_scenarios -- number of replicates called by Simulate()
	hourly_labor_cost -- assumed cost/hour for repair labor ($)
	stop_at_first_repair -- terminates each replicate at first new failure (not
		read in from failure events) if true
	stop_at_first_repair -- terminates each replicate at first new failure (not
		read in from failure events) if true
	*/
    m_sim_params.read_periods = read_periods;
    m_sim_params.sim_length = sim_length;
	m_sim_params.steplength = steplength;
	m_sim_params.epsilon = epsilon;
    m_sim_params.print_output = print_output;
	m_sim_params.num_scenarios = num_scenarios;
	m_sim_params.hourly_labor_cost = hourly_labor_cost;
	m_sim_params.stop_at_first_repair = stop_at_first_repair;
	m_sim_params.stop_at_first_failure = stop_at_first_failure;
	m_current_scenario = 0;
	for (int i = 0; i < m_sim_params.num_scenarios; i++)
	{
		m_results.period_of_last_failure[i] = -1;
		m_results.period_of_last_repair[i] = -1;
		m_results.cycle_capacity[i] = std::vector<double>(m_sim_params.sim_length, 1.);
		m_results.cycle_efficiency[i] = std::vector<double>(m_sim_params.sim_length, 1.);
	}
}

void PowerCycle::SetCondenserEfficienciesCold(std::vector<double> eff_cold)
{
	/*
	Mutator for condenser efficiencies when ambient temperature is lower than the
	threshold.
	*/
	int num_streams = 0;
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (m_components.at(i).GetType() == "Condenser train")
			num_streams++;
	}
	if (num_streams != m_num_condenser_trains)
	{
		throw std::runtime_error("condenser trains not created correctly");
	}
	if ((int)eff_cold.size() != num_streams + 1)
		throw std::runtime_error("efficiencies must be equal to one plus number of streams"); 
	m_condenser_efficiencies_cold = eff_cold;
}

void PowerCycle::SetCondenserEfficienciesHot(std::vector<double> eff_hot)
{
	/*
	Mutator for condenser efficiencies when ambient temperature is lower than the
	threshold.
	*/
	int num_streams = 0;
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (m_components.at(i).GetType() == "Condenser train")
			num_streams++;
	}
	if ((int)eff_hot.size() != num_streams+1)
		throw std::runtime_error("efficiencies must be equal to one plus number of streams");
	m_condenser_efficiencies_hot = eff_hot;
}

void PowerCycle::ClearComponents()
{
	/*
	Clears components, component index references, and any stored component status.
	*/
	m_components.clear();
	m_condenser_idx.clear();
	m_turbine_idx.clear();
	m_sst_idx.clear();
	m_salt_pump_idx.clear();
	m_start_component_status.clear();
	m_num_condenser_trains = 0;
	m_fans_per_condenser_train = 0;
	m_radiators_per_condenser_train = 0;
	m_num_feedwater_heaters = 0;
	m_num_salt_pumps = 0;
	m_num_salt_pumps_required = 0;
	m_num_water_pumps = 0;
	m_num_water_pumps_required = 0;
	m_num_boiler_pumps = 0;
	m_num_boiler_pumps_required = 0;
	m_num_salt_steam_trains = 0;
	m_num_turbines = 0;
	m_condenser_efficiencies_cold.clear();
	m_condenser_efficiencies_hot.clear();
}

void PowerCycle::ReadComponentStatus(
	std::unordered_map< std::string, ComponentStatus > dstat
)
{
	/*
	Mutator for component status for all components in the plant.
	Used at initialization.
	*/
	m_start_component_status = dstat;
}

void PowerCycle::ClearComponentStatus()
{
	/* clears stored status of all components. */
	m_start_component_status.clear();
}

void PowerCycle::ReadCycleStateFromResults()
{
	/*
	Reads cycle and component state from results stored in memory from previous run.
	*/
	m_current_cycle_state = m_results.plant_status[m_current_scenario];
	m_begin_cycle_state = m_results.plant_status[m_current_scenario];
	m_start_component_status = m_results.component_status[m_current_scenario];
	SetStartComponentStatus();

	m_life_gen->assignStates(3 * m_current_scenario);
	m_repair_gen->assignStates(3 * m_current_scenario + 1);
	m_binary_gen->assignStates(3 * m_current_scenario + 2);

	m_failure_events = m_results.failure_events[m_current_scenario];
	m_failure_event_labels = m_results.failure_event_labels[m_current_scenario];
}

void PowerCycle::SetStartComponentStatus()
{
	/*
	Sets the status of all components to what is stored in
	m_start_component_status. Any components not stored in the status dictionary
	are currently skipped.
	*/
	for (std::vector< Component >::iterator it = m_components.begin(); 
			it != m_components.end(); it++)
	{
		if (m_start_component_status.find(it->GetName()) != 
			m_start_component_status.end())
		{
			it->ReadStatus(m_start_component_status.at(it->GetName()));
			for (int i = 0; i < it->GetFailureTypes().size(); i++)
			{
				it->SetFailLifeOrProb(i, m_start_component_status.at(it->GetName()).lifetimes.at(i));
			}
		}
		/*
		else
		{
			it->GenerateInitialLifesAndProbs(*m_gen);
		}
		*/
	}
}

void PowerCycle::StoreComponentState()
{
	/*
	Stores the current state of each component to the starting 
	component state.  Done either right after the initialization
	or after a simulation run generates no new failures.
	*/
	for (
		std::vector< Component >::iterator it = m_components.begin();
		it != m_components.end();
		it++
		)
	{
		m_start_component_status[it->GetName()] = it->GetState();
	}
}

void PowerCycle::StorePlantParamsState()
{
	m_begin_cycle_state.capacity = m_current_cycle_state.capacity*1.0;
	m_begin_cycle_state.cold_start_penalty = m_current_cycle_state.cold_start_penalty*1.0;
	m_begin_cycle_state.warm_start_penalty = m_current_cycle_state.warm_start_penalty*1.0;
	m_begin_cycle_state.hot_start_penalty = m_current_cycle_state.hot_start_penalty*1.0;
	m_begin_cycle_state.downtime = m_current_cycle_state.downtime*1.0;
	m_begin_cycle_state.downtime_threshold = m_current_cycle_state.downtime_threshold*1.0;
	m_begin_cycle_state.is_online = m_current_cycle_state.is_online && true;
	m_begin_cycle_state.is_on_standby = m_current_cycle_state.is_on_standby && true;
	m_begin_cycle_state.maintenance_duration = m_current_cycle_state.maintenance_duration*1.0;
	m_begin_cycle_state.maintenance_interval = m_current_cycle_state.maintenance_interval*1.0;
	m_begin_cycle_state.hours_to_maintenance = m_current_cycle_state.hours_to_maintenance*1.0;
	m_begin_cycle_state.temp_threshold = m_current_cycle_state.temp_threshold*1.0;
	m_begin_cycle_state.time_in_standby = m_current_cycle_state.time_in_standby*1.0;
	m_begin_cycle_state.time_online = m_current_cycle_state.time_online*1.0;
	m_begin_cycle_state.power_output = m_current_cycle_state.power_output*1.0;
	m_begin_cycle_state.thermal_output = m_current_cycle_state.thermal_output*1.0;
	m_begin_cycle_state.salt_pump_switch_time = m_current_cycle_state.salt_pump_switch_time*1.0;
	m_begin_cycle_state.boiler_pump_switch_time = m_current_cycle_state.boiler_pump_switch_time*1.0;
	m_begin_cycle_state.water_pump_switch_time = m_current_cycle_state.water_pump_switch_time*1.0;
}

void PowerCycle::StoreCycleState()
{
	/* stores a copy of component, plant and RNG engine status. */
	StoreComponentState();
	StorePlantParamsState();

	m_life_gen->saveStates(3 * m_current_scenario);
	m_repair_gen->saveStates(3 * m_current_scenario + 1);
	m_binary_gen->saveStates(3 * m_current_scenario + 2);
}



void PowerCycle::RecordFinalState()
{
	/* 
	Assigns current power cycle state to the beginning component state.  
	Done if no failures are found in the run.
	*/
	m_results.plant_status[m_current_scenario] = m_current_cycle_state;
	m_results.component_status[m_current_scenario] = GetComponentStates();

	m_life_gen->saveStates(3 * m_current_scenario);
	m_repair_gen->saveStates(3 * m_current_scenario + 1);
	m_binary_gen->saveStates(3 * m_current_scenario + 2);
}

void PowerCycle::RevertToStartState(bool reset_rng)
{
	/*
	Assigns beginning power cycle state to the current state.
	Done if any component failures are found in the run, or if the 
	capacity or efficiency change from the prior run.
	*/
	m_current_cycle_state.capacity = m_begin_cycle_state.capacity*1.0;
	m_current_cycle_state.cold_start_penalty = m_begin_cycle_state.cold_start_penalty*1.0;
	m_current_cycle_state.warm_start_penalty = m_begin_cycle_state.warm_start_penalty*1.0;
	m_current_cycle_state.hot_start_penalty = m_begin_cycle_state.hot_start_penalty*1.0;
	m_current_cycle_state.downtime = m_begin_cycle_state.downtime * 1.0;
	m_current_cycle_state.downtime_threshold = m_begin_cycle_state.downtime_threshold * 1;
	m_current_cycle_state.is_online = m_begin_cycle_state.is_online && true;
	m_current_cycle_state.is_on_standby = m_begin_cycle_state.is_on_standby && true;
	m_current_cycle_state.maintenance_duration = m_begin_cycle_state.maintenance_duration*1.0;
	m_current_cycle_state.maintenance_interval = m_begin_cycle_state.maintenance_interval*1.0;
	m_current_cycle_state.hours_to_maintenance = m_begin_cycle_state.hours_to_maintenance*1.0;
	m_current_cycle_state.temp_threshold = m_begin_cycle_state.temp_threshold*1.0;
	m_current_cycle_state.time_in_standby = m_begin_cycle_state.time_in_standby*1.0;
	m_current_cycle_state.time_online = m_begin_cycle_state.time_online*1.0;
	m_current_cycle_state.power_output = m_begin_cycle_state.power_output*1.0;
	m_current_cycle_state.thermal_output = m_begin_cycle_state.thermal_output*1.0;
	m_current_cycle_state.salt_pump_switch_time = m_begin_cycle_state.salt_pump_switch_time*1.0;
	m_current_cycle_state.boiler_pump_switch_time = m_begin_cycle_state.boiler_pump_switch_time*1.0;
	m_current_cycle_state.water_pump_switch_time = m_begin_cycle_state.water_pump_switch_time*1.0;

	SetStartComponentStatus();
	if (reset_rng)
	{
		m_life_gen->assignStates(3 * m_current_scenario);
		m_repair_gen->assignStates(3 * m_current_scenario + 1);
		m_binary_gen->assignStates(3 * m_current_scenario + 2);
	}
		
}

void PowerCycle::WriteStateToFiles()
{
	/*
	Writes the current state of the plant to output files, one for all components and one for the plant.
	*/
	WritePlantLayoutFile();
	WriteComponentFile();
	WritePlantStateFile();
	WriteSimParamsFile();
	m_life_gen->WriteRNGStateFile(
		m_file_settings.rng_state_filename
		//+std::to_string(m_current_scenario)
		+"l.csv",
		3 * m_current_scenario
		);
	m_repair_gen->WriteRNGStateFile(
		m_file_settings.rng_state_filename
		//+std::to_string(m_current_scenario)
		+ "r.csv",
		3 * m_current_scenario + 1
	);
	m_binary_gen->WriteRNGStateFile(
		m_file_settings.rng_state_filename
		//+std::to_string(m_current_scenario)
		+ "b.csv",
		3 * m_current_scenario + 2
	);
	WriteCapEffFile();
}

void PowerCycle::WritePlantLayoutFile()
{ 
	std::string filename = (
		m_file_settings.plant_comp_info +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ofstream ofile;
	ofile.open(filename);
	ofile << "num_condener_trains, fans_per_train, radiators_per_train, "
		<< "num_salt_steam_trains, num_fwh, num_salt_pumps, "
		<< "num_salt_pumps_required, num_water_pumps, num_water_pumps_required, "
		<< "num_boiler_pumps, num_boiler_pumps_required, num_turbines\n";
	ofile << m_num_condenser_trains << ","
		<< m_fans_per_condenser_train << ","
		<< m_radiators_per_condenser_train << ","
		<< m_num_salt_steam_trains << ","
		<< m_num_feedwater_heaters << ","
		<< m_num_salt_pumps << ","
		<< m_num_salt_pumps_required << ","
		<< m_num_water_pumps << "," 
		<< m_num_water_pumps_required << ","
		<< m_num_boiler_pumps << ","
		<< m_num_boiler_pumps_required << ","
		<< m_num_turbines << "\n";
	for (size_t i = 0; i < m_condenser_efficiencies_cold.size()-1; i++)
	{
		ofile << m_condenser_efficiencies_cold.at(i) << ",";
	}
	ofile << m_condenser_efficiencies_cold.at(m_condenser_efficiencies_cold.size() - 1) << "\n";
	for (size_t i = 0; i < m_condenser_efficiencies_hot.size()-1; i++)
	{
		ofile << m_condenser_efficiencies_hot.at(i) << ",";
	}
	ofile << m_condenser_efficiencies_hot.at(m_condenser_efficiencies_hot.size() - 1) << "\n";
	ofile.close();
}

void PowerCycle::WriteComponentFile()
{
	std::string component_filename = (
		m_file_settings.component_out_state +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ofstream cfile;
	std::unordered_map <std::string, ComponentStatus> component_status;
	if (m_results.period_of_last_failure[m_current_scenario] == -1 && 
		m_results.period_of_last_repair[m_current_scenario] == -1)
		component_status = GetComponentStates();
	else
		component_status = m_start_component_status;
	cfile.open(component_filename);
	cfile << "name,hazard_rate,downtime_remaining,repair_event_time,lifesprobs\n";
	ComponentStatus status;
	for (Component c : GetComponents())
	{
		status = component_status[c.GetName()];
		cfile << c.GetName() << "," << status.hazard_rate << "," << status.downtime_remaining << "," << status.repair_event_time;
		for (double d : status.lifetimes)
		{
			cfile << "," << d;
		}
		cfile << "\n";
	}
}

void PowerCycle::WritePlantStateFile()
{
	std::string plant_filename = (
		m_file_settings.plant_out_state +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ofstream pfile;
	pfile.open(plant_filename);
	pfile << "capacity,cs_penalty,ws_penalty,hs_penalty,downtime,dt_threshold,is_on,is_standby,"
		<< "mx_dur,mx_int,hrs_to_mx,temp_threshold,time_in_standby,time_online,cycle_output,"
		<< "salt_switch_time,boiler_switch_time,water_switch_time\n";
	pfile << m_current_cycle_state.capacity << "," << m_current_cycle_state.cold_start_penalty << ","
		<< m_current_cycle_state.warm_start_penalty << "," << m_current_cycle_state.hot_start_penalty << ","
		<< m_current_cycle_state.downtime << "," << m_current_cycle_state.downtime_threshold << ","
		<< m_current_cycle_state.is_online << "," << m_current_cycle_state.is_on_standby << ","
		<< m_current_cycle_state.maintenance_duration << "," << m_current_cycle_state.maintenance_interval << ","
		<< m_current_cycle_state.hours_to_maintenance << "," << m_current_cycle_state.temp_threshold << ","
		<< m_current_cycle_state.time_in_standby << "," << m_current_cycle_state.time_online << ","
		<< m_current_cycle_state.power_output << "," << m_current_cycle_state.thermal_output << ","
		<< m_current_cycle_state.salt_pump_switch_time << "," << m_current_cycle_state.boiler_pump_switch_time << ","
		<< m_current_cycle_state.water_pump_switch_time << "\n";
	pfile.close();
}

void PowerCycle::WriteSimParamsFile()
{
	/*
	int read_periods = 0,
		int sim_length = 48,
		double steplength = 1,
		double epsilon = 1.E-10,
		bool print_output = false,
		int num_scenarios = 1,
		double hourly_labor_cost = 50.,
		bool stop_at_first_repair = false,
		bool stop_at_first_failure = false
	*/
	std::string rng_filename = (
		m_file_settings.sim_params_filename +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ofstream pfile;
	pfile.open(rng_filename);
	pfile << "read_periods,sim_length,steplength,epsilon,print_output,"
		<< "num_scenarios,hourly_labor_cost,stop_at_first_repair,"
		<< "stop_at_first_failure\n";
	pfile << m_sim_params.read_periods << ","
		<< m_sim_params.sim_length << ","
		<< m_sim_params.steplength << ","
		<< m_sim_params.epsilon << ","
		<< m_sim_params.print_output << ","
		<< m_sim_params.num_scenarios << ","
		<< m_sim_params.hourly_labor_cost << ","
		<< m_sim_params.stop_at_first_repair << ","
		<< m_sim_params.stop_at_first_failure << "\n";
	pfile.close();
}

void PowerCycle::WriteFailuresFile()
{
	std::string failure_filename = (
		m_file_settings.failure_file +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ofstream ofile;
	if (m_file_settings.day_idx == 0)
	{
		ofile.open(failure_filename);
	}
	else
	{
		ofile.open(failure_filename, std::ios::app);
	}
	for (size_t i = 0; i < m_failure_event_labels.size(); i++)
	{
		ofile << m_failure_event_labels.at(i) << ","
			<< m_failure_events[m_failure_event_labels.at(i)].time << ","
			<< m_failure_events[m_failure_event_labels.at(i)].component << ","
			<< m_failure_events[m_failure_event_labels.at(i)].fail_idx << ","
			<< m_failure_events[m_failure_event_labels.at(i)].duration << ","
			<< m_failure_events[m_failure_event_labels.at(i)].labor << ","
			<< m_failure_events[m_failure_event_labels.at(i)].new_life << ","
			<< m_failure_events[m_failure_event_labels.at(i)].scen_index
			<< "\n";
	}
	ofile.close();
}


void PowerCycle::WriteAMPLParams(int extra_periods)
{
	/*
	Writes the cycle efficiency and capacity over time, as well as
	the period of the final repair, to file.
	*/
	std::string filename = (
		m_file_settings.ampl_param_file
		//+ m_file_settings.day_idx 
		//+ ".dat"
		);
/* 	if (m_current_scenario > 0)
	{
		filename += std::to_string(m_current_scenario);
		filename += "_";
	}
	filename += std::to_string(m_file_settings.day_idx); */
	filename += ".dat";
	std::ofstream outfile;
	outfile.open(filename);
	int period = -1;
	if (m_sim_params.stop_at_first_failure &&
		!m_sim_params.stop_at_first_repair && NewFailureOccurred())
		period = m_results.period_of_last_failure[m_current_scenario] + 1;
	else if (!m_sim_params.stop_at_first_failure &&
		m_sim_params.stop_at_first_repair && NewRepairOccurred())
		period = m_results.period_of_last_repair[m_current_scenario] + 1;
	else if (
		m_sim_params.stop_at_first_failure &&
		m_sim_params.stop_at_first_repair &&
		(NewRepairOccurred() || NewFailureOccurred())
		)
		period = std::max(m_results.period_of_last_repair[m_current_scenario], m_results.period_of_last_failure[m_current_scenario]) + 1;
	else  // here, either no failures and no repairs occurred, or settings don't require stoppage at a failure or repair
		period = std::max(m_results.period_of_last_repair[m_current_scenario], m_results.period_of_last_failure[m_current_scenario]) + 1;
	outfile << "param last_fixed_period := " << period << ";\n\n";
	outfile << "param Feff := \n";
	for (int i = 0; i < m_sim_params.sim_length; i++)
	{
		outfile << (i + 1) << "  " << m_results.cycle_efficiency[m_current_scenario].at(i) << "\n";
	}
	for (
		int i = m_sim_params.sim_length;
		i < m_sim_params.sim_length + extra_periods;
		i++
		)
	{
		outfile << (i + 1) << "  " << m_results.cycle_efficiency[m_current_scenario].at(m_sim_params.sim_length-1) << "\n";
	}
	outfile << ";\n\n";
	outfile << "param Fcap := \n";
	for (int i = 0; i < m_sim_params.sim_length; i++)
	{
		outfile << (i + 1) << "  " << m_results.cycle_capacity[m_current_scenario].at(i) << "\n";
	}
	for (
		int i = m_sim_params.sim_length;
		i < m_sim_params.sim_length + extra_periods;
		i++
		)
	{
		outfile << (i + 1) << "  " << m_results.cycle_capacity[m_current_scenario].at(m_sim_params.sim_length - 1) << "\n";
	}
	outfile << ";\n\n";
	/*
	outfile << "param x_fixed := \n";
	double x;
	for (int i = 0; i < m_sim_params.sim_length; i++)
	{
		x = m_current_cycle_state.thermal_capacity * m_results.cycle_capacity[m_current_scenario].at(i);
		outfile << (i + 1) << "  " 
			<< std::min(x, m_dispatch["thermal_power"].at(i))
			<< "\n";
	}
	for (
		int i = m_sim_params.sim_length;
		i < m_sim_params.sim_length + extra_periods;
		i++
		)
	{
		outfile << (i + 1) << "  " << "1.0" << "\n";
	}
	outfile << ";\n\n";


	outfile.close();
	*/
}

void PowerCycle::WriteAMPLParamsToDefault()
{
	/*
	//create empty MxSim.dat file so that dispatch defaults are use (i.e., param last_fixed_period := -1, Fcap = Feff = 1)
		std::string filename = "MxSim.dat";
		std::ofstream outfile;
		outfile.open(filename);
		outfile.close();
	*/
	
	
	//write first 24 hours of effective capacity and efficiency for use with first run of next day (for ongoing repairs)
	/*
	Writes the cycle efficiency and capacity over time, to file.
	*/
	std::string filename = (
		m_file_settings.ampl_param_file
		);
	filename += ".dat";
	std::ofstream outfile;
	outfile.open(filename);
	outfile << "param Feff := \n";
	//for (int i = 0; i < m_sim_params.sim_length; i++)
	//{
	//	outfile << (i + 1) << "  " << m_results.cycle_efficiency[m_current_scenario].at(i) << "\n";
	//}
	int ctr = 1;
	for (
		int i = m_sim_params.sim_length;
		i < m_sim_params.sim_length + 24;
		i++
		)
	{
		outfile << (ctr) << "  " << m_results.cycle_efficiency[m_current_scenario].at(m_sim_params.sim_length-1) << "\n";
		ctr++;
	}
	outfile << ";\n\n";
	outfile << "param Fcap := \n";
	//for (int i = 0; i < m_sim_params.sim_length; i++)
	//{
	//	outfile << (i + 1) << "  " << m_results.cycle_capacity[m_current_scenario].at(i) << "\n";
	//}
	ctr = 1;
	for (
		int i = m_sim_params.sim_length;
		i < m_sim_params.sim_length + 24;
		i++
		)
	{
		outfile << (ctr) << "  " << m_results.cycle_capacity[m_current_scenario].at(m_sim_params.sim_length - 1) << "\n";
		ctr++;
	}
	outfile << ";\n\n";
}

void PowerCycle::WriteCapEffFile()
{
	/*
	The capacity and efficiency file consists of four lines:
	(1) A time series of cycle capacity
	(2) A time series of cycle efficiency
	(3) The period of last failure and the period of last repair
	*/
	std::string cap_eff_file = (
		m_file_settings.cap_eff_filename
		//+ m_current_scenario
		+".csv"
		);
	std::ofstream ofile;
	ofile.open(cap_eff_file);
	//period of last failure
	ofile << m_results.period_of_last_failure[m_current_scenario] << ",";
	//period of last repair
	ofile << m_results.period_of_last_repair[m_current_scenario] << "\n";
	//capacity time series
	for (size_t i = 0; i < m_results.cycle_capacity[m_current_scenario].size() - 1; i++)
	{
		ofile << m_results.cycle_capacity[m_current_scenario].at(i) << ",";
	}
	ofile << m_results.cycle_capacity[m_current_scenario].at(m_results.cycle_capacity[m_current_scenario].size() - 1) << "\n";
	//efficiency time series
	for (size_t i = 0; i < m_results.cycle_efficiency[m_current_scenario].size() - 1; i++)
	{
		ofile << m_results.cycle_efficiency[m_current_scenario].at(i) << ",";
	}
	ofile << m_results.cycle_efficiency[m_current_scenario].at(m_results.cycle_efficiency[m_current_scenario].size() - 1) << "\n";
	ofile.close();
}

void PowerCycle::WriteFinalEffFile()
{
	/*
	The efficiency file contains the final efficiency per period for the entire time horizon.
	This file is used by ssc to lower power generated during efficiency reductions.
	*/
	
	std::string final_cap_eff_file = ("final_eff.csv");
	std::ofstream ofile;
	if (m_file_settings.day_idx == 0){
		ofile.open(final_cap_eff_file, std::ofstream::trunc);
	} else ofile.open(final_cap_eff_file, std::ofstream::app);
	
	//capacity and efficiency time series
	for (size_t i = 0; i < m_results.cycle_efficiency[m_current_scenario].size(); i++)
	{
		ofile << m_results.cycle_efficiency[m_current_scenario].at(i) << ",";
	}
	ofile.close();
}

void PowerCycle::WriteFailureStats()
{
	std::ofstream ofile;
	if (m_file_settings.day_idx == 0) 
	{
		ofile.open(m_file_settings.num_failures_file+".csv", std::ofstream::trunc);
	}
	else 
	{
		ofile.open(m_file_settings.num_failures_file + ".csv", std::ofstream::app);
	}
	ofile << m_file_settings.day_idx << "," << m_failure_event_labels.size() << "\n";
	ofile.close();
}

void PowerCycle::ReadStateFromFiles(bool init)
{
	ReadSimParamsFile();
	ReadPolicyFile();
	ReadDayIDXFile();
	ReadPlantLayoutFile();
	ReadDispatchFile();
	if (init)
	{
		Initialize(0., m_current_scenario);
		WriteAMPLParamsToDefault();
		return;
	}
	ReadComponentFile();
	ReadPlantFile();
	//ReadFailuresFromFile();
	ReadCapEffFile();
	m_life_gen->ReadRNGStateFile(
		m_file_settings.rng_state_filename
		//+std::to_string(m_current_scenario)
		+ "l.csv",
		3 * m_current_scenario
	);
	m_repair_gen->ReadRNGStateFile(
		m_file_settings.rng_state_filename
		//+std::to_string(m_current_scenario)
		+ "r.csv",
		3 * m_current_scenario + 1
	);
	m_binary_gen->ReadRNGStateFile(
		m_file_settings.rng_state_filename
		//+std::to_string(m_current_scenario)
		+ "b.csv",
		3 * m_current_scenario + 2
	);
}

void PowerCycle::ReadPlantLayoutFile()
{
	std::string filename = (
		m_file_settings.plant_comp_info +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ifstream pfile;
	int cindex = 0;
	size_t pos = 0;
	std::string delimiter = ",";
	pfile.open(filename);
	std::string pline;
	std::string token;
	std::vector<std::string> split_line = {};
	getline(pfile, pline);
	//std::cerr << pline << "\n";
	cindex++;
	getline(pfile, pline);
	//std::cerr << pline << "\n";
	cindex++;
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	int num_condenser_trains = std::stoi(split_line[0]);
	int fans_per_condenser_train = std::stoi(split_line[1]);
	int radiators_per_condenser_train = std::stoi(split_line[2]);
	int num_salt_steam_trains = std::stoi(split_line[3]);
	int num_feedwater_heaters = std::stoi(split_line[4]);
	int num_salt_pumps = std::stoi(split_line[5]);
	int num_salt_pumps_required = std::stoi(split_line[6]);
	int num_water_pumps = std::stoi(split_line[7]);
	int num_water_pumps_required = std::stoi(split_line[8]);
	int num_boiler_pumps = std::stoi(split_line[9]);
	int num_boiler_pumps_required = std::stoi(split_line[10]);
	int num_turbines = std::stoi(split_line[11]);
	split_line.clear();
	//condener efficiencies cold
	getline(pfile, pline);
	//std::cerr << pline << "\n";
	pos = pline.find(delimiter);
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	std::vector<double> eff_cold = {};
	for (int i = 0; i <= num_condenser_trains; i++)
	{
		eff_cold.push_back(std::stod(split_line[i]));
	}
	//condener efficiencies hot
	split_line.clear();
	getline(pfile, pline);
	//std::cerr << pline << "\n";
	pos = pline.find(delimiter);
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	std::vector<double> eff_hot = {};
	for (int i = 0; i <= num_condenser_trains; i++)
	{
		eff_hot.push_back(std::stod(split_line[i]));
	}
	GeneratePlantComponents(
		num_condenser_trains,
		fans_per_condenser_train,
		radiators_per_condenser_train,
		num_salt_steam_trains,
		num_feedwater_heaters,
		num_salt_pumps,
		num_salt_pumps_required,
		num_water_pumps,
		num_water_pumps_required,
		num_boiler_pumps,
		num_boiler_pumps_required, 
		num_turbines,
		eff_cold,
		eff_hot
	);
}

void PowerCycle::ReadComponentFile()
{
	std::string component_filename = (
		m_file_settings.component_in_state +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ifstream cfile;
	size_t pos = 0;
	std::string delimiter = ",";
	cfile.open(component_filename);
	std::string cline;
	std::vector<std::string> split_line = {};
	getline(cfile, cline);
	//std::vector <double> lifes_probs;
	ComponentStatus status;
	int cindex = 0;
	std::string token;
	while (!cfile.eof())
	{
		//lifes_probs.clear();
		status.lifetimes.clear();
		pos = 0;
		getline(cfile, cline);
		//std::cerr << cline;
		while (pos != std::string::npos) {
			pos = cline.find(delimiter);
			token = cline.substr(0, pos);
			split_line.push_back(token);
			cline.erase(0, pos + delimiter.length());
		}
		if (split_line.size() > 4)
		{
			status.hazard_rate = std::stod(split_line[1]);
			status.downtime_remaining = std::stod(split_line[2]);
			status.repair_event_time = std::stod(split_line[3]);
			for (size_t i = 4; i < split_line.size(); i++)
			{
				status.lifetimes.push_back(std::stod(split_line[i]));
			}
			GetComponents().at(cindex).ReadStatus(status);
		}
		split_line.clear();
		cindex++;
	}
	StoreComponentState();
}

void PowerCycle::ReadPlantFile()
{
	std::string plant_filename = (
		m_file_settings.plant_in_state +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ifstream pfile;
	std::string delimiter = ",";
	size_t pos = 0;
	std::string pline;
	std::vector<std::string> split_line = {};
	std::string token;
	pfile.open(plant_filename);
	getline(pfile, pline);
	getline(pfile, pline);
	while (pos != std::string::npos) 
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	m_current_cycle_state.capacity = std::stod(split_line[0]);
	m_current_cycle_state.cold_start_penalty = std::stod(split_line[1]);
	m_current_cycle_state.warm_start_penalty = std::stod(split_line[2]);
	m_current_cycle_state.hot_start_penalty = std::stod(split_line[3]);
	m_current_cycle_state.downtime = std::stod(split_line[4]);
	m_current_cycle_state.downtime_threshold = std::stod(split_line[5]);
	m_current_cycle_state.is_online = std::stoi(split_line[6]);
	m_current_cycle_state.is_on_standby = std::stoi(split_line[7]);
	m_current_cycle_state.maintenance_duration = std::stod(split_line[8]);
	m_current_cycle_state.maintenance_interval = std::stod(split_line[9]);
	m_current_cycle_state.hours_to_maintenance = std::stod(split_line[10]);
	m_current_cycle_state.temp_threshold = std::stod(split_line[11]);
	m_current_cycle_state.time_in_standby = std::stod(split_line[12]);
	m_current_cycle_state.time_online = std::stod(split_line[13]);
	m_current_cycle_state.power_output = std::stod(split_line[14]);
	m_current_cycle_state.thermal_output = std::stod(split_line[15]);
	m_current_cycle_state.salt_pump_switch_time = std::stod(split_line[16]);
	m_current_cycle_state.boiler_pump_switch_time = std::stod(split_line[17]);
	m_current_cycle_state.water_pump_switch_time = std::stod(split_line[18]);

	StorePlantParamsState();
}

void PowerCycle::ReadSimParamsFile()
{
	std::string sp_filename = (
		m_file_settings.sim_params_filename +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ifstream pfile;
	std::string delimiter = ",";
	size_t pos = 0;
	std::string pline;
	std::vector<std::string> split_line = {};
	std::string token;
	pfile.open(sp_filename);
	getline(pfile, pline);
	getline(pfile, pline);
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	m_sim_params.read_periods = std::stoi(split_line[0]);
	m_sim_params.sim_length = std::stoi(split_line[1]);
	m_sim_params.steplength = std::stod(split_line[2]);
	m_sim_params.epsilon = std::stod(split_line[3]);
	m_sim_params.print_output = std::stoi(split_line[4]);
	m_sim_params.num_scenarios = std::stoi(split_line[5]);
	m_sim_params.hourly_labor_cost = std::stod(split_line[6]);
	m_sim_params.stop_at_first_repair = std::stoi(split_line[7]);
	m_sim_params.stop_at_first_failure = std::stoi(split_line[8]);
}

void PowerCycle::ReadDayIDXFile()
{
	std::string policy_filename = (
		m_file_settings.day_idx_filename + ".dat"
		);
	std::ifstream pfile;
	std::string delimiter = ";";
	size_t pos = 0;
	int cindex = 0;
	std::string pline;
	std::vector<std::string> split_line = {};
	std::string token;
	pfile.open(policy_filename);
	for (int i = 0; i < 4; i++)
	{
		getline(pfile, pline);
		//std::cerr << pline << "\n";
		cindex++;
	} 
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	m_file_settings.day_idx = std::stoi(split_line.at(0).substr(21,std::string::npos));
}

void PowerCycle::ReadPolicyFile()
{
	std::string policy_filename = (
		m_file_settings.policy_filename + ".csv"
		);
	std::ifstream pfile;
	std::string delimiter = ",";
	size_t pos = 0;
	std::string pline;
	std::vector<std::string> split_line = {};
	std::string token;
	pfile.open(policy_filename);
	getline(pfile, pline);
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	
	m_shutdown_capacity = std::stod(split_line[0]);
	m_no_restart_capacity = std::stod(split_line[1]);
	m_shutdown_efficiency = std::stod(split_line[2]);
	m_no_restart_efficiency = std::stod(split_line[3]);
	SetScenarioIndex(std::stoi(split_line[4]));
	
	//now that the current scenario is established, initialize cycle
	//capacity, efficiency, and periods of last failure/repair.
	m_results.period_of_last_failure[m_current_scenario] = -1;
	m_results.period_of_last_repair[m_current_scenario] = -1;
	m_results.cycle_capacity[m_current_scenario] = std::vector<double>(m_sim_params.sim_length, 1.);
	m_results.cycle_efficiency[m_current_scenario] = std::vector<double>(m_sim_params.sim_length, 1.);

	pfile.close();

}

void PowerCycle::ReadFailuresFromFile()
{
	std::string failure_filename = (
		m_file_settings.failure_file +
		//std::to_string(m_current_scenario) + 
		".csv"
		);
	std::ifstream cfile;
	size_t pos = 0;
	std::string delimiter = ",";
	cfile.open(failure_filename);
	std::string cline;
	std::vector<std::string> split_line = {};
	//getline(cfile, cline);
	//std::vector <double> lifes_probs;
	std::string label;
	int time;
	std::string component;
	int fail_idx;
	double duration; 
	double labor; 
	double new_life;
	int scen_index;
	//(int time, std::string component, int fail_idx, double duration,
	//	double labor, double new_life, int scen_index)
	int cindex = 0;
	std::string token;


	while (!cfile.eof())
	{
		//lifes_probs.clear();
		m_failure_events.clear();
		m_failure_event_labels.clear();
		pos = 0;
		getline(cfile, cline);
		//std::cerr << cline;
		while (pos != std::string::npos) {
			pos = cline.find(delimiter);
			token = cline.substr(0, pos);
			split_line.push_back(token);
			cline.erase(0, pos + delimiter.length());
		}
		if (split_line.size() > 6)
		{
			label = split_line[0];
			time = std::stoi(split_line[1]);
			component = split_line[2];
			fail_idx = std::stoi(split_line[3]);
			duration = std::stod(split_line[4]);
			labor = std::stod(split_line[5]);
			new_life = std::stod(split_line[6]);
			scen_index = std::stoi(split_line[7]);
			
			m_failure_event_labels.push_back(label);
			m_failure_events[label] = failure_event(
				time,
				component,
				fail_idx,
				duration,
				labor,
				new_life,
				scen_index
			);
		}
		split_line.clear();
		cindex++;
	}
}

void PowerCycle::ReadDispatchFile(int max_rows)
{
	std::unordered_map<std::string, std::vector <double> > data = {};
	data["standby"] = {};
	data["cycle_power"] = {};
	data["ambient_temperature"] = {};
	data["thermal_power"] = {};
	std::string dispatch_file = (
		m_file_settings.ampl_dispatch_file +
		//std::to_string(m_file_settings.day_idx) +
		".csv"
		);
	std::ifstream dfile;
	size_t pos = 0;
	std::string delimiter = ",";
	dfile.open(dispatch_file);
	std::string dline;
	std::vector<std::string> split_line = {};
	std::string token;
	int cindex = 0;
	getline(dfile, dline);
	while (cindex <= max_rows && !dfile.eof())
	{
		cindex++;
		pos = 0;
		getline(dfile, dline);
		while (pos != std::string::npos) {
			pos = dline.find(delimiter);
			token = dline.substr(0, pos);
			split_line.push_back(token);
			dline.erase(0, pos + delimiter.length());
		}
		if (split_line.size() > 3)
		{
			data["cycle_power"].push_back(std::stod(split_line[0]));
			data["standby"].push_back(std::stod(split_line[1]));
			data["ambient_temperature"].push_back(std::stod(split_line[2]));
			data["thermal_power"].push_back(std::stod(split_line[3]));
		}
		split_line.clear();
	}

	SetDispatch(data, true);
}

void PowerCycle::ReadCapEffFile()
{
	/*
	send cycle capacity, cycle_efficiency, last period of failure and 
	last period of repair to results.  if the last period of failure
	and last period of repair are equal to -1, then cycle_efficiency
	and cycle_capacity are overridden to all ones.
	*/
	std::string cap_eff_filename = "capeff.csv";
	std::ifstream pfile;
	std::string delimiter = ",";
	size_t pos = 0;
	std::string pline;
	std::vector<std::string> split_line = {};
	std::string token;
	pfile.open(cap_eff_filename);
	//first line: failure and repair periods
	getline(pfile, pline);
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	m_results.period_of_last_failure[m_current_scenario] = std::stoi(split_line[0]);
	m_results.period_of_last_repair[m_current_scenario] = std::stoi(split_line[1]);
	if (
		m_results.period_of_last_failure[m_current_scenario] == -1 &&
		m_results.period_of_last_repair[m_current_scenario] == -1
		)
	{
		m_results.cycle_capacity[m_current_scenario] = std::vector<double>(m_sim_params.sim_length, 1.);
		m_results.cycle_efficiency[m_current_scenario] = std::vector<double>(m_sim_params.sim_length, 1.);
		return;
	}
	//if the period of last failure/repair is not -1, then read in the 
	//cycle efficiency and cycle capacity from the file. 
	split_line.clear();
	getline(pfile, pline);
	pos = 0;
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	m_results.cycle_capacity[m_current_scenario].clear();
	m_results.cycle_efficiency[m_current_scenario].clear();
	for (int i = 0; i < m_sim_params.sim_length; i++)
	{
		m_results.cycle_capacity[m_current_scenario].push_back(std::stod(split_line[i]));
	}
	split_line.clear();
	getline(pfile, pline);
	pos = 0;
	while (pos != std::string::npos)
	{
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		pline.erase(0, pos + delimiter.length());
	}
	for (int i = 0; i < m_sim_params.sim_length; i++)
	{
		m_results.cycle_efficiency[m_current_scenario].push_back(std::stod(split_line[i]));
	}
	pfile.close();
}

std::vector< Component > &PowerCycle::GetComponents()
{
    /*
	accessor for components in the plant, stored as a vector.
	*/
    return m_components;
}

std::vector< double > PowerCycle::GetComponentLifetimes()
{

    /*
	retval -- linked list of life remaining of each component.
	*/
	std::vector<double> life;
	std::vector<double> clife;
	for (size_t i = 0; i < m_components.size(); i++)
	{
		clife = m_components.at(i).GetLifetimesAndProbs();
		life.insert(life.end(), clife.begin(), clife.end());
	}
    return life;

}

double PowerCycle::GetHoursToMaintenance()
{
	/* accessor for hours left to calendar-based maintenance. */
	return m_current_cycle_state.hours_to_maintenance;
}

std::vector< double >  PowerCycle::GetComponentDowntimes()
{
    /*
	accessor to component downtimes (zero if component is operational).
	retval -- linked list of downtime remaining of each component.
	*/
    std::vector<double> down;

    for(size_t i=0; i<m_components.size(); i++)
        down.push_back( m_components.at(i).GetDowntimeRemaining() );
    return down;
}

bool PowerCycle::AirstreamOnline()
{
	/*
	Returns true if at least one condenser airstream is operational, and false
	otherwise  Used to determine cycle Capacity (which is zero if this
	is false, as the plant can't operate with no airstreams.)
	*/
	for (size_t i = 0; i<m_components.size(); i++)
		if (m_components.at(i).GetType() == "Condenser train" && m_components.at(i).IsOperational())
		{
			return true;
		}
	return false;
}

bool PowerCycle::FWHOnline()
{
	/*
	Returns true if at least one feedwater heater is operational, and false
	otherwise.  Used to determine cycle Capacity (which is zero if this
	is false, as the plant can't operate with no feedwater heaters.)
	*/
	for (size_t i = 0; i<m_components.size(); i++)
		if (m_components.at(i).GetType() == "Feedwater heater" && m_components.at(i).IsOperational())
		{
			return true;
		}
	return false;
}

bool PowerCycle::IsOnline()
{

    /*
	retval -- a boolean indicator of whether the plant's power cycle is
    generating power.
	*/
    return m_current_cycle_state.is_online; 
}

bool PowerCycle::IsOnStandby()
{
    /*
	retval --  a boolean indicator of whether the plant is in standby 
    mode.
	*/
    return m_current_cycle_state.is_on_standby;
}

bool PowerCycle::NewRepairOccurred()
{
	/*
	returns true if a new repair occurred in the current period of dispatch.
	*/
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (m_components.at(i).IsNewRepair())
		{
			//std::cerr << m_components.at(i).GetName() << " repair detected." << m_components.at(i).IsNewRepair() << "\n";
			return true;
		}
	}
	return false;
}

bool PowerCycle::NewFailureOccurred()
{
	/*
	returns true if a new failure occurred in the current period of dispatch.
	*/
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (m_components.at(i).IsNewFailure())
		{
			//std::cerr << m_components.at(i).GetName() << " failure detected." << m_components.at(i).IsNewFailure() << "\n";
			return true;
		}
	}
	return false;
}

void PowerCycle::ResetCycleEventFlags()
{
	/* resets new failure and new repair flags for components. */
	for (size_t i = 0; i < m_components.size(); i++)
	{
		m_components.at(i).ResetFailureAndRepairFlags();
	}
}

double PowerCycle::GetTimeInStandby()
{
	/* 
	accessor to time elapsed since the start of standby 
	for the plant, if the plant is in standby; returns
	zero otherwise.
	*/
	return m_current_cycle_state.time_in_standby;
}

double PowerCycle::GetTimeOnline()
{
	/*
	accessor to time elapsed since the start of online mode
	for the plant, if the power cycle's generator is active; returns
	zero otherwise.
	*/
	return m_current_cycle_state.time_online;
}

double PowerCycle::GetRampThreshold()
{
	/* 
	accessor to ramping threshold (reference value for power cycle output
	without a ramping multplier)
	*/
	return m_ramp_threshold;
}

double PowerCycle::GetSteplength()
{
	/* accessor to length of a time period (in hours).	*/
	return m_sim_params.steplength;
}

std::unordered_map< std::string, failure_event > PowerCycle::GetFailureEvents()
{
	/* accessor to dictionary of failure events. */
	return m_failure_events;
}

std::vector<std::string> PowerCycle::GetFailureEventLabels()
{
	/* accessor to linked list of failure event dictionary keys. */
	return m_failure_event_labels;
}

double PowerCycle::GetHotStartPenalty()
{
	/* accessor for hot start penalty. */
	return m_current_cycle_state.hot_start_penalty;
}

double PowerCycle::GetWarmStartPenalty()
{
	/* accessor for warm start penalty. */
	return m_current_cycle_state.warm_start_penalty;
}

double PowerCycle::GetColdStartPenalty()
{
	/* accessor for cold start penalty. */
	return m_current_cycle_state.cold_start_penalty;
}

int PowerCycle::GetSimLength()
{
	/* accessor for simulation length (in time periods) */
	return m_sim_params.sim_length;
}

double PowerCycle::GetShutdownCapacity()
{
	return m_shutdown_capacity;
}

double PowerCycle::GetNoRestartCapacity()
{
	return m_no_restart_capacity;
}

double PowerCycle::GetShutdownEfficiency()
{
	return m_shutdown_efficiency;
}

double PowerCycle::GetNoRestartEfficiency()
{
	return m_no_restart_efficiency;
}

int PowerCycle::GetScenarioIndex()
{
	return m_current_scenario;
}

std::unordered_map<std::string, std::vector<double>> PowerCycle::GetDispatch()
{
	/* accessor to dispatch data. */
	return m_dispatch;
}

void PowerCycle::SetShutdownCapacity(double capacity) 
{
	/* accessor to cycle capacity threshold for immediate shutdown. */
	m_shutdown_capacity = capacity;
}


void PowerCycle::SetNoRestartCapacity(double capacity)
{
	/* 
	accessor to cycle capacity threshold for beginning repairs at next 
	power cycle shutdown. 
	*/
	m_no_restart_capacity = capacity;
}

void PowerCycle::SetShutdownEfficiency(double efficiency)
{
	/* accessor to cycle efficiency threshold for immediate shutdown. */
	m_shutdown_efficiency = efficiency;
}

void PowerCycle::SetNoRestartEfficiency(double efficiency)
{
	/*
	accessor to cycle efficiency threshold for beginning repairs at next
	power cycle shutdown.
	*/
	m_no_restart_efficiency = efficiency;
}

void PowerCycle::SetScenarioIndex(int idx)
{
	m_current_scenario = idx;

	m_life_gen->assignStates(3 * idx);
	m_repair_gen->assignStates(3 * idx + 1);
	m_binary_gen->assignStates(3 * idx + 2);
}


void PowerCycle::AddComponent(
	std::string name,
	std::string type,
	double repair_rate,
	double repair_cooldown_time,
	double capacity_reduction,
	double efficiency_reduction,
	double repair_cost,
	std::string repair_mode
)
{
	/* 
	adds a component to the power cycle via the Component(...) constructor.
	components are stored as a vector.

	name -- component identifer
    type -- component type description
    repair_rate -- rate at which repairs take place (events/h)
    repair_cooldown_time -- added required downtime for repair (h)
	capacity_reduction -- reduction in cycle capacity if component fails (fraction)
	efficiency_reduction -- reduction in cycle capacity if component fails (fraction)
    repair_cost -- dollar cost of repairs, not including revenue lost ($)
    repair_mode -- indicator of in which modes the component may be repaired
	*/
	m_components.push_back(Component(name, type,
		repair_rate, repair_cooldown_time, &m_failure_events,
		capacity_reduction, efficiency_reduction, repair_cost, repair_mode,
		&m_failure_event_labels));
	if (type == "Turbine")
		m_turbine_idx.push_back(m_components.size() - 1);
	else if (type == "Condenser train")
		m_condenser_idx.push_back(m_components.size() - 1);
	else if (type == "Salt-to-steam train")
		m_sst_idx.push_back(m_components.size() - 1);
	else if (type == "Molten salt pump")
		m_salt_pump_idx.push_back(m_components.size() - 1);
	else if (type == "Water pump")
		m_water_pump_idx.push_back(m_components.size() - 1);
	else if (type == "Boiler pump")
		m_boiler_pump_idx.push_back(m_components.size() - 1);
}

void PowerCycle::AddFailureType(std::string component, std::string id, std::string failure_mode,
	std::string dist_type, double alpha, double beta)
{
	for (size_t j = 0; j < m_components.size(); j++)
	{
		if (m_components.at(j).GetName() == component)
		{
			m_components.at(j).AddFailureMode(component,
				id, failure_mode, dist_type, alpha, beta);
			return;
		}
	}
}

void PowerCycle::CreateComponentsFromFile(std::string component_data)
{

    /*
	parses a string containing component information for the plant.
    the components are stored as a linked list, with each component's
    data stored in a dictionary.
    
    dispatch_file -- location of the component input file
    retval - linked list of plant data dictionaries
    
    e.g.:
    name	component_type	              failure_alpha failure_beta	repair_rate	repair_cooldown_time	hot_start_penalty	warm_start_penalty	cold_start_penalty
    >> begin file >> 
    SH1,  heat exchanger (salt-to-steam), 0.001,0.5, 0,0.01,0.02,0.03;
    HP1,  high-pressure turbine,          0.001,0.5,24,0.01,0.02,0.03;
    IP1,  medium-pressure turbine,        0.001,0.5,24,0.01,0.02,0.03;
    LP1,  low-pressure turbine,           0.001,0.5,24,0.01,0.02,0.03;
    ...
	*/

    std::vector< std::string > entries = util::split( component_data, ";" );
    for( size_t i=0; i<entries.size(); i++)
    {
        std::vector< std::string > entry = util::split( entries.at(i), "," );

        if( entry.size() != 8 )
            throw std::runtime_error( "Mal-formed cycle Capacity model table. Component table must contain 8 entries per row (comma separated values), with each entry separated by a ';'." );

        std::vector< double > dat(6);

        for(size_t j=0; j<6; j++)
            util::to_double( entry.at(j+2), &dat.at(j) );
        //                                  name	  component_type  dist_type failure_alpha failure_beta repair_rate repair_cooldown_time hot_start_penalty warm_start_penalty cold_start_penalty
        m_components.push_back( Component( entry.at(0), entry.at(1), //entry.at(2), dat.at(0),   dat.at(1),  
			dat.at(2),  dat.at(3),          &m_failure_events ) );
        
    }
    
}


void PowerCycle::AddCondenserTrains(int num_trains, int num_fans, int num_radiators)
{
	m_num_condenser_trains = num_trains;
	m_fans_per_condenser_train = num_fans;
	m_radiators_per_condenser_train = num_radiators;
	std::string component_name, train_name;
	for (int j = 0; j < num_trains; j++)
	{
		train_name = "C" + std::to_string(j+1);
		AddComponent(train_name + "-T", "Condenser train", 15.55, 0, 0, 0, 7.777, "S");
		for (int i = 1; i <= num_fans; i++)
		{
			component_name = train_name + "-F" + std::to_string(i);
			AddComponent(component_name, "Condenser fan", 35.5, 0, 0, 0, 7.777, "S");
			AddFailureType(component_name, "Fan Failure", "O", "gamma", 1, 841188);
		}
		for (int i = 1; i <= num_radiators; i++)
		{
			AddFailureType(train_name + "-T", "Radiator " + std::to_string(i) + " Failure", "O", "gamma", 1, 28279.8); //note: Army source = 698976
		}
	}
}

void PowerCycle::AddSaltToSteamTrains(int num_trains)
{
	std::string component_name;
	double capacity_reduction = 1.0 / (double)num_trains;
	for (int i = 0; i < num_trains; i++)
	{
		m_num_salt_steam_trains += 1;
		component_name = "SST" + std::to_string(m_num_salt_steam_trains);
		AddComponent(component_name, "Salt-to-steam train", 120, 120, capacity_reduction, 0., 7.777, "A");
		/*
		AddFailureType(component_name, "Boiler External_Leak_Large_(shell)", "ALL", "inv-gamma", 0.3, 75000000);
		AddFailureType(component_name, "Boiler External_Leak_Large_(tube)", "ALL", "inv-gamma", 0.3, 10000000);
		AddFailureType(component_name, "Boiler External_Leak_Small_(shell)", "ALL", "inv-gamma", 0.5, 10000000);
		AddFailureType(component_name, "Boiler External_Leak_Small_(tube)", "ALL", "inv-gamma", 0.3, 1200000);
		AddFailureType(component_name, "Boiler Plug/Foul", "ALL", "inv-gamma", 1.5, 2500000);
		AddFailureType(component_name, "Economizer External_Leak_Large_(shell)", "ALL", "inv-gamma", 0.3, 75000000);
		AddFailureType(component_name, "Economizer External_Leak_Large_(tube)", "ALL", "inv-gamma", 0.3, 10000000);
		AddFailureType(component_name, "Economizer External_Leak_Small_(shell)", "ALL", "inv-gamma", 0.5, 10000000);
		AddFailureType(component_name, "Economizer External_Leak_Small_(tube)", "ALL", "inv-gamma", 0.3, 1200000);
		AddFailureType(component_name, "Economizer Plug/Foul", "ALL", "inv-gamma", 1.5, 2500000);
		AddFailureType(component_name, "Reheater External_Leak_Large_(shell)", "ALL", "inv-gamma", 0.3, 75000000);
		AddFailureType(component_name, "Reheater External_Leak_Large_(tube)", "ALL", "inv-gamma", 0.3, 10000000);
		AddFailureType(component_name, "Reheater External_Leak_Small_(shell)", "ALL", "inv-gamma", 0.5, 10000000);
		AddFailureType(component_name, "Reheater External_Leak_Small_(tube)", "ALL", "inv-gamma", 0.3, 1200000);
		AddFailureType(component_name, "Reheater Plug/Foul", "ALL", "inv-gamma", 1.5, 2500000);
		AddFailureType(component_name, "Superheater External_Leak_Large_(shell)", "ALL", "inv-gamma", 0.3, 75000000);
		AddFailureType(component_name, "Superheater External_Leak_Large_(tube)", "ALL", "inv-gamma", 0.3, 10000000);
		AddFailureType(component_name, "Superheater External_Leak_Small_(shell)", "ALL", "inv-gamma", 0.5, 10000000);
		AddFailureType(component_name, "Superheater External_Leak_Small_(tube)", "ALL", "inv-gamma", 0.3, 1200000);
		AddFailureType(component_name, "Superheater Plug/Foul", "ALL", "inv-gamma", 1.5, 2500000);
		*/
		AddFailureType(component_name, "Boiler Failure", "ALL", "gamma", 1., 306624);
		AddFailureType(component_name, "Economizer Failure", "ALL", "gamma", 1., 306624);
		AddFailureType(component_name, "Reheater Failure", "ALL", "gamma", 1., 306624);
		AddFailureType(component_name, "Superheater Failure", "ALL", "gamma", 1., 306624);
		
		
	}
}

void PowerCycle::AddFeedwaterHeaters(int num_fwh)
{
	std::string component_name;
	for (int i = 0; i < num_fwh; i++)
	{
		m_num_feedwater_heaters += 1;
		component_name = "FWH" + std::to_string(m_num_feedwater_heaters);
		AddComponent(component_name, "Feedwater heater", 2.14, 24., 0., 0.05, 7.777, "A");
		AddFailureType(component_name, "MTBF", "ALL", "gamma", 1., 2259200); //water-to-water heat exchanger, IEEE 2007
		/*
		AddFailureType(component_name, "External_Leak_Large_(shell)", "O", "inv-gamma", 0.3, 75000000);
		AddFailureType(component_name, "External_Leak_Large_(tube)", "O", "inv-gamma", 0.3, 10000000);
		AddFailureType(component_name, "External_Leak_Small_(shell)", "O", "inv-gamma", 0.5, 10000000);
		AddFailureType(component_name, "External_Leak_Small_(tube)", "O", "inv-gamma", 0.3, 1200000);
		AddFailureType(component_name, "Plug/Foul", "O", "inv-gamma", 1.5, 2500000);
		*/
	}
}

void PowerCycle::AddSaltPumps(int num_pumps, int num_required)
{
	std::string component_name;
	m_num_salt_pumps_required = num_required;
	for (int i = 0; i < num_pumps; i++)
	{
		m_num_salt_pumps += 1;
		component_name = "SP" + std::to_string(m_num_salt_pumps);
		AddComponent(component_name, "Molten salt pump", 7.09, 24., 0., 0., 7.777, "A");
		AddFailureType(component_name, "External_Leak_Large", "ALL", "inv-gamma", 0.3, 37500000);
		AddFailureType(component_name, "External_Leak_Small", "ALL", "gamma", 1, 8330000);
		AddFailureType(component_name, "Fail_to_Run_<=_1_hour_(standby)", "OF", "inv-gamma", 1.5, 3750);
		AddFailureType(component_name, "Fail_to_Run_>_1_hour_(standby)", "OO", "inv-gamma", 0.5, 83300);
		AddFailureType(component_name, "Fail_to_Start_(standby)", "OS", "beta", 0.9, 599);
		//AddFailureType(component_name, "Fail_to_Start_(running)", "OS", "beta", 0.9, 449);
		//AddFailureType(component_name, "Fail_to_Run_(running)", "O", "gamma", 1.5, 300000);
	}
}

void PowerCycle::AddBoilerPumps(int num_pumps, int num_required)
{
	std::string component_name;
	m_num_boiler_pumps_required = num_required;
	for (int i = 0; i < num_pumps; i++)
	{
		m_num_boiler_pumps += 1;
		component_name = "BP" + std::to_string(m_num_boiler_pumps);
		AddComponent(component_name, "Boiler pump", 7.09, 5., 0., 0., 7.777, "D");
		AddFailureType(component_name, "External_Leak_Large", "ALL", "inv-gamma", 0.3, 37500000);
		AddFailureType(component_name, "External_Leak_Small", "ALL", "gamma", 1, 8330000);
		AddFailureType(component_name, "Fail_to_Run_<=_1_hour_(standby)", "OF", "inv-gamma", 1.5, 3750);
		AddFailureType(component_name, "Fail_to_Run_>_1_hour_(standby)", "OO", "inv-gamma", 0.5, 83300);
		AddFailureType(component_name, "Fail_to_Start_(standby)", "OS", "beta", 0.9, 599);
	}
}

void PowerCycle::AddWaterPumps(int num_pumps, int num_required)
{
	std::string component_name;
	m_num_water_pumps_required = num_required;
	for (int i = 0; i < num_pumps; i++)
	{
		m_num_water_pumps += 1;
		component_name = "WP" + std::to_string(m_num_water_pumps);
		AddComponent(component_name, "Water pump", 7.09, 1., 0., 0., 7.777, "D");
		AddFailureType(component_name, "External_Leak_Large", "ALL", "inv-gamma", 0.3, 37500000);
		AddFailureType(component_name, "External_Leak_Small", "ALL", "gamma", 1, 8330000);
		AddFailureType(component_name, "Fail_to_Run_<=_1_hour_(standby)", "OF", "inv-gamma", 1.5, 3750);
		AddFailureType(component_name, "Fail_to_Run_>_1_hour_(standby)", "OO", "inv-gamma", 0.5, 83300);
		AddFailureType(component_name, "Fail_to_Start_(standby)", "OS", "beta", 0.9, 599);
	}
}

void PowerCycle::AddTurbines(int num_turbines)
{
	/*
	Adds turbines to the plant; assumes that the turbine-generator shaft shares a 
	single failure rate, based on anecdotal feedback of maintenance every ~6 years, and 
	the incidence of turbine failures being rare.

	source for efficiency reduction: anecdotal (2-5% decrease)
	*/
	std::string component_name;
	double capacity_reduction = 1.0 / (double)num_turbines;
	for (int i = 0; i < num_turbines; i++)
	{
		m_num_turbines += 1;
		component_name = "T" + std::to_string(m_num_turbines);
		AddComponent(component_name, "Turbine", 32.7, 36, capacity_reduction, 0., 7.777, "D");
		AddFailureType(component_name, "MBTF", "O", "gamma", 1, 51834.31953);
		//AddFailureType(component_name, "MBTF_High-pressure", "O", "gamma", 1, 51834.31953);
		//AddFailureType(component_name, "MBTF_Medium-pressure", "O", "gamma", 1, 51834.31953);
		//AddFailureType(component_name, "MBTF_Low-pressure", "O", "gamma", 1, 51834.31953);
	}
}

void PowerCycle::GeneratePlantComponents(
	int num_condenser_trains,
	int fans_per_train,
	int radiators_per_train,
	int num_salt_steam_trains,
	int num_fwh,
	int num_salt_pumps,
	int num_salt_pumps_required,
	int num_water_pumps,
	int num_water_pumps_required,
	int num_boiler_pumps,
	int num_boiler_pumps_required,
	int num_turbines,
	std::vector<double> condenser_eff_cold,
	std::vector<double> condenser_eff_hot,
	bool reset_hazard
)
{
	/* 
	Generates all the components in the plant.  Aggregates the other 
	component-specific methods.  Starts by clearing any existing components,
	i.e., assumes that the plant specification in the arguments is complete.

	num_condenser_trains -- number of condenser trains/airstreams
	fans_per_train -- number of fans in each condenser train
	radiators_per_train -- number of radiators in each condenser train
	num_salt_steam_trains -- Number of salt-to-steam trains in power cycle
	num_fwh -- Number of feedwater heaters in power cycle
	num_salt_pumps -- Number of molten salt pumps in power cycle
	num_salt_pumps_required -- Number of molten salt pumps required to operate power cycle at capacity
	num_water_pumps -- Number of water pumps in power cycle
	num_water_pumps_required -- Number of water pumps required to operate power cycle at capacity
	num_boiler_pumps -- Number of boiler pumps in power cycle
	num_boiler_pumps_required -- Number of boiler pumps required to operate power cycle at capacity
	num_turbines -- Number of turbine-generator shafts in power cycle
	condenser_eff_cold -- Efficiency of condenser according to how many trains are operational for low ambient temperatures 
	condenser_eff_hot -- Efficiency of condenser according to how many trains are operational for high ambient temperatures 
	*/
	ClearComponents();
	bool b1 = condenser_eff_cold.size() != (num_condenser_trains + 1);
	bool b2 = condenser_eff_hot.size() != (num_condenser_trains + 1);
	if (b1 || b2)
	{
		throw std::runtime_error("condenser efficiencies do not reconcile with number of trains.");
	}
	AddCondenserTrains(num_condenser_trains, fans_per_train, radiators_per_train);
	AddSaltToSteamTrains(num_salt_steam_trains);
	AddFeedwaterHeaters(num_fwh);
	AddSaltPumps(num_salt_pumps, num_salt_pumps_required);
	AddWaterPumps(num_water_pumps, num_water_pumps_required);
	AddBoilerPumps(num_boiler_pumps, num_boiler_pumps_required);
	AddTurbines(num_turbines);
	m_condenser_efficiencies_cold = condenser_eff_cold;
	m_condenser_efficiencies_hot = condenser_eff_hot;
	if (reset_hazard)
	{
		for (Component c : GetComponents())
			c.SetResetHazardRatePolicy(reset_hazard);
	}
}

void PowerCycle::SetPlantAttributes(
	double maintenance_interval,
	double maintenance_duration,
	double downtime_threshold,
	double hours_to_maintenance,
	double power_output,
	double thermal_output,
	bool current_standby,
	double capacity,
	double thermal_capacity,
	double temp_threshold,
	double time_online,
	double time_in_standby,
	double downtime,
	double shutdown_capacity,
	double no_restart_capacity,
	double shutdown_efficiency,
	double no_restart_efficiency
)
{
    /*
	Initializes plant attributes.
    
	maintenance_interval -- Operation time before maintenance (h)
	maintenance_duration -- Maintenance event duration (h)
	downtime_threshold -- Length of downtime required to qualify as a cold start (h)
	hours_to_maintenance -- Operation duration before the next maintenance event (h)
	power_output -- Plant power cycle output at start of simulation (W)
	current_standby -- true if plant is currently on standby, false otherwise
	capacity -- Upper bound for power cycle output (W)
	temp_threshold -- Threshold for two condenser streams required for cooling the plant (Celsius)
	time_online -- Time elapsed since plant was last offline or in standby (h)
	time_in_standby -- Time elapsed since plant was last offline or generating power (h)
	downtime -- Time elapsed since plant was last in standby or generating power (h)
	shutdown_capacity -- Capacity threshold for power cycle immediate shutdown (fraction)
	no_restart_capacity -- Capacity threshold for maintenance at next power cycle shutdown (fraction)
	shutdown_efficiency -- Efficiency threshold for power cycle immediate shutdown (fraction)
	no_restart_efficiency -- Efficiency threshold for maintenance at next power cycle shutdown (fraction)
	*/

    m_current_cycle_state.maintenance_interval = maintenance_interval;
    m_current_cycle_state.maintenance_duration = maintenance_duration; 
    m_current_cycle_state.downtime_threshold = downtime_threshold;
    m_current_cycle_state.hours_to_maintenance = hours_to_maintenance;
    m_current_cycle_state.power_output = power_output;
	m_current_cycle_state.thermal_output = thermal_output;
    m_current_cycle_state.is_on_standby = current_standby;
    m_current_cycle_state.is_online = m_current_cycle_state.power_output > 0.;
	m_current_cycle_state.capacity = capacity;
	m_current_cycle_state.thermal_capacity = thermal_capacity;
	m_current_cycle_state.time_in_standby = time_in_standby;
	m_current_cycle_state.time_online = time_online;
	m_condenser_temp_threshold = temp_threshold;
	m_current_cycle_state.downtime = downtime;
	m_shutdown_capacity = shutdown_capacity;
	m_no_restart_capacity = no_restart_capacity;
	m_shutdown_efficiency = shutdown_efficiency;
	m_no_restart_efficiency = no_restart_efficiency;
	SetRampingThresholds();
}

void PowerCycle::SetRampingThresholds()
{
	m_ramp_threshold = m_current_cycle_state.thermal_capacity * 0.2;  //using Gas-CC as source, Kumar 2012,  updated 10/03 to be based on thermal
	m_ramp_threshold_min = 1.1*m_ramp_threshold;   //Table 1-1
	m_ramp_threshold_max = 2.0*m_ramp_threshold;
}

void PowerCycle::SetDispatch(std::unordered_map< std::string, std::vector< double > > &data, bool clear_existing)
{

    /*
    Sets an attribute of the dispatch series. 

    ## available attributes include ##
    cost
    standby
    can_cycle_run
    cycle_standby_energy
    thermal_energy
    can_receiver_run
    thermal_storage
    startup_inventory
    receiver_energy
    thermal_energy
    cycle_power
    */

    if( clear_existing )
        m_dispatch.clear();

    for( std::unordered_map< std::string, std::vector< double > >::iterator it=data.begin(); it!= data.end(); it++)
    {
        m_dispatch[ it->first ] = it->second;
    }

    
}

int PowerCycle::NumberOfAirstreamsOnline()
{
	/*
	returns the number of condenser airstreams that are online.
	*/
	int num_streams = 0;
	for (size_t i = 0; i<m_components.size(); i++)
		if (m_components.at(i).GetType() == "Condenser train" && m_components.at(i).IsOperational())
		{
			num_streams++;
		}
	return num_streams;
}

double PowerCycle::GetCondenserEfficiency(double temp)
{
	return 1.0;
}

double PowerCycle::GetCondenserCapacity(double temp)
{
	/*
	Returns the condener efficiency according to any fans and trains are down,
	as well as the ambient temperature.

	temp -- ambient temperature (Celsius)
	retval -- efficiency of power cycle condenser (fraction)
	*/
	int num_streams = 0;
	for (size_t i : m_condenser_idx)
	{
		//std::cerr << i << " " << GetComponents().at(i).GetType() << "\n";
		if (m_components.at(i).IsOperational())
		{
			num_streams++;
		}
	}
	if (num_streams == 0)
		return 0.0;
	if (temp < m_condenser_temp_threshold)
		return 1.0;
	return double(num_streams) / double(m_num_condenser_trains);
}

double PowerCycle::GetTurbineEfficiency(bool age, bool include_failures)
{
	/*
	Returns a weighted average of the efficiencies of all turbines, 
	in which the weights are the relative capacity of each turbine to 
	the plant capacity.

	age -- true if aging model is included, false o.w.
	include_failures -- include failed turbines in calculation, false o.w.
	*/
	double eff = 0.0;
	double total_cap = 0.0;
	if (age)
	{
		for (size_t i : m_turbine_idx)
		{
			if (m_components.at(i).IsOperational() || include_failures)
			{
				eff += m_components.at(i).GetEfficiency(include_failures) * m_components.at(i).GetCapacityReduction();
				total_cap += m_components.at(i).GetCapacityReduction();
			}
		}
		return eff / total_cap;
	}
	else
	{
		eff = 1.0;
		total_cap = 1.0;
		for (size_t i : m_turbine_idx)
		{
			if (!m_components.at(i).IsOperational() && !include_failures)
			{
				eff -= 1 * m_components.at(i).GetEfficiencyReduction();
			}
		}
	}
	return eff; 
}

double PowerCycle::GetTurbineCapacity(bool age, bool include_failures)
{
	/*
	Returns the capacity of all turbines.

	age -- true if aging model is included, false o.w.
	include_failures -- include failed turbines in calculation, false o.w.
	*/
	double cap = 0.0;
	for (size_t i : m_turbine_idx)
	{
		if (include_failures || m_components.at(i).IsOperational())
		{
			//The first component is an age-weighted index, while the second is baseline 
			//relative capacity vs. that of all turbines in the system.
			if (age)
				cap += m_components.at(i).GetCapacity(include_failures) * m_components.at(i).GetCapacityReduction();
			else
				cap += m_components.at(i).GetCapacityReduction();
		}
	}
	return cap;
}

double PowerCycle::GetSaltSteamTrainCapacity()
{
	/*
	Returns the relative capacity of all salt-to-steam trains that
	are operational.
	*/
	double cap = 0.0;
	for (size_t i : m_sst_idx)
	{
		if (m_components.at(i).IsOperational())
		{
			cap += m_components.at(i).GetCapacityReduction();
		}
	}
	return cap;
}

double PowerCycle::GetSaltPumpCapacity()
{
	/*
	Returns the relative capacity of all salt pumps that
	are operational.  If switching is currently taking place, 
	remove one operational pump.
	*/
	double num_pumps_operational = 0.;
	for (size_t i : m_salt_pump_idx)
	{
		if (m_components.at(i).IsOperational())
		{
			num_pumps_operational += 1;
		}
	}
	if (m_current_cycle_state.salt_pump_switch_time > DBL_EPSILON)
	{
		return std::max(
			0.0,
			std::min(
				1.0, (num_pumps_operational - 1) / (double)m_num_salt_pumps_required
			)
		);
	}
	return std::min(1.0, num_pumps_operational / (double)m_num_salt_pumps_required);
}

double PowerCycle::GetWaterPumpCapacity()
{
	double num_pumps_operational = 0.;
	for (size_t i : m_water_pump_idx)
	{
		if (m_components.at(i).IsOperational())
		{
			num_pumps_operational += 1;
		}
	}
	if (m_current_cycle_state.water_pump_switch_time > DBL_EPSILON)
	{
		return std::max(
			0.0,
			std::min(
				1.0, (num_pumps_operational - 1) / (double)m_num_water_pumps_required
			)
		);
	}
	return std::min(1.0, num_pumps_operational / (double)m_num_water_pumps_required);
}

double PowerCycle::GetBoilerPumpCapacity()
{
	double num_pumps_operational = 0.;
	for (size_t i : m_boiler_pump_idx)
	{
		if (m_components.at(i).IsOperational())
		{
			num_pumps_operational += 1;
		}
	}

	if (m_current_cycle_state.boiler_pump_switch_time > DBL_EPSILON)
	{
		return std::max(
			0.0,
			std::min(
				1.0, (num_pumps_operational - 1) / (double)m_num_boiler_pumps_required
			)
		);
	}

	return std::min(1.0, num_pumps_operational / (double)m_num_boiler_pumps_required);
}

double PowerCycle::GetSaltPumpEfficiency()
{
	/*
	Returns the relative efficiency of all salt pumps that
	are operational.  This model assumes that efficiency
	reductions are only due to reductions in capacity; 
	hence, we have removed them from consideration.
	*/
	return 1.0;
	/*
	double num_pumps_operational = 0.;
	for (size_t i : m_salt_pump_idx)
	{
		if (m_components.at(i).IsOperational())
		{
			num_pumps_operational += 1;
		}
	}
	return 1.0 - 0.035 * std::max(0.0, m_num_salt_pumps_required - num_pumps_operational);
	*/
}

double PowerCycle::GetWaterPumpEfficiency()
{
	return 1.0;
}

double PowerCycle::GetBoilerPumpEfficiency()
{
	return 1.0;
}

void PowerCycle::SetCycleCapacityAndEfficiency(double temp, bool age)
{
	/* 
	Provides the power cycle's capacity, which is limited by components that 
	are not operational.  Turbines and Heat exchangers may have parallel 
	trains, and so the effecitve capacity starts with the minimum of the 
	capacities accounting for failures of those components.
	Assumes that when multiple other components are
	down, the effect on cycle capacity is additive. 

	temp -- ambient temperature (affects condenser efficiency)
	*/
	//if no condensers are online or no feedwater heaters are online,
	//assume the plant is shut down.
	if (!AirstreamOnline() || !FWHOnline())
	{
		m_cycle_capacity = 0.;
		m_cycle_efficiency = 0.;
		//std::cerr << "OFF";
		return;
	}
	double condenser_eff = GetCondenserEfficiency(temp);
	double condenser_cap = GetCondenserCapacity(temp);
	double turbine_eff = GetTurbineEfficiency(age, false);
	double turbine_cap = GetTurbineCapacity(age, false);
	double sst_cap = GetSaltSteamTrainCapacity();
	double salt_pump_cap = GetSaltPumpCapacity();
	double salt_pump_eff = GetSaltPumpEfficiency();
	double water_pump_cap = GetWaterPumpCapacity();
	double water_pump_eff = GetWaterPumpEfficiency();
	double boiler_pump_cap = GetBoilerPumpCapacity();
	double boiler_pump_eff = GetBoilerPumpEfficiency();
	double rem_eff = 1.0;
	double rem_cap = 1.0;
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (!m_components.at(i).IsOperational())
		{
			rem_eff -= m_components.at(i).GetEfficiencyReduction();
			rem_cap -= m_components.at(i).GetCapacityReduction();
		}
	}
	//efficiency and capacity reductions assumed equal for all components but
	//turbines and salt-to-steam trains
	m_cycle_capacity = std::max(0., 
		std::min(salt_pump_cap,
			std::min(water_pump_cap,
				std::min(boiler_pump_cap,
					std::min(turbine_cap,
						std::min(condenser_cap,
							std::min(sst_cap, rem_cap)
						)
					)
				)
			) 
		) 
	);
	m_cycle_efficiency = std::max(
		0., salt_pump_eff*water_pump_eff*boiler_pump_eff*turbine_eff*condenser_eff*rem_eff
	); 
}

double PowerCycle::GetCycleCapacity()
{
	/* accessor for current cycle capacity. */
	return m_cycle_capacity;
}

double PowerCycle::GetCycleEfficiency()
{
	/* accessor for current cycle efficiency. */
	return m_cycle_efficiency;
}

void PowerCycle::TestForComponentFailures(double ramp_mult, int t, std::string start, std::string mode)
{
	/*
	Determines whether a component is to fail, based on current dispatch.
	ramp_mult - ramping penalty used to accelerate component wear
	t -- time period index
	start -- indicates plant start (Hot, Warm, Cold, or None)
	mode -- string indicating operating mode (e.g., Offline, Standby)
	*/
	double hazard_increase = 0.;
	double hazard_multiplier = 100.; // multiplier due to NREL Cycling Cost EFOR being too low
	if (start == "HotStart")
		hazard_increase = m_current_cycle_state.hot_start_penalty*hazard_multiplier;
	else if (start == "WarmStart")
		hazard_increase = m_current_cycle_state.warm_start_penalty*hazard_multiplier;
	else if (start == "ColdStart")
		hazard_increase = m_current_cycle_state.cold_start_penalty*hazard_multiplier;
	for (size_t i = 0; i < m_components.size(); i++)
	{ 
		if (m_components.at(i).IsOperational())
		{
			m_components.at(i).TestForFailure(
				m_sim_params.steplength, ramp_mult, *m_life_gen,
				*m_repair_gen, *m_binary_gen, t,
				hazard_increase, mode, m_current_scenario
			);
			//if a failure occurred, and the component is a pump, 
			//create a pump switching event as well.
			if ( (!m_components.at(i).IsOperational()) && (
				m_components.at(i).GetType() == "Molten salt pump" ||
				m_components.at(i).GetType() == "Boiler pump" ||
				m_components.at(i).GetType() == "Water pump" 
				)
			)
			{
				AddPumpSwitchingEvent(
					t,
					m_components.at(i).GetName(),
					m_components.at(i).GetType(),
					true,
					0.0
				);
			}
		}
	}
}

bool PowerCycle::AllComponentsOperational()
{

    /*
	retval -- a boolean that is True if no components have experienced
    a failure that has not yet been repaired, and False otherwise.
	*/

    bool ok = true;

    for( size_t i=0; i<m_components.size(); i++)
        ok = ok && m_components.at(i).IsOperational();

    return ok;
    
}

double PowerCycle::GetMaxComponentDowntime()
{
	/* 
	returns the maximum downtime remaining of any component in the plant.
	*/
	double t = 0;

	for (size_t i = 0; i < m_components.size(); i++)
	{
		t = std::max(t, m_components.at(i).GetDowntimeRemaining());
	}

	return t;
}

void PowerCycle::PlantMaintenanceShutdown(int t, bool reset_time, bool record, 
		double duration)
{
    /*
	creates a maintenance event that lasts for a fixed duration.  No
    power cycle operation take place at this time. 
    t -- period index
	reset_time -- true if the maintenance clock should be reset, false o.w.
    record -- true if outputting failure event to failure dictionary, false o.w.
    duration -- length of outage; this is only used when reset_time==false
	*/
	std::string label;
	if (reset_time)
	{
		label = "MAINTENANCE";
		double penalty_reduction = (
			m_begin_cycle_state.hot_start_penalty * 
			m_sim_params.num_annual_starts
			);
		for (size_t i = 0; i < m_components.size(); i++)
		{
			m_components.at(i).Shutdown(m_current_cycle_state.maintenance_duration);
			m_components.at(i).PerformMaintenance(penalty_reduction);
		}
	}
	else
	{
		label = "UNPLANNEDMAINTENANCE";
		for (size_t i = 0; i<m_components.size(); i++)
			m_components.at(i).Shutdown(duration);
	}
	

	if (record)
	{
		m_failure_events[
			"S" + std::to_string(m_current_scenario) + "T" + std::to_string(t) + label
		] = failure_event(t, label, -1, duration, 0., 0., m_current_scenario);
		m_failure_event_labels.push_back(
			"S" + std::to_string(m_current_scenario) + "T" + std::to_string(t) + label
		);
	}

	if (reset_time)
	    m_current_cycle_state.hours_to_maintenance = m_current_cycle_state.maintenance_interval * 1.0;

}

void PowerCycle::AddPumpSwitchingEvent(
	int t, 
	std::string component, 
	std::string pump_type, 
	bool record,
	double switch_time
)
{
	/* 
	Create a short downtime associated with the switching of pumps, 
	that changes the pump efficiencies.
    t -- period index
	component -- component identifier
	pump_type -- component type (salt, boiler, or water pump)
    record -- true if outputting failure event to failure dictionary, false o.w.
	 */
	
	//generate the time it takes to make the pump switch, if this is new.
	//otherwise, read in the duration as intput.
	double var, duration;
	if (record)
	{
		var = m_repair_gen->getVariate();
		duration = -log(1 - var) * m_pump_switch_mean_time;
	}
	else
		duration = switch_time;

	//apply the switch to the appropriate pump type; if there is an existing
	//switch, override the time remaining if the new time is greater.
	if (pump_type == "Molten salt pump")
	{
		m_current_cycle_state.salt_pump_switch_time = std::max(
			m_current_cycle_state.salt_pump_switch_time, duration
		);
	}
	else if (pump_type == "Boiler pump")
	{
		m_current_cycle_state.boiler_pump_switch_time = std::max(
			m_current_cycle_state.boiler_pump_switch_time, duration
		);
	}
	else if (pump_type == "Water pump")
	{
		m_current_cycle_state.water_pump_switch_time = std::max(
			m_current_cycle_state.water_pump_switch_time, duration
		);
	}
	else
	{
		std::exception("Bad pump type passed to pump switch event.");
	}
	
	if (record)
	{
		std::string label = component + "-SWITCH";
		m_failure_events[
			"S" + std::to_string(m_current_scenario) + "T" + std::to_string(t) + label
		] = failure_event(t, label, -1, duration, 0., 0., m_current_scenario);
			m_failure_event_labels.push_back(
				"S" + std::to_string(m_current_scenario) + "T" + std::to_string(t) + label
			);
	}
}

void PowerCycle::AdvanceDowntime(std::string mode)
{

    /*
	When the plant is not operational, advances time by a period.  This
    updates the repair time and/or maintenance time remaining in the plant.
	mode -- string indicating operating mode (e.g., Offline, Standby)
	*/
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (!m_components.at(i).IsOperational())
			m_components.at(i).AdvanceDowntime(m_sim_params.steplength, mode);
	}
}

double PowerCycle::GetRampMult(double thermal_out)
{

	/*
	Returns the ramping penalty, a multiplier applied to the lifetime
	expended by each component in the plant.  If the change in power output
	compared to the previous time period is larger than the ramp rate threshold
	provided as input, the ramping penalty is returned; otherwise, a
	multiplier of 1 (i.e., no penalty due to ramping) is returned.

	thermal_out -- thermal power out for current time period
	retval -- floating point multiplier
	*/
	if (thermal_out <= m_sim_params.epsilon)
		return 1.0;
	if (std::fabs(thermal_out - m_current_cycle_state.thermal_output) >= m_ramp_threshold_min)
	{
		double ramp_penalty = m_ramping_penalty_min + (m_ramping_penalty_max - m_ramping_penalty_min)*
			(std::fabs(thermal_out - m_current_cycle_state.thermal_output) - m_ramp_threshold_min ) / (m_ramp_threshold_max - m_ramp_threshold_min);
		ramp_penalty = std::min(ramp_penalty, m_ramping_penalty_max);
		return ramp_penalty;
	}
	return 1.0;

}

void PowerCycle::OperateComponents(double ramp_mult, int t, std::string start, std::string mode)
{

    /*
	Simulates operation by reducing the life of each component according
    to the ramping multiplier given; this also includes any cumulative 
    penalties on the multiplier due to hot, warm or cold starts.
    
    ramp_mult -- ramping penalty (multiplier for life degradation)
    t -- period index (indicator of whether read-only or not)
    mode -- string indicating operating mode (e.g., Offline, Standby)
    
	*/
    //print t - m_sim_params.read_periods
	double hazard_increase = 0.;
	if (start == "HotStart")
		hazard_increase = m_current_cycle_state.hot_start_penalty;
	else if (start == "WarmStart")
		hazard_increase = m_current_cycle_state.warm_start_penalty;
	else if (start == "ColdStart")
		hazard_increase = m_current_cycle_state.cold_start_penalty;
    //bool read_only = (t < m_sim_params.read_periods);
	for (size_t i = 0; i < m_components.size(); i++) 
	{
		if (m_components.at(i).IsOperational())
			m_components.at(i).Operate(
				m_sim_params.steplength, ramp_mult,
				t > m_results.period_of_last_repair[m_current_scenario], 
				hazard_increase, mode
			);
		else
		{
			m_components.at(i).AdvanceDowntime(m_sim_params.steplength, mode);
		}
	}
}

void PowerCycle::ResetHazardRates()
{
    /*
	resets the plant (restores component hazard rates
	to "as-good-as-new")
	*/
	for (size_t i = 0; i < m_components.size(); i++)
		m_components.at(i).ResetHazardRate();
}

std::unordered_map<std::string, ComponentStatus> PowerCycle::GetComponentStates()
{
	/*
	Stores the current state of each component, as of 
	the end of the read-in periods.
	*/
	std::unordered_map<std::string, ComponentStatus> component_states;
	for (
		std::vector< Component >::iterator it = m_components.begin(); 
		it != m_components.end(); 
		it++
		)
	{
		component_states[it->GetName()] = it->GetState();
	}
	return component_states;
}

std::string PowerCycle::GetStartMode(int t)
{
	/* 
	returns the start mode as a string, or "none" if there is no start.
	t -- time period index
	*/
	double power_out = m_dispatch.at("cycle_power").at(t);
	if (power_out > m_sim_params.epsilon)
	{
		if (IsOnline())
			return "None";
		if (IsOnStandby())
			return "HotStart";
		else if (m_current_cycle_state.downtime <= m_current_cycle_state.downtime_threshold)
			return "WarmStart";
		return "ColdStart";
	}
	return "None";
}

std::string PowerCycle::GetOperatingMode(int t)
{
	/*
	Returns the operating mode as a string.
	t -- time period index
	*/
	double power_out = m_dispatch.at("cycle_power").at(t);
	double standby = m_dispatch.at("standby").at(t);
	if (power_out > m_sim_params.epsilon)
	{
		if (IsOnline())
			if (m_current_cycle_state.time_online <= 1.0 - m_sim_params.epsilon)
				return "OF"; //in the first hour of power cycle operation
			else
				return "OO"; //ongoing (>1 hour) power cycle operation
		return "OS";  //starting power cycle operation
	}
	else if (standby >= 0.5)
	{
		if (IsOnStandby())
			if (m_current_cycle_state.time_in_standby <= 1.0 - m_sim_params.epsilon)
				return "SF"; //in first hour of standby
			else
				return "SO"; // ongoing standby (>1 hour)
		return "SS";  // if not currently on standby, then starting standby
	}
	return "OFF";
}

void PowerCycle::ReadInComponentFailures(int t)
{
	/*
	Reads in component failures from failure_events dictionary object.
	t -- time period index
	*/
	for (size_t j = 0; j < m_components.size(); j++)
	{
		for (size_t k = 0; k < m_components.at(j).GetFailureTypes().size(); k++)
		{
			if (m_failure_events.find(
				"S" + std::to_string(m_current_scenario) + "T" + 
				std::to_string(t) + m_components.at(j).GetName() + 
				"F" + std::to_string(k)) != m_failure_events.end()
				)
			{
				std::string label = (
					"S" + std::to_string(m_current_scenario) + 
					"T" + std::to_string(t) + m_components.at(j).GetName() + 
					"F" + std::to_string(k)
					);
				m_components.at(j).ReadFailure(
					m_failure_events[label].duration,
					m_failure_events[label].new_life,
					m_failure_events[label].fail_idx
				);

				if (m_sim_params.print_output)
					output_log.push_back(util::format(
						"Failure Read: %d, %d, %s",
						t, m_sim_params.read_periods, 
						m_failure_events[label].print().c_str())
					);

				//if the component is a pump, read in the switching event too
				if (
					m_components.at(j).GetType() == "Molten salt pump" ||
					m_components.at(j).GetType() == "Boiler pump" ||
					m_components.at(j).GetType() == "Water pump"
					)
				{
					label = (
						"S" + std::to_string(m_current_scenario) +
						"T" + std::to_string(t) + m_components.at(j).GetName() +
						"-SWITCH"
						);
					AddPumpSwitchingEvent(
						t,
						m_components.at(j).GetName(),
						m_components.at(j).GetType(),
						false,
						m_failure_events[label].duration
					);
				}
			}
		}
	}
}

void PowerCycle::ReadInMaintenanceEvents(int t)
{
	/* 
	Reads in planned maintenance events from failure_events dictionary object.
	t -- time period index
	*/
	if (
		m_failure_events.find("S" + std::to_string(m_current_scenario) + "T" + std::to_string(t) + "MAINTENANCE")
		!= m_failure_events.end()
		)
	{
		PlantMaintenanceShutdown(t, true, false);
	}
	//Read in unplanned maintenance events, if any
	if (
		m_failure_events.find("S" + std::to_string(m_current_scenario) + "T" + std::to_string(t) + "UNPLANNEDMAINTENANCE")
		!= m_failure_events.end()
		)
	{
		PlantMaintenanceShutdown(
			t, false, false,
			m_failure_events["S" + std::to_string(m_current_scenario) + "T" + std::to_string(t) + "UNPLANNEDMAINTENANCE"].duration
		);
	}
}

int PowerCycle::FirstPeriodOfDifference(
	std::vector<double> cycle_efficiencies,
	std::vector<double> cycle_capacities
)
{
	for (int i = 0; i < m_sim_params.sim_length; i++)
	{
		if (
			std::abs(cycle_efficiencies.at(i) - m_results.cycle_efficiency[m_current_scenario].at(i)) > m_sim_params.epsilon ||
			std::abs(cycle_capacities.at(i) - m_results.cycle_capacity[m_current_scenario].at(i)) > m_sim_params.epsilon
			)
		{
			return i;
		}
	}
	return -1;
}

void PowerCycle::RunDispatch()
{
    /*
	runs dispatch for entire time horizon.
    failure_file -- file to output failures
    retval -- array of binary variables that are equal to 1 if the plant 
        is able to operate (i.e., no maintenance or repair events in 
        progress), and 0 otherwise.  This includes the read-in period.
    
	*/
	m_new_failure_occurred = false;
	m_new_repair_occurred = false;
    std::vector< double > cycle_capacities( m_sim_params.sim_length, 0 );
	std::vector< double > cycle_efficiencies( m_sim_params.sim_length, 0 );
	double power_output = 0.;
	double thermal_output = 0.;
	for( int t = 0; t < m_sim_params.sim_length; t++)
    {
		ResetCycleEventFlags();
		//Shut all components down for maintenance if such an event is 
		//read in inputs, or the hours to maintenance is <= zero.
		//record the event at the period to be read in next.
		if( m_current_cycle_state.hours_to_maintenance <= 0  && t > m_results.period_of_last_repair[m_current_scenario] )
        {
            PlantMaintenanceShutdown(t, true, true);
        }
		
		
		//Read in any component failures, if in the read-only stage.
		if (t <= m_results.period_of_last_repair[m_current_scenario] ||
			t <= m_results.period_of_last_failure[m_current_scenario])
		{
			ReadInComponentFailures(t);	
			ReadInMaintenanceEvents(t);
		}
		
		// If cycle Capacity is zero or there is no power output, advance downtime.  
		// Otherwise, check for failures, and operate if there is still Capacity.
		power_output = m_dispatch.at("cycle_power").at(t);
		thermal_output = m_dispatch.at("thermal_power").at(t);
		SetCycleCapacityAndEfficiency(m_dispatch.at("ambient_temperature").at(t));
		std::string start = GetStartMode(t);
		std::string mode = GetOperatingMode(t);
		if (m_cycle_capacity < m_sim_params.epsilon)
		{
			power_output = 0.0;
			mode = "OFF";
		}
		//ajz: This was formerly for only periods after read-in
		if (t > m_results.period_of_last_repair[m_current_scenario]  && 
			t > m_results.period_of_last_failure[m_current_scenario])
		{
			double ramp_mult = GetRampMult(thermal_output);
			TestForComponentFailures(ramp_mult, t, start, mode);
			SetCycleCapacityAndEfficiency(m_dispatch.at("ambient_temperature").at(t));
			//if the cycle Capacity is set to zero, this means the plant is in maintenace
			//or a critical failure has occurred, so shut the plant down.
			if (m_cycle_capacity < m_sim_params.epsilon)
			{
				power_output = 0.0;
				mode = "OFF";
			}
			else if (m_cycle_capacity < m_shutdown_capacity || m_cycle_efficiency < m_shutdown_efficiency)
			{
				PlantMaintenanceShutdown(t, false, true, GetMaxComponentDowntime());
			}
			else if (
				mode == "OFF" && (
					m_cycle_capacity < m_no_restart_capacity ||
					m_cycle_efficiency < m_no_restart_efficiency
					)
				)
			{
				PlantMaintenanceShutdown(t, false, true, GetMaxComponentDowntime());
			}
			SetCycleCapacityAndEfficiency(m_dispatch.at("ambient_temperature").at(t));
			if (m_cycle_capacity  <= 1.0 - DBL_EPSILON)
			{
				power_output = std::min(power_output, m_cycle_capacity*m_current_cycle_state.capacity);
				thermal_output = std::min(thermal_output, m_cycle_capacity*m_current_cycle_state.thermal_capacity);
			}
		}
		OperatePlant(power_output, thermal_output, t, start, mode);
		cycle_capacities[t] = m_cycle_capacity;
		cycle_efficiencies[t] = m_cycle_efficiency;
		if (t > m_results.period_of_last_failure[m_current_scenario] &&
			NewFailureOccurred())
		{
			m_new_failure_occurred = true;
			m_results.period_of_last_failure[m_current_scenario] = t;
			if (m_sim_params.stop_at_first_failure)
			{
				for (size_t i = 0; i < m_components.size(); i++)
				{
					if (!m_components.at(i).IsOperational())
					{
						m_components.at(i).SetDowntimeRemaining(
							m_components.at(i).GetMeanRepairTime() +
							m_components.at(i).GetCooldownTime()
						);
					}
				}
				for (int tp = t + 1; tp < m_sim_params.sim_length; tp++)
				{
					//We assume that components with online repairs may 
					//continue to be repaired and will be back online after 
					//the mean repair time.  Any other components won't be 
					//repaired for the rest of the day.
					SetCycleCapacityAndEfficiency(m_dispatch.at("ambient_temperature").at(t));
					AdvanceDowntime("OO");
					cycle_capacities[tp] = m_cycle_capacity * 1.0;
					cycle_efficiencies[tp] = m_cycle_efficiency * 1.0;
				}
				break;
			}
		}
		if (t > m_results.period_of_last_repair[m_current_scenario] &&
			NewRepairOccurred())
		{
			//If we get to this point, a new repair has ocurred.
			//Set all component downtimes to their mean downtime, 
			//and assume no new failures for the rest of the time
			//horizon.  
			m_new_repair_occurred = true;
			m_results.period_of_last_repair[m_current_scenario] = t;
			if (m_sim_params.stop_at_first_repair)
			{
				for (size_t i = 0; i < m_components.size(); i++)
				{
					if (!m_components.at(i).IsOperational())
					{
						m_components.at(i).SetDowntimeRemaining(
							m_components.at(i).GetMeanRepairTime() + 
							m_components.at(i).GetCooldownTime()
						);
					}
				}
				for (int tp = t+1; tp < m_sim_params.sim_length; tp++)
				{
					//We assume that components with online repairs may 
					//continue to be repairedand will be back online after 
					//the mean repair time.  Any other components won't be 
					//repaired for the rest of the day.
					SetCycleCapacityAndEfficiency(m_dispatch.at("ambient_temperature").at(t));
					AdvanceDowntime("OO");
					cycle_capacities[tp] = m_cycle_capacity * 1.0;
					cycle_efficiencies[tp] = m_cycle_efficiency * 1.0;
				}
				break;
			}
		}
    }
	//One additional check: if no new failures or repairs occurred, but the 
	//last repair duration did not match the last run in the simulation, the 
	//optimization model needs to rerun.  In this case, the period of last 
	//failure and repair will be set to the simulation length.  This can only
	//happen if the last periods efficiency/capacity do not match those of the
	//results from the last run, or there would be a prior event triggered.
	if (!m_new_failure_occurred && !m_new_repair_occurred && (
		cycle_efficiencies[m_sim_params.sim_length - 1] != m_results.cycle_efficiency[m_current_scenario].at(m_sim_params.sim_length - 1) ||
		cycle_capacities[m_sim_params.sim_length - 1] != m_results.cycle_capacity[m_current_scenario].at(m_sim_params.sim_length - 1)
		))
	{
		
		int effp = FirstPeriodOfDifference(cycle_efficiencies,cycle_capacities);
		m_results.period_of_last_failure[m_current_scenario] = effp;
		m_results.period_of_last_repair[m_current_scenario] = effp;
		m_new_repair_occurred = true;
		m_new_failure_occurred = true;
	}
	StoreScenarioResults(cycle_efficiencies, cycle_capacities);
}

void PowerCycle::OperatePlant(double power_out, 
	double thermal_out, int t, std::string start, std::string mode)
{

    /*
	
    Simulates operation by:
        (i) advancing downtime one period if any components are under 
        repair or maintenance;
        (ii) producing a component failure event, followed by (i), and
        recording the event, if the period is not read-only and
        dispatch provided as input would reduce a component's life to zero;
        (iii) operating the plant and updating component lifetimes 
        otherwise.
		power_out -- electrical power output for plant (determines ramping multiple)
		thermal_out -- thermal power output for plant (determines ramping multiple)
		t -- time index
		mode -- operation mode
	*/
	double ramp_mult = GetRampMult(thermal_out);
	m_current_cycle_state.thermal_output = thermal_out;
	m_current_cycle_state.power_output = power_out;
	m_current_cycle_state.salt_pump_switch_time = (
		std::max(
			0.0, m_current_cycle_state.salt_pump_switch_time - 
			m_sim_params.steplength
		)
		);
	m_current_cycle_state.boiler_pump_switch_time = (
		std::max(
			0.0, m_current_cycle_state.boiler_pump_switch_time -
			m_sim_params.steplength
		)
		);
	m_current_cycle_state.water_pump_switch_time = (
		std::max(
			0.0, m_current_cycle_state.water_pump_switch_time -
			m_sim_params.steplength
		)
		);
	if (mode == "OFF")
	{
		m_current_cycle_state.is_online = false;
		m_current_cycle_state.is_on_standby = false;
		m_current_cycle_state.downtime += m_sim_params.steplength;
		m_current_cycle_state.time_in_standby = 0.0;
		m_current_cycle_state.time_online = 0.0;
		AdvanceDowntime("OFF");
		return;
	}
	else if (mode == "SS") //standby - start
	{
		m_current_cycle_state.is_online = false;
		m_current_cycle_state.is_on_standby = true;
		m_current_cycle_state.time_in_standby = m_sim_params.steplength;
		m_current_cycle_state.downtime = 0.0;
		m_current_cycle_state.time_online = 0.0;
	}
	else if (mode == "SF" || mode == "SO") //standby - first hour; standby ongoing (>1 hour)
	{
		m_current_cycle_state.is_online = false;
		m_current_cycle_state.is_on_standby = true;
		m_current_cycle_state.time_in_standby += m_sim_params.steplength;
		m_current_cycle_state.downtime = 0.0;
		m_current_cycle_state.time_online = 0.0;
	}
	else if (mode == "OS") //online - start
	{
		m_current_cycle_state.is_online = true;
		m_current_cycle_state.is_on_standby = false;
		m_current_cycle_state.time_in_standby = 0.0;
		m_current_cycle_state.downtime = 0.0;
		m_current_cycle_state.time_online = m_sim_params.steplength;
		m_current_cycle_state.hours_to_maintenance -= m_sim_params.steplength;
	}
	else if (mode == "OF" || mode == "OO") //standby - first hour; standby ongoing (>1 hour)
	{
		m_current_cycle_state.is_online = true;
		m_current_cycle_state.is_on_standby = false;
		m_current_cycle_state.time_in_standby = 0.0;
		m_current_cycle_state.downtime = 0.0;
		m_current_cycle_state.time_online += m_sim_params.steplength;
		m_current_cycle_state.hours_to_maintenance -= m_sim_params.steplength;
	}
	else
		throw std::runtime_error("invalid operating mode.");
	OperateComponents(ramp_mult, t, start, mode); 

}

void PowerCycle::SingleScen(bool read_state_from_file, bool read_from_memory,
	bool init)
{
    /*failure_file = open(
        os.path.join(m_sim_params.print_output_dir,"component_failures.csv"),'w'
        )*/
	if (read_state_from_file)
	{
		ReadStateFromFiles(init);
	}
	else if (read_from_memory)
	{
		ReadCycleStateFromResults();
		ReadComponentStatus(m_results.component_status[m_current_scenario]);
	}
	m_start_component_status = GetComponentStates();
	RunDispatch();
	if ((m_new_repair_occurred && m_sim_params.stop_at_first_repair) 
		|| (m_new_failure_occurred && m_sim_params.stop_at_first_failure) )
	{
		RevertToStartState(false);
	}
	else
	{
		StoreCycleState();
		m_results.period_of_last_failure[m_current_scenario] = -1;
		m_results.period_of_last_repair[m_current_scenario] = -1;
		if (m_file_settings.output_to_files)
		{
			WriteFailureStats();
			WriteFailuresFile();
		}
		ClearFailureEvents();
	}
	if (m_file_settings.output_ampl_file)
		WriteAMPLParams();
	RecordFinalState();
}

void PowerCycle::GetSummaryResults()
{
	/*
	Calculates the time series of the sample mean 
	efficiency and capacity of the power cycle.
	*/
	m_results.avg_cycle_efficiency.clear();
	m_results.avg_cycle_capacity.clear();
	double avg_eff = 0.; 
	double avg_cap = 0.;
	double avg_labor = 0.;
	double avg_turb_cap = 0.;
	double avg_turb_eff = 0.;
	for (int s = 0; s < m_sim_params.num_scenarios; s++)
	{
		avg_labor += m_results.labor_costs[s];
		avg_turb_cap += m_results.turbine_capacity[s];
		avg_turb_eff += m_results.turbine_efficiency[s];
	}
	m_results.avg_labor_cost = avg_labor / m_sim_params.num_scenarios;
	m_results.avg_turbine_capacity = avg_turb_cap / m_sim_params.num_scenarios;
	m_results.avg_turbine_efficiency = avg_turb_eff / m_sim_params.num_scenarios;
	for (int t = 0; t < m_sim_params.sim_length; t++)
	{
		avg_eff = 0;
		avg_cap = 0;
		for (int s = 0; s < m_sim_params.num_scenarios; s++)
		{
			avg_eff += m_results.cycle_efficiency[s].at(t);
			avg_cap += m_results.cycle_capacity[s].at(t);
		}
		m_results.avg_cycle_efficiency.push_back(avg_eff / m_sim_params.num_scenarios);
		m_results.avg_cycle_capacity.push_back(avg_cap / m_sim_params.num_scenarios);
	}
	
}

double PowerCycle::GetLaborCosts(size_t start_fail_idx)
{
	/*
	Calculates the cost of labor associated with all the failures that occurred
	in the current scenario.

	start_fail_idx -- starting index of failure event labels to check; this value
	   is the size of m_failure_event_labels at the start of the scenario.

	retval - estimated labor costs, in dollars
	*/
	double hours = 0;
	
	for (size_t i = start_fail_idx; i < m_failure_event_labels.size(); i++)
	{
		hours += m_failure_events[m_failure_event_labels[i]].labor;
	}
	return hours * m_sim_params.hourly_labor_cost;
}

void PowerCycle::StoreScenarioResults(std::vector <double> cycle_efficiencies,
	std::vector <double> cycle_capacities)
{
	/* 
	Assigns key statistics of cycle performance to the results data structure.
	*/
	m_results.cycle_efficiency[m_current_scenario] = cycle_efficiencies;
	m_results.cycle_capacity[m_current_scenario] = cycle_capacities;
	m_results.labor_costs[m_current_scenario] = GetLaborCosts(0);
	m_results.turbine_capacity[m_current_scenario] = GetTurbineCapacity(true, true);
	m_results.turbine_efficiency[m_current_scenario] = GetTurbineEfficiency(true, true);
	m_results.expected_time_to_failure = GetEstimatedMinimumLifetime();
	m_results.expected_starts_to_failure = GetExpectedStartsToNextFailure();
}

void PowerCycle::Simulate(
	bool read_state_from_file, 
	bool read_state_from_memory,
	bool run_only_previous_failures,
	bool init
)
{
	/*
	Generates a collection of Monte Carlo realizations of failure and
	maintenance events in the power block. 
	read_state_from_file -- reads start state from, and write out states to, 
			filesystem if true
	read_state_from memory -- reads start state from, and write out state to,
			memory / results object if true
	run_only_previous_failures -- only runs scenarios that generated a new 
			failure in the last iteration if true

	Note: labor costs are in aggregate and not scenario-specific.
	*/
	for (int i = 0; i < m_sim_params.num_scenarios; i++)
	{
		m_current_scenario = i;
		if (!run_only_previous_failures || m_results.period_of_last_failure[i] > -1)
		{
			SingleScen(read_state_from_file, read_state_from_memory, init);

			//Record failure events
			m_results.failure_events[m_current_scenario] = m_failure_events;
			m_results.failure_event_labels[m_current_scenario] = m_failure_event_labels;
		}
	}
	
	//Obtain Summary Statistics
	GetSummaryResults();
}

void PowerCycle::ResetPlant()
{
	/* 
	Resets the plant and its components to initial conditions.
	*/
	m_current_cycle_state.is_online = false;
	m_current_cycle_state.time_online = 0.;
	m_current_cycle_state.is_on_standby = false;
	m_current_cycle_state.time_in_standby = 0.;
	m_current_cycle_state.power_output = 0.;
	m_current_cycle_state.hours_to_maintenance = m_current_cycle_state.maintenance_interval;
	for (size_t c = 0; c < GetComponents().size(); c++)
	{
		GetComponents().at(c).Reset(*m_life_gen);
	}
}

void PowerCycle::PrintComponentStatus()
{
	/*
	checks lifetimes and status of each component. for testing only.
	*/
	for (int i = 0; i < m_components.size(); i++)
	{
		std::cerr << "\n" << m_components.at(i).GetName() << " ";
		for (int j = 0; j < m_components.at(i).GetLifetimesAndProbs().size(); j++)
		{
			std::cerr << m_components.at(i).GetLifetimesAndProbs().at(j) << " ";
		}
		std::cerr << "\n";
		std::cerr << m_components.at(i).GetDowntimeRemaining();
		std::cerr << "\n";
		for (int j = 0; j < m_components.at(i).GetLifetimesAndProbs().size(); j++)
		{
			std::cerr << m_components.at(i).GetFailureTypes().at(j).GetFailureDist()->IsBinary() << " ";
		}
		std::cerr << "\n";
	}
}

void PowerCycle::ClearFailureEvents()
{
	for (std::string f : m_failure_event_labels)
		m_all_failures.push_back(m_failure_events[f]);
	m_failure_events.clear();
	m_failure_event_labels.clear();
	m_results.period_of_last_failure[m_current_scenario] = -1;
	m_results.period_of_last_repair[m_current_scenario] = -1;
}

bool PowerCycle::AnyFailuresOccurred()
{
	for (int i = 0; i < m_sim_params.num_scenarios; i++)
	{
		if (m_results.period_of_last_failure[i] > -1)
			return true;
	}
	return false;
}

double PowerCycle::GetEstimatedMinimumLifetime(double frac_operational)
{
	/*
	Estimates the number of hours before the next failure occurs, assuming
	operation of the power cycle for a fixed fraction of each day.  Does not 
	account for the increase in hazard rate that accompanies cycle starts.
	*/
	double min_life = INFINITY;
	for (Component c : m_components)
	{
		for (size_t i = 0; i < c.GetFailureTypes().size(); i++)
		{
			if (!c.GetFailureTypes().at(i).GetFailureDist()->IsBinary())
			{
				min_life = std::min(
					min_life, c.GetLifetimesAndProbs().at(i) / c.GetHazardRate()
				);
			}
		}
	}
	return min_life / frac_operational;
}

double PowerCycle::GetExpectedStartsToNextFailure()
{
	/*
	Estimates the number of starts before the next failure occurs, assuming
	operation of the power cycle for a fixed fraction of each day.  Does not 
	account for the increase in hazard rate that accompanies cycle starts.
	*/
	double p = 1.0;
	for (Component c : m_components)
	{
		for (size_t i=0; i<c.GetFailureTypes().size(); i++)
		{
			if (c.GetFailureTypes().at(i).GetFailureDist()->IsBinary())
			{
				p *= (1 - c.GetLifetimesAndProbs().at(i)*c.GetHazardRate());
			}
		}
	}
	if (1.0 - p < m_sim_params.epsilon)
	{
		return INFINITY;
	}
	return 1.0 / (1.0 - p);
}

void PowerCycle::AgePlant(double age)
{
	/*
	Ages the plant by running pre-specified dispatch for a collection of days. 
	To keep the runtime of this minimal, we assume "40 hours on, 20 hours off" 
	for dispatch, which yields 146 warm starts and 219 days of runtime per
	year of age.
	*/
	//skip this if age = 0.0
	if (age < m_sim_params.epsilon)
	{
		return;
	}

	//save old steplength and downtime threshold
	double t = m_sim_params.steplength*1.0;
	double threshold = m_current_cycle_state.downtime_threshold;

	//set aging steplength and downtime threshold
	m_sim_params.steplength = 20;
	m_current_cycle_state.downtime_threshold = 48;
	m_begin_cycle_state.downtime_threshold = 48;
	
	//determine number of start/stop cycles
	int num_cycles = (int)(146 * age);
	
	//set up dispatch
	m_dispatch["power_output"] = { 0, m_ramp_threshold_min*0.2, m_ramp_threshold_min*0.2 };
	m_dispatch["thermal_output"] = { 0, m_ramp_threshold_min*0.5, m_ramp_threshold_min*0.5 };
	m_dispatch["standby"] = { 0,0,0 };
	m_dispatch["ambient_temperature"] = { 0,0,0 };

	//run dispatch
	m_sim_params.stop_at_first_failure = false;
	m_sim_params.stop_at_first_repair = false;
	for (int i = 0; i < num_cycles; i++)
	{
		RunDispatch();
		m_results.period_of_last_failure[m_current_scenario] = -1;
		m_results.period_of_last_repair[m_current_scenario] = -1;
		ClearFailureEvents();
	}

	//stop to repair any components that have still failed
	double m = GetMaxComponentDowntime();
	if (m > m_sim_params.epsilon)
	{
		m_sim_params.steplength = 2 * m;
		AdvanceDowntime("OFF");
	}

	//reset steplength and downtime thresholds
	m_sim_params.steplength = t;
	m_current_cycle_state.downtime_threshold = threshold;
	m_begin_cycle_state.downtime_threshold = threshold;


	//store states
	StoreCycleState();

}


std::vector<std::string> PowerCycle::GetAllFailureEventLabels()
{
	std::vector<std::string> v = {};
	for (failure_event f : m_all_failures)
	{
		v.push_back(f.component + "-T:" + std::to_string(f.time) + "-D:" + std::to_string(f.duration));
	}
	return v;
}


std::vector<double> PowerCycle::GetAllFailureEventDowntimes()
{
	std::vector<double> v = {};
	for (failure_event f : m_all_failures)
	{
		v.push_back(f.duration);
	}
	return v;
}

std::vector<int> PowerCycle::GetAllFailureEventPeriods()
{
	std::vector<int> v = {};
	for (failure_event f : m_all_failures)
	{
		v.push_back(f.time);
	}
	return v;
}

