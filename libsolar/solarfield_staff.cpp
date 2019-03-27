
#include <algorithm>
#include "solarfield_staff.h"


solarfield_staff_member::solarfield_staff_member()
{
	m_max_hours_per_day = std::numeric_limits<double>::quiet_NaN();
	m_max_hours_per_week = std::numeric_limits<double>::quiet_NaN();
}

void solarfield_staff_member::initialize(double max_hours_per_day, double max_hours_per_week)
{
	m_hours_worked = 0.0;
	m_hours_this_week = 0.0;
	m_hours_today = 0.0;
	m_n_repairs_completed = 0;
	m_n_repairs_started = 0;
	m_max_hours_per_day = max_hours_per_day;
	m_max_hours_per_week = fmin(7 * max_hours_per_day, max_hours_per_week);
	m_helio_assigned = NULL;
	free();
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
	//m_hours_today += time;
	//m_hours_this_week += time;
	m_hours_worked += time;
	return;
}

void solarfield_staff_member::assign_heliostat(int helio_id)
{
	m_busy = true;
	m_helio_assigned = helio_id;
}

void solarfield_staff_member::free()
{
	m_busy = false;
}

bool solarfield_staff_member::is_busy()
{
	return m_busy;
}

int solarfield_staff_member::get_assigned_heliostat()
{
	return m_helio_assigned;
}



solarfield_repair_staff::solarfield_repair_staff()
{
	m_members.clear();
	m_is_prioritize_partial_repairs = true;
	m_n_staff = 0;
	m_max_hours_per_day = std::numeric_limits<double>::quiet_NaN();
	m_max_hours_per_week = std::numeric_limits<double>::quiet_NaN();

	m_total_repair_length = 0;
	m_total_repair_time = 0.0;
};

bool solarfield_repair_staff::is_staff_available()
{
	for (solarfield_staff_member* m : m_members)
		if (!(m->is_busy()))
			return true;
	return false;
}

solarfield_staff_member* solarfield_repair_staff::get_assigned_member(int helio_id)
{
	for (solarfield_staff_member* m : m_members)
		if (m->get_assigned_heliostat() == helio_id)
			return m;
	throw std::exception("helio id not assigned to any staff member.");
	return NULL;
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
		if (!m->m_busy)
			return m;
	throw std::exception("no members available");
	return NULL;
}