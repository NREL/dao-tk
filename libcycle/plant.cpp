#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include "plant.h"
#include "lib_util.h"


PowerCycle::PowerCycle()
{
	WELLFiveTwelve *gen(0);
	AssignGenerator(gen);
	SetSimulationParameters();
	SetPlantAttributes();
	GeneratePlantComponents();
}

void PowerCycle::Initialize()
{
	InitializeCyclingDists();
	GeneratePlantCyclingPenalties();
	ResetPlant();
	m_cycle_capacity = 1.;
	m_cycle_efficiency = 1.;
	for (int i = 0; i < m_sim_params.num_scenarios; i++)
	{
		m_results.period_of_last_failure[i] = -1;
	}
}

void PowerCycle::InitializeCyclingDists()
{
	/*
	Sets distributions from which cycling penalties are generated.
	*/
	m_hs_dist = BoundedJohnsonDist(1.044471, 0.256348, 4.137E-5, 5.163E-4, "BoundedJohnson");
	m_ws_dist = BoundedJohnsonDist(2.220435, 0.623145, 2.914E-5, 1.773E-3, "BoundedJohnson");
	m_cs_dist = BoundedJohnsonDist(0.469391, 0.581813, 3.691E-5, 2.369E-4, "BoundedJohnson");
}

void PowerCycle::AssignGenerator( WELLFiveTwelve *gen )
{
	/*
	Assigns an RNG object to the plant, which is used to generate component
	lifetimes, failure probabilities, tests fo binary failures, and repair times.
	Here, we use the WELL512 implementation by Panneton et al. (2006).
	*/
    m_gen = gen;
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
	double unif = m_gen->getVariate();
	SetHotStartPenalty(m_hs_dist.GetInverseCDF(unif));
	SetWarmStartPenalty(m_ws_dist.GetInverseCDF(unif));
	SetColdStartPenalty(m_cs_dist.GetInverseCDF(unif));
}

void PowerCycle::SetHotStartPenalty(double pen)
{
	m_current_cycle_state.hot_start_penalty = pen;
}

void PowerCycle::SetWarmStartPenalty(double pen)
{
	m_current_cycle_state.warm_start_penalty = pen;
}

void PowerCycle::SetColdStartPenalty(double pen)
{
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
	bool stop_at_first_failure
)
{
    m_sim_params.read_periods = read_periods;
    m_sim_params.sim_length = sim_length;
	m_sim_params.steplength = steplength;
	m_sim_params.epsilon = epsilon;
    m_sim_params.print_output = print_output;
	m_sim_params.num_scenarios = num_scenarios;
	m_sim_params.hourly_labor_cost = hourly_labor_cost;
	m_sim_params.stop_at_first_failure = stop_at_first_failure;
	m_current_scenario = 0;
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
	if (eff_cold.size() != num_streams + 1)
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
	if (eff_hot.size() != num_streams+1)
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
	m_start_component_status.clear();
	m_num_condenser_trains = 0;
	m_num_feedwater_heaters = 0;
	m_num_salt_pumps = 0;
	m_num_salt_steam_trains = 0;
	m_num_turbines = 0;
	m_num_water_pumps = 0;
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
	m_start_component_status.clear();
}

void PowerCycle::ReadCycleStateFromResults(int scen_idx)
{
	/*
	Reads cycle and component state from results stored in memory from previous run.
	*/
	m_current_cycle_state = m_results.plant_status[m_current_scenario];
	m_begin_cycle_state = m_results.plant_status[m_current_scenario];
	m_start_component_status = m_results.component_status[m_current_scenario];
	SetStartComponentStatus();
	m_gen->assignStates(scen_idx);
}

