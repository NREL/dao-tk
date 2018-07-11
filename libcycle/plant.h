#ifndef _PLANT_AVAIL_
#define _PLANT_AVAIL_

#include <vector>
#include <string>
#include <unordered_map>

#include "component.h"
#include "well512.h"


class CSPPlant
{
    WELLFiveTwelve *m_gen;
    std::vector< Component > m_components;
    std::unordered_map<std::string, std::vector<double> > m_dispatch;
    std::unordered_map< std::string, failure_event > m_failure_events;
	std::unordered_map< std::string, ComponentStatus > m_component_status;
	std::unordered_map< std::string, double > m_plant_status;
    int m_read_periods;
    double m_maintenance_interval;
	double m_maintenance_duration;
	double m_ramp_threshold;
	double m_ramp_threshold_min;
	double m_ramp_threshold_max;
	double m_ramping_penalty_min = 2.0;
	double m_ramping_penalty_max = 8.0;
    double m_hours_to_maintenance;
    bool m_online;
    double m_power_output;
    bool m_standby;
    double m_steplength;
    int m_num_periods;
    double m_downtime_threshold;
    double m_downtime;
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

	

public:
    std::vector< std::string > output_log;
	void InitializeCyclingDists();
    void AssignGenerator( WELLFiveTwelve *gen );
	void GeneratePlantCyclingPenalties();
    void SetSimulationParameters( int read_periods, int num_periods, double epsilon, bool print_output);
	void SetCondenserEfficienciesCold(std::vector<double> eff_cold);
	void SetCondenserEfficienciesHot(std::vector<double> eff_hot);
	void ReadComponentStatus(std::unordered_map< std::string, ComponentStatus > dstat);
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
	std::unordered_map< std::string, failure_event > GetFailureEvents();
    void AddComponent( std::string name, std::string type, //std::string dist_type, double failure_alpha, double failure_beta, 
		double repair_rate, double repair_cooldown_time, 
        double availability_reduction = 1.0, 
		double repair_cost = 0., std::string repair_mode = "D");
	void AddFailureType(std::string component, std::string id, std::string failure_mode,
		std::string dist_type, double alpha, double beta);
    void CreateComponentsFromFile(std::string component_data);
	void AddCondenserTrain(int num_fans, int num_radiators);
	void AddSaltToSteamTrains(int num_trains);
	void AddFeedwaterHeaters(int num_fwh);
	void AddSaltPumps(int num_pumps);
	void AddWaterPumps(int num_pumps);
	void SetPlantAttributes(double maintenance_interval,
		double maintenance_duration,
		double ramp_threshold, double downtime_threshold,
		double steplength, double hours_to_maintenance,
		double power_output, bool standby, double capacity,
		double temp_threshold, double time_online, double time_in_standby,
		double downtime
		);
    void SetDispatch(std::unordered_map< std::string, std::vector< double > > &data, bool clear_existing=false);
	int NumberOfAirstreamsOnline();
	double GetCondenserEfficiency(double temp);
	double GetCycleAvailability(double temp);

	void TestForComponentFailures(double ramp_mult, int t, std::string start, std::string mode);
	bool AllComponentsOperational();
    void PlantMaintenanceShutdown(int t, bool record);
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

};



#endif