#pragma once

#ifndef _WASH_OPT_STRUCT_
#define _WASH_OPT_STRUCT_

#include <unordered_map>

class SoilingFunction
{
public:
	double day_degradation;
	double week_degradation;
	double month_degradation;
	double sixmo_degradation;
	SoilingFunction();
	SoilingFunction(
		double day, 
		double week=0., 
		double month=0., 
		double six_month=0.
	);
	virtual double Evaluate(double t);
	virtual std::string GetType();
};

class LinearSoilingFunc : public SoilingFunction
{
public:
	LinearSoilingFunc() : SoilingFunction() {}
	LinearSoilingFunc(
		double day,
		double week = 0.,
		double month = 0.,
		double six_month = 0.
	) : SoilingFunction::SoilingFunction(day, week, month, six_month) {}
	double Evaluate(double t) override;
	std::string GetType() override;
};

class ExponentialSoilingFunc : public SoilingFunction
{
public:
	ExponentialSoilingFunc() : SoilingFunction() {}
	ExponentialSoilingFunc(
		double day,
		double week = 0.,
		double month = 0.,
		double six_month = 0.
	);
	double Evaluate(double t) override;
};

struct wash_crew_settings
{
	
	int max_num_crews;
	double capital_cost_per_crew; // fixed
	double hourly_cost_per_crew;  //per hour
	double discount_rate;
	double wash_rate; //in m^2 per hour
	double heliostat_size; //m^2
	double crew_hours_per_week;
	double crew_hours_per_day;
	double total_cost_per_crew;  //NPV of labor and capital costs
	double system_efficiency; //assumed efficiency including receiver, TES losses, power cycle
	double profit_per_kwh;  //per kwh dni sent to receiver, assuming no soiling/degr.
	double num_years;      //years of operation to calculate NPV of annual costs
	double price_per_kwh;  //assumed average for grid output
	double annual_multiplier;
	int vehicle_life;  //years
	
	bool use_uniform_assignment;

	wash_crew_settings();
	void print();
};

struct solar_field_data
{
	int *names;
	double *x_pos;
	double *y_pos;
	double *mirror_output;
	double total_mirror_output;
	int *num_mirrors_by_group;
	int num_mirror_groups;
	int scale;
	//double mirror_size;
	//double annual_dni;
	std::unordered_map<int, std::vector<int>> groupings;
	solar_field_data();
	//~solar_field_data();
};

struct wash_crew_opt_results
{
	std::vector<int> assignments;
	double *objective_values;
	int *parents;
	double *distances;
	int num_wash_crews;
	solar_field_data solution_data;
	wash_crew_opt_results();
	//~wash_crew_opt_results();
};

struct wash_crew_file_settings
{
	std::string distance_file;
	std::string obj_file;
	std::string path_file;
	std::string parents_file;
	wash_crew_file_settings();
};


#endif

