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

public:

	solarfield_helio_component();
	solarfield_helio_component(const helio_component_inputs &inputs);
	~solarfield_helio_component() {};


	double gen_lifetime(WELLFiveTwelve &gen, double age);
	double gen_repair_time(WELLFiveTwelve &gen);

	double get_mean_repair_time();
};










class solarfield_heliostat
{
private:
	std::vector<int> m_n_failures;							// Total number of failures per component
	std::vector<int> m_n_repairs;							// Total number of repairs completed per component
	std::vector<double> m_lifetimes;   
	std::vector<solarfield_helio_component*> m_components;

	unsigned int m_status;				// Current operational state 
	int m_n_components;					// Number of components	

	double m_scale;						// Heliostat scale (i.e number of heliostats this heliostat represents)
	double m_output;                    // annual energy output
	double m_performance;				// Performance metric (i.e. annual energy output) that can be used to prioritize repair

	double m_time_operating;			// Total time operational [hr]
	double m_time_repairing;			// Total time spent being repaired [hr]
	double m_time_failed;				// Total time spent waiting to be repaired [hr]

	double m_time_of_last_event;        // Time at last failure, repair start, 
										// or repair end - used to track waiting time
	double m_time_to_next_failure;      // Time to next failure - used for event generation
	int m_next_component_to_fail;       // Used for event generation
	double m_repair_time;               // Repair time - used for event generation

	bool m_is_track_repair_time;
	std::vector<double> m_repair_time_per_component;

public:

	solarfield_heliostat();
	~solarfield_heliostat(){};

	void initialize(std::vector<solarfield_helio_component*> components, WELLFiveTwelve &gen, double scale = 1.0, double performance = 1.0);

	int get_n_components();
	unsigned int get_operational_state();
	double get_performance();
	double get_time_to_next_failure();
	void update_next_failure();
	unsigned int get_next_component_to_fail();
	std::vector<solarfield_helio_component*> get_components();

	void fail(double time, WELLFiveTwelve &gen);
	void start_repair(double time);
	void end_repair(double time, int idx);

	std::vector<int> get_failures_per_component();
	std::vector<int> get_repairs_per_component();

	void initialize_repair_time_tracking();
	std::vector<double>* get_repair_time_tracking();

};









class heliostat_field
{

public:
	std::vector<solarfield_heliostat *> m_helios;
	std::vector<solarfield_helio_component *>  m_components;

	heliostat_field() {};
	~heliostat_field()
	{
		for (size_t i = 0; i < m_helios.size(); i++)
			delete(m_helios[i]);
		m_helios.clear();
		for (size_t i = 0; i < m_components.size(); i++)
			delete(m_components[i]);
		m_components.clear();
	};

	void add_component(const helio_component_inputs &inputs);
};


#endif
