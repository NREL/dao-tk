#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
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

std::string WashCrewOptimizer::int_pair_to_string(int i, int j)
{
	std::string s = std::to_string(i);
	s += ",";
	s += std::to_string(j);
	return s;
}

void WashCrewOptimizer::Initialize()
{
	/*
	Initializes settings and parameters to default values.
	*/
	//m_solar_data.mirror_size = 115.7;  //m^2 per mirror 
	//m_solar_data.annual_dni = 2685;  //in kWh per m^2 per year
	m_settings.capital_cost_per_crew = 100000; //no source
	m_settings.crew_hours_per_week = 40;  //no source
	m_settings.discount_rate_rev = 0.10; //no source
	m_settings.discount_rate_labor = 0.10; //no source
	m_settings.discount_rate_capital = 0.10; //no source
	m_settings.hourly_cost_per_crew = 100;  //no source
	m_settings.wash_rate = 3680; //Wolferstetter 2018
	m_settings.system_efficiency = 0.4;  // power to grid / DNI received
	m_settings.num_years = 15.;
	m_settings.price_per_kwh = 0.15; //$/kWh
	m_settings.operating_margin = 1.;  //% of revenue going to profit
	m_settings.max_num_crews = 40;
}

void WashCrewOptimizer::ReadSolarDataFromFiles()
{
	/*
	Reads in probem data from file, and stores the results in the 
	m_solar_data structure.
	*/
	//output variables
	std::vector<double> x = {};
	std::vector<double> y = {};
	std::vector<double> output = {};
	std::vector<int> mirrors_by_group = {};
	std::vector<int> hel_id = {};

	//file-related variables
	std::ifstream pfile;
	pfile.open(m_file_settings.solar_data_file);
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
	int line = 0;
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
			hel_id.push_back(std::stoi(split_line[3]));
			x.push_back(std::stod(split_line[0]));
			y.push_back(std::stod(split_line[1]));
			output.push_back(std::stod(split_line[2]));
			mirrors_by_group.push_back(std::stoi(split_line[4]));
		}
		split_line.clear();
		line++;
	}
	double *x_arr = new double[x.size()];
	double *y_arr = new double[y.size()];
	double *output_arr = new double[output.size()];
	int *id_arr = new int[hel_id.size()];
	int *mirror_arr = new int[mirrors_by_group.size()];
	std::copy(x.begin(), x.end(), x_arr);
	std::copy(y.begin(), y.end(), y_arr);
	std::copy(output.begin(), output.end(), output_arr);
	std::copy(hel_id.begin(), hel_id.end(), id_arr);
	std::copy(mirrors_by_group.begin(), mirrors_by_group.end(), mirror_arr);
	m_solar_data.names = id_arr;
	m_solar_data.x_pos = x_arr;
	m_solar_data.y_pos = y_arr;
	m_solar_data.mirror_output = output_arr;
	m_solar_data.num_mirrors_by_group = mirror_arr;
	m_solar_data.num_mirror_groups = mirrors_by_group.size();
	
}

void WashCrewOptimizer::ReadWeatherData()
{
	/*
	Reads in weather from a file in the format available in the samples section
	of the dao-tk 'deploy' folder.  Here, the 7th column is DNI, which is the
	output we track over time and divide into monthly intervals.
	*/
	std::ifstream pfile;
	pfile.open(m_file_settings.weather_file);
	//int cindex = 0;
	size_t pos = 0;
	std::string delimiter = ",";
	std::string pline;
	std::string token;
	std::vector<std::string> split_line = {};

	std::vector<int> cum_hours = { 0,744,1416,2160,2880,3624,4344,5088,5832,6552,7296,8016,8760 };
	std::vector<double> month_dni(12, 0.);
	std::vector<double> weather_file_dni = {};
	std::vector<double> month_labor(12, 0.);
	double sum_dni = 0.;

	//skip header row, location identifier rows
	for (int i = 0; i < 3; i++)
	{
		getline(pfile, pline);
		//cindex++;
	}

	//for each line that remains, record DNI in vector
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
			weather_file_dni.push_back(std::stod(split_line[5]));
		}
		split_line.clear();
	}

	//aggregate total dni and dni by month
	for (int i = 0; i < 12; i++)
	{
		for (int j = cum_hours.at(i); j < cum_hours.at(i + 1); j++)
		{
			sum_dni += weather_file_dni.at(j);
			month_dni.at(i) += weather_file_dni.at(j);
		}
	}

	//convert monthly dni into fractions
	for (int i = 0; i < 12; i++)
	{
		month_dni.at(i) /= sum_dni;
	}

	//assign monthly dni to data object
	m_solar_data.dni_by_period.clear();
	m_solar_data.labor_by_period.clear();
	for (int i = 0; i < 12; i++)
	{
		m_solar_data.dni_by_period.push_back(month_dni.at(i)*1.);
		m_solar_data.labor_by_period.push_back(
			(double)(cum_hours.at(i + 1) - cum_hours.at(i)) 
			/ cum_hours.at(cum_hours.size()-1)
		);
	}
}