void PowerCycle::SetStartComponentStatus()
{
	/*
	Sets the status of all components to what is stored in
	m_start_component_status.
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
	m_begin_cycle_state.capacity = m_current_cycle_state.capacity;
	m_begin_cycle_state.cold_start_penalty = m_current_cycle_state.cold_start_penalty;
	m_begin_cycle_state.warm_start_penalty = m_current_cycle_state.warm_start_penalty;
	m_begin_cycle_state.hot_start_penalty = m_current_cycle_state.hot_start_penalty;
	m_begin_cycle_state.downtime = m_current_cycle_state.downtime;
	m_begin_cycle_state.downtime_threshold = m_current_cycle_state.downtime_threshold;
	m_begin_cycle_state.is_online = m_current_cycle_state.is_online;
	m_begin_cycle_state.is_on_standby = m_current_cycle_state.is_on_standby;
	m_begin_cycle_state.maintenance_duration = m_current_cycle_state.maintenance_duration;
	m_begin_cycle_state.maintenance_interval = m_current_cycle_state.maintenance_interval;
	m_begin_cycle_state.hours_to_maintenance = m_current_cycle_state.hours_to_maintenance;
	m_begin_cycle_state.temp_threshold = m_current_cycle_state.temp_threshold;
	m_begin_cycle_state.time_in_standby = m_current_cycle_state.time_in_standby;
	m_begin_cycle_state.time_online = m_current_cycle_state.time_online;
	m_begin_cycle_state.power_output = m_current_cycle_state.power_output;
}

void PowerCycle::StoreCycleState()
{
	StoreComponentState();
	StorePlantParamsState();
	m_gen->saveStates(m_current_scenario);
}



void PowerCycle::RecordFinalState()
{
	/* 
	Assigns current power cycle state to the beginning component state.  
	Done if no failures are found in the run.
	*/
	m_results.plant_status[m_current_scenario] = m_current_cycle_state;
	m_results.component_status[m_current_scenario] = GetComponentStates();
	m_gen->saveStates(m_current_scenario);
}

void PowerCycle::RevertToStartState()
{
	/*
	Assigns beginning power cycle state to the current state.
	Done if any component failures are found in the run, or if the 
	capacity or efficiency change from the prior run.
	*/
	m_current_cycle_state.capacity = m_begin_cycle_state.capacity;
	m_current_cycle_state.cold_start_penalty = m_begin_cycle_state.cold_start_penalty;
	m_current_cycle_state.warm_start_penalty = m_begin_cycle_state.warm_start_penalty;
	m_current_cycle_state.hot_start_penalty = m_begin_cycle_state.hot_start_penalty;
	m_current_cycle_state.downtime = m_begin_cycle_state.downtime;
	m_current_cycle_state.downtime_threshold = m_begin_cycle_state.downtime_threshold;
	m_current_cycle_state.is_online = m_begin_cycle_state.is_online;
	m_current_cycle_state.is_on_standby = m_begin_cycle_state.is_on_standby;
	m_current_cycle_state.maintenance_duration = m_begin_cycle_state.maintenance_duration;
	m_current_cycle_state.maintenance_interval = m_begin_cycle_state.maintenance_interval;
	m_current_cycle_state.hours_to_maintenance = m_begin_cycle_state.hours_to_maintenance;
	m_current_cycle_state.temp_threshold = m_begin_cycle_state.temp_threshold;
	m_current_cycle_state.time_in_standby = m_begin_cycle_state.time_in_standby;
	m_current_cycle_state.time_online = m_begin_cycle_state.time_online;
	m_current_cycle_state.power_output = m_begin_cycle_state.power_output;

	SetStartComponentStatus();

	m_gen->assignStates(m_current_scenario);
}

