#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <string>
#include "wash_opt.h"

WashCrewOptimizer::WashCrewOptimizer()
{
	m_solar_data = solar_field_data();
	Initialize();
}

WashCrewOptimizer::WashCrewOptimizer(
	double *x_pos,
	double *y_pos,
	double *mirror_eff,
	int max_wash_crews,
	int num_mirrors,
	int scale
)
{
	m_solar_data = solar_field_data();
	m_solar_data.x_pos = x_pos;
	m_solar_data.y_pos = y_pos;
	m_solar_data.mirror_eff = mirror_eff;
	m_settings.max_num_crews = max_wash_crews;
	m_solar_data.num_mirror_groups = num_mirrors;
	GroupMirrors(scale);
}

void WashCrewOptimizer::Initialize()
{
	/*
	Initializes settings and parameters to default values.
	*/
	m_solar_data.mirror_size = 115.7;  //m^2 per mirror
	m_solar_data.annual_dni = 2685;  //in kWh per m^2 per year
	m_settings.capital_cost_per_crew = 100000;
	m_settings.crew_hours_per_week = 40;
	m_settings.discount_rate = 0.10;
	m_settings.labor_cost_per_crew = 100;
	m_settings.materials_cost_per_crew = 10;
	m_settings.wash_time = 4. / 3;
	m_settings.system_efficiency = 0.4;  // power to grid / DNI received
	m_settings.num_years = 15.;
	m_settings.price_per_kwh = 0.15; //$/kWh
	m_settings.operating_margin = 0.1;  //% of revenue going to profit
	m_settings.max_num_crews = 40;
}

void WashCrewOptimizer::ReadFromFiles()
{
	/*
	Reads in probem data from file, and stores the results in the 
	m_solar_data structure.
	*/
	//output variables
	std::vector<double> x = {};
	std::vector<double> y = {};
	std::vector<double> eff = {};
	std::vector<int> hel_id = {};

	//file-related variables
	std::ifstream pfile;
	pfile.open("simulation_results.csv");
	//int cindex = 0;
	size_t pos = 0;
	std::string delimiter = ",";
	std::string pline;
	std::string token;
	std::vector<std::string> split_line = {};
	for (int i = 0; i < 1; i++)
	{
		getline(pfile, pline);
		std::cerr << pline << "\n";
		//cindex++;
	}
	while (!pfile.eof())
	{
		pos = 0;
		getline(pfile, pline);
		while (pos != std::string::npos)
		{
			pos = pline.find(delimiter);
			token = pline.substr(0, pos);
			split_line.push_back(token);
			pline.erase(0, pos + delimiter.length());
		}
		if (split_line.size() > 3)
		{
			hel_id.push_back(std::stoi(split_line[0]));
			x.push_back(std::stod(split_line[1]));
			y.push_back(std::stod(split_line[2]));
			eff.push_back(std::stod(split_line[9]));
		}
		split_line.clear();
	}
	double *x_arr = new double[x.size()];
	double *y_arr = new double[y.size()];
	double *eff_arr = new double[eff.size()];
	int *id_arr = new int[hel_id.size()];
	std::copy(x.begin(), x.end(), x_arr);
	std::copy(y.begin(), y.end(), y_arr);
	std::copy(eff.begin(), eff.end(), eff_arr);
	std::copy(hel_id.begin(), hel_id.end(), id_arr);
	m_solar_data.names = id_arr;
	m_solar_data.x_pos = x_arr;
	m_solar_data.y_pos = y_arr;
	m_solar_data.mirror_eff = eff_arr;
	m_solar_data.num_mirrors_by_group = new int[hel_id.size()];
	for (int i = 0; i < hel_id.size(); i++)
	{
		m_solar_data.num_mirrors_by_group[i] = 1;
	}
	m_solar_data.num_mirror_groups = hel_id.size();
	
}

