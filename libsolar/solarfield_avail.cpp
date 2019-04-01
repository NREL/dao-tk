#include "solarfield_structures.h"
#include "solarfield_avail.h"
#include "./../libcycle/distributions.h"

#include <random>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>

#include <iostream>
#include <fstream>

solarfield_availability::solarfield_availability()
{
	//initialize
	m_sim_available = false;
}

/*
double solarfield_availability::operating_hours(std::vector<double> operating_hours, double t_start, double t_end)
{
	// Operating hours between start/end points provided as input
	double op_time = operating_hours[(int)t_start] * ((int)t_start - t_start + 1.);
	if ((size_t)t_end != operating_hours.size())
		op_time += operating_hours[(int)t_end] * ((int)t_start - t_start + 1.);
	for (int t = (int)t_start + 1; t < (int)t_end; t++)
		op_time += operating_hours[t];
	return op_time;
}
*/

void solarfield_availability::initialize()
{
	double hscale = (double)m_settings.n_helio_sim;
	double problem_scale = (double)m_settings.n_helio / hscale;
	int n_helio_s = (int)hscale;
	int n_components = (int)m_settings.helio_components.size();
	create_om_staff(m_settings.n_om_staff, m_settings.max_hours_per_day, m_settings.max_hours_per_week);
	get_operating_hours();
	create_helio_field(n_components, n_helio_s, problem_scale);
	initialize_results();
	m_repair_queue_length = 0;
	m_current_availability = 1.0;
}

void solarfield_availability::initialize_results()
{
	int nsteps = (int)(m_settings.n_years * 8760);

	m_results.avg_avail = 0.0;
	m_results.min_avail = 1.0;

	m_results.avail_schedule.clear();
	m_results.avail_schedule.assign(nsteps,0);
	m_results.yearly_avg_avail.clear();
	m_results.yearly_avg_avail.assign(m_settings.n_years,0);


	m_results.n_repairs = 0;
	m_results.n_repairs_per_component.assign(m_settings.helio_components.size(), 0);
	m_results.n_failures_per_component.assign(m_settings.helio_components.size(), 0);
	m_results.repair_cost_per_year.assign(m_settings.n_years, 0);

	m_results.staff_utilization = 0.;


	m_results.staff_time.clear();
	m_results.failures_per_year.clear();
	//m_results.repair_time_per_year.clear();
	m_results.queue_size_vs_time.clear();
	m_results.queue_size_vs_time.assign(nsteps, 0);
	//m_results.queue_time_vs_time.clear();

	if (m_settings.is_tracking)
	{
		std::vector<double> zeros(m_settings.n_years, 0.0);
		for (size_t i = 0; i < m_settings.helio_components.size(); i++)
		{
			m_results.failures_per_year.push_back(zeros);
			m_results.repair_time_per_year.push_back(zeros);
		}

		m_results.queue_size_vs_time.reserve(nsteps);
		//m_results.queue_time_vs_time.reserve(nsteps);
	}

}

void solarfield_availability::create_om_staff(int n_staff, double max_hours_per_day, double max_hours_per_week)
{
	m_staff = solarfield_repair_staff();
	m_staff.m_max_hours_per_day = max_hours_per_day;
	m_staff.m_max_hours_per_week = max_hours_per_week;
	m_staff.m_n_staff = n_staff;
	for (int m = 0; m < n_staff; m++)
	{
		solarfield_staff_member *staff = new solarfield_staff_member();
		staff->initialize(max_hours_per_day, max_hours_per_week);
		m_staff.m_members.push_back(staff);
	}
}

void solarfield_availability::create_helio_field(int n_components, int n_heliostats, double scale)
{
	m_field.m_components.clear();
	m_field.m_helios.clear();
	m_field.m_helios.reserve(n_heliostats);
	m_field.m_components.reserve(n_components);
	m_settings.sum_performance = 0.0;
	for (int c = 0; c < n_components; c++)
	{
		m_field.add_component(m_settings.helio_components.at(c));
	}

	for (int i = 0; i < n_heliostats; i++)
	{
		solarfield_heliostat *hel = new solarfield_heliostat;

		m_settings.sum_performance += m_settings.helio_performance[i];

		hel->initialize(m_field.m_components, *m_gen, scale, m_settings.helio_performance[i]);
		m_field.m_helios.push_back(hel);

		if (m_settings.is_tracking)
			hel->initialize_repair_time_tracking();
	}
}

void solarfield_availability::assign_generator(WELLFiveTwelve &gen)
{
	m_gen = &gen;
}