void PowerCycle::WriteStateToFiles(bool current, std::string component_filename, std::string plant_filename)
{
	/*
	Writes the current state of the plant to output files, one for all components and one for the plant.
	*/
	//components
	std::ofstream cfile;
	std::unordered_map <std::string, ComponentStatus> component_status;
	if (current)
		component_status = GetComponentStates();
	else
		component_status = m_start_component_status;
	cfile.open(component_filename);
	cfile << "hazard_rate,downtime_remaining,repair_event_time,lifesprobs\n";
	ComponentStatus status;
	for (Component c : GetComponents())
	{
		status = component_status[c.GetName()];
		cfile << status.hazard_rate << "," << status.downtime_remaining << "," << status.repair_event_time;
		for (double d : status.lifetimes)
		{
			cfile << "," << d;
		}
		cfile << "\n";
	}

	//plant state
	cycle_state cstate;
	if (current)
		cstate = m_current_cycle_state;
	else
		cstate = m_begin_cycle_state;

	std::ofstream pfile;
	pfile.open(plant_filename);
	pfile << "hours_to_maintenance,power_output,standby,time_online,time_in_standby,downtime,hs_penalty,ws_penalty,cs_penalty\n";
	pfile << cstate.hours_to_maintenance << "," << cstate.power_output << ",";
	pfile << cstate.is_on_standby << "," << cstate.time_online << ",";
	pfile << cstate.time_in_standby << "," << cstate.downtime << ",";
	pfile << cstate.hot_start_penalty << "," << cstate.warm_start_penalty << ",";
	pfile << cstate.cold_start_penalty << "\n";

}

void PowerCycle::ReadStateFromFiles(std::string component_filename, std::string plant_filename)
{
	//set up reading of component file
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
		if (split_line.size() > 3)
		{
			status.hazard_rate = std::stod(split_line[0]);
			status.downtime_remaining = std::stod(split_line[1]);
			status.repair_event_time = std::stod(split_line[2]);
			for (size_t i = 3; i < split_line.size(); i++)
			{
				status.lifetimes.push_back(std::stod(split_line[i]));
			}
			GetComponents().at(cindex).ReadStatus(status);
		}
		split_line.clear();
		cindex++;
	}
	std::ifstream pfile;
	pos = 0;
	pfile.open(plant_filename);
	std::string pline;
	getline(pfile, pline);
	getline(pfile, pline);
	while (pos != std::string::npos) {
		pos = pline.find(delimiter);
		token = pline.substr(0, pos);
		split_line.push_back(token);
		cline.erase(0, pos + delimiter.length());
	}
	m_current_cycle_state.hours_to_maintenance = std::stod(split_line[0]);
	m_current_cycle_state.power_output = std::stod(split_line[1]);
	m_current_cycle_state.is_on_standby = std::stod(split_line[2]);
	m_current_cycle_state.time_online = std::stod(split_line[3]);
	m_current_cycle_state.time_in_standby = std::stod(split_line[4]);
	m_current_cycle_state.downtime = std::stod(split_line[5]);
	m_current_cycle_state.hot_start_penalty = std::stod(split_line[6]);
	m_current_cycle_state.warm_start_penalty = std::stod(split_line[7]);
	m_current_cycle_state.cold_start_penalty = std::stod(split_line[8]);
	//std::vector <double> lifes_probs;
}

