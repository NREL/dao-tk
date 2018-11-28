#include "clustersim.h"




cluster_sim::cluster_sim()
{
	inputs.days.ncount = std::numeric_limits<int>::quiet_NaN();
	inputs.days.nprev = std::numeric_limits<int>::quiet_NaN();
	inputs.days.nnext = std::numeric_limits<int>::quiet_NaN();

	inputs.cluster_results.ncluster = std::numeric_limits<int>::quiet_NaN();
	inputs.cluster_results.partition_matrix.clear();
	inputs.cluster_results.means.clear();
	inputs.cluster_results.exemplars.clear();

	inputs.is_combine_consecutive = true;
	inputs.is_run_continuous = false;
	inputs.is_clusteravg_sfavail = true;
	inputs.is_initial_charge_heuristic = true;
	inputs.initial_hot_charge = std::numeric_limits<double>::quiet_NaN();
};


void cluster_sim::set_default_inputs()
{
	inputs.days.ncount = 2;
	inputs.days.nprev = 1;
	inputs.days.nnext = 1;

	inputs.is_combine_consecutive = true;
	inputs.is_run_continuous = false;
	inputs.is_clusteravg_sfavail = true;
	inputs.is_initial_charge_heuristic = true;
	inputs.initial_hot_charge = 30;

	return;
}


void cluster_sim::assign_first_last(const s_metric_outputs &metric_results)
{
	// Assign first/last days for year y (not included in any possible exemplar grouping) to a cluster and adjust weights 


	int nobs = (int)inputs.cluster_results.partition_matrix.nrows;  // Number of existing groups
	int nc = (int)inputs.cluster_results.partition_matrix.ncols;    // Number of clusters

																	// Determine if existing partition matrix uses hard or soft weighting and use the same for skipped days
	bool is_hard_partitions = true;
	int g = 0;
	while (is_hard_partitions && g<nobs)
	{
		int c = 0;
		while (is_hard_partitions && c < nc)
		{
			double p = inputs.cluster_results.partition_matrix.at(g, c);
			if (p > 1e-6 && p < 0.999)
				is_hard_partitions = false;
			c += 1;
		}
		g += 1;
	}

	//--- Assign first/last days to a cluster
	double wcss;
	std::vector<double> distmin;
	cluster_alg cl;
	cl.assign_to_cluster(metric_results.data_firstday, inputs.cluster_results.means, is_hard_partitions, 2.0, wcss, distmin, inputs.skip_first.index, inputs.skip_first.partition_matrix);
	cl.assign_to_cluster(metric_results.data_lastday, inputs.cluster_results.means, is_hard_partitions, 2.0, wcss, distmin, inputs.skip_last.index, inputs.skip_last.partition_matrix);


	//--- Recompute cluster weights
	int nsimdays = inputs.days.ncount;
	int ngroup = int(363. / nsimdays);			// Number of possible simulation groups per year
	int nyears = nobs / ngroup;					// Number of years 

	int skip_first = 1;							// Number of days skipped at beginning of the year
	int skip_last = 364 - ngroup * nsimdays;	// Number of days skipped at the end of the year
	double nobs_tot = nobs + nyears * (skip_first + skip_last) / double(nsimdays);  // Total number of groups including neglected first/last days

	for (int j = 0; j < nc; j++)
	{
		double n = inputs.cluster_results.weights.at(j) * nobs;  // Number of observations per cluster

		for (int y = 0; y < nyears; y++)
		{
			n += (skip_first / double(nsimdays)) * inputs.skip_first.partition_matrix.at(y, j);
			n += (skip_last / double(nsimdays)) * inputs.skip_last.partition_matrix.at(y, j);
		}
		inputs.cluster_results.weights.at(j) = n / nobs_tot;
	}

	return;
}


double cluster_sim::initial_charge_heuristic(double prev_dni, double sm)
{
	// Heuristic for initial hot charge state (%) based on previous-day DNI (kWh/m2/day) and solar multiple (sm)
	double ic;

	if (prev_dni <= 6.0)
		ic = 5.0;
	else
	{
		if (sm < 1.5)
			ic = 5.0;
		else if (sm < 2.0)
			ic = 10.;
		else
			ic = 20.;
	}
	return ic;
}


int cluster_sim::firstday(int g)
{
	// First day counted in simulation for group g
	int d1 = g * inputs.days.ncount + 1;
	return d1;
}

