#ifndef _PLANT_CAPACITY_
#define _PLANT_CAPACITY_

#include <vector>
#include <string>
#include <unordered_map>

#include "component.h"
#include "well512.h"
#include "distributions.h"
#include "plant_struct.h"
#include "lib_util.h"


class PowerCycle
{
	//RNG Engine
	WELLFiveTwelve *m_life_gen;
	WELLFiveTwelve *m_repair_gen;
	WELLFiveTwelve *m_binary_gen;

	//Components
	std::vector< Component > m_components;
	std::vector< size_t > m_turbine_idx;  //turbine indices 
 	std::vector< size_t > m_sst_idx;   //salt-to-steam train indices 
	std::vector< size_t > m_condenser_idx; //condenser train indices 
	std::vector< size_t > m_salt_pump_idx; //salt pump indices 
	std::vector< size_t > m_water_pump_idx; //water pump indices 
	std::vector< size_t > m_boiler_pump_idx; //boiler pump indices
	
	int m_num_condenser_trains = 0;
	int m_fans_per_condenser_train = 0;
	int m_radiators_per_condenser_train = 0;
	double m_eff_loss_per_fan = 0.01;   //currently hard-coded as an assumption
	double m_condenser_temp_threshold = 20.;
	double m_pump_switch_mean_time = 0.3; //hours
	std::vector<double> m_condenser_efficiencies_cold;
	std::vector<double> m_condenser_efficiencies_hot;
	int m_num_feedwater_heaters = 0;
	int m_num_salt_steam_trains = 0;
	int m_num_salt_pumps = 0;
	int m_num_salt_pumps_required = 0;   //number required for capacity operation
	int m_num_water_pumps = 0;
	int m_num_water_pumps_required = 0;
	int m_num_boiler_pumps = 0;
	int m_num_boiler_pumps_required = 0;
	int m_num_turbines = 0; // HP, IP, LP turbines and generator count 
							// as a single component
	//Dispatch inputs
	std::unordered_map<std::string, std::vector<double> > m_dispatch;

	//Failure Events
	std::unordered_map< std::string, failure_event >  m_failure_events;
	std::vector <std::string>  m_failure_event_labels;
	std::vector <failure_event> m_all_failures;

	std::unordered_map< std::string, ComponentStatus > m_start_component_status;

	//ramping parameters
	double m_ramp_threshold;   //these are calculated with capacity as input
	double m_ramp_threshold_min;
	double m_ramp_threshold_max;
	double m_ramping_penalty_min = 1.2;  //Using Gas-CC
	double m_ramping_penalty_max = 4.0;  //Using Gas-CC

	//Cycling distriutions for hazard rate increase
	BoundedJohnsonDist m_hs_dist;
	BoundedJohnsonDist m_ws_dist;
	BoundedJohnsonDist m_cs_dist;

	//These thresholds for shutdown represent a policy, and are assumed to be 
	//decision variables
	double m_shutdown_capacity = 0.0;      
	double m_no_restart_capacity = 0.0;   
	double m_shutdown_efficiency = 0.0;
	double m_no_restart_efficiency = 0.0;

	//Current status members
	int m_current_scenario;
	double m_cycle_efficiency;
	double m_cycle_capacity;
	bool m_new_failure_occurred;
	bool m_new_repair_occurred;

public:
	PowerCycle();
	void Initialize(double age = 0., int scen_idx = 0);
	cycle_state m_current_cycle_state;
	cycle_state m_begin_cycle_state;
	cycle_file_settings m_file_settings;
	cycle_results m_results;
	simulation_params m_sim_params;
	std::vector< std::string > output_log;
	void InitializeCyclingDists();
	void AssignGenerators(
		WELLFiveTwelve *gen1,
		WELLFiveTwelve *gen2,
		WELLFiveTwelve *gen3
	);
	void GeneratePlantCyclingPenalties();
	void SetHotStartPenalty(double pen);
	void SetWarmStartPenalty(double pen);
	void SetColdStartPenalty(double pen);
	void SetSimulationParameters(
		int read_periods = 0,
		int sim_length = 48,
		double steplength = 1,
		bool print_output = false,
		int num_scenarios = 1,
		double hourly_labor_cost = 50.,
		bool stop_at_first_repair = false,
		bool stop_at_first_failure = false
	);
	void SetCondenserEfficienciesCold(std::vector<double> eff_cold);
	void SetCondenserEfficienciesHot(std::vector<double> eff_hot);
	void ClearComponents();
	void ReadComponentStatus(
		std::unordered_map< std::string, ComponentStatus > dstat);
	void ClearComponentStatus();
	void ReadCycleStateFromResults();
	void SetStartComponentStatus();
	void StoreComponentState();
	void StorePlantParamsState();
	void StoreCycleState();
	void RecordFinalState();
	void RevertToStartState(bool reset_rng);

