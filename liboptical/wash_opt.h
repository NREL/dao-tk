#pragma once
#include <vector>

#include "wash_opt_structure.h"

#ifndef _WASH_OPT_
#define _WASH_OPT_

class WashCrewOptimizer
{
	SoilingFunction *m_func;
public:
	WashCrewOptimizer();
	WashCrewOptimizer(
		std::vector<double> x_pos,
		std::vector<double> y_pos,
		std::vector<double> mirror_output,
		int max_wash_crews,
		int num_mirrors,
		int scale
	);

	wash_crew_settings m_settings;
	solar_field_data m_solar_data;
	solar_field_data m_condensed_data;
	solar_field_data m_solution_data;
	wash_crew_opt_results m_results;
	wash_crew_file_settings m_file_settings;

	std::string int_pair_to_string(int i, int j);

	void Initialize();

	void ReadWeatherData();

	void ReadSolarDataFromFiles();

	void ReadInputsFile();

	void ReadAllFiles();

	void AddNewHeliostats(int num_heliostats);

	double GetFunctionDailyLoss();

	double GetSoilingAccumulation(double accumulation);

	void SortMirrors();

	void GroupMirrors(int scale);

	void GroupSolutionMirrors(int hours = 1);

	void CalculateRevenueAndCosts();

	void AssignSoilingFunction(SoilingFunction *func);

	void GetTotalFieldOutput();

	double GetNumberOfMirrors(int i, int j);

	double GetAssignmentCost(int i, int j);

	double EvaluatePath(std::vector<int> path);

	double EvaluateFieldEfficiency(std::vector<int> path);

	void ObtainOBJs();

	int FindMinDistaceNode(
		std::vector<double> distances,
		std::vector<bool> available,
		int array_size
	);

	std::vector<int> GetEqualAssignmentPath(int num_crews);

	void RunDynamicProgram();

	std::vector<int> RetracePath(std::vector<int> parents, int num_rows, int row_length);

	void CalculateSolutionObjective(std::unordered_map<std::string, double> rev_losses);

	void OptimizeWashCrews(int scale=-1, bool output=false);


	void Output2DArrayToFile(
		std::string filename, 
		std::vector<double> arr,
		int num_rows, 
		int row_length
	);

	void Output2DIntArrayToFile(
		std::string filename,
		std::vector<int> arr,
		int num_rows,
		int row_length
	);

	void OutputVectorToFile(
		std::string filename,
		std::vector<int> vec
	);

	void OutputWCAssignment();

	void OutputResults();

	solar_field_data GetSolutionData();

};

#endif
