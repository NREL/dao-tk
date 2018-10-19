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
	int scale
)
{
	m_solar_data = solar_field_data();
	m_solar_data.x_pos = x_pos;
	m_solar_data.y_pos = y_pos;
	m_solar_data.mirror_eff = mirror_eff;
	m_solar_data.num_mirror_groups = sizeof(mirror_eff)/sizeof(double);
	m_settings.max_num_crews = max_wash_crews;
	GroupMirrors(scale);
}

void WashCrewOptimizer::Initialize()
{
	m_solar_data.mirror_size = 1;
	m_solar_data.annual_dni = 3000;
}

void WashCrewOptimizer::ReadFromFiles()
{
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
			x.push_back(std::stoi(split_line[1]));
			y.push_back(std::stoi(split_line[2]));
			eff.push_back(std::stoi(split_line[16]));
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

void WashCrewOptimizer::GroupMirrors(int scale)
{
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

void WashCrewOptimizer::AssignSoilingFunction(SoilingFunction *func)
{
	m_func = func;
}

double WashCrewOptimizer::GetAverageEfficiencyLoss(int num_mirrors)
{
	return 0.0;
}

double WashCrewOptimizer::GetNumberOfMirrors(int i, int j)
{
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
	if (j == i)
	{
		return 0.;
	}
	double total_eff = 0.;
	for (int k = i; k < j; k++)
	{
		total_eff += m_condensed_data.mirror_eff[k];
	}
	double time = GetNumberOfMirrors(i, j) * m_settings.wash_time;
	/*
	if (j > 860)
	{
		std::cerr << i << "," << j << "," << total_eff << "," << time << "," << m_func->Evaluate(time) << "\n";
	}
	*/
	return total_eff * m_func->Evaluate(time);
}

double WashCrewOptimizer::EvaluatePath(std::vector<int> path)
{
	double sum = 0;
	for (unsigned int i = 0; i < path.size() - 1; i++)
	{
		sum += GetAssignmentCost(path[i], path[i + 1]);
	}
	return sum;
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
			objs[i* (m_condensed_data.num_mirror_groups + 1) + j] = 0.;
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
	for (int i = 2; i < m_condensed_data.num_mirror_groups + 1; i++)
	{
		available[i] = false;
		available[
			(m_settings.max_num_crews + 1)*(m_condensed_data.num_mirror_groups + 1) - i
			] = false;
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
		crew_idx = current_node / (m_condensed_data.num_mirror_groups + 1);
		mirror_idx = current_node % (m_condensed_data.num_mirror_groups + 1);
		//update min distance and best path to all destination nodes,
		//via the local arrays 'distances' and 'parents'.
		if (crew_idx < m_settings.max_num_crews)
		{
			for (int i = mirror_idx; i <= m_condensed_data.num_mirror_groups; i++)
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

int * WashCrewOptimizer::RetracePath(int* parents, int num_rows, int row_length)
{
	int* path = new int[num_rows];
	int parent = row_length-1; //row length - 1 = num_mirrors
	for (int i = num_rows-1; i >= 0; i--)
	{
		path[i] = parent;
		parent = parents[i*row_length + parent];
	}
	return path;
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
	Output2DIntArrayToFile(
		m_file_settings.path_file,
		m_results.assignments,
		1,
		m_settings.max_num_crews + 1
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
