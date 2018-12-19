#ifndef _SF_HELIO_
#define _SF_HELIO_

#include <string>
#include <vector>
#include <random>

#include "./../libcycle/distributions.h"


enum helio_status {
	OPERATIONAL,   // Operational
	FAILED,		   // In a failed state and waiting for repairs
	REPAIRING,     // Being repaired
};




struct helio_component_inputs
{
	// Failure distribution parameters
	double m_beta;					// Weibull shape parameter
	double m_eta;					// Weibull characteristic life [hr]

	// Repair time distribution parameters (assumed exponential)
	double m_mean_repair_time;		// Mean repair time [hr]
	double m_min_repair_time;		// Min repair time [hr]
	double m_max_repair_time;		// Max repair time [hr]
	bool m_is_good_as_new;			// Repairs produce "good as new" conditions?

	// Repair cost
	double m_repair_cost;			// Cost per repair ($)

	helio_component_inputs();
	helio_component_inputs(double beta, double eta, double mean_repair, double min_repair, double max_repair, bool good_as_new, double repair_cost);
};




class solarfield_helio_component
{

private:

	helio_component_inputs m_properties;

	unsigned int m_status;			// Component status
	double m_repair_time_remaining;	// Time remaining until component repairs are completed [hr]
	double m_time_to_next_failure;  // Time until next failure [hr]
	double m_operational_age;		// Current age [hr]
	double m_age_at_last_failure;	// Age at last failure [hr]
	double m_scale;		
public:

	solarfield_helio_component();
	solarfield_helio_component(const helio_component_inputs &inputs);
	~solarfield_helio_component() {};

	void set_scale(double scale);
	void set_time_to_next_failure(WELLFiveTwelve &gen);
	void set_time_to_repair(WELLFiveTwelve &gen);

	bool test_for_failure(double timestep);
	void fail(WELLFiveTwelve &gen);
	void operate(double timestep);
	void repair(double repair_time);

	unsigned int get_operational_state();
	double get_repair_time();
	double get_time_to_next_failure();
	double get_mean_repair_time();
	double get_operational_age();
};










class solarfield_heliostat
{
private:
	std::vector<solarfield_helio_component *> m_components;	 // Components
	std::vector<int> m_n_failures;							// Total number of failures per component
	std::vector<int> m_n_repairs;							// Total number of repairs completed per component

	unsigned int m_status;				// Current operational state 
	int m_n_components;					// Number of components	

	double m_scale;						// Heliostat scale (i.e number of heliostats this heliostat represents)
	double m_performance;				// Performance metric (i.e. efficiency) that can be used to prioritize repairs
	double m_mean_repair_time;			// Mean time to repair (all failed components)
	double m_time_to_next_failure;		// Time to next failure [h]
	double m_repair_time_remaining;		// Total repair time needed for all components [h]

	double m_time_operating;			// Total time operational [hr]
	double m_time_repairing;			// Total time spent being repaired [hr]
	double m_time_failed;				// Total time spent waiting to be repaired [hr]

	bool m_is_track_repair_time;
	std::vector<double> m_repair_time_per_component;

	void add_component(const helio_component_inputs &inputs);

public:

	solarfield_heliostat();
	~solarfield_heliostat()
	{
		for (size_t i = 0; i < m_components.size(); i++)
			delete(m_components[i]);
		m_components.clear();
	};

	void initialize(const std::vector<helio_component_inputs> & components, WELLFiveTwelve &gen, double scale = 1.0, double performance = 1.0);

	int get_n_components();
	unsigned int get_operational_state();
	double get_total_repair_time();
	double get_mean_repair_time();
	double get_time_to_next_failure();
	double get_performance();
	std::vector<int> get_failed_components();
	std::vector<solarfield_helio_component *> get_components();

	void fail(double timestep, WELLFiveTwelve &gen);
	void operate(double timestep);
	int repair(double timestep);

	std::vector<int> get_failures_per_component();
	std::vector<int> get_repairs_per_component();

	void initialize_repair_time_tracking();
	std::vector<double>* get_repair_time_tracking();

};









class heliostat_field
{

public:
	std::vector<solarfield_heliostat *> m_helios;

	heliostat_field() {};
	~heliostat_field()
	{
		for (size_t i = 0; i < m_helios.size(); i++)
			delete(m_helios[i]);
		m_helios.clear();
	};
};


#endif