	void WriteStateToFiles();
	void WritePlantLayoutFile();
	void WriteComponentFile();
	void WritePlantStateFile();
	void WriteSimParamsFile();
	void WriteFailuresFile();
	void WriteAMPLParams(int extra_periods = 0);
	void WriteAMPLParamsToDefault();
	void WriteCapEffFile();
	void WriteFinalEffFile();
	void WriteFailureStats();

	void ReadStateFromFiles(bool init);
	void ReadPlantLayoutFile();
	void ReadComponentFile();
	void ReadPlantFile();
	void ReadSimParamsFile();
	void ReadDayIDXFile();
	void ReadPolicyFile();
	void ReadFailuresFromFile();
	void ReadDispatchFile(int max_rows=8760);
	void ReadCapEffFile();

	std::vector< Component >& GetComponents();
	std::vector< double > GetComponentLifetimes();
	std::vector< double >  GetComponentDowntimes();
	double GetHoursToMaintenance();

	bool AirstreamOnline();
	bool FWHOnline();
	bool IsOnline();
	bool IsOnStandby();
	bool NewRepairOccurred();
	bool NewFailureOccurred();
	void ResetCycleEventFlags();

	double GetTimeInStandby();
	double GetTimeOnline();
	double GetRampThreshold();
	double GetSteplength();
	double GetHotStartPenalty();
	double GetWarmStartPenalty();
	double GetColdStartPenalty();
	int GetSimLength();
	double GetShutdownCapacity();
	double GetNoRestartCapacity();
	double GetShutdownEfficiency();
	double GetNoRestartEfficiency();
	int GetScenarioIndex();
	std::unordered_map<std::string, std::vector<double> > GetDispatch();

	void SetShutdownCapacity(double capacity);
	void SetNoRestartCapacity(double capacity);
	void SetShutdownEfficiency(double efficiency);
	void SetNoRestartEfficiency(double efficiency);
	void SetScenarioIndex(int idx);