void solarfield_availability::get_operating_hours()
{
	/* 
	Determines the operating schedule based on the daily sunrise and sunset 
	assumed for the location. The result is stored in m_settings.op_schedule.
	Assumes that the heliostats operate from sunrise to sunset.
	*/
	std::vector<double> daily_sunrise, daily_sunset, op_hours;
	if (m_settings.is_fix_hours)
	{
		daily_sunrise.assign(365, m_settings.sunrise);
		daily_sunset.assign(365, m_settings.sunset);
	}
	else
	{
		clearsky csky(m_settings.location);
		csky.calculate_sunrise_sunset();
		daily_sunrise = csky.m_sunrise;
		daily_sunset = csky.m_sunset;
	}
	op_hours.reserve(8760 * m_settings.n_years);
	for (int d = 0; d < 365; d++)
	{
		for (int h = 0; h < 24; h++)
		{
			if ((double)h < daily_sunrise[d] - 1 || (double)h > daily_sunset[d])
				op_hours.push_back(0.);
			else if ((double)h >= daily_sunrise[d] && (double)h <= daily_sunset[d] - 1)
				op_hours.push_back(1.);
			else if ((double)h <= daily_sunrise[d])
				op_hours.push_back((double)h + 1 - daily_sunrise[d]);
			else if ((double)h >= daily_sunset[d] - 1)
				op_hours.push_back(daily_sunset[d] - (double)h);
			else
				throw std::exception("Logic broken!");
		}
	}
	m_settings.op_schedule.clear();
	m_settings.op_schedule.reserve(m_settings.n_years*m_settings.op_schedule.size());
	for (int y = 0; y < m_settings.n_years; y++)
	{
		m_settings.op_schedule.insert(m_settings.op_schedule.end(), op_hours.begin(), op_hours.end());
	}
}



double solarfield_availability::get_time_of_failure(double t_start, double op_life)
{
	/*
	Determines the time of failure, given operating hours (i.e., daily sunrise and sunset) and operating life.
	sunrise -- daily sunrise (hour, 0-24)
	sunset -- daily sunset (hour, 0-24)
	t_start -- time period of start of operating life
	op_life -- operating lifetime (hours)
	retval -- time period of end of life
	*/
	
	double life_remaining = op_life;
	int idx = int(t_start);
	//check for end of op_life durign the current hour
	if (m_settings.op_schedule[idx] * (t_start + 1 - idx) > op_life)
		return t_start + op_life / (m_settings.op_schedule[idx] * (t_start + 1 - idx));
	life_remaining -= (idx+1-t_start)*m_settings.op_schedule[idx];
	while (idx < (int)m_settings.op_schedule.size()-1 && life_remaining > DBL_EPSILON)
	{
		idx++;
		life_remaining -= m_settings.op_schedule[idx];
		if (life_remaining < -DBL_EPSILON)  //if op_life expired, subtract overage
			return (double)idx + life_remaining / m_settings.op_schedule[idx] + 1;
	}
	//if finishing at the end of an hour, return that value.
	return (double)idx + 1;
}

double solarfield_availability::get_time_of_repair(double t_start, double repair_time, solarfield_staff_member* staff)
/*
Assigns a repair event to a free staff member, calculates the time of repair 
completion, and updates the staff member's daily and weekly usage statistics 
through the end of the repair event.

t_start -- start time of repair event
repair_time -- time of repair completion
staff -- staff member (include labor stats)
*/
{
	double t_end = t_start * 1.0;
	//try to finish today
	double hours_left_today = 24 - remainder(t_start, 24.);
	if (repair_time <= fmin(staff->get_time_available(), hours_left_today))
	{
		staff->m_hours_this_week += repair_time;
		staff->m_hours_today += repair_time;
		return t_start += repair_time;
	}
	//advance through future days, updating daily and weekly labor as required.
	//stop when the available working hours is greater than the remaining 
	//repair time.
	t_end += hours_left_today;
	double repair_remaining = repair_time - fmin(staff->get_time_available(), hours_left_today);
	double week_labor = staff->get_time_available_week() - fmin(staff->get_time_available(), hours_left_today);
	double day_labor = fmin(week_labor, staff->m_max_hours_per_day);
	int days_left = (int)remainder(t_end, 168) / 24;
	while (true)
	{
		for (int i = 0; i < days_left; i++)
		{
			day_labor = fmin(week_labor, staff->m_max_hours_per_day);
			if (repair_remaining < fmin(week_labor, staff->m_max_hours_per_day))
			{
				staff->m_hours_this_week = staff->m_max_hours_per_week - week_labor + repair_remaining;
				staff->m_hours_today = repair_remaining;
				staff->add_time_worked(repair_time);
				return t_end + repair_remaining;
			}
			week_labor -= day_labor;
			repair_remaining -= day_labor;
			t_end += 24;
		}
		days_left = 7;
		week_labor = staff->m_max_hours_per_week;
	}
}