int cluster_sim::firstsimday(int g)
{
	// First day simulated for group g
	int d0 = std::max(g * inputs.days.ncount + 1 - inputs.days.nprev, 0);
	return d0;
}


std::vector<bool> cluster_sim::set_all_sim_days()
{
	std::vector<int> exemplars = inputs.cluster_results.exemplars;
	std::vector<bool> select_days(365, 0);

	for (int g = 0; g < (int)exemplars.size(); g++)
	{
		int d1 = firstday(exemplars.at(g));
		int d0 = d1 - inputs.days.nprev;
		for (int d = 0; d < inputs.days.ncount + inputs.days.nprev; d++)
		{
			if (d0 + d >= 0 && d0 + d < 365)
				select_days.at(d0 + d) = true;
		}
	}
	return select_days;

}






void cluster_sim::cluster_avg_from_timeseries(const std::vector<double> &timeseries, matrix<double> & clusteravg, matrix<double> & count, int year)
{
	/*
	Compute cluster-average values for exemplar simulation duration (single year)
	Inputs: timeseries = annual time-series data
	Outputs: clusteravg = matrix of cluster-average timeseries values with rows = time points, cols = clusters
	count = matrix of # of values contributing to each average (only used for multiyear simulations)
	*/

	int npts = (int)timeseries.size();
	int nperday = npts / 365;
	int nc = inputs.cluster_results.ncluster;
	int nd = inputs.days.ncount + inputs.days.nprev + inputs.days.nnext;

	int ngroup = int(363. / inputs.days.ncount);			// Number of possible simulation groups per year
	int nskip = 365 - 1 - ngroup * inputs.days.ncount;		// Number of days skipped at the end of the year

	if (year == 0)
	{
		clusteravg.resize_fill(nperday*nd, nc, 0.0);
		count.resize_fill(nd, nc, 0.0);
	}
	else  // Revert to non-normalized values
	{
		for (int j = 0; j < nc; j++)
		{
			for (int d = 0; d < nd; d++)
			{
				for (int h = 0; h<nperday; h++)
					clusteravg.at(d*nperday + h, j) *= count.at(d, j);
			}
		}
	}

	for (int j = 0; j < nc; j++)
	{
		// Allowable simulation groups
		for (int g = 0; g < ngroup; g++)
		{
			int r = year * ngroup + g;	// Index of row in partition matrix
			int d1 = firstday(g) - inputs.days.nprev;  // First day simulated in group g
			for (int d = 0; d < nd; d++)
			{
				int doy = d1 + d;
				if (doy >= 0 && doy <= 364)
				{
					double p = inputs.cluster_results.partition_matrix.at(r, j);
					count.at(d, j) += p;
					for (int h = 0; h < nperday; h++)
						clusteravg.at(d*nperday + h, j) += timeseries.at(doy*nperday + h) * p;
				}
			}
		}

		// First day: include day 0 in cluster average (within time points for first simulation day)
		count.at(inputs.days.nprev, j) += inputs.skip_first.partition_matrix.at(year, j);
		for (int h = 0; h < nperday; h++)
			clusteravg.at(inputs.days.nprev*nperday + h, j) += timeseries.at(h)*inputs.skip_first.partition_matrix.at(year, j);

		// Last days: include previous day and all skipped days in cluster average
		for (int d = 0; d < inputs.days.nprev + nskip; d++)
		{
			int doy = (365 - nskip - inputs.days.nprev) + d;
			count.at(d, j) += inputs.skip_last.partition_matrix.at(year, j);
			for (int h = 0; h < nperday; h++)
				clusteravg.at(d*nperday + h, j) += timeseries.at(doy*nperday + h)*inputs.skip_last.partition_matrix.at(year, j);
		}

		// Normalize
		for (int d = 0; d < nd; d++)
		{
			for (int h = 0; h < nperday; h++)
				clusteravg.at(d*nperday + h, j) /= count.at(d, j);
		}
	}

	return;

}

