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
	double *mirror_output,
	int max_wash_crews,
	int num_mirrors,
	int scale
)
{
	m_solar_data = solar_field_data();
	m_solar_data.x_pos = x_pos;
	m_solar_data.y_pos = y_pos;
	m_solar_data.mirror_output = mirror_output;
	m_settings.max_num_crews = max_wash_crews;
	m_solar_data.num_mirror_groups = num_mirrors;
	GroupMirrors(scale);
}

void WashCrewOptimizer::Initialize()
{
	/*
	Initializes settings and parameters to default values.
	*/
	m_settings.capital_cost_per_crew = 100000; //Kolb 2007
	m_settings.crew_hours_per_week = 40;  //no source
	m_settings.discount_rate = 0.06; // roger 2016
	m_settings.hourly_cost_per_crew = 50;  //$/hour, Kolb 2007 (several items rolled up)
	m_settings.wash_rate = 3680; //meters-squared/hour, Wolferstetter 2018
	m_settings.system_efficiency = 0.4;  // power to grid / DNI received
	m_settings.num_years = 25.;
	m_settings.price_per_kwh = 0.137; //$/kWh, source = Crescent Dunes 
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
	std::vector<double> output = {};
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
			output.push_back(std::stod(split_line[16]) / 1000.);
		}
		split_line.clear();
	}
	double *x_arr = new double[x.size()];
	double *y_arr = new double[y.size()];
	double *output_arr = new double[output.size()];
	int *id_arr = new int[hel_id.size()];
	std::copy(x.begin(), x.end(), x_arr);
	std::copy(y.begin(), y.end(), y_arr);
	std::copy(output.begin(), output.end(), output_arr);
	std::copy(hel_id.begin(), hel_id.end(), id_arr);
	m_solar_data.names = id_arr;
	m_solar_data.x_pos = x_arr;
	m_solar_data.y_pos = y_arr;
	m_solar_data.mirror_output = output_arr;
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
	{
		return 1.0;
	}
		
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
		
	return 0.0;
}