std::priority_queue<solarfield_event> solarfield_availability::create_initial_queue(std::vector<double> operating_hours)
{
	/*int _helio_id,
		bool _is_repair,
		double _time,
		double _priority*/
	double end_time;
	std::priority_queue<solarfield_event> queue = {};
	for (int idx=0; idx<m_field.m_helios.size(); idx++)
	{
		end_time = get_time_of_failure(
				0.,
			m_field.m_helios.at(idx)->get_op_time_to_next_failure()
			);
		queue.push(solarfield_event(
			idx,
			m_field.m_helios.at(idx)->get_next_component_to_fail(),
			false,
			end_time,
			1. / end_time
		));
	}
	return queue;
}

void solarfield_availability::process_failure()
{
	/* 
	Processes a failure by changing the current field availability
	and send generating a repair event if a staff member is availabile,
	or sending an event to the repair queue otherwise.
	
	t_last -- time of last event [h]
	*/
	m_current_availability -= m_settings.helio_performance[m_current_event.helio_id] / m_settings.sum_performance;
	if (m_results.min_avail > m_current_availability)
		m_results.min_avail = m_current_availability;
	m_results.n_failures_per_component[m_current_event.component_idx] += 1;
	m_field.m_helios.at(m_current_event.helio_id)->fail(m_current_event.time, *m_gen);
	if (m_staff.is_staff_available())
	{
		solarfield_staff_member* staff = m_staff.get_available_staff();
		staff->assign_heliostat(m_current_event.helio_id);
		double end_time = get_time_of_repair(
			m_current_event.time,
			m_field.m_helios.at(m_current_event.helio_id)->get_repair_time(),
			staff
		);
		m_event_queue.push(
			solarfield_event(
				m_current_event.helio_id,
				m_current_event.component_idx,
				true,
				end_time,
				1. / end_time
			)
		);
		m_field.m_helios.at(m_current_event.helio_id)->start_repair(m_current_event.time);
	}
	else
	{
		add_repair_to_queue();
	}
}

void solarfield_availability::process_repair()
{
	/*
	Processes a repair by changing the current field availability
	and assigning a staff member to a new repair, if the repair
	queue is not empty.

	t_last -- time of last event [h]
	*/
	m_current_availability += m_settings.helio_performance[m_current_event.helio_id] / m_settings.sum_performance;
	m_results.n_repairs_per_component[m_current_event.component_idx] += 1;
	m_results.repair_cost_per_year[int((m_current_event.time-DBL_EPSILON) / 8760)] += m_field.m_components.at(m_current_event.component_idx)->get_repair_cost();
	m_field.m_helios.at(m_current_event.helio_id)->end_repair(
		m_current_event.time,
		m_current_event.component_idx
	);
	solarfield_staff_member* staff = m_staff.get_assigned_member(m_current_event.helio_id);

	solarfield_heliostat* hel = m_field.m_helios.at(m_current_event.helio_id);
	hel->update_failure_time();
	double fail_time = get_time_of_failure(m_current_event.time, hel->get_op_time_to_next_failure());
	m_event_queue.push(
		solarfield_event(
			m_current_event.helio_id,
			hel->get_next_component_to_fail(),
			false,
			fail_time,
			1. / fail_time
		)
	);

	if (m_repair_queue_length > 0)
	{
		//new repair assignment
		solarfield_event e = m_repair_queue.top();
		staff->assign_heliostat(e.helio_id);
		double end_time = get_time_of_repair(
			m_current_event.time,
			m_field.m_helios.at(e.helio_id)->get_repair_time(),
			staff
		);
		m_event_queue.push(
			solarfield_event(
				e.helio_id,
				e.component_idx,
				true,
				end_time,
				1. / end_time
			)
		);
		m_field.m_helios.at(e.helio_id)->start_repair(m_current_event.time);
		m_repair_queue_length -= 1;
		m_repair_queue.pop();
	}
	else
	{
		staff->free();
	}
}

