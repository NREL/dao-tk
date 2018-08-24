#include "component.h"
#include <cmath>
#include <algorithm>
#include <iostream>


//##################################################################################

failure_event::failure_event(){}

failure_event::failure_event(int _time, std::string _component, int _fail_idx, double _duration, double _new_life, int _scen_index)
    : time( _time ), component( _component ), fail_idx( _fail_idx), duration( _duration ), new_life( _new_life ), scen_index(_scen_index)
{}


std::string failure_event::print()
{
    return "<" + component + ", " + std::to_string(duration) + ", " + std::to_string(new_life) + ", scenario " + std::to_string(scen_index) + ", period " + std::to_string(time);
}

//##################################################################################

ComponentStatus::ComponentStatus(){}

ComponentStatus::ComponentStatus(std::vector<double> lifes, double hazard, double downtime,
		double repair_event_time)
    : lifetimes(lifes), hazard_rate( hazard ), downtime_remaining( downtime ), repair_event_time( repair_event_time )
{}

//##################################################################################

Component::Component()
{
}

Component::Component(  std::string name, std::string type, 
			//std::string dist_type, double failure_alpha, double failure_beta, 
			double repair_rate, double repair_cooldown_time,
            std::unordered_map< std::string, failure_event > *failure_events, 
			double capacity_reduction, double repair_cost, std::string repair_mode,
			std::vector<std::string> *failure_event_labels)
{
    /*
    Description of attributes:
    name -- component identifer
    type -- component type description
    repair_rate -- rate at which repairs take place (events/h)
    repair_cooldown_time -- added required downtime for repair (h)
    repair_cost -- dollar cost of repairs, not including revenue lost ($)
	capacity_reduction -- reduction in cycle capacity if component fails (fraction)

    */

    if( name == "MAINTENANCE" )
        throw std::exception("cannot name a component 'MAINTENANCE'");
    
	m_failure_types = {};
    m_name = name;
    m_type = type;
    m_repair_cost = repair_cost;
	m_capacity_reduction = capacity_reduction;
	m_cooldown_time = repair_cooldown_time;
	m_repair_mode = repair_mode;

	m_status.hazard_rate = 1.0;
    m_status.downtime_remaining = 0.0;
	m_status.operational = true;
	m_status.repair_event_time = 0.0;

	Distribution *edist = new ExponentialDist(repair_rate, repair_cooldown_time, "exponential");
	m_repair_dist = (ExponentialDist *) edist;

    m_parent_failure_events = failure_events;
	m_parent_failure_event_labels = failure_event_labels;
}


void Component::ReadStatus( ComponentStatus &status )
{
	m_status.hazard_rate =  status.hazard_rate;
    m_status.downtime_remaining = status.downtime_remaining;
	m_status.operational = (m_status.downtime_remaining < 1e-8);
	m_status.repair_event_time = status.repair_event_time;
	for (size_t j = 0; j < m_failure_types.size(); j++)
	{
		m_failure_types.at(j).SetLifeOrProb(status.lifetimes.at(j));
	}
}

        
std::string Component::GetName()
{
	return m_name;
}

        
std::string Component::GetType()
{
	return m_type;
}

std::vector<FailureType> Component::GetFailureTypes()
{
	return m_failure_types;
}

void Component::AddFailureMode(std::string component, std::string id, std::string failure_mode,
	std::string dist_type, double alpha, double beta)
{
	m_failure_types.push_back(FailureType(component, id, failure_mode, dist_type, alpha, beta));
}
        
double Component::GetHazardRate()
{
	return m_status.hazard_rate;
}

    
double Component::GetRepairCost()
{
	return m_repair_cost;
}

double Component::GetCapacityReduction()
{
	return m_capacity_reduction;
}

double Component::GetCooldownTime()
{
	return m_cooldown_time;
}

        
bool Component::IsOperational()
{
	return m_status.operational;
}

        
void Component::Shutdown(double time)
{
    /*
    Removes component from operation for a given period of time 
    (used for maintenance).
    time -- required downtime for maintenance
    retval -- None 
    */
	m_status.operational = false;
    m_status.downtime_remaining = time;
	m_status.repair_event_time = time;
}

        
void Component::RestoreComponent()
{
	m_status.operational = true;
	m_status.downtime_remaining = 0.0;
	m_status.repair_event_time = 0.0;
}

        
void Component::ResetHazardRate()
{
	m_status.hazard_rate = 1.;
}

    
double Component::GetDowntimeRemaining()
{
	return m_status.downtime_remaining;
}

        
void Component::SetDowntimeRemaining(double time)
{
	m_status.downtime_remaining = time;
}

        
void Component::GenerateTimeToRepair(WELLFiveTwelve &gen)
{
	/* 
    generate a random downtime, which is the number of hours of
    operation to a component failure given normal operations that do not
    increase the hazard rate.
    gen -- random U[0,1] generator object
    retval -- lifetime in adjusted hours of operation
    */
    m_status.downtime_remaining = m_repair_dist->GetVariate(gen);
	m_status.repair_event_time = m_status.downtime_remaining*1.0;
	//std::cerr << "NEW FAILURE - DOWNTIME: " << std::to_string(m_status.downtime_remaining) << "\n";
}

        
double Component::HoursToFailure(double ramp_mult, std::string mode)
{
    /* 
    Returns number of hours of operation that would lead to failure 
    under the operation parameters given as input.
        
    ramp_mult -- degradation multiplier due to ramping
	mode -- operating mode
    retval - floating point number indicating hours of operation to failure
    */
    if( ramp_mult == 0 )
        return INFINITY;
	double hours = INFINITY;
	for (size_t j = 0; j < m_failure_types.size(); j++)
	{
		if (m_failure_types.at(j).GetFailureMode() == mode)
		{
			hours = std::min(hours, 
				m_failure_types.at(j).GetLifeRemaining() / 
				(m_status.hazard_rate * ramp_mult)
				);
		}
	}
    return INFINITY;
}