void cluster_sim::cluster_avg_from_timeseries(const matrix<double> &timeseries, matrix<double> & clusteravg)
{
	/*
	Compute cluster-average values for exemplar simulation duration using multiple years of data
	Inputs: timeseries = annual time-series data for all years with rows = time points, columns = years
	Output: matrix of cluster-average timeseries values over all years with rows = time points, cols = clusters
	*/

	int nyears = (int)timeseries.ncols;
	int npts = (int)timeseries.nrows;

	std::vector<double> timeseries_single(npts, 0.0);
	matrix<double> count;

	for (int y = 0; y < nyears; y++)
	{
		for (int p = 0; p < npts; p++)
			timeseries_single.at(p) = timeseries.at(p, y);

		cluster_avg_from_timeseries(timeseries_single, clusteravg, count, y);
	}

	return;
}

void cluster_sim::overwrite_with_cluster_avg_values(std::vector<double> &timeseries, const matrix<double> &clusteravg, bool overwrite_surrounding = true, int year)
{
	/*
	Overwrite sections of single annual timeseries wtih cluster average values
	Inputs: timeseries = time series data (single year)
	clusteravg = cluster-average profiles for each exemplar with rows = time points, cols = clusters
	year = index for single year of data
	days = structure containing number of simulation days (ncount), previous days (nprev), and next days (nnext)
	overwrite_surrounding: true = data will be overwritten on both simulation days and previous/next days, false = data only overwritten on simulation days
	*/

	int npts = (int)timeseries.size();
	int nperday = npts / 365;
	int nc = inputs.cluster_results.ncluster;
	int ngroup = int(363. / inputs.days.ncount);

	// Overwrite previous / next days first
	if (overwrite_surrounding)
	{
		for (int j = 0; j < nc; j++)
		{
			int exemplar = inputs.cluster_results.exemplars.at(j);
			int y = int(exemplar / ngroup);
			if (y == year)		// Exemplar is in this year
			{
				int d1 = firstday(exemplar - y * ngroup);   // First day counted in this exemplar simulation

															// "next" days
				for (int h = 0; h < nperday*inputs.days.nnext; h++)
					timeseries.at((d1 + inputs.days.ncount)*nperday + h) = clusteravg.at((inputs.days.nprev + inputs.days.ncount)*nperday + h, j);

				// "previous" days
				for (int h = 0; h < nperday*inputs.days.nprev; h++)
					timeseries.at((d1 - inputs.days.nprev)*nperday + h) = clusteravg.at(h, j);
			}
		}
	}

	// Overwrite simulation days
	for (int j = 0; j < nc; j++)
	{
		int exemplar = inputs.cluster_results.exemplars.at(j);
		int y = int(exemplar / ngroup);
		if (y == year)		// Exemplar is in this year
		{
			int d1 = firstday(exemplar - y * ngroup);		// First day counted in this exemplar simulation
			for (int h = 0; h < nperday*inputs.days.ncount; h++)
				timeseries.at(d1*nperday + h) = clusteravg.at(inputs.days.nprev*nperday + h, j);
		}
	}

	return;
}



void cluster_sim::combine_consecutive_exemplars()
{
	// Combine consecutive exemplars

	int nc = inputs.cluster_results.ncluster;
	int ngroup = int(363. / inputs.days.ncount);		// Number of possible simulation groups per year

														//--- Find groups with consecutive exemplars
	inputs.combined.start.clear();
	inputs.combined.n.clear();
	for (int j = 0; j < nc; j++)
	{
		int k = inputs.cluster_results.exemplars.at(j);
		if (j == 0 || k % ngroup == 0 || k - inputs.cluster_results.exemplars.at(j - 1) != 1)  // Start new group
		{

			inputs.combined.start.push_back(k);
			inputs.combined.n.push_back(1);
		}
		else			// Combine with previous exemplar group
			inputs.combined.n.back() += 1;


	}

	return;
}

