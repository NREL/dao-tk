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
    double new_life;

	failure_event();
	failure_event(int time, std::string component, int fail_idx, double duration,
		double new_life);

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

    ComponentStatus();
    ComponentStatus( std::vector<double> lifes, double hazard, 
			double downtime );
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
    double m_hot_start_penalty;
    double m_warm_start_penalty;
    double m_cold_start_penalty;
	double m_availability_reduction;
	double m_cooldown_time;

    ComponentStatus m_status;

    std::unordered_map< std::string, failure_event > *m_parent_failure_events;

	Distribution *m_repair_dist;

public:

    Component();

    Component(std::string name, std::string type, //std::string dist_type, double failure_alpha, double failure_beta, 
		double repair_rate, double repair_cooldown_time,
		double hot_start_penalty, double warm_start_penalty, double cold_start_penalty,
		std::unordered_map< std::string, failure_event > *failure_events, double availability_reduction = 1.0, double repair_cost = 0.0
		);

    void ReadStatus( ComponentStatus &status );
        
    std::string GetName();
        
    std::string GetType();

	std::vector<FailureType> GetFailureTypes();
        
	void AddFailureMode(std::string component, std::string id, std::string failure_mode,
		std::string dist_type, double alpha, double beta);

    double GetHazardRate();
    
    double GetRepairCost();

	double GetAvailabilityReduction();

	double GetCooldownTime();
        
    bool IsOperational();
        
    void Shutdown(double time);
        
    void RestoreComponent();

	void GenerateTimeToRepair(WELLFiveTwelve &gen);
        
    void ResetHazardRate();

	double GetDowntimeRemaining();
        
    void SetDowntimeRemaining(double time);

	double GetHotStartPenalty();

	double GetWarmStartPenalty();

	double GetColdStartPenalty();
        
    double HoursToFailure(double ramp_mult, std::string mode);

	void TestForBinaryFailure(std::string mode, int t, WELLFiveTwelve &gen);
	
	void TestForFailure(double time, double ramp_mult, WELLFiveTwelve &gen, 
		int t, std::string start, std::string mode);
         
    void Operate(double time, double ramp_mult, WELLFiveTwelve &gen, 
		bool read_only, int t, std::string start, std::string mode);
        
    void ReadFailure(double downtime, double life_remaining,
		int fail_idx, bool reset_hazard);
                
    void GenerateFailure(WELLFiveTwelve &gen, int t, int j); 
        
    void AdvanceDowntime(double time);

	std::vector<double> GetLifetimesAndProbs();

	void GenerateInitialLifesAndProbs(WELLFiveTwelve &gen);
            
	ComponentStatus GetState();

};


#endif