std::vector< Component > &PowerCycle::GetComponents()
{
    /*
	accessor for components list.
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
	return m_current_cycle_state.hours_to_maintenance;
}

std::vector< double >  PowerCycle::GetComponentDowntimes()
{
    /*
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

double PowerCycle::GetTimeInStandby()
{
	return m_current_cycle_state.time_in_standby;
}

double PowerCycle::GetTimeOnline()
{
	return m_current_cycle_state.time_online;
}

double PowerCycle::GetRampThreshold()
{
	return m_ramp_threshold;
}

double PowerCycle::GetSteplength()
{
	return m_sim_params.steplength;
}

std::unordered_map< std::string, failure_event > PowerCycle::GetFailureEvents()
{
	return m_failure_events;
}

std::vector<std::string> PowerCycle::GetFailureEventLabels()
{
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
	return m_sim_params.sim_length;
}

void PowerCycle::SetShutdownCapacity(double capacity) 
{
	m_shutdown_capacity = capacity;
}


void PowerCycle::SetNoRestartCapacity(double capacity)
{
	m_no_restart_capacity = capacity;
}


void PowerCycle::AddComponent(std::string name,
	std::string type,
	double repair_rate,
	double repair_cooldown_time,
	double capacity_reduction,
	double efficiency_reduction,
	double repair_cost,
	std::string repair_mode
)
{
	m_components.push_back(Component(name, type,
		repair_rate, repair_cooldown_time, &m_failure_events,
		capacity_reduction, efficiency_reduction, repair_cost, repair_mode,
		&m_failure_event_labels));
	if (type == "Turbine")
		m_turbine_idx.push_back(m_components.size() - 1);
	if (type == "Condenser train")
		m_condenser_idx.push_back(m_components.size() - 1);
	if (type == "Salt-to-steam train")
		m_sst_idx.push_back(m_components.size() - 1);
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
	std::string component_name, train_name;
	for (int j = 0; j < num_trains; j++)
	{
		train_name = "C" + std::to_string(m_num_condenser_trains);
		AddComponent(train_name + "-T", "Condenser train", 15.55, 0, 0, 0, 7.777, "S");
		for (int i = 1; i <= num_fans; i++)
		{
			component_name = train_name + "-F" + std::to_string(i);
			AddComponent(component_name, "Condenser fan", 35.5, 0, 0, 0, 7.777, "S");
			AddFailureType(component_name, "Fan Failure", "O", "gamma", 1, 841188);
		}
		for (int i = 1; i <= num_radiators; i++)
		{
			AddFailureType(train_name + "-T", "Radiator " + std::to_string(i) + " Failure", "O", "gamma", 1, 698976);
		}
	}
}

void PowerCycle::AddSaltToSteamTrains(int num_trains)
{
	std::string component_name;
	double capacity_reduction = 1.0 / num_trains;
	for (int i = 0; i < num_trains; i++)
	{
		m_num_salt_steam_trains += 1;
		component_name = "SST" + std::to_string(m_num_salt_steam_trains);
		AddComponent(component_name, "Salt-to-steam train", 2.14, 72, capacity_reduction, 0., 7.777, "A");
		AddFailureType(component_name, "Boiler External_Leak_Large_(shell)", "ALL", "gamma", 0.3, 75000000);
		AddFailureType(component_name, "Boiler External_Leak_Large_(tube)", "ALL", "gamma", 0.3, 10000000);
		AddFailureType(component_name, "Boiler External_Leak_Small_(shell)", "ALL", "gamma", 0.5, 10000000);
		AddFailureType(component_name, "Boiler External_Leak_Small_(tube)", "ALL", "gamma", 0.3, 1200000);
		AddFailureType(component_name, "Economizer External_Leak_Large_(shell)", "ALL", "gamma", 0.3, 75000000);
		AddFailureType(component_name, "Economizer External_Leak_Large_(tube)", "ALL", "gamma", 0.3, 10000000);
		AddFailureType(component_name, "Economizer External_Leak_Small_(shell)", "ALL", "gamma", 0.5, 10000000);
		AddFailureType(component_name, "Economizer External_Leak_Small_(tube)", "ALL", "gamma", 0.3, 1200000);
		AddFailureType(component_name, "Reheater External_Leak_Large_(shell)", "ALL", "gamma", 0.3, 75000000);
		AddFailureType(component_name, "Reheater External_Leak_Large_(tube)", "ALL", "gamma", 0.3, 10000000);
		AddFailureType(component_name, "Reheater External_Leak_Small_(shell)", "ALL", "gamma", 0.5, 10000000);
		AddFailureType(component_name, "Reheater External_Leak_Small_(tube)", "ALL", "gamma", 0.3, 1200000);
		AddFailureType(component_name, "Superheater External_Leak_Large_(shell)", "ALL", "gamma", 0.3, 75000000);
		AddFailureType(component_name, "Superheater External_Leak_Large_(tube)", "ALL", "gamma", 0.3, 10000000);
		AddFailureType(component_name, "Superheater External_Leak_Small_(shell)", "ALL", "gamma", 0.5, 10000000);
		AddFailureType(component_name, "Superheater External_Leak_Small_(tube)", "ALL", "gamma", 0.3, 1200000);
	}
}

void PowerCycle::AddFeedwaterHeaters(int num_fwh)
{
	std::string component_name;
	for (int i = 0; i < num_fwh; i++)
	{
		m_num_feedwater_heaters += 1;
		component_name = "FWH" + std::to_string(m_num_feedwater_heaters);
		AddComponent(component_name, "Feedwater heater", 2.14, 48., 0.05, 0.05, 7.777, "A");
		AddFailureType(component_name, "External_Leak_Large_(shell)", "O", "gamma", 0.3, 75000000);
		AddFailureType(component_name, "External_Leak_Large_(tube)", "O", "gamma", 0.3, 10000000);
		AddFailureType(component_name, "External_Leak_Small_(shell)", "O", "gamma", 0.5, 10000000);
		AddFailureType(component_name, "External_Leak_Small_(tube)", "O", "gamma", 0.3, 1200000);
	}
}

void PowerCycle::AddSaltPumps(int num_pumps)
{
	std::string component_name;
	for (int i = 0; i < num_pumps; i++)
	{
		m_num_salt_pumps += 1;
		component_name = "SP" + std::to_string(m_num_salt_pumps);
		AddComponent(component_name, "Molten salt pump", 0.5, 0., 1., 1., 7.777, "D");
		AddFailureType(component_name, "External_Leak_Large", "ALL", "gamma", 0.3, 37500000);
		AddFailureType(component_name, "External_Leak_Small", "ALL", "gamma", 1, 8330000);
		AddFailureType(component_name, "Fail_to_Run_<=_1_hour_(standby)", "OF", "gamma", 1.5, 3750);
		AddFailureType(component_name, "Fail_to_Run_>_1_hour_(standby)", "OO", "gamma", 0.5, 83300);
		AddFailureType(component_name, "Fail_to_Start_(standby)", "OS", "beta", 0.9, 599);
		//AddFailureType(component_name, "Fail_to_Start_(running)", "OS", "beta", 0.9, 449);
		//AddFailureType(component_name, "Fail_to_Run_(running)", "O", "gamma", 1.5, 300000);
	}
}

void PowerCycle::AddWaterPumps(int num_pumps)
{
	std::string component_name;
	for (int i = 0; i < num_pumps; i++)
	{
		m_num_water_pumps += 1;
		component_name = "WP" + std::to_string(m_num_water_pumps);
		AddComponent(component_name, "Water pump", 0.5, 40., 1., 1, 7.777, "D");
		AddFailureType(component_name, "External_Leak_Large", "ALL", "gamma", 0.3, 37500000);
		AddFailureType(component_name, "External_Leak_Small", "ALL", "gamma", 1, 8330000);
		AddFailureType(component_name, "Fail_to_Run_<=_1_hour_(standby)", "OF", "gamma", 1.5, 3750);
		AddFailureType(component_name, "Fail_to_Run_>_1_hour_(standby)", "OO", "gamma", 0.5, 83300);
		AddFailureType(component_name, "Fail_to_Start_(standby)", "OS", "beta", 0.9, 599);
	}
}

void PowerCycle::AddTurbines(int num_turbines)
{
	/*
	Adds turbines to the plant; assumes that the turbine-generator shaft shares a 
	single failure rate, based on anecdotal feedback of maintenance every ~6 years, and 
	the incidence of turbine failures being rare.
	*/
	std::string component_name;
	double capacity_reduction = 1.0 / num_turbines;
	for (int i = 0; i < num_turbines; i++)
	{
		m_num_turbines += 1;
		component_name = "T" + std::to_string(m_num_turbines);
		AddComponent(component_name, "Turbine", 32.7, 72, capacity_reduction, 0, 7.777, "D");
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
	int num_water_pumps,
	int num_turbines,
	std::vector<double> condenser_eff_cold,
	std::vector<double> condenser_eff_hot 
)
{
	/* 
	Generates all the components in the plant.  Aggregates the other 
	component-specific methods.  Starts by clearing any existing components,
	i.e., assumes that the plant specification in the arguments is complete.
	*/
	ClearComponents();
	if (condenser_eff_cold.size() != (num_condenser_trains+1) ||
		condenser_eff_hot.size() != (num_condenser_trains+1))
	{
		throw std::runtime_error("condenser efficiencies do not reconcile with number of trains.");
	}
	AddCondenserTrains(num_condenser_trains, fans_per_train, radiators_per_train);
	AddSaltToSteamTrains(num_salt_steam_trains);
	AddFeedwaterHeaters(num_fwh);
	AddSaltPumps(num_salt_pumps);
	AddWaterPumps(num_water_pumps);
	AddTurbines(num_turbines);
	m_condenser_efficiencies_cold = condenser_eff_cold;
	m_condenser_efficiencies_hot = condenser_eff_hot;
}

void PowerCycle::SetPlantAttributes(
	double maintenance_interval,
	double maintenance_duration,
	double downtime_threshold,
	double hours_to_maintenance,
	double power_output,
	bool current_standby,
	double capacity,
	double temp_threshold,
	double time_online,
	double time_in_standby,
	double downtime,
	double shutdown_capacity,
	double no_restart_capacity
)
{
    /*
	Initializes plant attributes.
    retval - none
    
	*/

    m_current_cycle_state.maintenance_interval = maintenance_interval;
    m_current_cycle_state.maintenance_duration = maintenance_duration;
    m_ramp_threshold = capacity * 0.2;  //using Gas-CC as source, Kumar 2012,
	m_ramp_threshold_min = 1.1*m_ramp_threshold;   //Table 1-1
	m_ramp_threshold_max = 2.0*m_ramp_threshold;   
    m_current_cycle_state.downtime_threshold = downtime_threshold;
    m_current_cycle_state.hours_to_maintenance = hours_to_maintenance;
    m_current_cycle_state.power_output = power_output;
    m_current_cycle_state.is_on_standby = current_standby;
    m_current_cycle_state.is_online = m_current_cycle_state.power_output > 0.;
	m_current_cycle_state.capacity = capacity;
	m_current_cycle_state.time_in_standby = time_in_standby;
	m_current_cycle_state.time_online = time_online;
	m_condenser_temp_threshold = temp_threshold;
	m_current_cycle_state.downtime = downtime;
	m_shutdown_capacity = shutdown_capacity;
	m_no_restart_capacity = no_restart_capacity;
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
	double baseline_efficiency;
	int num_streams = 0;
	int num_online_fans_down = 0;
	for (size_t i : m_condenser_idx)
	{
		//std::cerr << i << " " << GetComponents().at(i).GetType() << "\n";
		if (m_components.at(i).IsOperational())
		{
			num_streams++;
			for (size_t j = 1; j <= m_fans_per_condenser_train; j++)
			{
				if (!m_components.at(i + j).IsOperational())
				{
					num_online_fans_down++;
				}
			}
		}
	}
	if (temp < m_condenser_temp_threshold)
		baseline_efficiency = m_condenser_efficiencies_cold[num_streams];
	else
		baseline_efficiency = m_condenser_efficiencies_hot[num_streams];
	return baseline_efficiency - num_online_fans_down * m_eff_loss_per_fan;
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
				eff += m_components.at(i).GetEfficiency() * m_components.at(i).GetCapacityReduction();
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
				cap += m_components.at(i).GetCapacity() * m_components.at(i).GetCapacityReduction();
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
	double turbine_eff = GetTurbineEfficiency(age, false);
	double turbine_cap = GetTurbineCapacity(age, false);
	double sst_cap = GetSaltSteamTrainCapacity();
	double rem_eff = 1.0;
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (!m_components.at(i).IsOperational())
			rem_eff -= m_components.at(i).GetCapacityReduction();
	}
	//efficiency and capacity reductions assumed equal for all components but
	//turbines and salt-to-steam trains
	m_cycle_capacity = std::max(0., std::min(turbine_cap,sst_cap)*condenser_eff*rem_eff);
	//salt
	m_cycle_efficiency = std::max(0., turbine_eff*condenser_eff*rem_eff); 
}