double WashCrewOptimizer::GetFunctionDailyLoss()
{
	return m_func->day_degradation;
}

double WashCrewOptimizer::GetSoilingAccumulation(double accumulation)
{
	/*
	Returns fraction of soiling accumulation that is applied to the 
	heliostat.
	*/
	if (m_func->GetType() == "linear")
		return 1.0;
	if (m_func->GetType() == "PWL")
	{
		if (accumulation <= m_func->day_degradation)
			return 1.0;
		if (accumulation <= m_func->week_degradation)
			return (m_func->week_degradation - m_func->day_degradation) / 6.0;
		if (accumulation <= m_func->month_degradation)
			return (m_func->month_degradation - m_func->week_degradation) / 23.0;
		if (accumulation <= m_func->sixmo_degradation)
			return (m_func->sixmo_degradation - m_func->month_degradation) / 150.0;
		return 0.0;
	}
		
}

void WashCrewOptimizer::SortMirrors()
{
	/*
	Sorts mirrors in m_solar_data by efficiency (mirror_eff), from most to 
	least efficient, and reorders the data in x_pos, y_pos and names 
	accordingly.
	*/
	double x;
	double y;
	double eff;
	int name;
	bool switch_occurred = true;
	while (switch_occurred)
	{
		switch_occurred = false;
		for (int i = 0; i < m_solar_data.num_mirror_groups - 1; i++)
		{
			if (m_solar_data.mirror_eff[i] < m_solar_data.mirror_eff[i + 1])
			{
				x = m_solar_data.x_pos[i] * 1.0;
				y = m_solar_data.y_pos[i] * 1.0;
				eff = m_solar_data.mirror_eff[i] * 1.0;
				name = m_solar_data.names[i] * 1;
				m_solar_data.x_pos[i] = m_solar_data.x_pos[i + 1] * 1.0;
				m_solar_data.y_pos[i] = m_solar_data.y_pos[i + 1] * 1.0;
				m_solar_data.mirror_eff[i] = m_solar_data.mirror_eff[i + 1] * 1.0;
				m_solar_data.names[i] = m_solar_data.names[i+1]*1;
				m_solar_data.x_pos[i + 1] = x;
				m_solar_data.y_pos[i + 1] = y;
				m_solar_data.mirror_eff[i + 1] = eff;
				m_solar_data.names[i] = name;
				switch_occurred = true;
			}
		}
	}
}


void WashCrewOptimizer::GroupMirrors(int scale)
{
	/*
	Arranges the heliostats into groups of uniform size, to reduce
	the time required to solve the dynamic program.

	scale -- number of mirrors per group
	*/
	if (scale == 1)
	{
		m_condensed_data = m_solar_data;
		return;
	}
	solar_field_data new_data;
	m_condensed_data = new_data;
	m_condensed_data.scale = scale;
	m_condensed_data.mirror_size = m_solar_data.mirror_size*1.0;
	m_condensed_data.annual_dni = m_solar_data.annual_dni*1.0;
	m_condensed_data.groupings.clear();
	int num_groups;
	if (m_solar_data.num_mirror_groups % scale == 0)
	{
		num_groups = m_solar_data.num_mirror_groups / scale;
	}
	else
	{
		num_groups = (m_solar_data.num_mirror_groups / scale) + 1;
	}
	m_condensed_data.mirror_eff = new double[num_groups];
	m_condensed_data.num_mirrors_by_group = new int[num_groups];
	m_condensed_data.num_mirror_groups = num_groups;
	for (int i = 0; i < num_groups; i++)
	{
		m_condensed_data.groupings[i] = {};
		m_condensed_data.num_mirrors_by_group[i] = 0;
		m_condensed_data.mirror_eff[i] = 0.;
	}
	for (int i = 0; i < m_solar_data.num_mirror_groups; i++)
	{
		m_condensed_data.mirror_eff[i / scale] += m_solar_data.mirror_eff[i];
		m_condensed_data.groupings[i / scale].push_back(m_solar_data.names[i]);
		m_condensed_data.num_mirrors_by_group[i / scale] += m_solar_data.num_mirrors_by_group[i];
		//std::cerr << i << "," << (i / scale) << "," << m_solar_data.names[i] << "," << m_solar_data.mirror_eff[i] << "," << m_condensed_data.mirror_eff[i / scale] << "\n";
	}
}

