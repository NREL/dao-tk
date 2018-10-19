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
		double *x_pos,
		double *y_pos,
		double *mirror_eff,
		int max_wash_crews,
		int scale
	);
	void Initialize();
	void ReadFromFiles();

	wash_crew_settings m_settings;
	solar_field_data m_solar_data;
	solar_field_data m_condensed_data;
	wash_crew_opt_results m_results;
	wash_crew_file_settings m_file_settings;

	void GroupMirrors(int scale);
	
	void AssignSoilingFunction(SoilingFunction *func);

	double GetAverageEfficiencyLoss(int num_mirrors);

	double GetNumberOfMirrors(int i, int j);

	double GetAssignmentCost(int i, int j);

	double EvaluatePath(std::vector<int> path);

	double* ObtainOBJs();

	int FindMinDistaceNode(
		double *distances, 
		bool *available, 
		int array_size
	);

	void RunDynamicProgram();

	int* RetracePath(int *parents, int num_rows, int row_length);

	void Output2DArrayToFile(
		std::string filename, 
		double* arr,
		int num_rows, 
		int row_length
	);

	void Output2DIntArrayToFile(
		std::string filename,
		int* arr,
		int num_rows,
		int row_length
	);

	void OutputWCAssignment();

	void OutputResults();

};

#endif