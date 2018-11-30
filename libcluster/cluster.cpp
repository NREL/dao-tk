#include "cluster.h"


#include <random>
#include <algorithm>



s_cluster_outputs::s_cluster_outputs()
{
	converged = false;
	ncluster = std::numeric_limits<int>::quiet_NaN();
	wcss = std::numeric_limits<double>::quiet_NaN();
	wcss_to_exemplars = std::numeric_limits<double>::quiet_NaN();
}


void s_cluster_outputs::clear()
{
	converged = false;
	ncluster = std::numeric_limits<int>::quiet_NaN();
	wcss = std::numeric_limits<double>::quiet_NaN();
	wcss_to_exemplars = std::numeric_limits<double>::quiet_NaN();
	index.clear();
	exemplars.clear();
	count.clear();
	weights.clear();
	means.clear();
	partition_matrix.clear();
}




cluster_alg::cluster_alg()
{
	inputs.alg = AFFINITY_PROPAGATION;
	inputs.ncluster = inputs.randseed = inputs.nitermax = std::numeric_limits<int>::quiet_NaN();
	inputs.hard_partitions = true;

	inputs.ncluster_tol = inputs.nc_itermax = inputs.nconverge = inputs.ninit = std::numeric_limits<int>::quiet_NaN();
	inputs.pref_mult = inputs.damping = inputs.converge = std::numeric_limits<double>::quiet_NaN();
}


void cluster_alg::set_default_inputs()
{
	inputs.alg = AFFINITY_PROPAGATION;
	inputs.ncluster = 40;
	inputs.randseed = 123;
	inputs.hard_partitions = true;
	inputs.nitermax = 200;

	// affinity propagation parameters
	inputs.enforce_ncluster = true;
	inputs.pref_mult = 1.0;
	inputs.ncluster_tol = 1;
	inputs.nc_itermax = 100;
	inputs.nconverge = 10;
	inputs.damping = 0.5;

	// kmeans parameters
	inputs.ninit = 20;
	inputs.converge = 0.001;

	return;
}




std::vector<int> cluster_alg::find_points_in_cluster(const std::vector<int>&index, int c)
{
	/*
	Inputs: index = vector containing cluster index to which each data point is assigned
	c = cluster index for which to find points
	Output: vector of all data points belong to cluster c
	*/
	std::vector<int> pts;
	for (int i = 0; i < (int)index.size(); i++)
	{
		if (index.at(i) == c)
			pts.push_back(i);
	}
	return pts;
}

matrix<double> cluster_alg::assign_means_from_exemplars(const matrix<double> &data, const std::vector<int> &exemplars)
{
	/*
	Assign cluster means based on specified set of exemplars
	Inputs: data = matrix with rows = # points, columns = # features
	exemplars = vector containing exemplar points
	Output:cluster means with row = cluster, column = cluster mean for each data feature
	*/

	int nfeatures = int(data.ncols);
	int nc = int(exemplars.size());

	matrix<double> means(nc, nfeatures, 0.0);

	for (int j = 0; j < nc; j++)
	{
		int m = exemplars.at(j);
		for (int f = 0; f < nfeatures; f++)
			means.at(j, f) = data.at(m, f);
	}
	return means;
}

matrix<double> cluster_alg::calculate_dist_to_clustermeans(const matrix<double> &data, const matrix<double> &means)
{
	/*
	Calculate squared Euclidean distance between data points
	Inputs: data: matrix with rows = # points, columns = # features
	means: row = cluster, column = cluster mean for each data feature
	Outputs: matrix with rows = # points, columns = distance to each cluster mean
	*/

	int nobs = int(data.nrows);
	int nfeatures = int(data.ncols);
	int nc = int(means.nrows);
	matrix<double> distsqr(nobs, nc, 0.0);
	for (int i = 0; i < nobs; i++)
	{
		for (int j = 0; j < nc; j++)
		{
			for (int f = 0; f < nfeatures; f++)
			{
				if (data.at(i, f) == data.at(i, f))   // Ignore NAN data
					distsqr.at(i, j) += pow(data.at(i, f) - means.at(j, f), 2.0);
			}
		}
	}
	return distsqr;
}


