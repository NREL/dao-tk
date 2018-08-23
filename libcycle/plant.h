#ifndef _PLANT_CAPACITY_
#define _PLANT_CAPACITY_

#include <vector>
#include <string>
#include <unordered_map>

#include "component.h"
#include "well512.h"


class PowerCycle
{
	WELLFiveTwelve *m_gen;
	std::vector< Component > m_components;
	std::unordered_map<std::string, std::vector<double> > m_dispatch;
	std::unordered_map< std::string, failure_event > m_failure_events;
	std::vector <std::string> m_failure_event_labels;
	std::unordered_map< std::string, ComponentStatus > m_component_status;
	std::unordered_map< std::string, double > m_plant_status;
	int m_read_periods;
	double m_maintenance_interval;
	double m_maintenance_duration;
	double m_ramp_threshold;
	double m_ramp_threshold_min;
	double m_ramp_threshold_max;
	double m_ramping_penalty_min = 1.2;  //Using Gas-CC
	double m_ramping_penalty_max = 4.0;  //Using Gas-CC
	double m_hours_to_maintenance;
	bool m_online;
	double m_power_output;
	bool m_standby;
	double m_steplength;
	int m_num_periods;
	int m_num_scenarios;
	double m_downtime_threshold;
	double m_downtime;
	double m_shutdown_capacity = 0.3;      //These represent a policy, and are 
	double m_no_restart_capacity = 0.96;   //assumed to be decision variables
	double m_eps;
	bool m_output;
	double m_capacity;
	double m_time_online;
	double m_time_in_standby;
	double m_condenser_temp_threshold;
	std::vector<double> m_condenser_efficiencies_cold;
	std::vector<double> m_condenser_efficiencies_hot;
	double m_hot_start_penalty;
	double m_warm_start_penalty;
	double m_cold_start_penalty;
	BoundedJohnsonDist m_hs_dist;
	BoundedJohnsonDist m_ws_dist;
	BoundedJohnsonDist m_cs_dist;
	int m_num_condenser_trains = 0;
	int m_num_feedwater_heaters = 0;
	int m_num_salt_steam_trains = 0;
	int m_num_salt_pumps = 0;
	int m_num_water_pumps = 0;
	int m_num_hp_turbines = 0;
	int m_num_mp_turbines = 0;
	int m_num_lp_turbines = 0;
	bool m_record_state_failure = true;


public:
	std::vector< std::string > output_log;
	void InitializeCyclingDists();
	void AssignGenerator(WELLFiveTwelve *gen);
	void GeneratePlantCyclingPenalties();
	void SetSimulationParameters(int read_periods, int num_periods, 
		double epsilon = 1.E-10, bool print_output = false, int num_scenarios = 1);
	void SetCondenserEfficienciesCold(std::vector<double> eff_cold);
	void SetCondenserEfficienciesHot(std::vector<double> eff_hot);
	void ReadComponentStatus(std::unordered_map< std::string, ComponentStatus > dstat);
	void ClearComponentStatus();
	void SetStatus();
	std::vector< Component >& GetComponents();
	std::vector< double > GetComponentLifetimes();
	std::vector< double >  GetComponentDowntimes();
	double GetHoursToMaintenance();
	bool AirstreamOnline();
	bool FWHOnline();
	bool IsOnline();
	bool IsOnStandby();
	double GetTimeInStandby();
	double GetTimeOnline();
	double GetRampThreshold();
	double GetSteplength();
	double GetHotStartPenalty();
	double GetWarmStartPenalty();
	double GetColdStartPenalty();
	void SetShutdownCapacity(double capacity);
	void SetNoRestartCapacity(double capacity);
	std::unordered_map< std::string, failure_event > GetFailureEvents();
	std::vector<std::string> GetFailureEventLabels();
	void AddComponent(std::string name, 
		std::string type, 
		double repair_rate, 
		double repair_cooldown_time,
		double Capacity_reduction,
		double repair_cost, 
		std::string repair_mode
	);
	void AddFailureType(std::string component, 
		std::string id,
		std::string failure_mode,
		std::string dist_type, 
		double alpha, 
		double beta
	);
	void CreateComponentsFromFile(std::string component_data);
	void AddCondenserTrain(int num_fans, int num_radiators);
	void AddSaltToSteamTrains(int num_trains);
	void AddFeedwaterHeaters(int num_fwh);
	void AddSaltPumps(int num_pumps);
	void AddWaterPumps(int num_pumps);
	void AddTurbines(int num_hi_pressure, int num_mid_pressure, int num_low_pressure);
	void GeneratePlantComponents(
		int num_condenser_trains,
		int fans_per_train,
		int radiators_per_train,
		int num_salt_steam_trains,
		int num_fwh,
		int num_salt_pumps,
		int num_water_pumps,
		int num_hi_pressure,
		int num_mid_pressure,
		int num_low_pressure,
		std::vector<double> condenser_eff_cold,
		std::vector<double> condenser_eff_hot
	);
	void SetPlantAttributes(double maintenance_interval,
		double maintenance_duration,
		double downtime_threshold,
		double steplength, double hours_to_maintenance,
		double power_output, bool standby, double capacity,
		double temp_threshold, double time_online, double time_in_standby,
		double downtime, double shutdown_capacity, double no_restart_capacity
	);
	void SetDispatch(std::unordered_map< std::string, std::vector< double > > &data, bool clear_existing = false);
	int NumberOfAirstreamsOnline();
	double GetCondenserEfficiency(double temp);
	double GetCycleCapacity(double temp);
	void TestForComponentFailures(double ramp_mult, int t, std::string start, std::string mode);
	bool AllComponentsOperational();
	double GetMaxComponentDowntime();
	void PlantMaintenanceShutdown(int t, bool reset_time, bool record, double duration = 0.);
	void AdvanceDowntime(std::string mode);
	double GetRampMult(double power_out);
	void OperateComponents(double ramp_mult, int t, std::string start, std::string mode);
	void ResetHazardRates();
	void StoreComponentState();
	void StorePlantState();
	void StoreState();
	std::string GetStartMode(int t);
	std::string GetOperatingMode(int t);
	std::vector< double > RunDispatch();
	void Operate(double power_out, int t, std::string start, std::string mode);
	std::vector< double > Simulate(bool reset_status);
	std::unordered_map< int, std::vector< double > > RunScenarios();
	void ResetPlant(WELLFiveTwelve &gen);
};



#endif