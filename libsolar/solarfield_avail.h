#ifndef _SOLARFIELD_AVAIL
#define _SOLARFIELD_AVAIL

#include "solarfield_structures.h"
#include <queue>

class solarfield_availability
{

	bool m_sim_available;

public:
	solarfield_availability();

	void initialize();

	void initialize_results();


	solarfield_settings m_settings;
	solarfield_results m_results;
	std::priority_queue<solarfield_event> m_event_queue;
	std::priority_queue<solarfield_event> m_repair_queue;
	solarfield_event m_current_event;
	solarfield_repair_staff m_staff;
	heliostat_field m_field;

	WELLFiveTwelve* m_gen;

	double m_current_availability;
	int m_repair_queue_length;

	void create_om_staff(int n_staff, double max_hours_per_day, double max_hours_per_week);

	void create_helio_field(int n_components, int n_heliostats, double scale);

	void assign_generator(WELLFiveTwelve &gen);

	void get_operating_hours();

	double get_time_of_failure(double t_start, double op_life);

	double get_time_of_repair(double t_start, double repair_time, solarfield_staff_member* staff);

	std::priority_queue<solarfield_event> create_initial_queue(std::vector<double> operating_hours = {});

	void process_failure();//double t_last);

	void process_repair();//double t_last);

	void run_current_event(double t_last);

	void add_repair_to_queue();

	void update_statistics(double t_start, double t_end);

	void simulate(bool (*callback)(float prg, const char *msg)=0, std::string *results_file_name = 0);
	

	

};

#endif