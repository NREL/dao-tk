#ifndef _SF_STAFFH_
#define _SF_STAFFH_

#include "solarfield_heliostat.h"
#include "solarfield_structures.h"
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
	bool m_busy;
	double m_fraction;				// Time fraction (i.e. full-time = 1, half-time = 0.5)
	int m_n_repairs_completed;		// Total number of repairs completed
	int m_n_repairs_started;		// Total number of repairs started
	int m_helio_assigned;			// Current Assignment
	double m_hours_worked;			// Total number of hours worked
	double m_hours_this_week;		// Hours worked this week
	double m_hours_today;			// Hours worked today
	double m_max_hours_per_day;		// Maximum hours per day 
	double m_max_hours_per_week;	// Maximum hours per week

	solarfield_staff_member();
	~solarfield_staff_member() {};
	void initialize(double max_hours_per_day, double max_hours_per_week);

	double get_time_available();
	double get_time_available_week();
	void add_time_worked(double time);
	void assign_heliostat(int helio_id);
	void free();
	bool is_busy();
	int get_assigned_heliostat();
};




class solarfield_repair_staff
{

public:

	bool m_is_prioritize_partial_repairs;   // Always prioritize partially-repaired heliostats?

	int m_n_staff;							// Total number of repair staff
	int m_total_repair_length;				// Number of heliostats either in queue or undergoing repairs
	double m_max_hours_per_day;		// Maximum hours per day for full-time staff 
	double m_max_hours_per_week;	// Maximum hours per week for full-time staff

	double m_total_repair_time;				// Repair time for heliostats either in repair queue or undergoing repairs

	std::vector<solarfield_staff_member*> m_members;	    // Staff members

	solarfield_repair_staff();
	~solarfield_repair_staff()
	{
		for (size_t i = 0; i < m_members.size(); i++)
			delete(m_members[i]);
		m_members.clear();
	};


	solarfield_repair_staff(int nstaff, double max_per_day, double max_per_week);
	void add_member(double max_per_day, double max_per_week);
	bool is_staff_available();
	solarfield_staff_member* get_assigned_member(int helio_id);
	solarfield_staff_member* get_available_staff();

	//void reset_time_fractions(std::vector<double> & time_fraction);

};





#endif