void solarfield_availability::run_current_event(double t_last)
{
	/* 
	Processes the current event in the event queue, by 
	updating the field availability from the last event time
	to the current event time, then processing either a failure
	or repair.
	
	t_last -- time of last event [h]
	*/
	update_statistics(t_last, m_current_event.time);
	if (m_current_event.is_repair)
		process_repair();
	else
		process_failure();
}

void solarfield_availability::add_repair_to_queue()
{
	/*
	Adds the failure in the current event to the repair queue.  
	*/
	double repair_priority;
	if (m_settings.repair_order == FAILURE_ORDER)
		repair_priority = 1. / m_current_event.time;
	else if (m_settings.repair_order == RANDOM)
		repair_priority = m_gen->getVariate();
	else if (m_settings.repair_order == PERFORMANCE)
		repair_priority = m_field.m_helios.at(m_current_event.helio_id)->get_performance();
	else if (m_settings.repair_order == REPAIR_TIME)
		repair_priority = 1. / m_field.m_helios.at(m_current_event.helio_id)->get_repair_time();
	else if (m_settings.repair_order == MEAN_REPAIR_TIME)
		repair_priority = 1. / m_field.m_components.at(m_current_event.component_idx)->get_mean_repair_time();
	else if (m_settings.repair_order == PERF_OVER_MRT)
		repair_priority = (
			m_field.m_helios.at(m_current_event.helio_id)->get_performance() /
			m_field.m_components.at(m_current_event.component_idx)->get_mean_repair_time()
			);
	else
		throw std::exception("invalid repair order");
	m_repair_queue.push( solarfield_event(
		m_current_event.helio_id,
		m_current_event.component_idx,
		true,
		m_current_event.time,
		repair_priority
	));
	m_repair_queue_length += 1;
}

void solarfield_availability::update_statistics(double t_start, double t_end)
{
	/* 
	Updates time-series summary statistics between t_start and t_end. 
	*/
	int idx = (int)t_start;

	//if start and end occur in same timestep, just update one index of results
	//and terminate.
	if (idx == int(t_end))
	{
		m_results.avail_schedule[idx] += (t_end - t_start)*m_current_availability;
		m_results.queue_size_vs_time[idx] += (t_end - t_start)*m_repair_queue_length;
		return;
	}

	//otherwise, update the results at (i) the timestep containing t_start, 
	// (ii) all those timesteps falling between t_start and t_end, and, 
	// (iii) , the timestep containing t_end.
	m_results.avail_schedule[idx] += (idx + 1 - t_start)*m_current_availability;
	m_results.queue_size_vs_time[idx] += (idx + 1 - t_start)*m_repair_queue_length;
	while (idx < (int)(t_end)-1 && idx < m_settings.n_years * 8760 -1)
	{
		idx++;
		m_results.avail_schedule[idx] += m_current_availability;
		m_results.queue_size_vs_time[idx] += m_repair_queue_length;
	}

	//only update the timestep containing t_end if t_end is not at the 
	//boundary between two time periods.
	if (t_end - int(t_end) > DBL_EPSILON)
	{
		m_results.avail_schedule[int(t_end)] += (t_end - (int)t_end)*m_current_availability;
		m_results.queue_size_vs_time[int(t_end)] += (t_end - (int)t_end)*m_repair_queue_length;
	}
}



