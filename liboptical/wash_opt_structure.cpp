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
	return -t;
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

wash_crew_settings::wash_crew_settings()
{
	max_num_crews = 20;
	capital_cost_per_crew = 0.;
	labor_cost_per_crew = 0.;
	materials_cost_per_crew = 0.;
	discount_rate = 0.;
	wash_time = 1.5;
}

solar_field_data::solar_field_data()
{
	//null pointers for arrays
	names = 0;
	x_pos = 0;
	y_pos = 0;
	mirror_eff = 0;
	scale = 1;
	
	mirror_size = NULL;
	annual_dni = NULL;
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
	if (mirror_eff != 0)
		delete[] mirror_eff;
	mirror_size = NULL;
	annual_dni = NULL;
}
*/
wash_crew_opt_results::wash_crew_opt_results()
{
	assignments = 0;
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