void Component::TestForBinaryFailure(std::string mode, int t, 
	WELLFiveTwelve &gen, int scen_index)
{
	double var = 0.0;
	for (size_t j = 0; j < m_failure_types.size(); j++)
	{
		if (m_failure_types.at(j).GetFailureMode() == mode)
		{
			var = gen.getVariate();
			if ( var <= m_failure_types.at(j).GetFailureProbability() *m_status.hazard_rate )
				GenerateFailure(gen, t, j, scen_index);
		}
	}
}

void Component::TestForFailure(double time, double ramp_mult,
	WELLFiveTwelve &gen, int t, double hazard_increase, std::string mode, 
	int scen_index)
{
	if (mode == "OFF")
		return;
	/*
	Generates failure events under the provided dispatch, if there is not sufficient life
	remaining in the component, or the RNG generates a failure on start.
	*/
	std::string opmode;
	if (mode == "OS")
	// if starting standby or online, test for fail on start, 
	// then operate as if in the first hour of that mode to test
	// for failures during the time period.
	{
		TestForBinaryFailure(mode, t, gen, scen_index);
		opmode = "OF";
	}
	else if (mode == "SS")
	{
		TestForBinaryFailure(mode, t, gen, scen_index);
		opmode = "SF";
	}
	else
		opmode = mode;
	for (size_t j = 0; j < m_failure_types.size(); j++)
	{
		if (m_failure_types.at(j).GetFailureMode() == opmode || m_failure_types.at(j).GetFailureMode() == "ALL")
		{
			if (time * (m_status.hazard_rate + hazard_increase) * ramp_mult > m_failure_types.at(j).GetLifeRemaining())
				GenerateFailure(gen, t, j, scen_index);
		}
		if (m_failure_types.at(j).GetFailureMode() == "O" && (opmode == "OO" || opmode == "OF" ) )
		{
			if (time * (m_status.hazard_rate + hazard_increase) * ramp_mult > m_failure_types.at(j).GetLifeRemaining())
				GenerateFailure(gen, t, j, scen_index);
		}
	}
	
}

         
void Component::Operate(double time, double ramp_mult, WELLFiveTwelve &gen, 
		bool read_only, int t, double hazard_increase, std::string mode,
		int scen_index)
{
    /* 
    assumes operation for a given period of time, with 
    no permanent change to the hazard rate.
    time -- time of operation
    ramp_mult -- degradation multiplier due to ramping
    gen -- random U[0,1] variate generator object
    read_only -- indicates whether to generate a failure event if 
        life_remaining falls below 0 during operation
    t -- period index
    retval -- None
    */
    if( ! IsOperational() )
        throw std::exception("can't operate a plant in downtime.");
	m_status.hazard_rate += hazard_increase;
	std::string opmode;
	//if starting a mode, operate as if
	//in the first hour of operation for that mode to degrade lifetimes.
	if (mode == "OS")
		// if starting standby or online, test for fail on start, 
		// then operate as if in the first hour of that mode to test
		// for failures during the time period.
	{
		TestForBinaryFailure(mode, t, gen, scen_index);
		opmode = "OF";
	}
	else if (mode == "SS")
	{
		TestForBinaryFailure(mode, t, gen, scen_index);
		opmode = "SF";
	}
	else
		opmode = mode;
	for (size_t j = 0; j < m_failure_types.size(); j++)
	{
		if (m_failure_types.at(j).GetFailureMode() == opmode || (opmode != "OFF" && m_failure_types.at(j).GetFailureMode() == "ALL"))
		{
			if (time * m_status.hazard_rate * ramp_mult > m_failure_types.at(j).GetLifeRemaining() && !read_only)
				throw std::exception("failure should be thrown.");
			m_failure_types.at(j).ReduceLifeRemaining(time * m_status.hazard_rate * ramp_mult);
		}
		if (m_failure_types.at(j).GetFailureMode() == "O" && (opmode == "OO" || opmode == "OF"))
		{
			if (time * m_status.hazard_rate * ramp_mult > m_failure_types.at(j).GetLifeRemaining() && !read_only)
				throw std::exception("failure should be thrown.");
			m_failure_types.at(j).ReduceLifeRemaining(time * m_status.hazard_rate * ramp_mult);
		}
	}		
}
         