std::vector<double> cluster_alg::calculate_weights(const matrix<double> &partition_matrix)
{
	/*
	Calculate weight for each cluster
	Input: partition = partition matrix with rows = data point, columns = fraction of data point belonging to each cluster
	Output: weights = vector with cluster weight
	*/

	int nobs = int(partition_matrix.nrows);
	std::vector<double> weights = partition_matrix.sum_rows();
	for (int j = 0; j < (int)weights.size(); j++)
		weights.at(j) /= (double)nobs;

	return weights;
}



void cluster_alg::assign_to_cluster(const matrix<double> &distsqr, bool is_hard_partition, double mfuzzy,
	double &wcss, std::vector<double>&distmin, std::vector<int>&index, matrix<double> &partition_matrix)
{
	/*
	Assign each data point to a cluster based on minimum Euclidean distance to closest cluster mean
	Inputs: distsqr = matrix with rows = # points, columns = distance to each cluster
	is_hard_partition = true: hard partitioning, false = soft partitions
	mfuzzy = fuzziness factor for soft partitions (ignored if is_hard_partition = true)
	Outputs: wcss = within-cluster sum-of-squares
	distmin = vector of distances between each data point and closest cluster mean
	index = vector containing cluster index to which each data point is assigned
	partition_matrix = partition matrix with rows = data point, columns = fraction of data point belonging to each cluster
	*/

	int nobs = int(distsqr.nrows);
	int nc = int(distsqr.ncols);
	wcss = 0.0;
	distmin.resize(nobs);
	index.assign(nobs, -1);
	partition_matrix.resize_fill(nobs, nc, 0.0);

	for (int i = 0; i < nobs; i++)
	{

		int jbest = 0;
		for (int j = 1; j < nc; j++)
			jbest = (distsqr.at(i, j) <= distsqr.at(i, jbest) ? j : jbest);

		distmin.at(i) = distsqr.at(i, jbest);
		wcss += distsqr.at(i, jbest);
		index.at(i) = jbest;

		if (is_hard_partition)
			partition_matrix.at(i, jbest) = 1.0;
		else
		{
			for (int j = 0; j < nc; j++)
			{
				double sum = 1.0;
				for (int k = 0; k < nc; k++)
				{
					if (k != j)
						sum += pow((distsqr.at(i, j) / distsqr.at(i, k)), 2.0 / (mfuzzy - 1));
				}
				partition_matrix.at(i, j) = pow(sum, -1.0);
			}
		}

	}

	return;
}




void cluster_alg::assign_to_cluster(const matrix<double> &data, const matrix<double> &means, bool is_hard_partition, double mfuzzy,
	double &wcss, std::vector<double>&distmin, std::vector<int>&index, matrix<double> &partition_matrix)
{

	matrix<double> distsqr = calculate_dist_to_clustermeans(data, means);
	assign_to_cluster(distsqr, is_hard_partition, mfuzzy, wcss, distmin, index, partition_matrix);
	return;
}


void cluster_alg::select_all(const matrix<double> &data)
{
	// Define clusters when each data point is it's own cluster

	int nobs = int(data.nrows);
	int nc = nobs;

	results.converged = true;
	results.ncluster = nobs;
	results.wcss = 0.0;
	results.weights.assign(nc, 1. / (double)nc);
	results.count.assign(nc, 1.0);
	results.means = data;
	results.exemplars.assign(nc, 0);
	results.index.assign(nobs, 0);
	results.partition_matrix.resize_fill(nobs, nc, 0.0);
	for (int i = 0; i < nobs; i++)
	{
		results.exemplars.at(i) = i;
		results.index.at(i) = i;
		results.partition_matrix.at(i, i) = 1.0;
	}

	return;
}