std::vector<double> cluster_sim::compute_combined_clusteravg(int combined_index, const matrix<double> &clusteravg)
{
	// Compute relevant array of associated cluster average timeseries values for combined exemplar simulation duration
	// Inputs: combined_index = Combined simulation index
	//		   clusteravg = matrix of cluster - average timeseries values with rows = time points in cluster exemplar, cols = clusters(optional)


	int n = inputs.combined.n.at(combined_index);		// Number of original exemplars in this combined simulation
	int ex = inputs.combined.start.at(combined_index);   // First exemplar in this combined simulation

	std::vector<int>::iterator it;
	it = std::lower_bound(inputs.cluster_results.exemplars.begin(), inputs.cluster_results.exemplars.end(), ex);
	int k = int(std::distance(inputs.cluster_results.exemplars.begin(), it));

	int ndays_single = inputs.days.ncount + inputs.days.nprev + inputs.days.nnext;  // Number of days in any single simulation grouping
	int ndays = ndays_single + (n - 1)*inputs.days.ncount;							// Number of days in this combined simulation grouping
	int nperday = (int)clusteravg.nrows / ndays_single;							// Number of time points per day


	std::vector<double> combined_clusteravg(ndays*nperday, 0.0);

	int h = 0;

	// Time points in previous days
	for (int t = 0; t < nperday*inputs.days.nprev; t++)
	{
		combined_clusteravg.at(h) = clusteravg.at(h, k);
		h += 1;
	}

	// Time points in simulation days
	for (int i = 0; i < n; i++)
	{
		for (int t = 0; t < nperday*inputs.days.ncount; t++)
		{
			combined_clusteravg.at(h) = clusteravg.at(nperday*inputs.days.nprev + t, k + i);
			h += 1;
		}
	}

	// Time points in next day
	for (int t = 0; t < nperday*inputs.days.nnext; t++)
	{
		combined_clusteravg.at(h) = clusteravg.at((inputs.days.nprev + inputs.days.ncount)*nperday + t, k + n - 1);
		h += 1;
	}

	return combined_clusteravg;
}




void cluster_sim::compute_annual_array_from_clusters(const std::vector<double> &exemplardata, std::vector<double> &fullyeardata)
{
	/*
	Create full annual array of timeseries data based on cluster simulation results (single year)
	Input: exemplardata = full annual timeseries with data filled in only at exemplar points
	Output: full annual timeseries with data filled in at all points
	*/


	int npts = (int)exemplardata.size();
	int nperday = npts / 365;

	int nc = inputs.cluster_results.ncluster;
	int ngroup = int(363. / inputs.days.ncount);

	int skip = 365 - 1 - ngroup * inputs.days.ncount;	// Number of days skipped at the end of the year
	int dlast = ngroup * inputs.days.ncount + 1;		// Index of first day skipped at the end of the year

	fullyeardata.assign(npts, 0.0);

	for (int j = 0; j < nc; j++)
	{
		int exemplar = inputs.cluster_results.exemplars.at(j);		// Exemplar for this cluster
		int de = firstday(exemplar);								// First day of exemplar

		for (int g = 0; g < ngroup; g++)
		{
			int d = firstday(g);	// First day
			for (int h = 0; h < nperday*inputs.days.ncount; h++)
				fullyeardata.at(d*nperday + h) += exemplardata.at(de*nperday + h) * inputs.cluster_results.partition_matrix.at(g, j);
		}

		// First day 
		for (int h = 0; h < nperday; h++)
			fullyeardata.at(h) += exemplardata.at(de*nperday + h) * inputs.skip_first.partition_matrix.at(0, j);

		// Last days 
		for (int h = 0; h < nperday*skip; h++)
			fullyeardata.at(dlast*nperday + h) += exemplardata.at(de*nperday + h) * inputs.skip_last.partition_matrix.at(0, j);

	}

	return;
}

void cluster_sim::compute_annual_array_from_clusters(const matrix<double> &exemplardata, matrix<double> &fullyeardata)
{
	/*
	Create full annual arrays of timeseries data based on cluster simulation results (multiple years)
	Input: exemplardata = full annual timeseries (for each year) with data filled in only at exemplar points (rows = time points, cols = years)
	Output: full annual timeseries (for each year) with data filled in at all points (rows = time points, cols = years)
	*/

	int npts = (int)exemplardata.nrows;
	int nyears = (int)exemplardata.ncols;
	int nperday = npts / 365;

	int nc = inputs.cluster_results.ncluster;
	int ngroup = int(363. / inputs.days.ncount);		// Number of possible simulation groups per year

	int skip = 365 - 1 - ngroup * inputs.days.ncount;	// Number of days skipped at the end of the year
	int dlast = ngroup * inputs.days.ncount + 1;		// Index of first day skipped at the end of the year

	fullyeardata.resize_fill(npts, nyears, 0.0);

	for (int j = 0; j < nc; j++)
	{
		int exemplar = inputs.cluster_results.exemplars.at(j);	// Exemplar for this cluster
		int ye = int(exemplar / ngroup);						// Exemplar year
		int de = firstday(exemplar - ye * ngroup);				// First day of exemplar

		for (int y = 0; y < nyears; y++)  // Loop over years
		{
			for (int g = 0; g < ngroup; g++)
			{
				int gf = y * ngroup + g;		// Group index in full data set
				int d = firstday(g);
				for (int h = 0; h < nperday*inputs.days.ncount; h++)
					fullyeardata.at(d*nperday + h, y) += exemplardata.at(de*nperday + h, ye) * inputs.cluster_results.partition_matrix.at(gf, j);
			}

			// First day (not included in any simulation group)
			for (int h = 0; h < nperday; h++)
				fullyeardata.at(h, y) += exemplardata.at(de*nperday + h, ye) * inputs.skip_first.partition_matrix.at(y, j);

			// Last days (not included in any simulation group)
			for (int h = 0; h < nperday*skip; h++)
				fullyeardata.at(dlast*nperday + h, y) += exemplardata.at(de*nperday + h, ye) * inputs.skip_last.partition_matrix.at(y, j);
		}
	}

	return;
}



