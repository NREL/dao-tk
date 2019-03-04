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
	t -- average time elapsed between heliostat cleanings (hours)

	retval -- average efficiency loss
	*/

return t * day_degradation / 48;
}

std::string LinearSoilingFunc::GetType()
{
	return "linear";
}

wash_crew_settings::wash_crew_settings()
{
	max_num_crews = 20;
	capital_cost_per_crew = 0.;
	labor_cost_per_ft_crew = 0.;
	labor_cost_per_seas_crew = 0.;
	hourly_cost_per_crew = 0.;
	discount_rate_capital = 0.;
	discount_rate_labor = 0.;
	discount_rate_rev = 0.;
	wash_rate = 3677.885;
	heliostat_size = 115;  //crescent dunes site
	crew_hours_per_week = 0.;
	total_cost_per_crew = 0.;  //NPV
	system_efficiency = 0.;
	operating_margin = 1.;
	profit_per_kwh = 0.;  //assuming 100% efficiency
	num_years = 0.;   //years of operation to calculate NPV of annual costs
	price_per_kwh = 0.;  //assumed average (no multiplier)
	annual_multiplier = 0.;
	vehicle_life = 10;
	vehicle_cost = 100000.;
	seasonal_cost_multiple = 1.1;

	use_uniform_assignment = false;
}

void wash_crew_settings::print()
{
	std::cerr << "wash crew settings parameters: \n"
		<< "max_num_crews" << ": " << max_num_crews << "\n"
		<< "hourly_cost_per_crew" << ": " << hourly_cost_per_crew << "\n"
		<< "discount_rate_labor" << ": " << discount_rate_labor << "\n"
		<< "discount_rate_capital" << ": " << discount_rate_capital << "\n"
		<< "discount_rate_rev" << ": " << discount_rate_rev << "\n"
		<< "wash_rate" << ": " << wash_rate << "\n"
		<< "heliostat_size" << ": " << heliostat_size << "\n"
		<< "crew_hours_per_week" << ": " << crew_hours_per_week << "\n"
		<< "total_cost_per_crew" << ": " << total_cost_per_crew << "\n"
		<< "system_efficiency" << ": " << system_efficiency << "\n"
		<< "operating_margin" << ": " << operating_margin << "\n"
		<< "num_years" << ": " << num_years << "\n"
		<< "price_per_kwh" << ": " << price_per_kwh << "\n"
		<< "annual_multiplier" << ": " << annual_multiplier << "\n"
		<< "vehicle_life" << ": " << vehicle_life << "\n"
		<< "vehicle_cost" << ": " << vehicle_cost << "\n"
		<< "seasonal_cost_multiple" << ": " << seasonal_cost_multiple << "\n"
		<< "use_uniform_assignment" << ": " << use_uniform_assignment << "\n"
		<< "capital_cost_per_crew" << ": " << capital_cost_per_crew << "\n"
		<< "labor_cost_per_ft_crew" << ": " << labor_cost_per_ft_crew << "\n"
		<< "labor_cost_per_seas_crew" << ": " << labor_cost_per_seas_crew << "\n"
		<< "profit_per_kwh" << ": " << profit_per_kwh << "\n";
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
	num_mirrors = NULL;
	dni_by_period = {};
	labor_by_period = {};
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
	assignments_by_crews = {};
	solution_assignments = {};
	num_crews_by_period = {};
	objective_values = 0;
	parents = 0;
	distances = 0;
	num_vehicles = 0;
	num_ft_crews = 0;
	wash_crew_obj = 0.;
	field_eff = 0.;
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
	weather_file = "daggett.csv";
	solar_data_file = "fieldbase.csv";
	inputs_file = "inputs-us.csv";
}