void WashCrewOptimizer::ReadInputsFile()
{
	std::ifstream pfile;
	pfile.open(m_file_settings.inputs_file);
	//int cindex = 0;
	size_t pos = 0;
	std::string delimiter = ",";
	std::string pline;
	std::string token;
	std::vector<std::string> split_line = {};

	std::unordered_map<std::string, double> inputs;
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
		if (split_line.size() > 1)
		{
			inputs[split_line[0]] = std::stod(split_line[1]);
		}
		split_line.clear();
	}
	m_settings.capital_cost_per_crew = inputs["vehicle_cost"];
	m_settings.vehicle_life = int(inputs["vehicle_life"]);
	m_settings.crew_hours_per_week = inputs["crew_hours_per_week"];
	m_settings.discount_rate_rev = inputs["discount_rate_rev"];
	m_settings.discount_rate_labor = inputs["discount_rate_labor"];
	m_settings.discount_rate_capital = inputs["discount_rate_capital"];
	m_settings.hourly_cost_per_crew = inputs["hourly_cost_per_crew"];
	m_settings.wash_rate = inputs["wash_rate"];
	m_settings.system_efficiency = inputs["system_efficiency"];
	m_settings.num_years = inputs["num_years"];
	m_settings.price_per_kwh = inputs["price_per_kwh"];
	m_settings.max_num_crews = inputs["max_num_crews"];
	m_settings.heliostat_size = inputs["helio_size"];
	m_settings.soiling_rate = inputs["soiling_rate"];
	m_settings.seasonal_cost_multiple = inputs["seasonal_multiple"];

}

void WashCrewOptimizer::ReadAllFiles()
{
	ReadInputsFile();
	ReadSolarDataFromFiles();
	ReadWeatherData();
}