void WashCrewOptimizer::CalculateRevenueAndCosts()
{
	/*
	Calculates the NPV of the revenue generated per heliostat, as well as the 
	NPV of the cost per crew, according to the settings and solar data 
	provided as input.  Revenue per mirror and cost per crew are stored in
	m_settings.revenuw_per_mirror and m_settings.total_cost_per_crew,
	respectively.
	*/
	double ann_cost = m_settings.capital_cost_per_crew + (
		(m_settings.labor_cost_per_crew + m_settings.materials_cost_per_crew)
			* m_settings.crew_hours_per_week * 52
		);
	double multiplier = (
		(1 - std::pow(1 / (1 + m_settings.discount_rate), m_settings.num_years))
		/ (1 - (1 / (1 + m_settings.discount_rate)))
		);
	std::cerr << "multiplier: " << multiplier << "\n";
	m_settings.total_cost_per_crew = ann_cost * multiplier;
	double rev_loss = (
		m_solar_data.mirror_size * m_solar_data.annual_dni
		* m_settings.system_efficiency * m_settings.price_per_kwh
		);
	m_settings.revenue_per_mirror = (
		rev_loss * m_settings.operating_margin * multiplier
		);
}

void WashCrewOptimizer::AssignSoilingFunction(SoilingFunction *func)
{
	/* 
	assigns the soiing function that determines the losses due to 
	soiling for any mirror as a function of the cleaning period.

	func -- soiling function object
	*/
	m_func = func;
}

double WashCrewOptimizer::GetNumberOfMirrors(int i, int j)
{
	/*
		obtains the number of mirrors starting at group i and ending at group
		j; allows for a non-uniform number of mirrors.

		i, j -- start and end indices of mirror groups
	*/
	int num_mirrors = 0;
	for (int k = i; k < j; k++)
	{
		num_mirrors += m_condensed_data.num_mirrors_by_group[k];
	}
	return num_mirrors;
	/*
	return std::accumulate(
		m_condensed_data.num_mirrors_by_group + i,
		m_condensed_data.num_mirrors_by_group + j,
		num_mirrors
		);
	*/
}

double WashCrewOptimizer::GetAssignmentCost(int i, int j)
{
	/*
		obtains the cost due to revenues lost, plus capital costs of an 
		additional crew, for any mirror starting in group i and ending 
		just before the start of group j.

		i, j -- start and end indices of mirror groups
	*/
	if (j <= i)
	{
		return 0.;
	}
	double total_eff = 0.;
	for (int k = i; k < j; k++)
	{
		total_eff += m_condensed_data.mirror_eff[k];
	}
	double time = GetNumberOfMirrors(i, j) * m_settings.wash_time * (168. / m_settings.crew_hours_per_week);
	/*
	if (j > 960)
	{
		std::cerr << i << "," << j << "," << total_eff << "," << time << "," << m_func->Evaluate(time) << "\n";
	}
	*/
	return m_settings.revenue_per_mirror * total_eff * m_func->Evaluate(time) + m_settings.capital_cost_per_crew;
}

double WashCrewOptimizer::EvaluatePath(std::vector<int> path)
{
	/*
	Evaluates the total cost of a heliostat assignment.
	path -- vector of ints defining the assignment of heliostat groups

	retval -- assignment cost (including hiring/labor costs and lost revenue)
	*/
	double sum = 0;
	for (unsigned int i = 0; i < path.size() - 1; i++)
	{
		sum += GetAssignmentCost(path[i], path[i + 1]);
	}
	return sum;
}

