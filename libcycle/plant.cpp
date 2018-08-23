#include "plant.h"
#include "lib_util.h"
#include <algorithm>
#include <iostream>
#include <fstream>

void PowerCycle::InitializeCyclingDists()
{
	m_hs_dist = BoundedJohnsonDist(0.995066, 0.252898, 4.139E-5, 4.489E-4, "BoundedJohnson");
	m_ws_dist = BoundedJohnsonDist(2.220435, 0.623145, 2.914E-5, 1.773E-3, "BoundedJohnson");
	m_cs_dist = BoundedJohnsonDist(0.469391, 0.581813, 3.691E-5, 2.369E-4, "BoundedJohnson");
}

void PowerCycle::AssignGenerator( WELLFiveTwelve *gen )
{
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
	m_hot_start_penalty = m_hs_dist.GetInverseCDF(unif);
	m_warm_start_penalty = m_ws_dist.GetInverseCDF(unif);
	m_cold_start_penalty = m_cs_dist.GetInverseCDF(unif);
}

void PowerCycle::SetSimulationParameters( int read_periods, int num_periods, double epsilon, bool print_output)
{
    m_read_periods = read_periods;
    m_num_periods = num_periods;
    m_output = print_output;
    m_eps = epsilon;
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
		throw std::exception("condenser trains not created correctly");
	}
	if (eff_cold.size() != num_streams + 1)
		throw std::exception("efficiencies must be equal to one plus number of streams"); 
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
		throw std::exception("efficiencies must be equal to one plus number of streams");
	m_condenser_efficiencies_hot = eff_hot;
}

void PowerCycle::ReadComponentStatus(
	std::unordered_map< std::string, ComponentStatus > dstat
)
{
	/*
	Mutator for component status for all components in the plant.
	Used at initialization.
	*/
	m_component_status = dstat;
}

void PowerCycle::ClearComponentStatus()
{
	m_component_status.clear();
}

void PowerCycle::SetStatus()
{

    for( std::vector< Component >::iterator it=m_components.begin(); it!= m_components.end(); it++)
    {
        if(m_component_status.find( it->GetName() ) != m_component_status.end() )
        {
            it->ReadStatus( m_component_status.at( it->GetName() ) );
        }
        else
        {
			it->GenerateInitialLifesAndProbs( *m_gen );
        }
    }

	SetPlantAttributes(
		m_maintenance_interval, m_maintenance_duration, 
		m_downtime_threshold, m_steplength, 
		m_plant_status["hours_to_maintenance"] * 1.0,
		m_plant_status["power_output"] * 1.0, m_plant_status["standby"] && true,
		m_capacity, m_condenser_temp_threshold, m_plant_status["time_online"] * 1.0,
		m_plant_status["time_in_standby"] * 1.0,
		m_plant_status["downtime"] * 1.0,
		m_shutdown_capacity,
		m_no_restart_capacity
		);

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
	return m_hours_to_maintenance;
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
    return m_online; 
}

bool PowerCycle::IsOnStandby()
{
    /*
	retval --  a boolean indicator of whether the plant is in standby 
    mode.
	*/
    return m_standby;
}

double PowerCycle::GetTimeInStandby()
{
	return m_time_in_standby;
}

double PowerCycle::GetTimeOnline()
{
	return m_time_online;
}

double PowerCycle::GetRampThreshold()
{
	return m_ramp_threshold;
}

double PowerCycle::GetSteplength()
{
	return m_steplength;
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
	return m_hot_start_penalty;
}

double PowerCycle::GetWarmStartPenalty()
{
	/* accessor for warm start penalty. */
	return m_warm_start_penalty;
}

double PowerCycle::GetColdStartPenalty()
{
	/* accessor for cold start penalty. */
	return m_cold_start_penalty;
}

void PowerCycle::SetShutdownCapacity(double capacity) 
{
	m_shutdown_capacity = capacity;
}


void PowerCycle::SetNoRestartCapacity(double capacity)
{
	m_no_restart_capacity = capacity;
}


