#ifndef _SF_STAFFH_
#define _SF_STAFFH_

#include "solarfield_heliostat.h"
#include "./../libcycle/distributions.h"

enum repair_order {
	FAILURE_ORDER,		// Repair in order failed
	PERFORMANCE,		// Prioritize repairs based on highest to lowest heliostat performance
	REPAIR_TIME,		// Prioritize repairs in order from shortest to longest repair time
	MEAN_REPAIR_TIME,	// Prioritize repairs in order from shortest to longest mean repair time
	RANDOM,				// Random prioritization of repairs
	PERF_OVER_MRT       // Prioritize repairs in order from largest to smallest expected rate of increased performance
};



class solarfield_staff_member
{

public:
	double m_fraction;				// Time fraction (i.e. full-time = 1, half-time = 0.5)
	int m_n_repairs_completed;		// Total number of repairs completed
	int m_n_repairs_started;		// Total number of repairs started
	double m_hours_worked;			// Total number of hours worked
	double m_hours_this_week;		// Hours worked this week
	double m_hours_today;			// Hours worked today
	solarfield_heliostat* m_helio_assigned;	// Heliostat currently assigned


	solarfield_staff_member();
	solarfield_staff_member(double fraction);
	~solarfield_staff_member() {};

	double get_time_available(double max_per_day, double max_per_week);
	double get_time_available_week(double max_per_week);
	double get_time_assigned();
	void add_time_worked(double time);

};




class solarfield_repair_staff
{

public:

	unsigned int m_repair_order;			// Order that repairs are prioritized: FAILURE_ORDER, EFFICIENCY, REPAIR_TIME, MEAN_REPAIR_TIME, RANDOM
	bool m_is_prioritize_partial_repairs;   // Always prioritize partially-repaired heliostats?

	int m_n_staff;							// Total number of repair staff
	int m_total_repair_length;				// Number of heliostats either in queue or undergoing repairs

	double m_max_hours_per_day;				// Maximum hours per day for any staff member
	double m_max_hours_per_week;			// Maximum hours per week for any staff member
	double m_total_repair_time;				// Repair time for heliostats either in repair queue or undergoing repairs

	std::vector<solarfield_staff_member*>m_members;	// Staff members
	std::vector<solarfield_heliostat*> m_queue;		// Heliostats needing repair
	std::vector<int> m_queue_length;		// Number of heliostats either in queue or undergoing repairs
	std::vector<double> m_queue_time;		// Repair time for heliostats either in repair queue or undergoing repairs


	solarfield_repair_staff();
	~solarfield_repair_staff()
	{
		for (size_t i = 0; i < m_members.size(); i++)
			delete(m_members[i]);
		m_members.clear();
	};


	solarfield_repair_staff(int nstaff, double max_per_day, double max_per_week, const std::vector<double> &time_fraction);
	void add_member(double fraction);

	void add_to_queue(solarfield_heliostat *h, WELLFiveTwelve &gen);
	void assign_from_queue(int staff_index, int queue_index);
	void assign_to_queue(int staff_index, WELLFiveTwelve &gen);
	void repair(int staff_index, double repair_time);
	void queue_tracking();

	void reset_time_fractions(std::vector<double> & time_fraction);

};





#endif