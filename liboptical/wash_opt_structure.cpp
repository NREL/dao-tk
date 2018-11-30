#include <iostream>
#include "wash_opt_structure.h"

SoilingFunction::SoilingFunction()
{
}

SoilingFunction::SoilingFunction(
	double day,
	double week,
	double month,
	double six_month
)
{
	day_degradation = day;
	week_degradation = week;
	month_degradation = month;
	sixmo_degradation = six_month;
}

double SoilingFunction::Evaluate(double t)
{
	return -1.0;
}

std::string SoilingFunction::GetType()
{
	return "NA";
}

double LinearSoilingFunc::Evaluate(double t)
{
	/*
	Estimates the average efficiency loss (complement of the average mirror
	efficiency) of a heliostat given the period between cleanings, t.
	t -- period between heliostat cleanings (minutes)

	retval -- average efficiency loss
	*/

return t * day_degradation / 2880;
}

std::string LinearSoilingFunc::GetType()
{
	return "linear";
}

wash_crew_settings::wash_crew_settings()
{
	max_num_crews = 20;
	capital_cost_per_crew = 0.;
	hourly_cost_per_crew = 0.;
	discount_rate = 0.;
	wash_time = 1.5;
	crew_hours_per_week = 0.;
	total_cost_per_crew = 0.;  //NPV
	system_efficiency = 0.;
	profit_per_kwh = 0.;  //assuming 100% efficiency
	num_years = 0.;   //years of operation to calculate NPV of annual costs
	price_per_kwh = 0.;  //assumed NPV (includes multiplier)
	annual_multiplier = 0.;
}

void wash_crew_settings::print()
{
	std::cerr << "wash crew settings parameters: \n"
		<< "max_num_crews" << ": " << max_num_crews << "\n"
		<< "capital_cost_per_crew" << ": " << capital_cost_per_crew << "\n"
		<< "hourly_cost_per_crew" << ": " << hourly_cost_per_crew << "\n"
		<< "discount_rate" << ": " << discount_rate << "\n"
		<< "wash_time" << ": " << wash_time << "\n"
		<< "crew_hours_per_week" << ": " << crew_hours_per_week << "\n"
		<< "total_cost_per_crew" << ": " << total_cost_per_crew << "\n"
		<< "system_efficiency" << ": " << system_efficiency << "\n"
		<< "profit_per_kwh" << ": " << profit_per_kwh << "\n"
		<< "operating_margin" << ": " << operating_margin << "\n"
		<< "num_years" << ": " << num_years << "\n"
		<< "price_per_kwh" << ": " << price_per_kwh << "\n"
		<< "annual_multiplier" << ": " << annual_multiplier << "\n";
}

/*
int max_num_crews;
	 capital_cost_per_crew; // fixed
	 hourly_cost_per_crew;  //per hour
	 materials_cost_per_crew;  //per year?
	 discount_rate;
	 wash_time; //in minutes per mirror
	 crew_hours_per_week;
	 total_cost_per_crew;  //NPV
	 system_efficiency;
	 profit_per_kwh;  //assuming 100% efficiency
	 num_years;      //years of operation to calculate NPV of annual costs
	 price_per_kwh;  //assumed average
*/

solar_field_data::solar_field_data()
{
	//null pointers for arrays
	names = 0;
	x_pos = 0;
	y_pos = 0;
	mirror_output = 0;
	scale = 1;
	
	num_mirror_groups = NULL;
	groupings = {};
}
/*
solar_field_data::~solar_field_data()
{
	if (names != 0)
		delete[] names;
	if (x_pos != 0)
		delete[] x_pos;
	if (y_pos != 0)
		delete[] y_pos;
	if (mirror_output != 0)
		delete[] mirror_output;
	mirror_size = NULL;
	annual_dni = NULL;
}
*/
wash_crew_opt_results::wash_crew_opt_results()
{
	assignments = {};
	objective_values = 0;
	parents = 0;
	distances = 0;
}
/*
wash_crew_opt_results::~wash_crew_opt_results()
{
	if (assignments != 0)
		delete[] assignments;
	if (objective_values != 0)
		delete[] objective_values;
	if (parents != 0)
		delete[] parents;
	if (distances != 0)
		delete[] distances;
}
*/
wash_crew_file_settings::wash_crew_file_settings()
{
	distance_file = "wc_dist.csv";
	obj_file = "wc_obj.csv";
	path_file = "wc_path.csv";
	parents_file = "wc_parents.csv";
}

