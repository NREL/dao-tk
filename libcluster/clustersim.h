#ifndef _clustersim_h_
#define _clustersim_h_


#include "cluster.h"
#include "metrics.h"
#include <cmath>



struct s_sim_days
{
	int ncount;   // Number of simulation days per group that contribute to results
	int nprev;	  // Number of days simulated prior to first day 
	int nnext;    // Number of days simulated after last day
};

struct s_combined_exemplars
{
	std::vector<int>n;					 // Number of original cluster exemplars per simulated grouping
	std::vector<int>start;				 // First cluster index in each combined grouping
};


struct s_clusters_skipped
{
	std::vector<int> index;				// Index identifying cluster to which "skipped" days belong
	matrix<double> partition_matrix;	// Partition matrix for "skipped" days
};



struct s_cluster_sim_params
{
	s_sim_days days;					// Days per simulation
	s_cluster_outputs cluster_results;	// Results from clustering: includes exemplars to be simulated and partition matrix
	s_combined_exemplars combined;		// Combined exemplars for simulation

	s_clusters_skipped skip_first;		// Assignment of "skipped" first day to clusters
	s_clusters_skipped skip_last;		// Assignment of "skipped" last day to clusters

	bool is_combine_consecutive;		// Combine simulations of consecutive exemplars
	bool is_run_continuous;				// Run single simulation continuously (skipping non-exemplar days) 
	bool is_clusteravg_sfavail;			// Use cluster-average solar field availability in simulations (if false, availability during exemplar time blocks will be used)

	bool is_initial_charge_heuristic;   // Use heuristic for initial charge state
	double initial_hot_charge;			// Uniform initial hot charge state (0-100) (ignored if is_initial_charge_heuristic)

};




class cluster_sim
{

public:

	s_cluster_sim_params inputs;

	cluster_sim();
	~cluster_sim() {};

	void set_default_inputs();

	void assign_first_last(const s_metric_outputs &metric_results);

	double initial_charge_heuristic(double prev_dni, double sm);

	int firstday(int g);
	int firstsimday(int g);
	std::vector<bool> set_all_sim_days();

	void cluster_avg_from_timeseries(const std::vector<double> &timeseries, matrix<double> & clusteravg, matrix<double> & count, int year = 0);
	void cluster_avg_from_timeseries(const matrix<double> &timeseries, matrix<double> & clusteravg);

	void overwrite_with_cluster_avg_values(std::vector<double> &timeseries, const matrix<double> &clusteravg, bool overwrite_surrounding, int year = 0);

	void combine_consecutive_exemplars();
	std::vector<double> compute_combined_clusteravg(int combined_index, const matrix<double> &clusteravg);

	void compute_annual_array_from_clusters(const std::vector<double> &exemplardata, std::vector<double> &fullyeardata);
	void compute_annual_array_from_clusters(const matrix<double> &exemplardata, matrix<double> &fullyeardata);

	void compute_dni_scaling_factors(const matrix<double> &daily_dni, matrix<double> &scale);


};


#endif

