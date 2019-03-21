#ifndef _SOLARFIELD_AVAIL
#define _SOLARFIELD_AVAIL

#include "solarfield_structures.h"
#include <queue>

class solarfield_availability
{

	bool m_sim_available;

public:
	solarfield_availability();

	solarfield_settings m_settings;
	solarfield_results m_results;
	solarfield_event current_event;

	heliostat_field create_helio_field(int n_components, int n_heliostats, double scale, WELLFiveTwelve gen);

	std::vector<double> get_operating_hours();

	double get_time_of_failure(std::vector<double> operating_hours, double t_start, double op_life);

	double get_time_of_failure(double sunrise, double sunset, double t_start, double op_life);

	double get_time_of_repair(double t_start, double repair_time, solarfield_staff_member* staff);

	std::priority_queue<solarfield_event> create_initial_queue(heliostat_field field, std::vector<double> operating_hours = {});

	void process_failure();

	void process_repair();

	void run_current_event();

	void update_availability(double t_start, double t_end, double availability);

	void simulate(bool (*callback)(float prg, const char *msg)=0, std::string *results_file_name = 0);
	
	void initialize_results();

};

#endif