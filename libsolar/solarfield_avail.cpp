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
	WELLFiveTwelve gen(m_settings.seed % 100);
	m_gen = &gen;
	m_staff = solarfield_repair_staff(m_settings.n_om_staff[0], m_settings.max_hours_per_day, m_settings.max_hours_per_week);
	create_helio_field(n_components, n_helio_s, problem_scale);
	initialize_results();
}

void solarfield_availability::initialize_results()
{
	int nsteps = (int)(m_settings.n_years * 8760 / m_settings.step);

	m_results.avg_avail = 0.0;
	m_results.min_avail = 1.0;

	m_results.avail_schedule.clear();
	m_results.avail_schedule.reserve(nsteps);
	m_results.yearly_avg_avail.clear();
	m_results.yearly_avg_avail.reserve(m_settings.n_years);


	m_results.n_repairs = 0;
	m_results.n_repairs_per_component.assign(m_settings.helio_components.size(), 0);
	m_results.n_failures_per_component.assign(m_settings.helio_components.size(), 0);

	m_results.staff_utilization = 0.;


	m_results.staff_time.clear();
	m_results.failures_per_year.clear();
	m_results.repair_time_per_year.clear();
	m_results.queue_size_vs_time.clear();
	m_results.queue_time_vs_time.clear();

	if (m_settings.is_tracking)
	{
		std::vector<double> zeros(m_settings.n_years, 0.0);
		for (size_t i = 0; i < m_settings.helio_components.size(); i++)
		{
			m_results.failures_per_year.push_back(zeros);
			m_results.repair_time_per_year.push_back(zeros);
		}

		m_results.queue_size_vs_time.reserve(nsteps);
		m_results.queue_time_vs_time.reserve(nsteps);
	}

}

void solarfield_availability::create_helio_field(int n_components, int n_heliostats, double scale)
{
	m_field.m_helios.reserve(n_heliostats);
	m_field.m_components.reserve(n_components);
	double sum_performance = 0.0;
	for (int c = 0; c < n_components; c++)
	{
		m_field.add_component(m_settings.helio_components.at(c));
	}

	for (int i = 0; i < n_heliostats; i++)
	{
		solarfield_heliostat *hel = new solarfield_heliostat;

		sum_performance += m_settings.helio_performance[i];

		hel->initialize(m_field.m_components, m_gen, scale, m_settings.helio_performance[i]);
		m_field.m_helios.push_back(hel);

		if (m_settings.is_tracking)
			hel->initialize_repair_time_tracking();
	}
}

std::vector<double> solarfield_availability::get_operating_hours()
{
	/* returns vectors of hours in which  */
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
		for (int h = 0; h < 24; h++)
		{
			if ((double)h < daily_sunrise[d] - 1 || (double)h > daily_sunset[d])
				op_hours[24*d+h] = 0.;
			else if ((double)h >= daily_sunrise[d] || (double)h <= daily_sunset[d]-1)
				op_hours[24*d+h] = 1.;
			else if ((double)h <= daily_sunrise[d])
				op_hours[24*d+h] = (double)h + 1 - daily_sunrise[d];
			else if ((double)h >= daily_sunset[d] - 1)
				op_hours[24*d+h] = daily_sunset[d] - (double)h;
			else
				throw std::exception("Logic broken!");
		}
	for (int y = 1; y < m_settings.n_years; y++)
		std::copy(op_hours.begin(), op_hours.begin() + 8761, op_hours.begin() + (8760 * y));
	return op_hours;
}



double solarfield_availability::get_time_of_failure(std::vector<double> operating_hours, double t_start, double op_life)
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
	if (operating_hours[idx] * (t_start + 1 - idx) > op_life)
		return t_start + op_life / (operating_hours[idx] * (t_start + 1 - idx));
	life_remaining -= (idx+1-t_start)*operating_hours[idx];
	while (idx < (int)operating_hours.size() && life_remaining > DBL_EPSILON)
	{
		idx++;
		life_remaining -= operating_hours[idx];
		if (life_remaining < -DBL_EPSILON)  //if op_life expired, subtract overage
			return (double)idx + life_remaining / operating_hours[idx] + 1;
	}
	//if finishing at the end of an hour, return that value.
	return (double)idx + 1;
}