void cluster_alg::sort()
{
	// Sort clusters in order of lowest to highest exemplar point

	int nobs = (int)results.index.size();
	int nc = (int)results.exemplars.size();

	if (nc > 1 && nobs > 1)
	{
		std::vector<int> pos(nc, -1);
		std::vector<int> newpos(nc, -1);
		std::vector<int> exemplars_sorted = results.exemplars;
		std::sort(exemplars_sorted.begin(), exemplars_sorted.end());
		for (int j = 0; j < nc; j++)
		{
			for (int k = 0; k < nc; k++)  // Loop over unsorted exemplars
			{
				if (results.exemplars.at(k) == exemplars_sorted.at(j))
				{
					pos.at(j) = k;
					newpos.at(k) = j;
					break;
				}
			}
		}
		results.exemplars = exemplars_sorted;
		for (int i = 0; i < nobs; i++)
			results.index.at(i) = newpos.at(results.index.at(i));

		results.means.sort_by_index(pos, true);
		results.partition_matrix.sort_by_index(pos, false);
	}
	return;
}


void cluster_alg::kmeans_algorithm(const matrix<double> &data)
{
	//Run kmeans algorithm.  Methods are adapted from python scikit-learn
	int nobs = int(data.nrows);
	int nfeatures = int(data.ncols);
	int nc = inputs.ncluster;

	int ntrials = 2 + (int)log(nc);

	double wcss, wcssnew, wcssmin;
	std::vector<int> exemplars;
	std::vector<double> cprob(nobs);
	std::vector<double> distmin(nobs);
	matrix<double> means;


	// Seed random number generator
	std::default_random_engine generator;
	generator.seed(inputs.randseed);
	std::uniform_real_distribution<double> uniformreal(0.0, 1.0);
	std::uniform_int_distribution<int> uniformint(0, nobs);


	// k-means algorithm
	wcssmin = 1.e10;
	std::vector<double>::iterator it;
	for (int i = 0; i < inputs.ninit; i++)  // Re-initializations
	{
		matrix<double> distsqr, distsqr_update, distsqr_new, means_new;
		std::vector<int> exemplar_new(1);
		std::vector<double> distmin_new;

		//--- Initialize using kmeans++ algorithm
		int p = uniformint(generator);  // First exemplar
		exemplars.resize(1, p);
		means = assign_means_from_exemplars(data, exemplars);
		distsqr = calculate_dist_to_clustermeans(data, means);
		assign_to_cluster(distsqr, true, 2.0, wcss, distmin, results.index, results.partition_matrix);

		for (int j = 1; j < nc; j++)
		{

			// Update cumulative probability distribution
			cprob.assign(nobs, 0.0);
			cprob[0] = distmin[0] / wcss;
			for (int k = 1; k < nobs; k++)
				cprob[k] = cprob[k - 1] + distmin[k] / wcss;

			// Select new point for exemplar of cluster j
			int pbest = -1;
			for (int t = 0; t < ntrials; t++)
			{
				// Choose point at random from cumulative probability distribution
				double r = uniformreal(generator);
				it = std::lower_bound(cprob.begin(), cprob.end(), r);
				p = int(std::distance(cprob.begin(), it));

				// Update distance to cluster exemplars and calculate wcss
				exemplar_new[0] = p;
				means_new = assign_means_from_exemplars(data, exemplar_new);
				distsqr_new = calculate_dist_to_clustermeans(data, means_new);
				distsqr_update.resize(nobs, j + 1);
				for (int g = 0; g < nobs; g++)
				{
					for (int k = 0; k < j; k++)
						distsqr_update.at(g, k) = distsqr.at(g, k);  // Distance to previously chosen clusters
					distsqr_update.at(g, j) = distsqr_new.at(g, 0);  // Distance to new cluster 
				}
				assign_to_cluster(distsqr_update, true, 2.0, wcssnew, distmin_new, results.index, results.partition_matrix);

				// Keep solution for best wcss
				if (wcssnew < wcss)
				{
					pbest = p;
					wcss = wcssnew;
					distsqr = distsqr_update;
					distmin = distmin_new;
				}

			}
			exemplars.push_back(pbest);
		}
		means = assign_means_from_exemplars(data, exemplars);  // Set current means from initialization


															   //--- Apply k-means algorithm
		int q = 0;
		double diff = 1000.0;
		wcss = 1e10;
		results.converged = false;
		while (q<inputs.nitermax && diff > inputs.converge)
		{
			// Assign data points to clusters based on current means
			assign_to_cluster(data, means, true, 2.0, wcssnew, distmin, results.index, results.partition_matrix);
			diff = fabs((wcssnew - wcss) / wcss);
			wcss = wcssnew;

			// Calculate cluster centroids for next iteration
			means.resize_fill(nc, nfeatures, 0.0);
			for (int j = 0; j < nc; j++)
			{
				std::vector<int> pts = find_points_in_cluster(results.index, j);
				int n = (int)pts.size();
				for (int k = 0; k < n; k++)  // Points in cluster j
				{
					int m = pts.at(k);
					for (int f = 0; f < nfeatures; f++)
						means.at(j, f) += data.at(m, f) / (double)n;
				}
			}

			q += 1;
		}

		if (wcss < wcssmin)
		{
			wcssmin = wcss;
			results.means = means;
			if (q < inputs.nitermax)
				results.converged = true;
		}

	}

	// Collect results
	results.ncluster = nc;
	assign_to_cluster(data, results.means, inputs.hard_partitions, 2.0, results.wcss, distmin, results.index, results.partition_matrix);

	// Set exemplar to data point closest to cluster centroid
	results.exemplars.assign(nc, 0);
	for (int j = 0; j < nc; j++)
	{
		std::vector<int> pts = find_points_in_cluster(results.index, j);
		int n = (int)pts.size();
		int kbest = pts.at(0);
		for (int p = 1; p < n; p++)  // Points assigned to cluster j
		{
			int k = pts.at(p);
			kbest = (distmin.at(k) < distmin.at(kbest) ? k : kbest);
		}
		results.exemplars.at(j) = kbest;
	}


	// wcss based on exemplar location instead of cluster centroid
	means = assign_means_from_exemplars(data, results.exemplars);
	std::vector<int> index;
	matrix<double> newmatrix;
	assign_to_cluster(data, means, inputs.hard_partitions, 2.0, results.wcss_to_exemplars, distmin, index, newmatrix);

	return;
}

