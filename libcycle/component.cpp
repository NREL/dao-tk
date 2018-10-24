#include "component.h"
#include <cmath>
#include <algorithm>
#include <iostream>


//##################################################################################

failure_event::failure_event(){}

failure_event::failure_event(int _time, std::string _component, int _fail_idx, double _duration, double _labor, double _new_life, int _scen_index)
    : time( _time ), component( _component ), fail_idx( _fail_idx), duration(_duration), labor(_labor), new_life( _new_life ), scen_index(_scen_index)
{}


std::string failure_event::print()
{
    return "<" + component + ", " + std::to_string(duration) + ", " + std::to_string(new_life) + ", scenario " + std::to_string(scen_index) + ", period " + std::to_string(time);
}

//##################################################################################

ComponentStatus::ComponentStatus(){}

ComponentStatus::ComponentStatus(std::vector<double> _lifes, double _hazard, 
	double _downtime, double _repair_event_time, double _age)
    : lifetimes(_lifes), hazard_rate( _hazard ), downtime_remaining( _downtime ), repair_event_time( _repair_event_time ), age( _age )
{}

//##################################################################################

Component::Component()
{}

Component::Component(std::string name, std::string type,
		//std::string dist_type, double failure_alpha, double failure_beta, 
		double mean_repair_time, double repair_cooldown_time,
		std::unordered_map< std::string, failure_event > *failure_events,
		double capacity_reduction, double efficiency_reduction, double repair_cost, std::string repair_mode,
		std::vector<std::string> *failure_event_labels)
{
    /*
    Description of attributes:
    name -- component identifer
    type -- component type description
    repair_rate -- rate at which repairs take place (events/h)
    repair_cooldown_time -- added required downtime for repair (h)
	failure_events -- parent failure events dictionary 
	capacity_reduction -- reduction in cycle capacity if component fails (fraction)
	efficiency_reduction -- reduction in cycle capacity if component fails (fraction)
    repair_cost -- dollar cost of repairs, not including revenue lost ($)
    repair_mode -- indicator of in which modes the component may be repaired
	failure_event_labels -- keys to parent failure events dictionary
    */

    if( name == "MAINTENANCE" )
        throw std::invalid_argument("cannot name a component 'MAINTENANCE'");
    
	m_failure_types = {};
    m_name = name;
    m_type = type;
    m_repair_cost = repair_cost;
	m_capacity_reduction = capacity_reduction;
	m_efficiency_reduction = efficiency_reduction;
	m_repair_mode = repair_mode;
	m_mean_repair_time = mean_repair_time;
	m_new_failure = false;
	m_new_repair = false;

	m_status.hazard_rate = 1.0;
    m_status.downtime_remaining = 0.0;
	m_status.operational = true;
	m_status.repair_event_time = 0.0;
	m_status.age = 0.0;
	m_status.lifetimes = {};

	Distribution *edist = new ExponentialDist(mean_repair_time, repair_cooldown_time, "exponential");
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
	m_status.age = status.age;
	m_status.lifetimes = status.lifetimes;
}
        
std::string Component::GetName()
{
	/* accessor for the component's name (identifier). */
	return m_name;
}

        
std::string Component::GetType()
{
	/* accessor for the component's type (description). */
	return m_type;
}

std::vector<FailureType> Component::GetFailureTypes()
{
	/* accessor for vector of failure types/modes for the component. */
	return m_failure_types;
}

void Component::AddFailureMode(
	std::string component, 
	std::string id, 
	std::string failure_mode,
	std::string dist_type, 
	double alpha, 
	double beta
)
{
	/* 
	Adds a new failure mode for the compnent. 
	component -- component name
	id -- identifier/note for failure type
	failure_mode -- operating mode in which failure takes place or lifetime
	    is reduced.  
	    "ALL" - all non-downtime modes (online or standby)
		"OS" - start of online mode
		"OF" - first hour of online mode
		"OO" - ongoing (2nd hour forward) of online mode
		"SS" - start of standby mode
		"SF" - first hour of standby mode
		"SO" - ongoing (2nd hour forward) of standby mode
		"O" - online mode 
	dist_type -- indicator of the distribution family used for life/probability
	    generation
	alpha -- numeric parameter for distribution
	beta -- numeric parameter for distribution
	*/
	m_failure_types.push_back(FailureType(component, id, failure_mode, dist_type, alpha, beta));
	m_status.lifetimes.push_back(1.);
}
        