double WashCrewOptimizer::EvaluateFieldEfficiency(std::vector<int> path)
{
	int start_idx;
	int end_idx;
	double time;
	double group_eff;
	double sum_clean_eff = 0.;
	double sum_soiling_eff = 0.;
	for (size_t i = 0; i < path.size()-1; i++)
	{
		group_eff = 0;
		start_idx = path.at(i);
		end_idx = path.at(i+1);
		//get sum efficiency of entire group.
		for (int k = start_idx; k < end_idx; k++)
		{
			group_eff += m_condensed_data.mirror_eff[k];
		}
		//get the time elapsed and average efficiency hit.
		time = GetNumberOfMirrors(start_idx, end_idx) * m_settings.wash_time * (168. / m_settings.crew_hours_per_week);
		sum_soiling_eff += group_eff * m_func->Evaluate(time);
		sum_clean_eff += group_eff;
	}
	return 1 - (sum_soiling_eff / sum_clean_eff);
}

double* WashCrewOptimizer::ObtainOBJs()
{
	/* Obtain the objective values that comes from allocating a collection of
	mirrors to a wash crew. */
	double *objs = new double[
		(m_condensed_data.num_mirror_groups + 1)*(m_condensed_data.num_mirror_groups + 1)
	];
	for (int i = 0; i < m_condensed_data.num_mirror_groups + 1; i++)
	{
		for (int j = 0; j < i; j++)
		{
			objs[i* (m_condensed_data.num_mirror_groups + 1) + j] = INFINITY;
		}
		for (int j = i; j < m_condensed_data.num_mirror_groups + 1; j++)
		{
			objs[i* (m_condensed_data.num_mirror_groups+1) + j] = GetAssignmentCost(i, j);
		}
	}
	return objs;
}

int WashCrewOptimizer::FindMinDistaceNode(
	double *distances, 
	bool *available,
	int array_size
)
{
	/*
	A subroutine in the dynamic program, this retrieves the lowest-cost node
	that has not yet been fathomed.  Here, a 'node' represents an assignment
	of heliostats to some subset of the available wash crews that may not be 
	complete.

	distances -- array of doubles indicating the total cost of each assignment
	available -- array of bools indicating the paths that have not been 
	   determined to be of minimal cost yet
	array_size -- length of 'distances' and 'available' arrays
	*/
	int best_node = -1;
	double min_distance = INFINITY;
	for (int i = 0; i < array_size; i++)
	{
		if (distances[i] < min_distance && available[i])
		{
			best_node = i;
			min_distance = distances[i];
		}
	}
	return best_node;
}

