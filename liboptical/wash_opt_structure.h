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
	double labor_cost_per_crew;  //per hour
	double materials_cost_per_crew;  //per year?
	double discount_rate;
	double wash_time; //in minutes per mirror
	double crew_hours_per_week;
	double total_cost_per_crew;  //NPV
	double system_efficiency;
	double revenue_per_mirror;  //assuming 100% efficiency
	double operating_margin;
	double num_years;      //years of operation to calculate NPV of annual costs
	double price_per_kwh;  //assumed average

	wash_crew_settings();
	void print();
};

struct solar_field_data
{
	int *names;
	double *x_pos;
	double *y_pos;
	double *mirror_eff;
	int *num_mirrors_by_group;
	int num_mirror_groups;
	int scale;
	double mirror_size;
	double annual_dni;
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
