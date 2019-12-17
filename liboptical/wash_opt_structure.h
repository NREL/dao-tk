#pragma once

#ifndef _WASH_OPT_STRUCT_
#define _WASH_OPT_STRUCT_

#include <unordered_map>
#include <vector>
#include <string>

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
	std::vector<int> periods;
	int max_num_crews;
	int vehicle_life;  //years

	double capital_cost_per_crew; // fixed
	double labor_cost_per_ft_crew;
	double labor_cost_per_seas_crew;
	double hourly_cost_per_crew;  //per hour
	double discount_rate_rev;
	double discount_rate_capital;
	double discount_rate_labor;
	double wash_rate; //in m^2 per hour
	double heliostat_size; //m^2
	double crew_hours_per_week;
	double system_efficiency; //assumed efficiency including receiver, TES losses, power cycle
	double profit_per_kwh;  //per kwh dni sent to receiver, assuming 100% mirror efficiency
	double operating_margin; //used in calculating profit losses
	double num_years;      //years of operation to calculate NPV of annual costs
	double price_per_kwh;  //assumed average for grid output
	double annual_rev_multiplier;
	double seasonal_cost_multiple;
	double vehicle_cost;
	
	bool use_uniform_assignment;

	wash_crew_settings();
	void print();
};

struct solar_field_data
{
	std::vector<int> names;
	std::vector<double> x_pos;
	std::vector<double> y_pos;
	std::vector<double> mirror_output;
	double total_mirror_output;
	std::vector<int> num_mirrors_by_group;
	int num_mirror_groups;
	int num_mirrors;
	int scale;
	//double mirror_size;
	//double annual_dni;
	std::unordered_map<int, std::vector<int>> groupings;
	std::vector<double> dni_by_period;  //period = month
	std::vector<double> labor_by_period;  //period = month
	solar_field_data();
	~solar_field_data();
};

struct wash_crew_opt_results
{
	std::unordered_map<int,std::vector<int>> assignments_by_crews;
	std::unordered_map<int, std::vector<int>> solution_assignments;
	std::vector<int> num_crews_by_period;
	int num_ft_crews;
	int num_vehicles;
	std::vector<double> objective_values;
	std::vector<int> parents;
	std::vector<double> distances;
	double wash_crew_obj;
	double field_eff;
	double annual_labor_cost;
	//int num_wash_crews;
	solar_field_data solution_data;
	wash_crew_opt_results();
	~wash_crew_opt_results();
};

struct wash_crew_file_settings
{
	std::string distance_file;
	std::string obj_file;
	std::string path_file;
	std::string parents_file;
	std::string weather_file;
	std::string solar_data_file;
	std::string inputs_file;
	wash_crew_file_settings();
};


#endif