double PowerCycle::GetCycleCapacity()
{
	return m_cycle_capacity;
}

double PowerCycle::GetCycleEfficiency()
{
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
	if (start == "HotStart")
		hazard_increase = m_current_cycle_state.hot_start_penalty;
	else if (start == "WarmStart")
		hazard_increase = m_current_cycle_state.warm_start_penalty;
	else if (start == "ColdStart")
		hazard_increase = m_current_cycle_state.cold_start_penalty;
	for (size_t i = 0; i < m_components.size(); i++)
		m_components.at(i).TestForFailure(
			m_sim_params.steplength, ramp_mult, *m_gen, t, 
			hazard_increase, mode, m_current_scenario
		);
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
    failure_file - output file to record failure
    t -- period index
	reset_time -- true if the maintenance clock should be reset, false o.w.
    record -- true if outputting failure event to file, false o.w.
    duration -- length of outage; this is only used when reset_time==false
	*/
	std::string label;
	if (reset_time)
	{
		label = "MAINTENANCE";
		for (size_t i = 0; i<m_components.size(); i++)
			m_components.at(i).Shutdown(m_current_cycle_state.maintenance_duration);
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
			m_failure_event_labels.push_back(std::to_string(t) + label);
	}

	if (reset_time)
	    m_current_cycle_state.hours_to_maintenance = m_current_cycle_state.maintenance_interval * 1.0;

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

double PowerCycle::GetRampMult(double power_out)
{

	/*
	Returns the ramping penalty, a multiplier applied to the lifetime
	expended by each component in the plant.  If the change in power output
	compared to the previous time period is larger than the ramp rate threshold
	provided as input, the ramping penalty is returned; otherwise, a
	multiplier of 1 (i.e., no penalty due to ramping) is returned.

	power_out -- power out for current time period
	retval -- floating point multiplier

	*/
	if (power_out <= m_sim_params.epsilon)
		return 1.0;
	if (std::fabs(power_out - m_current_cycle_state.power_output) >= m_ramp_threshold_min)
	{
		double ramp_penalty = m_ramping_penalty_min + (m_ramping_penalty_max - m_ramping_penalty_min)*
			(std::fabs(power_out - m_current_cycle_state.power_output) - m_ramp_threshold_min ) / (m_ramp_threshold_max - m_ramp_threshold_min);
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
				m_sim_params.steplength, ramp_mult, *m_gen, false, t, 
				hazard_increase, mode, m_current_scenario
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
				std::to_string(k)) != m_failure_events.end()
				)
			{
				std::string label = (
					"S" + std::to_string(m_current_scenario) + 
					"T" + std::to_string(t) + m_components.at(j).GetName() + 
					std::to_string(k)
					);
				m_components.at(j).ReadFailure(
					m_failure_events[label].duration,
					m_failure_events[label].new_life,
					m_failure_events[label].fail_idx,
					true //we reset all hazard rates after repair; may revisit
				);

				if (m_sim_params.print_output)
					output_log.push_back(util::format(
						"Failure Read: %d, %d, %s",
						t, m_sim_params.read_periods, 
						m_failure_events[label].print())
					);
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
	ClearFailureEvents();
    std::vector< double > cycle_capacities( m_sim_params.sim_length, 0 );
	std::vector< double > cycle_efficiencies(m_sim_params.sim_length, 0);
	size_t num_previous_failures = 0;
	for( int t = 0; t < m_sim_params.sim_length; t++)
    {
		//Shut all components down for maintenance if such an event is 
		//read in inputs, or the hours to maintenance is <= zero.
		//record the event at the period to be read in next.
		if( m_current_cycle_state.hours_to_maintenance <= 0 ) // && t >= m_sim_params.read_periods )
        {
            PlantMaintenanceShutdown(t, true, true);
        }
		
		/*
		//Read in any component failures, if in the read-only stage.
		if (t < m_sim_params.read_periods)
		{
			ReadInMaintenanceEvents(t);
			ReadInComponentFailures(t);
		}
		*/
		// If cycle Capacity is zero or there is no power output, advance downtime.  
		// Otherwise, check for failures, and operate if there is still Capacity.
		double power_output = m_dispatch.at("cycle_power").at(t);
		SetCycleCapacityAndEfficiency(m_dispatch.at("ambient_temperature").at(t));
		std::string start = GetStartMode(t);
		std::string mode = GetOperatingMode(t);
		if (m_cycle_capacity < m_sim_params.epsilon)
		{
			power_output = 0.0;
			mode = "OFF";
		}
		//ajz: This was formerly for only periods after read-in
		double ramp_mult = GetRampMult(power_output);
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
		power_output = std::min(power_output, m_cycle_capacity*m_current_cycle_state.capacity);
		OperatePlant(power_output, t, start, mode);
		cycle_capacities[t] = m_cycle_capacity;
		cycle_efficiencies[t] = m_cycle_efficiency;
		if (m_sim_params.stop_at_first_failure &&
			t > m_results.period_of_last_failure[m_current_scenario] &&
			m_failure_event_labels.size() > num_previous_failures)
		{
			//If a new failure occurs, and our solution approach dictates
			//that we must re-optimize, then fill the remaining 
			m_new_failure_occurred = true;
			m_results.period_of_last_failure[m_current_scenario] = t;
			for (int tp = t; tp < m_sim_params.sim_length; tp++)
			{
				cycle_capacities[tp] = cycle_capacities[t] * 1.0;
				cycle_efficiencies[tp] = cycle_efficiencies[t] * 1.0;
			}
			break;
		}
		if (t == m_results.period_of_last_failure[m_current_scenario])
		{
			num_previous_failures = m_failure_event_labels.size();
		}
    }
	StoreScenarioResults(cycle_efficiencies, cycle_capacities);
}

void PowerCycle::OperatePlant(double power_out, int t, std::string start, std::string mode)
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
		power_out -- power output for plant (determines ramping multiple)
		t -- time index
		mode -- operation mode
	*/
	double ramp_mult = GetRampMult(power_out);
	m_current_cycle_state.power_output = power_out;
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

void PowerCycle::SingleScen(bool read_state_from_file, bool read_from_memory)
{

    /*failure_file = open(
        os.path.join(m_sim_params.print_output_dir,"component_failures.csv"),'w'
        )*/
	if (read_state_from_file)
	{
		std::string cfilename = (
			m_filenames.component_in_state + 
			std::to_string(m_current_scenario) + ".csv"
			);
		std::string pfilename = (
			m_filenames.plant_in_state + 
			std::to_string(m_current_scenario) + ".csv"
			);
		ReadStateFromFiles(cfilename, pfilename);
	}
	else if (read_from_memory)
	{
		ReadCycleStateFromResults(m_current_scenario);
		ReadComponentStatus(m_results.component_status[m_current_scenario]);
	}
	m_start_component_status = GetComponentStates();
	RunDispatch();
	if (m_new_failure_occurred && m_sim_params.stop_at_first_failure)
	{
		RevertToStartState();
	}
	else
	{
		StoreCycleState();
		m_results.period_of_last_failure[m_current_scenario] = -1;
	}
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
		for (int t = 0; t < m_sim_params.sim_length; t++)
		{
			avg_eff += m_results.cycle_efficiency[s].at(t);
			avg_cap += m_results.cycle_capacity[s].at(t);
		}
		m_results.avg_cycle_efficiency.push_back(avg_eff / m_sim_params.num_scenarios);
		m_results.avg_cycle_capacity.push_back(avg_cap / m_sim_params.num_scenarios);
	}
	m_results.avg_labor_cost = avg_labor / m_sim_params.num_scenarios;
	m_results.avg_turbine_capacity = avg_turb_cap / m_sim_params.num_scenarios;
	m_results.avg_turbine_efficiency = avg_turb_eff / m_sim_params.num_scenarios;
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
}

void PowerCycle::Simulate(bool read_state_from_file, bool read_state_from_memory,
		bool run_only_previous_failures)
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
			SingleScen(read_state_from_file, read_state_from_memory);
	}
	
	//Record failure events
	m_results.failure_events = m_failure_events;
	m_results.failure_event_labels = m_failure_event_labels;
	
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
		GetComponents().at(c).Reset(*m_gen);
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
	m_failure_events.clear();
	m_failure_event_labels.clear();
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