void cluster_alg::random_algorithm(const matrix<double> &data)
{
	// Select cluster exemplars at random

	int nobs = int(data.nrows);
	int nc = inputs.ncluster;

	// Randomly select data points as exemplars
	std::vector<int> vec;
	std::vector<double> distmin;
	for (int i = 0; i < nobs; i++)
		vec.push_back(i);
	std::shuffle(vec.begin(), vec.end(), std::default_random_engine(inputs.randseed));

	results.exemplars.assign(nc, 0);
	for (int j = 0; j < nc; j++)
		results.exemplars.at(j) = vec.at(j);


	// Assign data points to clusters
	results.means = assign_means_from_exemplars(data, results.exemplars);
	assign_to_cluster(data, results.means, inputs.hard_partitions, 2.0, results.wcss, distmin, results.index, results.partition_matrix);
	results.ncluster = (int)results.exemplars.size();
	results.converged = true;
	results.wcss_to_exemplars = results.wcss;

	return;
}

void cluster_alg::afp_algorithm(const matrix<double> &data, matrix<double> &dist)
{
	/*
	Run affinity propagation algorithm based on current preference multiplier. Algorithm from Frey 2007 Science(315) 972-976. Methods are adapted from python scikit-learn
	Inputs: data = matrix of data points
	dist = matrix of all squared distances between data points.  Will be recomputed if not specified with correct size
	*/

	int nobs = int(data.nrows);

	//---Compute similarities between data points (negative of Euclidean distance)
	int nrow = (int)dist.nrows;
	int ncol = (int)dist.ncols;
	if (nrow != ncol || nrow != nobs)
		dist = calculate_dist_to_clustermeans(data, data);

	matrix<double>S;
	S = dist;
	double median = S.median();
	for (int i = 0; i < nobs; i++)
	{
		S.at(i, i) = -median * inputs.pref_mult;
		for (int j = i + 1; j < nobs; j++)
		{
			S.at(i, j) = -S.at(i, j);
			S.at(j, i) = S.at(i, j);
		}
	}

	double minS = abs(S.max());  // Smallest similarity (note all entries in S are negative)

								 //---Remove degeneracies
	std::default_random_engine generator;
	generator.seed(inputs.randseed);
	std::uniform_real_distribution<double> uniform(-0.5, 0.5);
	for (int i = 0; i < nobs; i++)
	{
		for (int j = 0; j < nobs; j++)
			S.at(i, j) += 1.e-8 *minS* S.at(i, j) * uniform(generator);
	}


	//--- Affinity propagation algorithm iterations
	matrix<double> A(nobs, nobs, 0.0);
	matrix<double> R(nobs, nobs, 0.0);
	std::vector<int> exemplars;
	std::vector<int> exemplars_prev;
	int q = 0;
	int count = 0;
	results.converged = false;
	while (q < inputs.nitermax && count < inputs.nconverge)
	{

		double maxval, maxval2, sum;
		int loc;
		exemplars_prev = exemplars;
		exemplars.clear();

		// Update responsibility matrix
		double update, m;
		for (int r = 0; r < nobs; r++)
		{
			maxval = maxval2 = -1e10;
			loc = 0;
			for (int c = 0; c < nobs; c++)
			{
				m = A.at(r, c) + S.at(r, c);
				if (m >= maxval2)
				{
					maxval2 = m;
					if (m >= maxval)
					{
						maxval2 = maxval;
						maxval = m;
						loc = c;
					}
				}
			}

			for (int c = 0; c < nobs; c++)
			{
				if (c != loc)
					update = S.at(r, c) - maxval;
				else
					update = S.at(r, c) - maxval2;
				R.at(r, c) = inputs.damping * R.at(r, c) + (1.0 - inputs.damping)*update;
			}
		}

		// Update availability matrix
		for (int c = 0; c < nobs; c++)
		{
			sum = 0.0;
			for (int r = 0; r < nobs; r++)
				sum += fmax(R.at(r, c), 0.0);
			sum -= fmax(R.at(c, c), 0.0);

			for (int r = 0; r < nobs; r++)
			{
				if (r != c)
					update = fmin(0.0, R.at(c, c) + sum - fmax(R.at(r, c), 0.0));
				else
					update = sum;
				A.at(r, c) = inputs.damping * A.at(r, c) + (1.0 - inputs.damping)*update;
			}
		}

		// Identify exemplars and check for changes from last iteration
		for (int r = 0; r < nobs; r++)
		{
			if (A.at(r, r) + R.at(r, r) > 0.0)
				exemplars.push_back(r);
		}

		// Check for exemplar changes from last iteration
		if (exemplars.size() != exemplars_prev.size() || exemplars != exemplars_prev)
			count = 0;
		else
			count += 1;

		q += 1;
	}

	A.clear();
	R.clear();

	int nclusters = (int)exemplars.size();
	if (count >= inputs.nconverge && q <inputs.nitermax)
		results.converged = true;


	// Revert S back to squared distance
	for (int i = 0; i < nobs; i++)
		S.at(i, i) = 0.0;


	// Modify final set of clusters
	std::vector<double> distmin;
	results.means = assign_means_from_exemplars(data, exemplars);
	assign_to_cluster(data, results.means, true, 2.0, results.wcss, distmin, results.index, results.partition_matrix);  // Assign data ponts to clusters

	for (int j = 0; j < nclusters; j++)
	{
		int i1, i2, npts;
		int minpt = 0;
		double mindist = 1.e10;
		double distsum;
		std::vector<int> pts = find_points_in_cluster(results.index, j);
		npts = (int)pts.size();

		if (npts > 2)
		{
			for (int p1 = 0; p1 < npts; p1++)   // Calculate total distance between point p1 and all other points in cluster k
			{
				i1 = pts.at(p1);
				distsum = 0.0;
				for (int p2 = 0; p2 < npts; p2++)
				{
					i2 = pts.at(p2);
					distsum -= S.at(i1, i2);
				}

				if (distsum < mindist)
				{
					minpt = i1;
					mindist = distsum;
				}
			}
			exemplars.at(j) = minpt;  // Replace exemplar j with point that minimizes wcss
		}
	}

	results.means = assign_means_from_exemplars(data, exemplars);
	assign_to_cluster(data, results.means, inputs.hard_partitions, 2.0, results.wcss, distmin, results.index, results.partition_matrix);  // Re-assign data ponts to clusters
	results.ncluster = (int)exemplars.size();
	results.exemplars = exemplars;
	results.wcss_to_exemplars = results.wcss;

	return;
}