double Component::GetHazardRate()
{
	/* Acessor to component's current hazard rate. */
	return m_status.hazard_rate;
}

    
double Component::GetRepairCost()
{
	/* 
	accessor to component's repair cost (not 
	used currently, due to a lack of reliable data
	on component materials costs associated
	with repairs. 
	*/
	return m_repair_cost;
}

double Component::GetMeanRepairTime()
{
	/* accessor to component's mean repair time. */
	return m_mean_repair_time;
}

double Component::GetCapacityReduction()
{
	/* accessor to component's capacity reduction. */
	return m_capacity_reduction;
}

double Component::GetEfficiencyReduction()
{
	/* accessor to component's efficiency reduction. */
	return m_efficiency_reduction;
}

double Component::GetEfficiency(bool override)
{
	/*
	returns impact of turbine aging on efficiency.  Source:
	Staffel 2014 (notes that previous studies and techincal 
	reports indicate a decrease in efficiency of 0.15%-0.55%
	per year of operation.) and anecdotal evidence from a 
	partner that the manufacturer specs indicate a decrease
	of about 0.2% per year.  
	*/
	if (!IsOperational() && !override)
		throw std::runtime_error("Efficiency checked for a non-operating component");
	return 1.0 - (m_status.age * 0.002 / 5000);  //source: anecdotal; Staffel 2014 cites 0.15%-0.55% decline in efficiency per year.
}

double Component::GetCapacity(bool override)
{
	/*
	returns impact of turbine aging on capacity.  Source:
	Diakunchuk 1992; this is for gas engines, but cites
	a 1% decrease in efficiency = ~2.5% decrease in power.
	Need to verify if this makes sense for a steam turbine too.
	*/
	if (!IsOperational() && !override)
		throw std::runtime_error("Capacity checked for a non-operating component");
	return 1.0 - (m_status.age * 0.005 / 5000); 
}

double Component::GetCooldownTime()
{
	/*
	Accessor to component's remaining cooldown time.
	*/
	return m_repair_dist->GetBeta();
}
        
bool Component::IsOperational()
{
	/*
	Accessor to boolean indicator of a component being operational.
	*/
	return m_status.operational;
}

bool Component::IsNewFailure()
{
	return m_new_failure;
}

bool Component::IsNewRepair()
{
	return m_new_repair;
}

void Component::ResetFailureAndRepairFlags()
{
	m_new_repair = false;
	m_new_failure = false;
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
	m_new_failure = true;
    m_status.downtime_remaining = time;
	m_status.repair_event_time = time;
}

        
void Component::RestoreComponent(bool reset_age)
{
	/*
	Sets a component to an operational state.

	reset_age -- indicator to set age to zero on repair
	*/
	m_status.operational = true;
	m_status.downtime_remaining = 0.0;
	m_status.repair_event_time = 0.0;
	if (reset_age)
		m_status.age = 0.0;
}

        
void Component::ResetHazardRate()
{
	/* Sets hazard rate to one, i.e., treats component as good as new. */
	m_status.hazard_rate = 1.;
}

    
double Component::GetDowntimeRemaining()
{
	/* accessor to downtime remaining before a component is online. */
	return m_status.downtime_remaining;
}

        
void Component::SetDowntimeRemaining(double time)
{
	/* mutator for downtime remaining before a component is online. */
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
				m_status.lifetimes.at(j) / 
				(m_status.hazard_rate * ramp_mult)
				);
		}
	}
    return hours;
}


