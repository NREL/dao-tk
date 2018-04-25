#ifndef _CLUSTER_H
#define _CLUSTER_H

struct cluster_inputs
{
	int a;
};

struct cluster_outputs
{
	int a;
};

class cluster
{
	int algorithm;							// Clustering algorithm
	int Ncluster;                      // Number of clusters
	int Nmaxiter;                     // Maximum iterations for clustering algorithm
	bool sim_hard_partitions;         // Use hard partitioning for simulation weighting factors ?

	double afp_preference_mult;          // Multiplier for default preference values(median of input similarities = negative Euclidean distance b / w points i and k)--> Larger multiplier = fewer clusters
	int afp_Nconverge;                 // Number of iterations without change in solution for convergence

	bool afp_enforce_Ncluster;       // Iterate on afp_preference_mult to create the number of clusters specified in Ncluster
	double afp_enforce_Ncluster_tol;       // Tolerance for number of clusters
	int afp_enforce_Ncluster_maxiter;  // Maximum number of iterations

public:
	enum ALGORITHM {AFFINITY_PROPOGATION};

	cluster_inputs inputs;
	cluster_outputs outputs;

	cluster();

	bool form_clusters();

};


#endif