void WashCrewOptimizer::SortMirrors()
{
	/*
	Sorts mirrors in m_solar_data by efficiency (mirror_output), from most to 
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
			if (m_solar_data.mirror_output[i] < m_solar_data.mirror_output[i + 1])
			{
				x = m_solar_data.x_pos[i] * 1.0;
				y = m_solar_data.y_pos[i] * 1.0;
				eff = m_solar_data.mirror_output[i] * 1.0;
				name = m_solar_data.names[i] * 1;
				m_solar_data.x_pos[i] = m_solar_data.x_pos[i + 1] * 1.0;
				m_solar_data.y_pos[i] = m_solar_data.y_pos[i + 1] * 1.0;
				m_solar_data.mirror_output[i] = m_solar_data.mirror_output[i + 1] * 1.0;
				m_solar_data.names[i] = m_solar_data.names[i+1]*1;
				m_solar_data.x_pos[i + 1] = x;
				m_solar_data.y_pos[i + 1] = y;
				m_solar_data.mirror_output[i + 1] = eff;
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
	solar_field_data new_data;
	m_condensed_data = new_data;
	m_condensed_data.scale = scale;
	//m_condensed_data.mirror_size = m_solar_data.mirror_size*1.0;
	//m_condensed_data.annual_dni = m_solar_data.annual_dni*1.0;
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
	m_condensed_data.mirror_output = new double[num_groups];
	m_condensed_data.num_mirrors_by_group = new int[num_groups];
	m_condensed_data.num_mirror_groups = num_groups;
	for (int i = 0; i < num_groups; i++)
	{
		m_condensed_data.groupings[i] = {};
		m_condensed_data.num_mirrors_by_group[i] = 0;
		m_condensed_data.mirror_output[i] = 0.;
	}
	for (int i = 0; i < m_solar_data.num_mirror_groups; i++)
	{
		m_condensed_data.mirror_output[i / scale] += m_solar_data.mirror_output[i];
		m_condensed_data.groupings[i / scale].push_back(m_solar_data.names[i]);
		m_condensed_data.num_mirrors_by_group[i / scale] += 1;
		//std::cerr << i << "," << (i / scale) << "," << m_solar_data.names[i] << "," << m_solar_data.mirror_output[i] << "," << m_condensed_data.mirror_output[i / scale] << "\n";
	}
	GetTotalFieldOutput();
}


void WashCrewOptimizer::GroupSolutionMirrors(int hours)
{
	/*
	After mirrors have been assigned to wash crews, this regroups the mirrors in a
	solar data object such that each group of mirrors take a number of hours
	(provided as input) to wash once.
	*/
	m_solution_data = solar_field_data();
	m_solution_data.scale = hours * (m_settings.wash_rate / m_settings.heliostat_size);
	m_solution_data.groupings.clear();

	std::vector<double> mirror_output = {};
	std::vector<int> num_mirrors_by_group = {};
	std::vector<int> new_assignments = { 0 };
	

	//determine the number of mirror groups;
	int mirror_idx = 0;
	double output = 0;
	int group_idx = 0;
	m_solution_data.groupings[group_idx] = {};
	int num_assigned_mirrors;
	for (int i = 0; i < m_results.num_wash_crews; i++)
	{   //i = wash crew index
		num_assigned_mirrors = 0;
		for (  //j = condensed data mirror group index
			int j = m_results.assignments.at(i);
			j < m_results.assignments.at(i + 1);
			j++
			)
		{
			for (int k = 0; k < m_condensed_data.num_mirrors_by_group[j]; k++)
			{  //k = mirror index within condensed data
				output += m_solar_data.mirror_output[mirror_idx];
				m_solution_data.groupings[group_idx].push_back(m_solar_data.names[mirror_idx]);
				num_assigned_mirrors++;
				mirror_idx++;
				if (num_assigned_mirrors == m_solution_data.scale)
				{
					mirror_output.push_back(output);
					num_mirrors_by_group.push_back(num_assigned_mirrors);
					num_assigned_mirrors = 0;
					output = 0.;
					group_idx++;
					m_solution_data.groupings[group_idx] = {};
				}
			}
		}
		if (num_assigned_mirrors > 0)
		{
			mirror_output.push_back(output);
			num_mirrors_by_group.push_back(num_assigned_mirrors);
			num_assigned_mirrors = 0;
			output = 0.;
			group_idx++;
			m_solution_data.groupings[group_idx] = {};
		}
		new_assignments.push_back(group_idx);
	}
	/*
	std::cerr << "Path: ";
	for (int j = 0; j < m_results.assignments.size(); j++)
	{
		std::cerr << m_results.assignments.at(j) << ",";
	}
	std::cerr << "\n";
	for (int i = 0; i < group_idx; i++)
	{
		std::cerr << "Group " << i << " num mirrors: "
			<< num_mirrors_by_group[i]
			<< " power: " << mirror_output[i] << "\n";
	}
	*/
	m_solution_data.num_mirrors_by_group = new int[num_mirrors_by_group.size()]; &num_mirrors_by_group[0];
	m_solution_data.mirror_output = new double[mirror_output.size()];
	for (size_t i = 0; i < mirror_output.size(); i++)
	{
		m_solution_data.mirror_output[i] = mirror_output.at(i);
		m_solution_data.num_mirrors_by_group[i] = num_mirrors_by_group.at(i);
	}
	m_solution_data.total_mirror_output = m_condensed_data.total_mirror_output*1.0;
	m_solution_data.num_mirror_groups = mirror_output.size();
	m_results.assignments = new_assignments;
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
	double ann_cost = (
		m_settings.hourly_cost_per_crew * m_settings.crew_hours_per_week * 52
		);

	double vehicle_cost = m_settings.capital_cost_per_crew;
	for (int yrs = m_settings.vehicle_life; yrs < m_settings.num_years; yrs += m_settings.vehicle_life)
	{
		vehicle_cost += m_settings.capital_cost_per_crew / std::pow(1 / (1 + m_settings.discount_rate), yrs);
	}
	
	double multiplier = (
		(1 - std::pow(1 / (1 + m_settings.discount_rate), m_settings.num_years))
		/ (1 - (1 / (1 + m_settings.discount_rate)))
		);

	double labor_multiplier = m_settings.num_years;
	
	m_settings.annual_multiplier = multiplier;

	m_settings.total_cost_per_crew = ann_cost * labor_multiplier + vehicle_cost;

	m_settings.profit_per_kwh = (   //NPV per annual kWh sent to receiver by heliostat
		m_settings.price_per_kwh * m_settings.system_efficiency * multiplier
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

void WashCrewOptimizer::GetTotalFieldOutput()
{
	/*
	Returns the sum output of the entire solar field.
	(sums the mirror_output of each group of mirrors.
	*/
	double sum_clean_output = 0.;
	for (size_t i = 0; i < m_solar_data.num_mirror_groups; i++)
	{
		sum_clean_output += m_solar_data.mirror_output[i];
	}
	m_solar_data.total_mirror_output = sum_clean_output;
	m_condensed_data.total_mirror_output = sum_clean_output;
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
	double total_output = 0.;
	for (int k = i; k < j; k++)
	{
		total_output += m_condensed_data.mirror_output[k];
	}
	double mirrors_per_hour = (m_settings.wash_rate / m_settings.heliostat_size);
	double time = GetNumberOfMirrors(i, j) * (60./mirrors_per_hour) * (168. / m_settings.crew_hours_per_week); //in minutes between cleanings
	/*
	if (j > 960)
	{
		std::cerr << i << "," << j << "," << total_output << "," << time << "," << m_func->Evaluate(time) << "\n";
	}
	*/
	return m_settings.profit_per_kwh * total_output * m_func->Evaluate(time) + m_settings.total_cost_per_crew;
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
	double sum_soiling_eff = 0.;
	for (size_t i = 0; i < path.size()-1; i++)
	{
		group_eff = 0;
		start_idx = path.at(i);
		end_idx = path.at(i+1);
		//get sum efficiency of entire group.
		for (int k = start_idx; k < end_idx; k++)
		{
			group_eff += m_condensed_data.mirror_output[k];
		}
		//get the time elapsed and average efficiency hit.
		time = GetNumberOfMirrors(start_idx, end_idx) * (60./(m_settings.wash_rate / m_settings.heliostat_size)) * (168. / m_settings.crew_hours_per_week);
		sum_soiling_eff += group_eff * m_func->Evaluate(time);
	}
	return 1 - (sum_soiling_eff / m_solar_data.total_mirror_output);
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

std::vector<int> WashCrewOptimizer::GetEqualAssignmentPath(int num_crews)
{
	std::vector<int> path = {0};
	for (int j = 1; j <= num_crews; j++)
	{
		path.push_back(round(j * (float)(m_condensed_data.num_mirror_groups) / num_crews));
	}
	return path;
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
	std::vector<int> equal_path;
	std::vector<int> best_path;
	
	double cost, cost_eq;
	double field_eff, field_eff_eq;
	double min_cost = INFINITY;
	for (int i = 1; i <= m_settings.max_num_crews; i++)
	{
		path = RetracePath(
			m_results.parents, i + 1, m_condensed_data.num_mirror_groups+1
		);
		
		cost = EvaluatePath(path);
		
		field_eff = EvaluateFieldEfficiency(path);
		
		/*
		std::cerr << "Cost for " << i << " wash crews: " << cost
			<< "\nAssignment: ";
		for (int j = 0; j < path.size(); j++)
		{
			std::cerr << path.at(j) << ",";
		}
		std::cerr << "\nAverage field efficiency: " << field_eff << "\n";
		*/
		equal_path = GetEqualAssignmentPath(i);
		cost_eq = EvaluatePath(equal_path);
		field_eff_eq = EvaluateFieldEfficiency(equal_path);
		
		/*
		std::cerr << "Cost for " << i << " equal-assigned wash crews: " << cost_eq
			<< "\nAssignment: ";
		for (int j = 0; j < equal_path.size(); j++)
		{
			std::cerr << equal_path.at(j) << ",";
		}
		std::cerr << "\nAverage field efficiency for equal assignment: " << field_eff_eq << "\n";
		*/

		//use the equal-assignment path if specified; otherwise, use DP output.
		if (m_settings.use_uniform_assignment)
		{
			if (cost_eq < min_cost)
			{
				best_path = equal_path;
				min_cost = cost_eq * 1.0;
			}
		}
		else
		{
			if (cost < min_cost)
			{
				best_path = path;
				min_cost = cost * 1.0;
			}
		}
	}
	std::cerr << "optimal cost: " << min_cost << "\nNumber of wash crews: "
		<< best_path.size()-1 << "\n";
	m_results.assignments = best_path;
	m_results.num_wash_crews = best_path.size() - 1;
	if (output)
	{
		OutputResults();
	}

	GroupSolutionMirrors();

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

solar_field_data WashCrewOptimizer::GetSolutionData()
{
	solar_field_data d;

	d.groupings = m_solution_data.groupings;
	d.mirror_output = m_solution_data.mirror_output;
	d.names = m_solution_data.names;
	d.num_mirrors_by_group = m_solution_data.num_mirrors_by_group;
	d.num_mirror_groups = m_solution_data.num_mirror_groups;
	d.scale = m_solution_data.scale;
	d.total_mirror_output = m_solution_data.total_mirror_output;
	d.x_pos = m_solution_data.x_pos;
	d.y_pos = m_solution_data.y_pos;

	return d;
}