void cluster_sim::compute_dni_scaling_factors(const matrix<double> &daily_dni, matrix<double> &scale)
{
	/* Compute daily scaling factors from ratio of (actual average DNI during simulation period) /  (average DNI of exemplars which are used in place of the simulation period)
	Input: daily_dni = matrix of daily DNI for all years (kWh/m2/day) with rows = day, columns = year
	Output: scale = matrix of scaling factors for all years with rows = day, columns = year
	*/


	int nc = (int)inputs.cluster_results.ncluster;
	int nobs = (int)inputs.cluster_results.index.size();
	int ngroup = int(363. / inputs.days.ncount);
	int nyears = nobs / ngroup;
	scale.resize_fill(365, nyears, 0.0);

	//--- Available average DNI (kWh/m2/day) for all counted days in simulation group
	std::vector<double> groupdni(nobs, 0.0);
	for (int y = 0; y < nyears; y++)
	{
		for (int g = 0; g < ngroup; g++)
		{
			int m = y * ngroup + g;
			int d1 = firstday(g);
			for (int d = 0; d < inputs.days.ncount; d++)
				groupdni.at(m) += daily_dni.at(d1 + d, y) / (double)inputs.days.ncount;
		}
	}

	//--- Group-average DNI for each exemplar
	std::vector<double>exemplardni(nc, 0.0);
	for (int j = 0; j < nc; j++)
	{
		int ex = inputs.cluster_results.exemplars.at(j);
		exemplardni.at(j) = groupdni.at(ex);
	}

	//--- Calculate scaling factors

	double dni_actual, dni_sim;
	for (int y = 0; y < nyears; y++)
	{
		for (int g = 0; g < ngroup; g++)
		{
			int m = y * ngroup + g;
			int d1 = firstday(g);
			dni_actual = groupdni.at(m);
			dni_sim = 0.0;
			for (int j = 0; j < nc; j++)
				dni_sim += exemplardni.at(j) * inputs.cluster_results.partition_matrix.at(m, j);

			for (int d = 0; d < inputs.days.ncount; d++)
				scale.at(d1 + d, y) = dni_actual / dni_sim;
		}

		// First day
		dni_actual = 0.0;
		for (int d = 0; d < inputs.days.ncount; d++)
			dni_actual += daily_dni.at(d, y) / (double)inputs.days.ncount;

		dni_sim = 0.0;
		for (int j = 0; j < nc; j++)
			dni_sim += exemplardni.at(j) * inputs.skip_first.partition_matrix.at(y, j);

		scale.at(0, y) = dni_actual / dni_sim;


		// Last days
		int nskip = 365 - 1 - ngroup * inputs.days.ncount;	// Number of days skipped at the end of the year
		int d1 = ngroup * inputs.days.ncount + 1;		// Index of first day skipped at the end of the year
		dni_actual = 0.0;
		for (int d = 0; d < nskip; d++)
			dni_actual += daily_dni.at(d1 + d, y) / (double)nskip;

		dni_sim = 0.0;
		for (int j = 0; j < nc; j++)
			dni_sim += exemplardni.at(j) * inputs.skip_last.partition_matrix.at(y, j);

		for (int d = 0; d < nskip; d++)
			scale.at(d1 + d, y) = dni_actual / dni_sim;

	}

	return;
}



