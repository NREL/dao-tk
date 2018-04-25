#include "cluster.h"
#include "lib_weatherfile.h"

cluster::cluster()
{
	algorithm = ALGORITHM::AFFINITY_PROPOGATION;							// Clustering algorithm
	Ncluster = 40;                       // Number of clusters
	Nmaxiter = 200;                      // Maximum iterations for clustering algorithm
	sim_hard_partitions = true;          // Use hard partitioning for simulation weighting factors ?

	afp_preference_mult = 1.0;           // Multiplier for default preference values(median of input similarities = negative Euclidean distance b / w points i and k)--> Larger multiplier = fewer clusters
	afp_Nconverge = 10;                  // Number of iterations without change in solution for convergence

	afp_enforce_Ncluster = false;        // Iterate on afp_preference_mult to create the number of clusters specified in Ncluster
	afp_enforce_Ncluster_tol = 1;        // Tolerance for number of clusters
	afp_enforce_Ncluster_maxiter = 50;   // Maximum number of iterations

}

bool cluster::form_clusters()
{
	weatherfile wf;

	return true;
}