void WashCrewOptimizer::AddNewHeliostats(int num_heliostats)
{
	double *x_arr = new double[m_solar_data.num_mirror_groups+num_heliostats];
	double *y_arr = new double[m_solar_data.num_mirror_groups + num_heliostats];
	double *output_arr = new double[m_solar_data.num_mirror_groups + num_heliostats];
	int *id_arr = new int[m_solar_data.num_mirror_groups + num_heliostats];

	for (int i = 0; i < m_solar_data.num_mirror_groups; i++)
	{
		x_arr[i] = m_solar_data.x_pos[i];
		y_arr[i] = m_solar_data.y_pos[i];
		output_arr[i] = m_solar_data.mirror_output[i];
		id_arr[i] = m_solar_data.names[i];
	}

	for (int i = 0; i < num_heliostats; i++)
	{
		id_arr[m_solar_data.num_mirror_groups + i] = 100000 + i;
		x_arr[m_solar_data.num_mirror_groups + i] = 100000 + i;
		y_arr[m_solar_data.num_mirror_groups + i] = 100000 + i;
		//output_arr[i] = m_solar_data.mirror_output[m_solar_data.num_mirror_groups - 1];
		output_arr[i] = m_solar_data.total_mirror_output / m_solar_data.num_mirror_groups;
	}

	m_solar_data.names = id_arr;
	m_solar_data.x_pos = x_arr;
	m_solar_data.y_pos = y_arr;
	m_solar_data.mirror_output = output_arr;

	m_solar_data.num_mirrors_by_group = new int[m_solar_data.num_mirror_groups + num_heliostats];
	for (int i = 0; i < m_solar_data.num_mirror_groups + num_heliostats; i++)
		m_solar_data.num_mirrors_by_group[i] = 1;
	
	//update number of total heliostats, total mirror output
	//m_solar_data.total_mirror_output += num_heliostats * m_solar_data.mirror_output[m_solar_data.num_mirror_groups - 1];
	//std::cerr << "   " << m_solar_data.total_mirror_output;
	m_solar_data.total_mirror_output += num_heliostats * m_solar_data.total_mirror_output / m_solar_data.num_mirror_groups;
	m_solar_data.num_mirror_groups += num_heliostats;
	//std::cerr << "  then  " << m_solar_data.total_mirror_output << "  " << m_solar_data.num_mirror_groups << "\n";
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
	least anual output, and reorders the data in x_pos, y_pos and names 
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
	if (scale == 1)
	{
		m_condensed_data = m_solar_data;
		GetTotalFieldOutput();
		return;
	}
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
	//copy dni by period to condensed data
	m_condensed_data.dni_by_period.clear();
	for (int i = 0; i < 12; i++)
	{
		m_condensed_data.dni_by_period.push_back(
			m_solar_data.dni_by_period.at(i)*1.
		);
	}
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
	for (int t = 0; t < m_results.num_crews_by_period.size(); t++)
		m_results.solution_assignments[t] = {0};
	

	//detremine the breakpoints that may be the end of a crew's assignment.
	std::set<int> assignment_breaks = {};
	if (m_settings.use_uniform_assignment)
		for (int t = 0; t < m_results.num_crews_by_period.size(); t++)
			for (int c = 0; c < m_results.num_crews_by_period.at(t); c++)
				assignment_breaks.insert((int)(
					m_solar_data.num_mirrors
					* c / m_results.num_crews_by_period.at(t)
					+ 0.5
					));
	else
		for (int t = 0; t < m_results.num_crews_by_period.size(); t++)
			for (int c = 0; c < m_results.num_crews_by_period.at(t); c++)
				assignment_breaks.insert((int)(
					GetNumberOfMirrors(
						m_results.assignments_by_crews[m_results.num_crews_by_period.at(t)].at(c),
						m_results.assignments_by_crews[m_results.num_crews_by_period.at(t)].at(c+1)
					)));

	//Add the hourly breaks.
	for (int c = 0; c <= m_solar_data.num_mirrors; c += m_solution_data.scale)
		assignment_breaks.insert(c);

	//Create a vector consisting of all the elemnents of the set 
	//(which are unique and ordered).
	m_solution_data.num_mirror_groups = assignment_breaks.size();
	m_solution_data.num_mirrors_by_group = new int[assignment_breaks.size() - 1];
	m_solution_data.mirror_output = new double[assignment_breaks.size() - 1];
	//std::vector<int> breakpoints = {};
	//for (int c : assignment_breaks)
	//	breakpoints.push_back(c);
	int idx = 0;
	int m;
	for (int c : assignment_breaks)
	{
		if (c != 0)
		{
			m_solution_data.num_mirrors_by_group[idx] = c - m;
		}
		m = c;
		idx++;
	}
		
	//assign mirrors to the groups, and update assignments as breakpoints are hit.
	int cumulative_mirrors = 0;
	int solar_data_idx = 0;
	int solar_mirrors = m_solar_data.num_mirrors_by_group[0];
	int solution_mirrors;
	double sol_group_output;
	for (int solution_idx = 0; solution_idx < assignment_breaks.size() - 1; solution_idx++)
	{
		solution_mirrors = m_solution_data.num_mirrors_by_group[solution_idx]*1;
		sol_group_output = 0.;
		while (solution_mirrors > 0)
		{
			if (solution_mirrors < solar_mirrors)
			{
				sol_group_output += m_solar_data.mirror_output[solar_data_idx] * (double)solution_mirrors / m_solar_data.num_mirrors_by_group[solar_data_idx];
				cumulative_mirrors += solution_mirrors;
				solar_mirrors -= solution_mirrors;
				solution_mirrors = 0;
			}
			else
			{
				sol_group_output += m_solar_data.mirror_output[solar_data_idx] * (double)solar_mirrors / m_solar_data.num_mirrors_by_group[solar_data_idx];
				cumulative_mirrors += solar_mirrors;
				solution_mirrors -= solar_mirrors;
				solar_data_idx++;
				solar_mirrors = m_solar_data.num_mirrors_by_group[solar_data_idx];
			}
		}
		m_solution_data.mirror_output[solution_idx] = sol_group_output;
		for (int t = 0; t < m_results.num_crews_by_period.size(); t++)
		{
			if (
				std::find(
					m_results.assignments_by_crews.at(m_results.num_crews_by_period.at(t)).begin(),
					m_results.assignments_by_crews.at(m_results.num_crews_by_period.at(t)).end(),
					cumulative_mirrors
				)
				!= m_results.assignments_by_crews.at(m_results.num_crews_by_period.at(t)).end()
				)
				m_results.solution_assignments[t].push_back(solution_idx);
		}
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
	double annual_labor_cost = (
		m_settings.hourly_cost_per_crew * m_settings.crew_hours_per_week * (365./7.)
		);

	double capital_cost = m_settings.vehicle_cost;
	for (int yrs = m_settings.vehicle_life; yrs < m_settings.num_years; yrs += m_settings.vehicle_life)
	{
		capital_cost += (
			m_settings.vehicle_cost
			/ std::pow((1. + m_settings.discount_rate_capital), yrs)
			);
	}
	
	double revenue_multiplier = (
		(1 - std::pow(1 / (1 + m_settings.discount_rate_rev), m_settings.num_years))
		/ (1 - (1 / (1 + m_settings.discount_rate_rev)))
		);

	double labor_multiplier = (
		(1 - std::pow(1 / (1 + m_settings.discount_rate_labor), m_settings.num_years))
		/ (1 - (1 / (1 + m_settings.discount_rate_labor)))
		);
	
	m_settings.labor_cost_per_ft_crew = annual_labor_cost * labor_multiplier;
	m_settings.labor_cost_per_seas_crew = m_settings.labor_cost_per_ft_crew * m_settings.seasonal_cost_multiple;
	m_settings.capital_cost_per_crew = capital_cost;

	m_settings.profit_per_kwh = (   //NPV per annual kWh sent to receiver by heliostat
		m_settings.price_per_kwh * m_settings.system_efficiency 
		* m_settings.operating_margin * revenue_multiplier
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
	(sums the mirror_output of each group of mirrors).
	*/
	double sum_clean_output = 0.;
	for (size_t i = 0; i < m_solar_data.num_mirror_groups; i++)
	{
		sum_clean_output += m_solar_data.mirror_output[i];
	}
	m_solar_data.total_mirror_output = sum_clean_output;
	m_condensed_data.total_mirror_output = sum_clean_output;
	//std::cerr << "gettotalfield: num mirrors " << m_solar_data.num_mirror_groups << " output " << m_solar_data.total_mirror_output << "\n";
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
		num_mirrors += m_condensed_data.num_mirrors_by_group[k];

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
		obtains the cost due to revenues lost the colletction of mirrors 
		starting in group i and ending just before the start of group j,
		given all these mirrors are assigned to a single crew.

		i, j -- start and end indices of mirror groups
	*/
	if (j <= i)
		return 0.;

	double total_output = 0.;
	for (int k = i; k < j; k++)
	{
		total_output += m_condensed_data.mirror_output[k];
	}

	double mirrors_per_hour = (m_settings.wash_rate / m_settings.heliostat_size);
	double time = GetNumberOfMirrors(i, j) * (168. / m_settings.crew_hours_per_week) / mirrors_per_hour; //in hours between cleanings

	return m_settings.profit_per_kwh * total_output * m_func->Evaluate(time);// + m_settings.total_cost_per_crew;
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
		sum += GetAssignmentCost(path[i], path[i + 1]);
	
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
			group_eff += m_condensed_data.mirror_output[k];

		//get the time elapsed and average efficiency hit.
		time = GetNumberOfMirrors(start_idx, end_idx) * (60./(m_settings.wash_rate / m_settings.heliostat_size)) * (168. / m_settings.crew_hours_per_week);
		sum_soiling_eff += group_eff * m_func->Evaluate(time);
		//std::cerr << "path size: " << path.size() << " idx: " << i << " start_idx: " << start_idx << " end_idx: " << end_idx << " group_eff: " << group_eff << " time: " << time << " soiling_loss: " << m_func->Evaluate(time) << "\n";
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
			objs[i* (m_condensed_data.num_mirror_groups + 1) + j] = INFINITY;
		for (int j = i; j < m_condensed_data.num_mirror_groups + 1; j++)
			objs[i* (m_condensed_data.num_mirror_groups+1) + j] = GetAssignmentCost(i, j);
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
		available[num_nodes - i - 1] = false;
	}
	for (int i = 1; i < m_settings.max_num_crews + 1; i++)
		for (int j = 0; j < i; j++)
			available[i*(row_length) + j] = false;

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
	//m_results.assignments = RetracePath(parents, num_rows, row_length);
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
		rev_path.push_back(path.at(i));

	return rev_path;
}

void WashCrewOptimizer::CalculateSolutionObjective(std::unordered_map<std::string,double> rev_losses)
{
	double cost = 0.;
	int crews;
	int ft_crews = m_results.num_crews_by_period.at(0);
	int vehicles = m_results.num_crews_by_period.at(0);

	//add revenue losses, and update number of full-time crews and vehicles
	for (size_t t = 0; t < m_results.num_crews_by_period.size(); t++)
	{
		crews = m_results.num_crews_by_period.at(t);
		cost += rev_losses[int_pair_to_string(crews, t+1)];
		ft_crews = std::min(crews, ft_crews);
		vehicles = std::max(crews, vehicles);
	}

	//calculate field efficiency as revenue losses / total revenue
	m_results.field_eff = 1 - (
		cost / (
		m_solar_data.total_mirror_output 
		* m_settings.operating_margin 
		* m_settings.profit_per_kwh
		)
	);
	std::cerr << "lost rev: " << cost << "\n";

	//add the vehicle and full-time crew costs
	cost += vehicles * m_settings.capital_cost_per_crew;
	cost += ft_crews * m_settings.labor_cost_per_ft_crew;

	std::cerr << "Full-time labor cost: " << ft_crews * m_settings.labor_cost_per_ft_crew << "\n";
	std::cerr << "Vehicle cost: " << vehicles * m_settings.capital_cost_per_crew << "\n";

	//add seasonal labor costs 
	double st_labor = 0.;
	for (size_t t = 0; t < m_results.num_crews_by_period.size(); t++)
	{
		cost += m_solar_data.labor_by_period.at(t) * m_settings.labor_cost_per_seas_crew * (
			m_results.num_crews_by_period.at(t) - ft_crews
			);
		st_labor += m_solar_data.labor_by_period.at(t) * m_settings.labor_cost_per_seas_crew * (
			m_results.num_crews_by_period.at(t) - ft_crews
			);
	}
	std::cerr << "Short-term labor cost: " << st_labor << "\n";

	m_results.wash_crew_obj = cost;
	m_results.num_ft_crews = ft_crews;
	m_results.num_vehicles = vehicles;
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

	// use a scale of 1 if uniform assignment.
	if (m_settings.use_uniform_assignment)
		scale = 1;
	//redefine scale, if solving a DP and problem size is large
	if (scale == -1)
		scale = 1 + (m_solar_data.num_mirror_groups / 1000);
	
	//place mirrors into groups according to scale
	GroupMirrors(scale);

	//calculate revenue losses/costs
	CalculateRevenueAndCosts();

	std::unordered_map<std::string, double> rev_losses = {};

	//If assignment is uniform, analytically calculate revenue losses.
	if (m_settings.use_uniform_assignment)
	{
		double total_time = ( //hours
			m_solar_data.num_mirror_groups * m_settings.heliostat_size  //m^2
			/ (
				m_settings.wash_rate * m_settings.crew_hours_per_week / 168.
				) //(m^2/hr), adjusted for non-working time
			);  

		double total_rev = m_settings.profit_per_kwh * m_solar_data.total_mirror_output;
		//double eff_losses;

		for (int c = 1; c <= m_settings.max_num_crews; c++)
			for (int t = 0; t < 12; t++)
				rev_losses[int_pair_to_string(c, t + 1)] = (
					total_rev * m_func->Evaluate(total_time / (double)(c))
					* m_solar_data.dni_by_period[t]
					);
	}
	else  //if nonuniform assignment is used, solve a DP to obtain revenue losses.
	{
		RunDynamicProgram();

		//determine the lowest-cost path from the possible number of crews.
		std::vector<int> path = {};
		std::vector<int> equal_path = {};
		std::vector<int> best_path = {};

		double cost, cost_eq;
		double field_eff, field_eff_eq;
		double min_cost = INFINITY;
		int cum_mirrors;
		for (int i = 1; i <= m_settings.max_num_crews; i++)
		{
			path = RetracePath(
				m_results.parents, i + 1, m_condensed_data.num_mirror_groups + 1
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
			//use the equal-assignment path if specified; otherwise, use DP output.
			
			m_results.assignments_by_crews[i] = {0};
			for (int c = 0; c < path.size()-1; c++)
				m_results.assignments_by_crews[i].push_back(
					GetNumberOfMirrors(path[c], path[c + 1])
				);
			m_results.assignments_by_crews[i] = path;
			for (int t = 0; t < 12; t++)
			{
				rev_losses[int_pair_to_string(i, t+1)] = (
					cost * m_solar_data.dni_by_period.at(t)
					);
			}
		}
	}
	
	//next, calculate the change in revenue losses as the number of crews  
	//increases for each time period.
	std::unordered_map<std::string, double> delta_rev_loss = {};
	for (int t = 1; t <= 12; t++)
	{
		delta_rev_loss[int_pair_to_string(1, t)] = rev_losses[int_pair_to_string(1, t)];
	}
	for (int c = 2; c <= m_settings.max_num_crews; c++)
	{
		for (int t = 1; t <= 12; t++)
		{
			delta_rev_loss[int_pair_to_string(c, t)] = (
				rev_losses[int_pair_to_string(c, t)] 
				- rev_losses[int_pair_to_string(c-1, t)]
			);
		}
	}

	//finally, determine the optimal number of crews over time.  
	//start with one full time crew (at least one crew is required)
	std::vector<int> crews_by_period(12, 1);
	std::vector<bool> change_crews(12, false);
	double seas_delta, ft_delta, month_delta;

	for (int c= 2; c <= m_settings.max_num_crews; c++)
	{
		// get the true change in cost after accounting for labor costs.
		// start with the vehicle cost associated with an additional crew.
		ft_delta = m_settings.capital_cost_per_crew + m_settings.labor_cost_per_ft_crew;
		seas_delta = m_settings.capital_cost_per_crew;  //seasonal labor accumulated by month
		for (int t = 1; t <= 12; t++)
		{
			ft_delta += delta_rev_loss[int_pair_to_string(c, t)];
			//only count months in which rev losses are reduced by more
			//than the labor costs of the seasonal crew.
			month_delta = (
				m_settings.labor_cost_per_seas_crew
				* m_solar_data.labor_by_period.at(t-1)
				+ delta_rev_loss[int_pair_to_string(c, t)]
				);
			if (month_delta < 0)
			{
				seas_delta += month_delta;
				change_crews.at(t-1) = true;
			}
			else
			{
				change_crews.at(t - 1) = false;
			}
			std::cerr << "month " << t << "," << month_delta << "," <<
				(m_settings.labor_cost_per_seas_crew
					* m_solar_data.labor_by_period.at(t - 1)) << "," <<
				delta_rev_loss[int_pair_to_string(c, t)] << "\n";
		}

		// if neither full-time nor seasonal hires reduce losses, terminate.
		if (ft_delta >= 0. && seas_delta >= 0.)
		{
			std::cerr << "END" << "," << ft_delta << "," << seas_delta << "\n";
			break;
		}
		// if a full-time crew provides a bigger benefit than a seasonal crew, 
		// hire the seasonal crew.
		else if (ft_delta < seas_delta)
			for (int t = 0; t < 12; t++)
				crews_by_period.at(t)++;
		//otherwise, update the solution only for the months in which loss 
		// reduction exceeds labor costs.
		else
			for (int t = 0; t < 12; t++)
				if (change_crews.at(t))
					crews_by_period.at(t)++;
		std::cerr << crews_by_period.at(6) << "," << ft_delta << "," << seas_delta << "\n";
	}

	//assign the solution to the results object and output results if desired
	m_results.num_crews_by_period.clear();
	for (size_t t = 0; t < crews_by_period.size(); t++)
		m_results.num_crews_by_period.push_back(crews_by_period.at(t));

	if (output)
		OutputResults();

	CalculateSolutionObjective(rev_losses);

	//GroupSolutionMirrors();


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
			ofile << arr[i*row_length+j] << ",";

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
			ofile << arr[i*row_length + j] << ",";
		
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
		ofile << arr.at(i) << ",";
	
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
			int j = 0;//(m_condensed_data.scale * m_results.assignments[i]);
			j < 0;//(m_condensed_data.scale * m_results.assignments[i + 1]);
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
	/*
	OutputVectorToFile(
		m_file_settings.path_file,
		m_results.assignments
	);
	*/
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