void WashCrewOptimizer::RunDynamicProgram()
{
	/*
	Runs a dynamic program to obtain the optimal allocation of wash crews
	to heliostats for all numbers of wash crews from 1 to the maximum defined
	in m_settings.

	We use 1-dimensional arrays to represent the collection of nodes in the 
	DAG, in which the nodes in the documentation are labeled (0,0) to 
	(num_crews,num_mirrors), in which num_crews is the maximum number of 
	crews allowed in the problem, and num_mirrors is the number of heliostats
	in the solar field. 
	*/
	int row_length = m_condensed_data.num_mirror_groups + 1;
	int num_rows = m_settings.max_num_crews + 1;
	int num_nodes = row_length * num_rows;
	double *objs = ObtainOBJs();
	
	double *distances = new double[num_nodes];
	bool *available = new bool[num_nodes];
	int *parents = new int[num_nodes];

	//initialize distance and available arrays.
	for (int i = 0; i < num_nodes; i++)
	{
		distances[i] = INFINITY;
		available[i] = true;
		parents[i] = -1;
	}
	for (int i = 1; i < row_length; i++)
	{
		available[i] = false;
		available[
			num_nodes - i - 1
			] = false;
	}
	for (int i = 1; i < m_settings.max_num_crews + 1; i++)
	{
		for (int j = 0; j < i; j++)
		{
			available[i*(row_length) + j] = false;
		}
	}

	//initialize the current node.
	int current_node = 0;
	/* The mapping for 1D arrays is as follows: for node n in any
	1-d array, (n/num_mirrors) is the index of the crew, and (n % num_mirrors)
	is the index of the heliostat, using integer math.  */
	int crew_idx, mirror_idx; 
	distances[0] = 0.;
	//This is the DP loop; it provides the min distance and path 
	//to each node in our graph. 
	while (current_node != -1)
	{
		//remove current node from those available, i.e., those that
		//have not been fully optimized.
		available[current_node] = false;
		crew_idx = current_node / row_length;
		mirror_idx = current_node % row_length;
		//update min distance and best path to all destination nodes,
		//via the local arrays 'distances' and 'parents'.
		if (crew_idx < m_settings.max_num_crews)
		{
			for (int i = mirror_idx+1; i < row_length; i++)
			{
				if (
					available[(crew_idx + 1)*(row_length) + i] &&
					(
						distances[(crew_idx)*(row_length)+mirror_idx] + 
						objs[row_length*mirror_idx+i]
						) < distances[(crew_idx + 1)*(row_length)+i]
					)
				{
					distances[(crew_idx + 1)*(row_length)+i] = (
						distances[(crew_idx)*(row_length)+mirror_idx] +
						objs[row_length*mirror_idx + i]
						);
					parents[(crew_idx + 1)*(row_length)+i] = mirror_idx;
				}
			}
		}
		//the next node to explore will be the one available that takes the 
		//minimum cost to traverse, i.e. the hiring and assignment of crews
		//that yields the minimum loss in efficiency (and possibly hiring
		//costs) to date.
		current_node = FindMinDistaceNode(distances, available, num_nodes);
	}
	//At this point, all nodes have been explored. Send relevant results to 
	//the results data structure.
	m_results.distances = distances;
	m_results.assignments = RetracePath(parents, num_rows, row_length);
	m_results.objective_values = objs;
	m_results.parents = parents;
}

std::vector<int> WashCrewOptimizer::RetracePath(
	int* parents, 
	int num_rows, 
	int row_length
)
{
	/*
	For an already-solved dynamic program, uses the parent of each node to 
	retrace the path that defines the optimal allocation of a given number 
	of wash crews to heliostats in the field.

	parents -- solution to Dynamic Program
	num_rows -- this is equal to one plus the number of wash crews.
	row_length -- equal to one plus the number of mirror groups.
	*/
	std::vector<int> path = {};
	std::vector<int> rev_path = {};
	int parent = row_length-1; //row length - 1 = num_mirrors
	for (int i = num_rows-1; i >= 0; i--)
	{
		path.push_back(parent);
		parent = parents[i*row_length + parent];
	}
	for (int i = path.size()-1; i >= 0; i--)
	{
		rev_path.push_back(path.at(i));
	}
	return rev_path;
}

