#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "matrixtools.h"
#include <vector>



enum CLUSTER_ALGORITHM { AFFINITY_PROPAGATION, KMEANS, RANDOM_SELECTION };


struct s_cluster_inputs
{
	unsigned int alg;		// Clustering algorithm
	int ncluster;			// Target number of clusters
	int randseed;			// Random seed
	bool hard_partitions;	// Compute partition matrix with hard partitions?
	int nitermax;			// Maximum number of iterations 

							// Parameters specific to affinity-propagation algorithm
	bool enforce_ncluster;  // Enforce specified number of clusters?
	double pref_mult;		// Preference multiplier (used as initial guess if enforce_ncluster = True)
	int ncluster_tol;		// Tolerance for number of clusters (only if enforce_ncluster = True
	int nc_itermax;			// Max iterations to adjust preference multipler to create specified number of clusters (only if enforce_ncluster = True)
	int nconverge;			// Number of iterations without change in selected exemplars 
	double damping;			// Damping factor (0.5-1)	

							// Parameters specific to k-means algorithm
	int ninit;				// Number of re-initializations 
	double converge;		// Convergence criteria (relative change in wcss)

};


struct s_cluster_outputs
{
	bool converged;						// Algorithm converged before iteration limits?
	int ncluster;						// Number of clusters created
	double wcss;						// Within cluster sum of squares
	double wcss_to_exemplars;			// Wither clutser sum of squares based on exemplar location 

	std::vector<int> index;				// Cluster index for each data point
	std::vector<int> exemplars;			// Cluster exemplar data points
	std::vector<double> count;			// Number of points per cluster (can be fractional in the case of hard partitioning)
	std::vector<double> weights;		// Cluster weights

	matrix<double> means;				// Cluster means
	matrix<double> partition_matrix;	// Partition matrix

};




class cluster_alg
{
protected:

	std::vector<int> find_points_in_cluster(const std::vector<int>&index, int c);

	matrix<double> assign_means_from_exemplars(const matrix<double> &data, const std::vector<int> &exemplars);

	matrix<double> calculate_dist_to_clustermeans(const matrix<double> &data, const matrix<double> &means);

	std::vector<double> calculate_weights(const matrix<double> &partition_matrix);

	void select_all(const matrix<double> &data);

	void sort();

	void assign_to_cluster(const matrix<double> &distsqr, bool is_hard_partition, double mfuzzy,
		double &wcss, std::vector<double>&distmin, std::vector<int>&index, matrix<double> &partition_matrix);

	void afp_algorithm(const matrix<double> &data, matrix<double> &dist);

	void kmeans_algorithm(const matrix<double> &data);

	void random_algorithm(const matrix<double> &data);


public:
	s_cluster_inputs inputs;
	s_cluster_outputs results;
	cluster_alg();
	~cluster_alg() {};

	void set_default_inputs();

	void create_clusters(const matrix<double> &data);

	void assign_to_cluster(const matrix<double> &data, const matrix<double> &means, bool is_hard_partition, double mfuzzy,
		double &wcss, std::vector<double>&distmin, std::vector<int>&index, matrix<double> &partition_matrix);

};



#endif