void PowerCycle::AddComponent( std::string name, 
		std::string type, 
		double repair_rate, 
		double repair_cooldown_time,
        double Capacity_reduction,
		double repair_cost,
		std::string repair_mode
)
{
    m_components.push_back( Component(name, type,  
		repair_rate, repair_cooldown_time, &m_failure_events, 
		Capacity_reduction, repair_cost, repair_mode, 
		&m_failure_event_labels) );
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
            throw std::exception( "Mal-formed cycle Capacity model table. Component table must contain 8 entries per row (comma separated values), with each entry separated by a ';'." );

        std::vector< double > dat(6);

        for(size_t j=0; j<6; j++)
            util::to_double( entry.at(j+2), &dat.at(j) );
        //                                  name	  component_type  dist_type failure_alpha failure_beta repair_rate repair_cooldown_time hot_start_penalty warm_start_penalty cold_start_penalty
        m_components.push_back( Component( entry.at(0), entry.at(1), //entry.at(2), dat.at(0),   dat.at(1),  
			dat.at(2),  dat.at(3),          &m_failure_events ) );
        
    }
    
}


void PowerCycle::AddCondenserTrain(int num_fans, int num_radiators)
{
	m_num_condenser_trains += 1;
	std::string component_name;
	std::string train_name = "C" + std::to_string(m_num_condenser_trains);
	for (int i = 1; i <= num_fans; i++)
	{
		component_name = train_name + "-F" + std::to_string(i);
		AddComponent(component_name, "Condenser fan", 35.5, 0, 0.01, 7.777, "S");
		AddFailureType(component_name, "Fan Failure", "O", "gamma", 1, 841188);
	}
	AddComponent(component_name + "-T", "Condenser train", 15.55, 0, 0.00, 7.777, "S");
	for (int i = 1; i <= num_radiators; i++)
	{
		AddFailureType(component_name + "-T", "Radiator " + std::to_string(i) + " Failure", "O", "gamma", 1, 698976);
	}
}

void PowerCycle::AddSaltToSteamTrains(int num_trains)
{
	std::string component_name;
	for (int i = 0; i < num_trains; i++)
	{
		m_num_salt_steam_trains += 1;
		component_name = "SST" + std::to_string(m_num_salt_steam_trains);
		AddComponent(component_name, "Salt-to-steam train", 2.14, 72, 0.5, 7.777, "A");
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
		AddComponent(component_name, "Feedwater heater", 2.14, 48., 0.05, 7.777, "A");
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
		AddComponent(component_name, "Molten salt pump", 0.5, 0., 1., 7.777, "D");
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
		AddComponent(component_name, "Water pump", 0.5, 0., 1., 7.777, "D");
		AddFailureType(component_name, "External_Leak_Large_(shell)", "O", "gamma", 0.3, 75000000);
		AddFailureType(component_name, "External_Leak_Large_(tube)", "O", "gamma", 0.3, 10000000);
		AddFailureType(component_name, "External_Leak_Small_(shell)", "O", "gamma", 0.5, 10000000);
		AddFailureType(component_name, "External_Leak_Small_(tube)", "O", "gamma", 0.3, 1200000);
	}
}

void PowerCycle::AddTurbines(int num_hi_pressure, int num_mid_pressure, int num_low_pressure)
/*
Adds Hi-, Medium-, and Low-Pressure Turbines to the plant object.
*/
{
	std::string component_name;
	for (int i = 0; i < num_hi_pressure; i++)
	{
		m_num_hp_turbines += 1;
		component_name = "HPT" + std::to_string(m_num_hp_turbines);
		AddComponent(component_name, "High-pressure turbine", 32.7, 72, 1, 7.777, "D");
		AddFailureType(component_name, "MBTF", "O", "gamma", 1, 51834.31953);
	}
	for (int i = 0; i < num_mid_pressure; i++)
	{
		m_num_mp_turbines += 1;
		component_name = "MPT" + std::to_string(m_num_mp_turbines);
		AddComponent(component_name, "Medium-pressure turbine", 32.7, 72, 1, 7.777, "D");
		AddFailureType(component_name, "MBTF", "O", "gamma", 1, 51834.31953);
	}
	for (int i = 0; i < num_low_pressure; i++)
	{
		m_num_lp_turbines += 1;
		component_name = "LPT" + std::to_string(m_num_lp_turbines);
		AddComponent(component_name, "Low-pressure turbine", 32.7, 72, 1, 7.777, "D");
		AddFailureType(component_name, "MBTF", "O", "gamma", 1, 51834.31953);
	}
}

