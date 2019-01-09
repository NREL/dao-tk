#ifndef _COMPONENT_
#define _COMPONENT_

#include <string>
#include <vector>
#include <unordered_map>

#include "well512.h"
#include "distributions.h"
#include "failure.h"

//##################################################################################

struct failure_event
{
	int time;
	std::string component;
	int fail_idx;
    double duration;
	double labor;
    double new_life;
	int scen_index;

	failure_event();
	failure_event(int time, std::string component, int fail_idx, double duration,
		double labor, double new_life, int scen_index);

    std::string print();
};

//##################################################################################

struct ComponentStatus
{
	std::vector<double> lifetimes; 
	double hazard_rate;
    double downtime_remaining;
    bool operational;
    bool running;
	double repair_event_time;
	double age;

    ComponentStatus();
    ComponentStatus( std::vector<double> _lifes, double _hazard, 
			double _downtime, double _repair_event_time, double _age);
};





////##################################################################################

/* 
Covers all components of the plant that have a failure rate, repair rate, 
and maintenance threshold.
*/
class Component
{
	std::vector<FailureType> m_failure_types;
    double m_repair_cost;
    std::string m_name;
    std::string m_type;
	double m_capacity_reduction;
	double m_efficiency_reduction;
	bool m_new_repair;
	bool m_new_failure;
	bool m_reset_hazard_rate;
	double m_mean_repair_time;
	std::string m_repair_mode; // "A"=Anytime; "S"=standby or downtime; "D"=downtime only
    ComponentStatus m_status;

	std::unordered_map< std::string, failure_event > *m_parent_failure_events;
	std::vector< std::string > *m_parent_failure_event_labels;

	Distribution *m_repair_dist;

public:

    Component();

    Component(std::string name, 
		std::string type, 
		double mean_repair_time, 
		double repair_cooldown_time,
		std::unordered_map< std::string, failure_event > *failure_events, 
		double availability_reduction = 1.0, 
		double efficiency_reduction = 1.0, 
		double repair_cost = 0.0,
		std::string repair_mode = "D", 
		std::vector< std::string > *failure_event_labels = {},
		bool reset_hazard_rate = true
		);

    void ReadStatus( ComponentStatus &status );
        
    std::string GetName();
        
	std::string GetType();

	std::vector<FailureType> GetFailureTypes();
        
	void AddFailureMode(
		std::string component,
		std::string id,
		std::string failure_mode,
		std::string dist_type,
		double alpha,
		double beta
	);

    double GetHazardRate();
    
    double GetRepairCost();

	double GetMeanRepairTime();

	double GetCapacityReduction();

	double GetEfficiencyReduction();

	double GetEfficiency(bool override = false);

	double GetCapacity(bool override = false);

	double GetCooldownTime();
        
    bool IsOperational();

	bool IsNewFailure();

	bool IsNewRepair();

	void SetResetHazardRatePolicy(bool reset_hazard);

	void ResetFailureAndRepairFlags();
        
    void Shutdown(double time);

	void PerformMaintenance(double penalty_reduction);
        
    void RestoreComponent(bool reset_age = true);

	void GenerateTimeToRepair(WELLFiveTwelve &gen);
        
    void ResetHazardRate();

	double GetDowntimeRemaining();
        
    void SetDowntimeRemaining(double time);
        
    double HoursToFailure(double ramp_mult, std::string mode);

	void TestForBinaryFailure(
		std::string mode, 
		int t,
		WELLFiveTwelve &life_gen,
		WELLFiveTwelve &repair_gen,
		WELLFiveTwelve &binary_gen,
		int scen_index
	);
	
	void TestForFailure(
		double time,
		double ramp_mult,
		WELLFiveTwelve &life_gen,
		WELLFiveTwelve &repair_gen,
		WELLFiveTwelve &binary_gen,
		int t,
		double hazard_increase,
		std::string mode,
		int scen_index
	);
         
    void Operate(
		double time, 
		double ramp_mult,
		bool read_only,
		double hazard_increase, 
		std::string mode
	);
        
    void ReadFailure(double downtime, double life_remaining, int fail_idx);
                
    void GenerateFailure(
		WELLFiveTwelve &life_gen,
		WELLFiveTwelve &repair_gen,
		int t, 
		int j, 
		int scen_index
	); 

	bool CanBeRepaired(std::string mode);

	void ResetDowntime();

    void AdvanceDowntime(double time, std::string mode);

	std::vector<double> GetLifetimesAndProbs();

	void GenerateInitialLifesAndProbs(WELLFiveTwelve &gen);
            
	ComponentStatus GetState();

	void Reset(WELLFiveTwelve &gen);

	void SetFailLifeOrProb(int fail_idx, double life_prob);

};


#endif