double solarfield_availability::get_time_of_failure(double sunrise, double sunset, double t_start, double op_life)
{
	/*
	Determines the time of failure, given operating hours (i.e., daily sunrise and sunset) and operating life.
	sunrise -- daily sunrise (hour, 0-24)
	sunset -- daily sunset (hour, 0-24)
	t_start -- time period of start of operating life
	op_life -- operating lifetime (hours)
	retval -- time period of end of life
	*/
	
	double t_end = t_start*1.0;

	//add full operating days
	int num_full_days = (int)(op_life / (sunset - sunrise));
	t_end += 24 * num_full_days;
	
	//add remaining hours
	double part_day = (op_life / (sunset - sunrise)) - num_full_days;
	double hour_start = remainder(t_start, 24);
	if (hour_start > sunset)  //advance to next day, finish part day
		t_end += 24 - hour_start + sunrise + part_day;
	else if (hour_start < sunrise) // advance to sunrise, finish part day
		t_end += sunrise - hour_start + part_day;
	else if (hour_start + part_day < sunset) // add part day
		t_end += part_day;
	else // finish rest of current day, move to next sunrise for rest
		t_end += 24 - sunset + sunrise + part_day;

	return t_end;
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
				return t_end + repair_remaining;
			}
			week_labor -= day_labor;
			repair_remaining -= day_labor;
			t_end += 24;
		}
		days_left = 7;
		week_labor = staff->m_max_hours_per_week;
	}
	staff->add_time_worked(repair_time);
}

std::priority_queue<solarfield_event> solarfield_availability::create_initial_queue(std::vector<double> operating_hours)
{
	/*int _helio_id,
		bool _is_repair,
		double _time,
		double _priority*/
	int idx = 0;
	double end_time;
	std::priority_queue<solarfield_event> queue = {};
	for (solarfield_heliostat * h : m_field.m_helios)
	{
		if (m_settings.is_fix_hours)
			end_time = get_time_of_failure(
				m_settings.sunrise,
				m_settings.sunset,
				0.,
				h->get_op_time_to_next_failure()
			);
		else
			end_time = get_time_of_failure(
				m_settings.sunrise,
				m_settings.sunset,
				0.,
				h->get_op_time_to_next_failure()
			);
		queue.push(solarfield_event(
			idx,
			m_field.m_helios.at(idx)->get_next_component_to_fail(),
			false,
			end_time,
			1. / end_time
		));
		idx++;
	}
	return queue;
}