void cluster_alg::create_clusters(const matrix<double> &data)
{

	int nobs = int(data.nrows);

	if (inputs.ncluster == nobs)
		select_all(data);
	else
	{

		switch (inputs.alg)
		{

		case KMEANS:
		{
			kmeans_algorithm(data);
			break;
		}
		case RANDOM_SELECTION:
		{
			random_algorithm(data);
			break;
		}
		case AFFINITY_PROPAGATION:
		{
			matrix<double> dist;

			if (!inputs.enforce_ncluster)
				afp_algorithm(data, dist);   // Run afp algorithm with defined preference mutliplier
			else
			{
				// Iterate over preference multiplier to create specified number of clusters
				double urf = 0.9;
				int q = 0;
				int ncprev = 0;
				double mult = inputs.pref_mult;
				double multprev = 0.0;
				double multnew = 0.0;
				bool finished = false;

				while (q < inputs.nc_itermax && !finished)
				{
					inputs.pref_mult = mult;
					afp_algorithm(data, dist);

					if (!results.converged)  // Affinity propagation algorithm didn't converge -> increase damping factor
					{
						inputs.damping += 0.05;
						inputs.damping = fmin(inputs.damping, 0.95);
					}
					else
					{
						if (fabs(results.ncluster - inputs.ncluster) <= inputs.ncluster_tol)
							finished = true;
						else
						{
							if (ncprev == 0 || results.ncluster == ncprev)
								multnew = mult * ((double)results.ncluster / (double)inputs.ncluster);
							else
							{
								double dncdmult = (results.ncluster - ncprev) / (mult - multprev);
								multnew = mult - urf * (results.ncluster - inputs.ncluster) / dncdmult;
							}
							if (multnew <= 0.0)
								multnew = mult * ((double)results.ncluster / (double)inputs.ncluster);

							ncprev = results.ncluster;
							multprev = mult;
							mult = multnew;
						}
					}
					q += 1;
				}
			}
			break;
		}
		}

		sort();   // Sort clusters by lowest - highest exemplar

		// Calculate cluster weights 
		results.weights = calculate_weights(results.partition_matrix);
		results.count.assign(results.ncluster, 0.0);
		for (int j = 0; j < results.ncluster; j++)
			results.count.at(j) = results.weights.at(j) * nobs;

	}

	return;
}