void Component::TestForBinaryFailure(
	std::string mode, 
	int t,
	WELLFiveTwelve &life_gen,
	WELLFiveTwelve &repair_gen,
	WELLFiveTwelve &binary_gen,
	int scen_index
)
{
	/*
	Tests for failures of components for the start of online mode
	(or standby mode).  Testing is done by generating a uniform
	(0,1) random variate and comparing that value to the failure
	probability times the hazard rate for each component for
	which the starting mode matches.

	mode -- operating mode identifier
	t -- time period index
	gen -- RNG engine
	scen_index -- scenario index for simulation; recorded with 
	failure events
	*/
	double var = 0.0;
	for (int j = 0; j < m_failure_types.size(); j++)
	{
		if (m_failure_types.at(j).GetFailureMode() == mode)
		{
			var = binary_gen.getVariate();
			if (var <= m_status.lifetimes.at(j)
				* m_status.hazard_rate)
			{
				GenerateFailure(life_gen, repair_gen, t, j, scen_index);
			}
		}
	}
}

void Component::TestForFailure(
	double time, 
	double ramp_mult,
	WELLFiveTwelve &life_gen,
	WELLFiveTwelve &repair_gen,
	WELLFiveTwelve &binary_gen,
	int t, 
	double hazard_increase, 
	std::string mode, 
	int scen_index
)
{
	/*
	Generates failure events under the provided dispatch, if there is not  
	sufficient life remaining in the component, or the RNG generates a 
	failure on start.
	*/
	if (mode == "OFF")
		return;
	std::string opmode;
	if (mode == "OS")
	// if starting standby or online, test for fail on start, 
	// then operate as if in the first hour of that mode to test
	// for failures during the time period.
	{
		TestForBinaryFailure(mode, t, life_gen, repair_gen, binary_gen, scen_index);
		opmode = "OF";
	}
	else if (mode == "SS")
	{
		TestForBinaryFailure(mode, t, life_gen, repair_gen, binary_gen, scen_index);
		opmode = "SF";
	}
	else
		opmode = mode;
	for (int j = 0; j < m_failure_types.size(); j++)
	{
		if (m_failure_types.at(j).GetFailureMode() == opmode || m_failure_types.at(j).GetFailureMode() == "ALL")
		{
			if (time * (m_status.hazard_rate + hazard_increase) * ramp_mult > m_status.lifetimes.at(j))
				GenerateFailure(life_gen, repair_gen, t, j, scen_index);
		}
		if (m_failure_types.at(j).GetFailureMode() == "O" && (opmode == "OO" || opmode == "OF" ) )
		{
			if (time * (m_status.hazard_rate + hazard_increase) * ramp_mult > m_status.lifetimes.at(j))
				GenerateFailure(life_gen, repair_gen, t, j, scen_index);
		}
	}
	
}

         
void Component::Operate(
	double time, 
	double ramp_mult, 
	bool read_only, 
	double hazard_increase, 
	std::string mode
)
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
        throw std::runtime_error("can't operate a plant in downtime.");
	m_status.hazard_rate += hazard_increase;
	std::string opmode;
	//if starting a mode, operate as if
	//in the first hour of operation for that mode to degrade lifetimes.
	if (mode == "OS")
		// if starting standby or online, test for fail on start, 
		// then operate as if in the first hour of that mode to test
		// for failures during the time period.
	{
		//TestForBinaryFailure(mode, t, gen, scen_index);
		opmode = "OF";
	}
	else if (mode == "SS")
	{
		//TestForBinaryFailure(mode, t, gen, scen_index);
		opmode = "SF";
	}
	else
		opmode = mode;
	for (size_t j = 0; j < m_failure_types.size(); j++)
	{
		if (m_failure_types.at(j).GetFailureMode() == opmode || (opmode != "OFF" && m_failure_types.at(j).GetFailureMode() == "ALL"))
		{
			if (time * m_status.hazard_rate * ramp_mult > m_status.lifetimes.at(j) && !read_only)
				throw std::runtime_error("failure should be thrown.");
			m_status.lifetimes.at(j) -= (time * m_status.hazard_rate * ramp_mult);
		}
		if (m_failure_types.at(j).GetFailureMode() == "O" && (opmode == "OO" || opmode == "OF"))
		{
			if (time * m_status.hazard_rate * ramp_mult > m_status.lifetimes.at(j) && !read_only)
				throw std::runtime_error("failure should be thrown.");
			m_status.lifetimes.at(j) -= (time * m_status.hazard_rate * ramp_mult);
		}
	}		
	if (opmode == "OO" || opmode == "OF")
		m_status.age += time;
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
	m_status.lifetimes.at(fail_idx) = (life_remaining);
	if (reset_hazard)
	{
		ResetHazardRate();
	}
}

                
void Component::GenerateFailure(
	WELLFiveTwelve &life_gen, 
	WELLFiveTwelve &repair_gen,
	int t, 
	int fail_idx, 
	int scen_index
)
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
	m_new_failure = true;
    GenerateTimeToRepair(repair_gen);
	double labor = m_status.downtime_remaining - GetCooldownTime();
	m_status.lifetimes.at(fail_idx) = m_failure_types.at(fail_idx).GenerateFailureVariate(life_gen);
    ResetHazardRate();
    //add a new failure to the parent (CSPPlant) failure queue
	std::string label = "S"+std::to_string(scen_index)+"T"+std::to_string(t)+GetName()+"F"+std::to_string(fail_idx);
    (*m_parent_failure_events)[label] = failure_event(
		t, GetName(), fail_idx, m_status.downtime_remaining, labor,
		m_status.lifetimes.at(fail_idx), scen_index
		);
	(*m_parent_failure_event_labels).push_back(label);
	//std::cerr << "FAILURE EVENT GENERATED. downtime: " << std::to_string(m_status.downtime_remaining) << " life_rem: " << std::to_string(m_failure_types.at(fail_idx).GetLifeOrProb()) << " fail idx: " << std::to_string(fail_idx) << " reset hazard rate: " << std::to_string(true) << "\n";

}