void WashCrewOptimizer::OptimizeWashCrews(int scale, bool output)
{
	/* 
	This method serves as a catch-all for processing inputs, optimizing, 
	and reporting outputs. When set to defaults, scale is set so that at most
	1,000 mirror groups are created, and output to disk is set to false.

	scale -- size of typical mirror group
	output -- true if writing output to disk, false o.w.
	*/

	//sort mirrors by efficiency
	SortMirrors();

	//redefine scale, if needed
	if (scale == -1)
	{
		scale = 1 + (m_solar_data.num_mirror_groups / 1000);
	}
	
	//place mirrors into groups according to scale
	GroupMirrors(scale);

	//calculate revenue losses/costs
	CalculateRevenueAndCosts();

	//solve dyamic program
	RunDynamicProgram();

	//determine the lowest-cost path from the possible number of crews.
	std::vector<int> path;
	std::vector<int> best_path;
	double cost;
	double field_eff;
	double min_cost = INFINITY;
	for (int i = 1; i <= m_settings.max_num_crews; i++)
	{
		path = RetracePath(
			m_results.parents, i + 1, m_condensed_data.num_mirror_groups+1
		);
		
		cost = EvaluatePath(path);
		
		field_eff = EvaluateFieldEfficiency(path);

		std::cerr << "Cost for " << i << " wash crews: " << cost
			<< "\nAssignment: ";
		for (int j = 0; j < path.size(); j++)
		{
			std::cerr << path.at(j) << ",";
		}
		std::cerr << "\nAverage field efficiency: " << field_eff << "\n";

		if (cost < min_cost)
		{
			best_path = path;
			min_cost = cost * 1.0;
		}
	}
	std::cerr << "optimal cost: " << min_cost << "\nNumber of wash crews: "
		<< best_path.size()-1 << "\n";
	m_results.assignments = best_path;
	if (output)
	{
		OutputResults();
	}

}

void WashCrewOptimizer::Output2DArrayToFile(
	std::string filename, 
	double* arr,
	int num_rows,
	int row_length
)
{
	std::ofstream ofile;
	ofile.open(filename);
	for (int i = 0; i < num_rows; i++)
	{
		for (int j = 0; j < row_length; j++)
		{
			ofile << arr[i*row_length+j] << ",";
		}
		ofile << "\n";
	}
	ofile.close();
}

void WashCrewOptimizer::Output2DIntArrayToFile(
	std::string filename,
	int* arr,
	int num_rows,
	int row_length
)
{
	std::ofstream ofile;
	ofile.open(filename);
	for (int i = 0; i < num_rows; i++)
	{
		for (int j = 0; j < row_length; j++)
		{
			ofile << arr[i*row_length + j] << ",";
		}
		ofile << "\n";
	}
	ofile.close();
}

void WashCrewOptimizer::OutputVectorToFile(
	std::string filename,
	std::vector<int> arr
)
{
	std::ofstream ofile;
	ofile.open(filename);
	for (size_t i = 0; i < arr.size(); i++)
	{
		ofile << arr.at(i) << ",";
	}
	ofile << "\n";
	ofile.close();
}

void WashCrewOptimizer::OutputWCAssignment()
{
	std::ofstream ofile; 
	std::string filename = "wcassignment";
	filename += std::to_string(m_settings.max_num_crews);
	filename += ".csv";
	ofile.open(filename);
	ofile << "heliostat,x,y,crew\n";
	for (int i = 0; i < m_settings.max_num_crews; i++)
	{
		for (
			int j = (m_condensed_data.scale * m_results.assignments[i]);
			j < (m_condensed_data.scale * m_results.assignments[i + 1]);
			j++
			)
		{
			if (j == m_solar_data.num_mirror_groups)
			{
				ofile.close();
				return;
			}
			ofile << m_solar_data.names[j] << "," << m_solar_data.x_pos[j]
				<< "," << m_solar_data.y_pos[j] << "," << (i + 1) << "\n";
		}
	}
	ofile.close();
}

void WashCrewOptimizer::OutputResults()
{
	Output2DArrayToFile(
		m_file_settings.distance_file,
		m_results.distances,
		m_settings.max_num_crews + 1,
		m_condensed_data.num_mirror_groups + 1
	);
	OutputVectorToFile(
		m_file_settings.path_file,
		m_results.assignments
	);
	Output2DIntArrayToFile(
		m_file_settings.parents_file,
		m_results.parents,
		m_settings.max_num_crews + 1,
		m_condensed_data.num_mirror_groups + 1
	);
	Output2DArrayToFile(
		m_file_settings.obj_file,
		m_results.objective_values,
		m_condensed_data.num_mirror_groups + 1,
		m_condensed_data.num_mirror_groups + 1
	);
}