void Component::ReadFailure(double downtime, double life_remaining, 
	int fail_idx, bool reset_hazard=true)
{
    /*
    reads a failure event.  This executes the failure without the 
    randomly generated failures.
    downtime -- downtime to apply to the failure
    life_remaining -- operational lifetime of component once online again
	reset_hazard -- true if the repair resets the hazard rate, false o.w.
    retval -- none
    */
	m_status.operational = false;
    SetDowntimeRemaining(downtime);
    m_failure_types.at(fail_idx).SetLifeOrProb(life_remaining);
	if (reset_hazard)
	{
		ResetHazardRate();
	}
}

                
void Component::GenerateFailure(WELLFiveTwelve &gen, int t, int fail_idx, int scen_index)
{
    /*
    creates a failure event, shutting down the plant for a period of time.
    gen -- random U[0,1] generator used to generate stochastic repair times
    failure_file -- failure event output file
    t -- period index
	fail_idx -- failure type (mode/part combination) that caused failure
    retval -- None
    */
    m_status.operational = false;
    GenerateTimeToRepair(gen);
    m_failure_types.at(fail_idx).GenerateFailureVariate(gen);
    ResetHazardRate();
    
    //add a new failure to the parent (CSPPlant) failure queue
	std::string label = "S"+std::to_string(scen_index)+"T"+std::to_string(t)+GetName()+std::to_string(fail_idx);
    (*m_parent_failure_events)[label] = failure_event(
		t, GetName(), fail_idx, m_status.downtime_remaining, 
		m_failure_types.at(fail_idx).GetLifeOrProb(), scen_index
		);
	(*m_parent_failure_event_labels).push_back(label);
	//std::cerr << "FAILURE EVENT GENERATED. downtime: " << std::to_string(m_status.downtime_remaining) << " life_rem: " << std::to_string(m_failure_types.at(fail_idx).GetLifeOrProb()) << " fail idx: " << std::to_string(fail_idx) << " reset hazard rate: " << std::to_string(true) << "\n";

}

bool Component::CanBeRepaired(std::string mode)
{
	//determines whether or not the component can be repaired.
	//if the plant is off, or the component can be repaired for any mode,
	//then return true.
	if (mode == "OFF" || m_repair_mode == "A")
	{
		return true;
	}
	//if the plant is in standby and the component may be repaired in standby,
	//return true.
	if (m_repair_mode == "S" && (mode == "SS" || mode == "SF" || mode == "SO") )
	{
		return true;
	}		
	return false;
}

void Component::ResetDowntime()
{
	m_status.downtime_remaining = m_status.repair_event_time*1.0;
}
        
void Component::AdvanceDowntime(double time, std::string mode)
{
    //moves forward in time while the plant is down, or on standby
	//if the repair mode is "S".
	if (CanBeRepaired(mode))
	{
		m_status.downtime_remaining -= time;
		if (m_status.downtime_remaining <= 0.0)
		{
			m_status.downtime_remaining = 0.0;
			m_status.operational = true;
		}
	}
}

std::vector<double> Component::GetLifetimesAndProbs()
{
	//returns a vector of component lifetimes/probabilities.
	std::vector<double> lifetimes_and_probs = {};
	for (size_t j=0; j < m_failure_types.size(); j++)
	{
		lifetimes_and_probs.push_back(m_failure_types.at(j).GetLifeOrProb());
	}
	return lifetimes_and_probs;
}
            
void Component::GenerateInitialLifesAndProbs(WELLFiveTwelve &gen)
{
	/*
	generates initial failures and probabilities for a component as if it were
	brand new.
	*/
	for (std::vector<FailureType>::iterator it = m_failure_types.begin(); 
		it != m_failure_types.end(); it++)
	{
		it->GenerateFailureVariate(gen);
	}
}

ComponentStatus Component::GetState()
{
    //returns current state as a ComponentStatus.
	return (
		ComponentStatus(
			GetLifetimesAndProbs(), m_status.hazard_rate*1.0,
			m_status.downtime_remaining*1.0, m_status.repair_event_time*1.0
			)
		); 
}

void Component::Reset(WELLFiveTwelve &gen)
{
	m_cooldown_time = 0.;
	m_status.downtime_remaining = 0.;
	m_status.hazard_rate = 1.;
	m_status.operational = true;
	m_status.repair_event_time = 0.;
	GenerateInitialLifesAndProbs(gen);
}