void solarfield_availability::simulate(bool(*callback)(float prg, const char *msg), std::string *results_file_name)
{
	double hscale = (double)m_settings.n_helio_sim;
	double problem_scale = (double)m_settings.n_helio / hscale;
	int n_helio_s = (int)hscale;
	int nhours = m_settings.n_years * 8760;
	int n_components = (int)m_settings.helio_components.size();

	//--- Initialize results
	initialize();
	m_event_queue = create_initial_queue();
	m_repair_queue = {};	
	
	double t = 0.;
	while (t < (double)nhours)
	{
		m_current_event = m_event_queue.top();
		//m_current_event.print();
		run_current_event(t);
		t = m_current_event.time;
		m_event_queue.pop();
	}

	//----------------------------------------------------------
		
	//double n_this_year = 0.0;
	//double yearly_avail = 0.0;
	//double total_time_this_year = 0.0;

	//availability stats
	m_results.yearly_avg_avail.assign(m_settings.n_years,0.);
	m_results.avg_avail = 0.;
	for (int y = 0; y < m_settings.n_years; y++)
	{
		m_results.yearly_avg_avail[y] = 0.;
		for (int h = 0; h < 8760; h++)
		{
			m_results.yearly_avg_avail[y] += m_results.avail_schedule[y * 8760 + h];
			m_results.avg_avail += m_results.avail_schedule[y * 8760 + h];
		}
		m_results.yearly_avg_avail[y] /= 8760;
	}
	m_results.avg_avail /= (8760 * m_settings.n_years);
	
	//fill in the return data
	double hours_worked = 0.;
	double repairs_made = 0;
	double max_hours = 0.;
	for (size_t s = 0; s < m_staff.m_members.size(); s++)
	{
		hours_worked += m_staff.m_members[s]->m_hours_worked;
		repairs_made += m_staff.m_members[s]->m_n_repairs_completed*problem_scale;
		max_hours += m_staff.m_members[s]->m_fraction * m_settings.n_years * m_settings.max_hours_per_week * (8760. / 168.);
	}

	m_results.n_repairs = repairs_made;
	m_results.staff_utilization = hours_worked / max_hours;

	solarfield_heliostat* hel;

	//--- Total failures and repairs per component
	for (int h = 0; h < n_helio_s; h++)
	{
		hel = m_field.m_helios[h];

		for (int c = 0; c < n_components; c++)
		{
			m_results.n_repairs_per_component[c] += hel->get_repairs_per_component()[c] * problem_scale;
			m_results.n_failures_per_component[c] += hel->get_failures_per_component()[c] * problem_scale;
		}
	}

	// Time per staff member
	if (m_settings.is_tracking)
	{
		for (size_t s = 0; s < m_staff.m_members.size(); s++)
			m_results.staff_time.push_back(m_staff.m_members[s]->m_hours_worked);
	}
	


	//if a file name is provided, write to that file
	if (results_file_name != 0)
	{
		//write the files
		std::ofstream ofs;
		ofs.open(*results_file_name, std::ofstream::out);

		ofs.clear();

		//-- staff summary
		//header
		ofs << " ";
		for (size_t s = 0; s< m_staff.m_members.size(); s++)
			ofs << ",staff_" << s;
		ofs << "\n";

		//summary
		ofs << "hrs_worked";
		for (size_t s = 0; s< m_staff.m_members.size(); s++)
			ofs << "," << m_staff.m_members.at(s)->m_hours_worked;
		ofs << "\nrepairs";
		for (size_t s = 0; s< m_staff.m_members.size(); s++)
			ofs << "," << m_staff.m_members.at(s)->m_n_repairs_completed*problem_scale;
		ofs << "\n";
		ofs << "\n";

		//--failures and repairs per component
		ofs << " ";
		for (size_t c = 0; c< (size_t)n_components; c++)
			ofs << ",component_" << c;
		ofs << "\nfailures";
		for (size_t c = 0; c < (size_t)n_components; c++)
			ofs << "," << m_results.n_failures_per_component[c];
		ofs << "\nrepairs";
		for (size_t c = 0; c < (size_t)n_components; c++)
			ofs << "," << m_results.n_repairs_per_component[c];
		ofs << "\n";
		ofs << "\n";


		//-- yearly-average results
		ofs << "year,avg_avail";
		if (m_settings.is_tracking)
		{
			for (size_t c = 0; c < (size_t)n_components; c++)
				ofs << ",component_" << c << "_failures";
			for (size_t c = 0; c < (size_t)n_components; c++)
				ofs << ",component_" << c << "_repair_time";
		}
		ofs << "\n";

		for (int y = 0; y < m_settings.n_years; y++)
		{
			ofs << y << "," << m_results.yearly_avg_avail[y];
			if (m_settings.is_tracking)
			{
				for (size_t c = 0; c < (size_t)n_components; c++)
					ofs << "," << m_results.failures_per_year[c][y];
				for (size_t c = 0; c < (size_t)n_components; c++)
					ofs << "," << m_results.repair_time_per_year[c][y];
			}
			ofs << "\n";
		}
		ofs << "\n";


		//-- hourly availability and queue length/time
		// header
		ofs << "hour,avail";
		if (m_settings.is_tracking)
			ofs << ",queue_fraction";
		ofs << "\n";

		//hourly data
		for (int t = 0; t<nhours; t++)
		{
			ofs << t << "," << m_results.avail_schedule.at(t);
			if (m_settings.is_tracking)
				ofs << "," << m_results.queue_size_vs_time[t]; //<< "," << m_results.queue_time_vs_time[t];
			ofs << "\n";
		}
		ofs.close();
	}


	m_sim_available = true;
}

