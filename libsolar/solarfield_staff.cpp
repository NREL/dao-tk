
#include <algorithm>
#include "solarfield_staff.h"


solarfield_staff_member::solarfield_staff_member()
{
	m_fraction = 1.0;
	m_hours_worked = 0.0;
	m_hours_this_week = 0.0;
	m_hours_today = 0.0;
	m_n_repairs_completed = 0;
	m_n_repairs_started = 0;
	m_helio_assigned = NULL;
}

solarfield_staff_member::solarfield_staff_member(double fraction)
{
	m_fraction = fraction;
	m_hours_worked = 0.0;
	m_hours_this_week = 0.0;
	m_hours_today = 0.0;
	m_n_repairs_completed = 0;
	m_n_repairs_started = 0;
	m_helio_assigned = NULL;
}


double solarfield_staff_member::get_time_available(double max_per_day, double max_per_week)
{
	double max_day = max_per_day;
	double max_week = m_fraction * max_per_week;
	return fmin(max_week - m_hours_this_week, max_day - m_hours_today);
}

double solarfield_staff_member::get_time_available_week(double max_per_week)
{
	double max_week = m_fraction * max_per_week;
	return max_week - m_hours_this_week;
}

double solarfield_staff_member::get_time_assigned()
{
	if (m_helio_assigned == NULL)
		return 0.0;
	else
		return m_helio_assigned->get_total_repair_time();
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

	m_queue.clear();
	m_queue_length.clear();
	m_queue_time.clear();
	m_total_repair_length = 0;
	m_total_repair_time = 0.0;
};

solarfield_repair_staff::solarfield_repair_staff(int nstaff, double max_per_day, double max_per_week, const std::vector<double> &time_fraction)
{
	m_repair_order = FAILURE_ORDER;
	m_n_staff = nstaff;
	m_max_hours_per_day = max_per_day;
	m_max_hours_per_week = max_per_week;

	for (int i = 0; i < nstaff; i++)
	{
		if (i < (int)time_fraction.size())
			add_member(time_fraction[i]);
		else
			add_member(1.0);  // assume full-time
	}

	m_queue.clear();
	m_queue_length.clear();
	m_queue_time.clear();
	m_total_repair_length = 0;
	m_total_repair_time = 0.0;
};

void solarfield_repair_staff::add_member(double fraction = 1.0)
{
	solarfield_staff_member *staff = new solarfield_staff_member(fraction);
	m_members.push_back(staff);
	return;
}



void solarfield_repair_staff::add_to_queue(solarfield_heliostat *h, WELLFiveTwelve &gen)
{
	int nq = (int)m_queue.size();
	m_total_repair_time += h->get_total_repair_time();
	m_total_repair_length += 1;

	if (nq == 0)
		m_queue.push_back(h);
	else
	{

		int j = 0;
		int jmin = 0;
		int jmax = nq;

		// Prioritize heliostats with repairs in progress?
		if (m_is_prioritize_partial_repairs)
		{
			//--- Find all heliostats in queue with repairs in progress
			while (j < nq && m_queue[j]->get_operational_state() == REPAIRING)
				j++;

			//--- Find section of queue in which heliostat h can be added
			if (h->get_operational_state() == REPAIRING)
				jmax = j;
			else
				jmin = j;
		}


		//--- Find position to add heliostat
		j = jmax;

		if (jmax > jmin)
		{
			if (m_repair_order == PERFORMANCE)  // Add in order of highest to lowest performance
			{
				double performance = h->get_performance();
				while (j > jmin && m_queue[j - 1]->get_performance() < performance)
					j--;
			}
			else if (m_repair_order == REPAIR_TIME)  // Add in order of lowest to highest repair time
			{
				double time = h->get_total_repair_time();
				while (j > jmin && m_queue[j - 1]->get_total_repair_time() > time)
					j--;
			}
			else if (m_repair_order == MEAN_REPAIR_TIME)   // Add in order of lowest to highest mean repair time
			{
				double time = h->get_mean_repair_time();
				while (j > jmin && m_queue[j - 1]->get_mean_repair_time() > time)
					j--;
			}
			else if (m_repair_order == RANDOM)	// Add at random position
			{
				j = (int)round(jmin + gen.getVariate() * (jmax-jmin));
			}
			else if (m_repair_order == PERF_OVER_MRT) // Prioritize repairs in order from largest to smallest expected rate of increased performance
			{
				double rate_inc = h->get_performance() / h->get_mean_repair_time();
				while (j > jmin && m_queue[j - 1]->get_performance() / m_queue[j - 1]->get_mean_repair_time() < rate_inc)
					j--;
			}
		}



		//--- Add heliostat
		if (j == nq)
			m_queue.push_back(h);
		else
		{
			std::vector<solarfield_heliostat *>::iterator it = m_queue.begin() + j;
			m_queue.insert(it, h);
		}
	}

	return;
}

void solarfield_repair_staff::assign_from_queue(int staff_index, int queue_index)
{
	solarfield_staff_member *st = m_members[staff_index];
	solarfield_heliostat* hel = m_queue[queue_index];
	st->m_helio_assigned = hel;
	m_queue.erase(m_queue.begin() + queue_index);
	if (hel->get_operational_state() != REPAIRING)
		st->m_n_repairs_started += 1;
	return;
}

void solarfield_repair_staff::assign_to_queue(int staff_index, WELLFiveTwelve &gen)
{
	solarfield_staff_member *st = m_members[staff_index];
	solarfield_heliostat* hel = st->m_helio_assigned;
	if (hel != NULL)
	{
		st->m_helio_assigned = NULL;
		add_to_queue(hel, gen);

		// Avoid double-counting total heliostats needing repair
		m_total_repair_length -= 1;
		m_total_repair_time -= hel->get_total_repair_time();

	}
	return;
}

void solarfield_repair_staff::repair(int staff_index, double repair_time)
{
	solarfield_staff_member *st = m_members[staff_index];
	solarfield_heliostat * hel = st->m_helio_assigned;

	if (hel != NULL)
	{
		int n_completed = hel->repair(repair_time);
		st->m_n_repairs_completed += n_completed;
		st->add_time_worked(repair_time);
		m_total_repair_time -= repair_time;

		if (hel->get_operational_state() == OPERATIONAL)
		{
			st->m_helio_assigned = NULL;
			m_total_repair_length -= 1;
		}
	}
	return;
}

void solarfield_repair_staff::queue_tracking()
{
	m_queue_length.push_back(m_total_repair_length);
	m_queue_time.push_back(m_total_repair_time);
	return;
}

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