void PowerCycle::GeneratePlantComponents(
	int num_condenser_trains = 2,
	int fans_per_train = 30,
	int radiators_per_train = 1,
	int num_salt_steam_trains = 2,
	int num_fwh = 6,
	int num_salt_pumps = 2,
	int num_water_pumps = 2,
	int num_hi_pressure = 1, 
	int num_mid_pressure = 1, 
	int num_low_pressure = 1
)
{
	/* 
	Generates all the components in the plant.  Aggregates the other 
	component-specific methods.
	*/
	for (int i = 0; i < num_condenser_trains; i++)
	{
		AddCondenserTrain(fans_per_train, radiators_per_train);
	}
	AddSaltToSteamTrains(num_salt_steam_trains);
	AddFeedwaterHeaters(num_fwh);
	AddSaltPumps(num_salt_pumps);
	AddWaterPumps(num_water_pumps);
	AddTurbines(num_hi_pressure, num_mid_pressure, num_low_pressure);
}

void PowerCycle::SetPlantAttributes(double maintenance_interval = 5000.,
				double maintenance_duration = 24.,
				double downtime_threshold = 24., 
				double steplength = 1., double hours_to_maintenance = 5000.,
				double power_output = 0., bool standby = false, double capacity = 500000.,
				double temp_threshold = 20., 
				double time_online = 0., double time_in_standby = 0.,
				double downtime = 0., double shutdown_capacity = 0.45, 
				double no_restart_capacity = 0.9
	)
{
    /*
	Initializes plant attributes.
    retval - none
    
	*/

    m_maintenance_interval = maintenance_interval;
    m_maintenance_duration = maintenance_duration;
    m_ramp_threshold = capacity * 0.2;  //using Gas-CC as source, Kumar 2012,
	m_ramp_threshold_min = 1.1*m_ramp_threshold;   //Table 1-1
	m_ramp_threshold_max = 2.0*m_ramp_threshold;   
    m_downtime_threshold = downtime_threshold;
    m_steplength = steplength;
    m_hours_to_maintenance = hours_to_maintenance;
    m_power_output = power_output;
    m_standby = standby;
    m_online = m_power_output > 0.;
	m_capacity = capacity;
	m_time_in_standby = time_in_standby;
	m_time_online = time_online;
	m_condenser_temp_threshold = temp_threshold;
	m_downtime = downtime;
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
	int num_streams = NumberOfAirstreamsOnline();
	if (temp < m_condenser_temp_threshold)
		return m_condenser_efficiencies_cold[num_streams];
	return m_condenser_efficiencies_hot[num_streams];
}

double PowerCycle::GetCycleCapacity(double temp)
{
	/* 
	Provides the cycles Capacity, based on components that
	are operational.  Assumes that when multiple components are
	down, the effect on cycle Capacity is additive

	temp -- ambient temperature (affects condenser efficiency)
	*/
	if (!AirstreamOnline())
		return 0.0; 
	if (!FWHOnline())
		return 0.0;
	double capacity = GetCondenserEfficiency(temp);
	for (size_t i = 0; i<m_components.size(); i++)
		if (!m_components.at(i).IsOperational())
			capacity -= m_components.at(i).GetCapacityReduction();
	if (capacity < 0.0)
		return 0.0;
	return capacity;
}

void PowerCycle::TestForComponentFailures(double ramp_mult, int t, std::string start, std::string mode)
{
	/*
	Determines whether a component is to fail, based on current dispatch.
	*/
	double hazard_increase = 0.;
	if (start == "HotStart")
		hazard_increase = m_hot_start_penalty;
	else if (start == "WarmStart")
		hazard_increase = m_warm_start_penalty;
	else if (start == "ColdStart")
		hazard_increase = m_cold_start_penalty;
	for (size_t i = 0; i < m_components.size(); i++)
		m_components.at(i).TestForFailure(
			m_steplength, ramp_mult, *m_gen, t - m_read_periods, hazard_increase, mode
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
			m_components.at(i).Shutdown(m_maintenance_duration);
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
			std::to_string(t) + label
		] = failure_event(t, label, -1, duration, 0.);
			m_failure_event_labels.push_back(std::to_string(t) + label);
	}

	if (reset_time)
	    m_hours_to_maintenance = m_maintenance_interval * 1.0;

	if (m_record_state_failure)
	{
		m_record_state_failure = false;
		StoreState();
	}
}