void solarfield_availability::process_failure(double t_last)
{
	/* 
	Processes a failure by changing the current field availability
	and send generating a repair event if a staff member is availabile,
	or sending an event to the repair queue otherwise.
	
	t_last -- time of last event [h]
	*/
	m_current_availability -= m_settings.helio_performance[m_current_event.helio_id];
	m_field.m_helios.at(m_current_event.helio_id)->fail(m_current_event.time, m_gen);
	if (m_staff.is_staff_available())
	{
		solarfield_staff_member* staff = m_staff.get_available_staff();
		double end_time = get_time_of_repair(
			t_last,
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
	}
	else
		add_repair_to_queue();
}

void solarfield_availability::process_repair(double t_last)
{
	/*
	Processes a repair by changing the current field availability
	and assigning a staff member to a new repair, if the repair
	queue is not empty.

	t_last -- time of last event [h]
	*/
	m_current_availability += m_settings.helio_performance[m_current_event.helio_id];


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
	update_availability(t_last, m_current_event.time);
	if (m_current_event.is_repair)
		process_repair(t_last);
	else
		process_failure(t_last);
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
}

void solarfield_availability::update_availability(double t_start, double t_end)
{
	int idx = (int)t_start;
	m_results.avail_schedule[idx] += (idx + 1 - t_start)*m_current_availability;
	while (idx < (int)(t_end))
	{
		idx++;
		m_results.avail_schedule[idx] += m_current_availability;
	}
	m_results.avail_schedule[int(t_end)] += (t_end - (int)t_end)*m_current_availability;

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
	
	std::vector<double> op_schedule = get_operating_hours();
	
	double t = 0;
	while (t < (double)nhours)
	{
		m_current_event = m_event_queue.top();
		run_current_event(t);
		t = m_current_event.time;
		m_event_queue.pop();
	}

	//----------------------------------------------------------
		
	//double n_this_year = 0.0;
	//double yearly_avail = 0.0;
	//double total_time_this_year = 0.0;


	/*
	for (int t = 0; t < nsteps; t++)
	{

		if (t / float(update_per) == t / update_per && callback != 0)
		{
			if (!callback((float)t / (float)nsteps, "Simulating solarfield availability"))
				return;
		}


		double start_time = t * ts;
		double end_time = (t + 1) * ts;

		int y = (int)floor(start_time / 8760.);				// year		

		double day_start = floor(start_time / 24.);			// day at start of time step
		double day_end = floor((end_time - 0.001) / 24.);	// day at end of time step
		int doy = (int)day_start - y * 365;					// day of year at start of time step

		double hod_start = start_time - day_start * 24;		// hour of day at start of time step
		double hod_end = end_time - day_end * 24;			// hour of day at end of time step

		double wk_start = floor(start_time / (24.*7.));		// week at start of time step

		double how_start = start_time - wk_start * 24 * 7;	// hour of week at start of time step
		double hoy_start = start_time - y * 8760;			// hour of year at start of time step


		//--- Reset yearly tracking and update staff for this year
		if (hoy_start < ts)
		{
			n_this_year = 0.0;
			yearly_avail = 0.0;
			total_time_this_year = 0.0;

			std::vector<double> time_fraction(n_staff[y], 1.0);
			if ((double)n_staff[y] != m_settings.n_om_staff[y])
				time_fraction[n_staff[y] - 1] = m_settings.n_om_staff[y] - floor(m_settings.n_om_staff[y]);
			staff.reset_time_fractions(time_fraction);

		}

		//--- Calculate operating time during this timestep
		double sunrise = 1;//daily_sunrise[doy];
		double sunset = 2;//daily_sunset[doy];
		double operating_time = operating_hours(sunrise, sunset, day_start, day_end, hod_start, hod_end);


		//--- Reset hours per day and hours per week for staff
		if (hod_start < ts && ts <= 24.0) // First timestep today, note staff hours per day not used for timesteps > 24 hours
		{
			for (int s = 0; s < n_staff[y]; s++)
				staff.m_members[s]->m_hours_today = 0.0;
		}
		if (how_start < ts && ts <= 168.0) // First timestep this week, note staff hours per week not used for timesteps > 168 hours
		{
			for (int s = 0; s < n_staff[y]; s++)
				staff.m_members[s]->m_hours_this_week = 0.0;
		}


		//--- Try to operate all currently operational heliostats
		double nop = 0;
		double op_performance = 0.0;

		for (int h = 0; h < n_helio_s; h++)
		{
			hel = field.m_helios[h];
			unsigned int state = hel->get_operational_state();

			if (state == OPERATIONAL)
			{
				bool is_operational = true;

				if (operating_time > 0.0)
				{

					if (hel->get_time_to_next_failure() > operating_time) // No failures
						hel->operate(operating_time);

					else   // At least one component fails during this time step
					{
						is_operational = false;
						hel->fail(operating_time, rand);
						staff.add_to_queue(hel, rand);

						if (m_settings.is_tracking)
						{
							failed_components = hel->get_failed_components();
							for (size_t c = 0; c < failed_components.size(); c++)
								m_results.failures_per_year[failed_components[c]][y] += 1 * problem_scale;
						}
					}

				}

				if (is_operational)
				{
					nop += 1;
					op_performance += hel->get_performance();
				}

			}
		}



		//--- Find staff available for repairs in this time step
		std::vector<double> staff_time_avail(n_staff[y]);
		staff_avail.clear();
		staff_not_avail.clear();
		for (int s = 0; s < n_staff[y]; s++)
		{
			if (ts <= 24.)
				staff_time_avail[s] = fmin(ts, staff.m_members[s]->get_time_available(staff.m_max_hours_per_day, staff.m_max_hours_per_week));  // daily and weekly limits

			else if (ts <= 168.)
			{
				double time_avail = fmin(ts, (ts / 24.)*staff.m_max_hours_per_day); // daily limits
				staff_time_avail[s] = fmin(time_avail, staff.m_members[s]->get_time_available_week(staff.m_max_hours_per_week));  // weekly limits
			}

			else
			{
				double f = staff.m_members[s]->m_fraction;
				staff_time_avail[s] = fmin(ts, fmin((ts / 24.)*staff.m_max_hours_per_day, (ts / 168.)*f*staff.m_max_hours_per_week));
			}

			if (staff_time_avail[s] > 0.0)
				staff_avail.push_back(s);
			else
				staff_not_avail.push_back(s);
		}



		//--- Is trading allowed? 
		bool is_trading_allowed = false;
		if (m_settings.is_trade_repairs && (int)staff_not_avail.size() > 0)
		{
			int si = 0;
			while (si < (int)staff_avail.size() && !is_trading_allowed)
			{
				double extra_time = staff_time_avail[staff_avail[si]] - staff.m_members[staff_avail[si]]->get_time_assigned();
				if (extra_time > 0.0)
					is_trading_allowed = true;
				si++;
			}
		}

		//--- Account for unavailable staff
		if (is_trading_allowed)
		{
			for (size_t si = 0; si < staff_not_avail.size(); si++)
				staff.assign_to_queue(staff_not_avail[si], rand);  // return partially repaired heliostats to queue
		}


		//--- Track queue length before repairs
		if (m_settings.is_tracking)
			staff.queue_tracking();


		//--- Carry out repairs
		for (size_t si = 0; si < staff_avail.size(); si++)
		{
			int s = staff_avail[si];
			solarfield_staff_member *st = staff.m_members[s];

			double timestep_remain = ts;
			double time_remain = staff_time_avail[s];
			double time_worked = 0.0;
			bool stop = false;

			while (time_remain > 0.0 && !stop)
			{

				// Try to get an assigned heliostat
				hel = st->m_helio_assigned;
				if (hel == NULL) // No current assignment 
				{
					if (staff.m_queue.size() == 0)  // Nothing in queue
						stop = true;
					else  // Take next heliostat
					{
						staff.assign_from_queue(s, 0);
						hel = st->m_helio_assigned;
					}
				}


				// Repair assigned hleiostat
				if (hel != NULL)
				{
					// Identify failed components
					if (m_settings.is_tracking)
						failed_components = hel->get_failed_components();

					// Do repairs
					double repair_time = fmin(time_remain, hel->get_total_repair_time());
					staff.repair(s, repair_time);

					total_time_this_year += repair_time;
					time_worked += repair_time;

					time_remain -= repair_time;
					timestep_remain -= repair_time;

					// Track component repair time per month
					if (m_settings.is_tracking)
					{
						if (failed_components.size() == 1)   // All repair time went to one component
							m_results.repair_time_per_year[failed_components[0]][y] += repair_time;
						else  // Multiple components were repaired
						{
							std::vector<double> *rt = hel->get_repair_time_tracking();
							for (size_t ci = 0; ci < failed_components.size(); ci++)
							{
								int c = failed_components[ci];
								m_results.repair_time_per_year[c][y] += (*rt)[c];
							}
						}
					}



					// Put fully repaired heliostat back in operation (approximate) -> Note components can only fail once per timestep... second failures that "should" happen will occur during the next timestep
					if (m_settings.is_split_operation && hel->get_operational_state() == OPERATIONAL)
					{
						double now = end_time - timestep_remain;  // Current time 

						if (operating_time > 0)
						{
							double op_time_remain = 0.0;
							if (operating_time == ts)
								op_time_remain = timestep_remain;
							else
							{
								double day_now = floor(now / 24.);
								double hod_now = now - day_now * 24.;
								op_time_remain = operating_hours(sunrise, sunset, day_now, day_end, hod_now, hod_end);
							}

							hel->operate(op_time_remain);
						}

						nop += (end_time - now) / ts;
						op_performance += hel->get_performance() * (end_time - now) / ts;
					}

				}
			}


			// Put assigned heliostat back in queue if multiple staff are allowed to work on single helio
			if (m_settings.is_allow_multiple_per_helio)
				staff.assign_to_queue(s, rand);

		}


		//--- Track availability in this time step
		double current_avail = op_performance / sum_performance;  // Performance-weighted average availability
		m_results.avg_avail += current_avail / (double)nsteps;
		if (current_avail < m_results.min_avail)
			m_results.min_avail = current_avail;
		m_results.avail_schedule.push_back(current_avail);
		n_this_year += 1.0;
		yearly_avail += m_results.avail_schedule.back();


		//--- Save yearly-average results
		if (hoy_start + ts > 8759.99 || t == nsteps - 1)
			m_results.yearly_avg_avail.push_back(yearly_avail / n_this_year);
	}
	*/

	/*
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

	*/

	/*
	// Time per staff member and queue size
	if (m_settings.is_tracking)
	{
		for (size_t s = 0; s < staff.m_members.size(); s++)
			m_results.staff_time.push_back(staff.m_members[s]->m_hours_worked);
		
		for (size_t i = 0; i < staff.m_queue_length.size(); i++)
		{
			m_results.queue_size_vs_time.push_back((double)staff.m_queue_length[i] / (double)n_helio_s);
			m_results.queue_time_vs_time.push_back(staff.m_queue_time[i]);
		}
	}
	*/


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
			ofs << ",queue_fraction,queue_time";
		ofs << "\n";

		//hourly data
		for (int t = 0; t<nhours; t++)
		{
			ofs << t << "," << m_results.avail_schedule.at(t);
			if (m_settings.is_tracking)
				ofs << "," << m_results.queue_size_vs_time[t] << "," << m_results.queue_time_vs_time[t];
			ofs << "\n";
		}
		ofs.close();
	}


	m_sim_available = true;
}

