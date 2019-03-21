
#include <algorithm>
#include "solarfield_staff.h"


solarfield_staff_member::solarfield_staff_member()
{
	m_hours_worked = 0.0;
	m_hours_this_week = 0.0;
	m_hours_today = 0.0;
	m_n_repairs_completed = 0;
	m_n_repairs_started = 0;
	m_max_hours_per_day = std::numeric_limits<double>::quiet_NaN();
	m_max_hours_per_week = std::numeric_limits<double>::quiet_NaN();
}

solarfield_staff_member::solarfield_staff_member(double max_per_day, double max_per_week)
{
	m_hours_worked = 0.0;
	m_hours_this_week = 0.0;
	m_hours_today = 0.0;
	m_n_repairs_completed = 0;
	m_n_repairs_started = 0;
	m_max_hours_per_day = max_per_day;
	m_max_hours_per_week = fmin(7*max_per_day,max_per_week);
}

double solarfield_staff_member::get_time_available()
{
	return fmin(m_max_hours_per_week - m_hours_this_week, 
		m_max_hours_per_day - m_hours_today);
}

double solarfield_staff_member::get_time_available_week()
{
	return m_max_hours_per_week - m_hours_this_week;
}

void solarfield_staff_member::add_time_worked(double time)
{
	m_hours_today += time;
	m_hours_this_week += time;
	m_hours_worked += time;
	return;
}



solarfield_repair_staff::solarfield_repair_staff()
{
	m_members.clear();
	m_repair_order = PERF_OVER_MRT;
	m_is_prioritize_partial_repairs = true;
	m_n_staff = 0;
	m_max_hours_per_day = std::numeric_limits<double>::quiet_NaN();
	m_max_hours_per_week = std::numeric_limits<double>::quiet_NaN();

	m_total_repair_length = 0;
	m_total_repair_time = 0.0;
};

solarfield_repair_staff::solarfield_repair_staff(int nstaff, double max_per_day, double max_per_week)
{
	m_repair_order = FAILURE_ORDER;
	m_n_staff = nstaff;
	m_max_hours_per_day = max_per_day;
	m_max_hours_per_week = fmin(7 * max_per_day, max_per_week);

	for (int i = 0; i < nstaff; i++)
		add_member(max_per_day, max_per_week);  // assume full-time

	m_total_repair_length = 0;
	m_total_repair_time = 0.0;
};

void solarfield_repair_staff::add_member(double max_per_day, double max_per_week)
{
	solarfield_staff_member *staff = new solarfield_staff_member(max_per_day, max_per_week);
	m_members.push_back(staff);
	return;
}

/*
void solarfield_repair_staff::reset_time_fractions(std::vector<double> & time_fraction)
{
	for (size_t s = 0; s < m_members.size(); s++)
	{
		if (s < time_fraction.size())
			m_members[s]->m_fraction = time_fraction[s];
		else
			m_members[s]->m_fraction = 0.0;
	}
	return;
}
*/

solarfield_staff_member* solarfield_repair_staff::get_available_staff()
{
	for (solarfield_staff_member* m : m_members)
		if (!m->busy)
			return m;
}