void PowerCycle::AdvanceDowntime(std::string mode)
{

    /*
	When the plant is not operational, advances time by a period.  This
    updates the repair time and/or maintenance time remaining in the plant.

	*/
	for (size_t i = 0; i < m_components.size(); i++)
	{
		if (!m_components.at(i).IsOperational())
			m_components.at(i).AdvanceDowntime(m_steplength, mode);
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
	if (power_out <= m_eps)
		return 1.0;
	if (std::fabs(power_out - m_power_output) >= m_ramp_threshold_min)
	{
		double ramp_penalty = m_ramping_penalty_min + (m_ramping_penalty_max - m_ramping_penalty_min)*
			(std::fabs(power_out - m_power_output) - m_ramp_threshold_min ) / (m_ramp_threshold_max - m_ramp_threshold_min);
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
    mode -- operating mode (e.g., HotStart, Running)
    retval -- None
    
	*/
    //print t - m_read_periods
	double hazard_increase = 0.;
	if (start == "HotStart")
		hazard_increase = m_hot_start_penalty;
	else if (start == "WarmStart")
		hazard_increase = m_warm_start_penalty;
	else if (start == "ColdStart")
		hazard_increase = m_cold_start_penalty;
    bool read_only = (t < m_read_periods);
	for (size_t i = 0; i < m_components.size(); i++) 
	{
		if (m_components.at(i).IsOperational())
			m_components.at(i).Operate(
				m_steplength, ramp_mult, *m_gen, read_only, t - m_read_periods, hazard_increase, mode
			);
		else if (mode == "OFF" || mode == "SF" || mode == "SS" || mode == "SO")
		{
			m_components.at(i).AdvanceDowntime(m_steplength, mode);
		}
	}
}

void PowerCycle::ResetHazardRates()
{

    /*
	resets the plant (restores component lifetimes)
	*/
    for( size_t i=0; i<m_components.size(); i++ )
        m_components.at(i).ResetHazardRate();
        
}

void PowerCycle::StoreComponentState()
{
	/*
	Stores the current state of each component, as of 
	the end of the read-in periods.
	*/
	for (
		std::vector< Component >::iterator it = m_components.begin(); 
		it != m_components.end(); 
		it++
		)
	{
		m_component_status[it->GetName()] = it->GetState();
	}
}

void PowerCycle::StorePlantState()
{
	/*
	Stores the current state of the plant, as of
	the end of the read-in periods.
	*/
	m_plant_status["hours_to_maintenance"] = m_hours_to_maintenance*1.0;
	m_plant_status["power_output"] = m_power_output*1.0;
	m_plant_status["standby"] = m_standby && true;
	m_plant_status["running"] = m_power_output > 1e-8;
	m_plant_status["hours_running"] = m_time_online*1.0;
	m_plant_status["hours_in_standby"] = m_time_in_standby*1.0;
	m_plant_status["downtime"] = m_downtime*1.0;
}

void PowerCycle::StoreState()
{
	/*
	Stores the current state of the plant and each component, as of
	the end of the read-in periods.
	*/
	StoreComponentState();
	StorePlantState();
}

std::string PowerCycle::GetStartMode(int t)
{
	/* 
	returns the start mode as a string, or "none" if there is no start.
	*/
	double power_out = m_dispatch.at("cycle_power").at(t);
	if (power_out > m_eps)
	{
		if (IsOnline())
			return "None";
		if (IsOnStandby())
			return "HotStart";
		else if (m_downtime <= m_downtime_threshold)
			return "WarmStart";
		return "ColdStart";
	}
	return "None";
}

std::string PowerCycle::GetOperatingMode(int t)
{
	/*
	returns the operating mode as a string.
	*/
	double power_out = m_dispatch.at("cycle_power").at(t);
	double standby = m_dispatch.at("standby").at(t);
	if (power_out > m_eps)
	{
		if (IsOnline())
			if (m_time_online <= 1.0 - m_eps)
				return "OF"; //in the first hour of power cycle operation
			else
				return "OO"; //ongoing (>1 hour) power cycle operation
			return "OS";  //starting power cycle operation
	}
	else if (standby >= 0.5)
	{
		if (IsOnStandby())
			if (m_time_in_standby <= 1.0 - m_eps)
				return "SF"; //in first hour of standby
			else
				return "SO"; // ongoing standby (>1 hour)
		return "SS";  // if not currently on standby, then starting standby
	}
	return "OFF";
}

std::vector< double > PowerCycle::RunDispatch()
{

    /*
	runs dispatch for entire time horizon.
    failure_file -- file to output failures
    retval -- array of binary variables that are equal to 1 if the plant 
        is able to operate (i.e., no maintenance or repair events in 
        progress), and 0 otherwise.  This includes the read-in period.
    
	*/
    std::vector< double > operating_periods( m_num_periods, 0 );
	m_record_state_failure = true;
    for( int t=0; t<m_num_periods; t++)
    {
		if ( t == m_read_periods )
		{
			StoreState();
			//ajz: I moved the failure events removal to the point at which 
			//we stop reading old failure events and start writing new ones.
			m_failure_events.clear();
			m_failure_event_labels.clear();
		}
		//Shut all components down for maintenance if such an event is 
		//read in inputs, or the hours to maintenance is <= zero.
		if( m_hours_to_maintenance <= 0 && t >= m_read_periods )
        {
            PlantMaintenanceShutdown(t-m_read_periods,true,true);
        }
		//Read in planned maintenance events
		if (
			m_failure_events.find(std::to_string(t) + "MAINTENANCE")
			!= m_failure_events.end()
			)
		{
			PlantMaintenanceShutdown(t, true, false);
		}
		//Read in unplanned maintenance events, if any
		if (
			m_failure_events.find(std::to_string(t) + "UNPLANNEDMAINTENANCE")
			!= m_failure_events.end()
			)
		{
			PlantMaintenanceShutdown(
				t, false, false, 
				m_failure_events[std::to_string(t) + "UNPLANNEDMAINTENANCE"].duration
			);
		}
		//Read in any component failures, if in the read-only stage.
		if (t < m_read_periods)
		{
			for (size_t j = 0; j < m_components.size(); j++)
			{
				for (size_t k = 0; k < m_components.at(j).GetFailureTypes().size(); k++)
				{
					if (m_failure_events.find(std::to_string(t) + m_components.at(j).GetName() + std::to_string(k)) != m_failure_events.end())
					{
						std::string label = std::to_string(t) + m_components.at(j).GetName() + std::to_string(k);
						m_components.at(j).ReadFailure(
							m_failure_events[label].duration,
							m_failure_events[label].new_life,
							m_failure_events[label].fail_idx,
							true //This indicates that we reset all hazard rates after repair; may want to revisit
						);

						if (m_output)
							output_log.push_back(util::format("Failure Read: %d, %d, %s", t, m_read_periods, m_failure_events[label].print()));
					}
				}
			}
		}
		// If cycle Capacity is zero or there is no power output, advance downtime.  
		// Otherwise, check for failures, and operate if there is still Capacity.
		double power_output = m_dispatch.at("cycle_power").at(t);
		double cycle_capacity = GetCycleCapacity(m_dispatch.at("ambient_temperature").at(t));
		std::string start = GetStartMode(t);
		std::string mode = GetOperatingMode(t);
		
		if (cycle_capacity < m_eps)
		{
			power_output = 0.0;
			mode = "OFF";
		}
		if (t >= m_read_periods)  //we only generate 'new' failures after read-in period
		{
			double ramp_mult = GetRampMult(power_output);
			TestForComponentFailures(ramp_mult, t, start, mode);
			cycle_capacity = GetCycleCapacity(m_dispatch.at("ambient_temperature").at(t));
			//if the cycle Capacity is set to zero, this means the plant is in maintenace
			//or a critical failure has occurred, so shut the plant down.
			if (cycle_capacity < m_eps)
			{
				power_output = 0.0;
				if (mode != "OFF" && m_record_state_failure)
				{
					StoreState();
					m_record_state_failure = false;
				}
				mode = "OFF";
			}
			else if (cycle_capacity < m_shutdown_capacity)
			{
				PlantMaintenanceShutdown(t, false, true, GetMaxComponentDowntime());
			}
			else if (cycle_capacity < m_no_restart_capacity && mode == "OFF")
			{
				PlantMaintenanceShutdown(t, false, true, GetMaxComponentDowntime());
			}
		}
		power_output = std::min(power_output, cycle_capacity*m_capacity);
		Operate(power_output, t, start, mode);
		operating_periods[t] = cycle_capacity;

		//std::cerr << "Period " << std::to_string(t) << " Capacity: " << std::to_string(cycle_capacity) << ".  Mode: " << mode <<"\n";
    }

	if (m_record_state_failure)
	{
		StoreState();  //if no failures have occurred, save final plant state. otherwise, 
					   //record the state at the first full plant shutdown.
	}

	return operating_periods;                   
}

void PowerCycle::Operate(double power_out, int t, std::string start, std::string mode)
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
	m_power_output = power_out;
	if (mode == "OFF")
	{
		m_online = false;
		m_standby = false;
		m_downtime += m_steplength;
		m_time_in_standby = 0.0;
		m_time_online = 0.0;
		AdvanceDowntime("OFF");
		return;
	}
	else if (mode == "SS") //standby - start
	{
		m_online = false;
		m_standby = true;
		m_time_in_standby = m_steplength;
		m_downtime = 0.0;
		m_time_online = 0.0;
	}
	else if (mode == "SF" || mode == "SO") //standby - first hour; standby ongoing (>1 hour)
	{
		m_time_in_standby += m_steplength;
	}
	else if (mode == "OS") //online - start
	{
		m_online = true;
		m_standby = false;
		m_time_in_standby = 0.0;
		m_downtime = 0.0;
		m_time_online = m_steplength;
		m_hours_to_maintenance -= m_steplength;
	}
	else if (mode == "OF" || mode == "OO") //standby - first hour; standby ongoing (>1 hour)
	{
		m_time_online += m_steplength;
		m_hours_to_maintenance -= m_steplength;
	}
	else
		throw std::exception("invalid operating mode.");
	OperateComponents(ramp_mult, t, start, mode); 

}

std::vector< double > PowerCycle::Simulate(bool reset_status)
{

    /*failure_file = open(
        os.path.join(m_output_dir,"component_failures.csv"),'w'
        )*/
    std::vector< double > operating_periods = RunDispatch();
	if (reset_status)
	{
		SetStatus();
	}

	return operating_periods;
    //outfile = open(
    //    os.path.join(m_output_dir,"operating_periods.txt"),'w'
    //    )
    //for i in range(len(operating_periods)):
    //    outfile.write(str(operating_periods[i])+",")
    //failure_file.close()
    //outfile.close()
}

std::unordered_map < int, std::vector<double> > PowerCycle::RunScenarios(
	int num_scenarios = 1)
{
	/*
	Generates a collection of Monte Carlo realizations of failure and
	maintenance events in the power block.  
	*/
	InitializeCyclingDists();
	std::vector< double > operating_periods;
	std::unordered_map< int, std::vector< double > > results;
	for (int i = 0; i < num_scenarios; i++)
	{
		m_gen->assignStates(i);
		GeneratePlantCyclingPenalties();
		ResetPlant(*m_gen);
		results[i] = Simulate(false);
		m_gen->saveStates(i);
	}
	return results;
}

void PowerCycle::ResetPlant(WELLFiveTwelve &gen)
{
	m_online = false;
	m_time_online = 0.;
	m_standby = false;
	m_time_in_standby = 0.;
	m_power_output = 0.;
	m_hours_to_maintenance = m_maintenance_interval;
	for (size_t c = 0; c < GetComponents().size(); c++)
	{
		GetComponents().at(c).Reset(gen);
	}
}