bool Component::CanBeRepaired(std::string mode)
{
	/*
	determines whether or not the component can be repaired.
	if the plant is off, or the component can be repaired for any mode,
	then return true.

	mode -- operating mode indicator
	*/
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
	/* 
	Resets downtime; used if plant comes online before repair is completed 
	and component cannot be repaired while the plant is online.
	*/
	m_status.downtime_remaining = m_status.repair_event_time*1.0;
}
        
void Component::AdvanceDowntime(double time, std::string mode)
{
    //moves forward in time while the plant is down, or on standby
	//if the repair mode is "S".
	if (m_status.operational)
		return;
	if (CanBeRepaired(mode))
	{
		m_status.downtime_remaining -= time;
		if (m_status.downtime_remaining <= 0.0)
		{
			m_status.downtime_remaining = 0.0;
			m_status.operational = true;
			m_new_repair = true;
		}
	}
}

std::vector<double> Component::GetLifetimesAndProbs()
{
	//returns a vector of component lifetimes/probabilities.
	return m_status.lifetimes;
}
            
void Component::GenerateInitialLifesAndProbs(WELLFiveTwelve &gen)
{
	/*
	generates initial failures and probabilities for a component as if it were
	brand new.
	*/
	for (int i=0; i<m_failure_types.size(); i++)
	{
		SetFailLifeOrProb (
			i, m_failure_types.at(i).GenerateFailureVariate(gen)
		);
	}
}

ComponentStatus Component::GetState()
{
    //returns current state as a ComponentStatus.
	return (
		ComponentStatus(
			GetLifetimesAndProbs(), m_status.hazard_rate*1.0,
			m_status.downtime_remaining*1.0, m_status.repair_event_time*1.0,
			m_status.age * 1.0
			)
		); 
}

void Component::Reset(WELLFiveTwelve &gen)
{
	/*
	Reverts the component to an initial state, and generates a new collection
	of failure lifetimes and probabilities.

	gen -- RNG Engine
	*/
	m_status.downtime_remaining = 0.;
	m_status.hazard_rate = 1.;
	m_status.operational = true;
	m_status.repair_event_time = 0.;
	m_status.age = 0.;
	GenerateInitialLifesAndProbs(gen);
	ResetFailureAndRepairFlags();
}

void Component::SetFailLifeOrProb(int fail_idx, double life_prob)
/*
Mutator for a failure lifetime or probability for a component. 

fail_idx -- index of failure to mutate
life_prob -- value to assign to given lifetime / probability of failure
*/
{
	m_status.lifetimes.at(fail_idx) = life_prob;
}