	std::unordered_map< std::string, failure_event > GetFailureEvents();
	std::vector<std::string> GetFailureEventLabels();
	void AddComponent(std::string name, 
		std::string type, 
		double repair_rate, 
		double repair_cooldown_time,
		double capacity_reduction,
		double efficicency_reduction,
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
	void AddCondenserTrains(int num_trains, int num_fans, int num_radiators);
	void AddSaltToSteamTrains(int num_trains, double hx_failure_rate_mult);
	void AddFeedwaterHeaters(int num_fwh);
	void AddSaltPumps(int num_pumps, int num_required);
	void AddWaterPumps(int num_pumps, int num_required);
	void AddBoilerPumps(int num_pumps, int num_required);
	void AddTurbines(int num_turbines);
	void GeneratePlantComponents(
		int num_condenser_trains = 2,
		int fans_per_train = 30,
		int radiators_per_train = 1,
		int num_salt_steam_trains = 2,
		int num_fwh = 6,
		int num_salt_pumps = 3,
		int num_salt_pumps_required = 2,
		int num_water_pumps = 2,
		int num_water_pumps_required = 1,
		int num_boiler_pumps = 2,
		int num_boiler_pumps_required = 1,
		int num_turbines = 1,
		std::vector<double> condenser_eff_cold = { 0.,1.,1. },
		std::vector<double> condenser_eff_hot = { 0.,0.95,1. },
		double hx_failure_rate_mult = 1.0,
		bool reset_hazard = false
	);
	void SetPlantAttributes(
		double maintenance_interval = 1.e6,
		double maintenance_duration = 168.,
		double downtime_threshold = 24.,
		double hours_to_maintenance = 1.e6,
		double power_output = 0.,
		double thermal_output = 0.,
		bool current_standby = false,
		double capacity = 5.e5,
		double thermal_capacity = 1.5e6,
		double temp_threshold = 20.,
		double time_online = 0.,
		double time_in_standby = 0.,
		double downtime = 0.,
		double shutdown_capacity = 0.3,
		double no_restart_capacity = 0.8,
		double shutdown_efficiency = 0.7,
		double no_restart_efficiency = 0.9
	);
	void SetRampingThresholds();
	void SetDispatch(
		std::unordered_map< std::string, 
		std::vector< double > > &data, 
		bool clear_existing = false
	);
	int NumberOfAirstreamsOnline();
	double GetCondenserEfficiency(double temp);
	double GetCondenserCapacity(double temp);
	double GetTurbineEfficiency(bool age, bool include_failures = false);
	double GetTurbineCapacity(bool age, bool include_failures = false);
	double GetSaltSteamTrainCapacity();
	double GetSaltPumpCapacity();
	double GetWaterPumpCapacity();
	double GetBoilerPumpCapacity();
	double GetSaltPumpEfficiency();
	double GetWaterPumpEfficiency();
	double GetBoilerPumpEfficiency();
	void SetCycleCapacityAndEfficiency(double temp, bool age = false);
	double GetCycleCapacity();
	double GetCycleEfficiency();
	void TestForComponentFailures(
		double ramp_mult, 
		int t, 
		std::string start, 
		std::string mode
	);
	bool AllComponentsOperational();
	double GetMaxComponentDowntime();
	void PlantMaintenanceShutdown(
		int t,
		bool reset_time, 
		bool record, 
		double duration = 0.
	);
	void AddPumpSwitchingEvent(
		int t,
		std::string component,
		std::string pump_type,
		bool record,
		double switch_time = 0.0
	);
	void AdvanceDowntime(std::string mode);
	double GetRampMult(double power_out);
	void OperateComponents(
		double ramp_mult, 
		int t, 
		std::string start, 
		std::string mode,
		double duration = 1.0
	);
	void ResetHazardRates();
	std::unordered_map<std::string, ComponentStatus> GetComponentStates();
	std::string GetStartMode(int t);
	std::string GetOperatingMode(int t);
	void ReadInComponentFailures(int t);
	void ReadInMaintenanceEvents(int t);
	int FirstPeriodOfDifference(
		std::vector<double> cycle_efficiencies,
		std::vector<double> cycle_capacities
	);
	void RunDispatch();
	void OperatePlant(double power_out, double thermal_out, int t, 
		std::string start, std::string mode);
	void SingleScen(bool reset_plant, bool read_state_from_file = false,
		bool init = false);
	void GetSummaryResults();
	double GetLaborCosts(size_t start_fail_idx);
	void StoreScenarioResults(std::vector <double> cycle_efficiencies,
		std::vector <double> cycle_capacities);
	void Simulate(
		bool read_state_from_file = false, 
		bool read_state_from_memory = false,
		bool run_only_previous_failures = false,
		bool init = false
	);
	void ResetPlant();
	void PrintComponentStatus();
	void ClearFailureEvents();
	bool AnyFailuresOccurred();
	double GetEstimatedMinimumLifetime(double frac_operational = 1.0);
	double GetExpectedStartsToNextFailure();
	void AgePlant(double age);
	std::vector<std::string> GetAllFailureEventLabels();
	std::vector<double> GetAllFailureEventDowntimes();
	std::vector<int> GetAllFailureEventPeriods();
};



#endif