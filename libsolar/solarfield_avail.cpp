#include "solarfield_structures.h"
#include "solarfield_avail.h"

#include <random>
#include <vector>
#include <algorithm>
#include <string>

#include <iostream>
#include <fstream>

solarfield_availability::solarfield_availability()
{
	//initialize
	m_sim_available = false;
}


double solarfield_availability::operating_hours(double sunrise, double sunset, double day_start, double day_end, double hod_start, double hod_end)
{
	// Operating hours between start/end points designated by day_start/hod_start and day_end/hod_end
	double op_time;
	if (day_start == day_end)
		op_time = fmax(0.0, fmin(sunset, hod_end) - fmax(sunrise, hod_start));
	else
	{
		op_time = fmax(0.0, day_end - day_start - 1) * (sunset - sunrise);  // Days completely contained in timestep
		op_time += fmax(0.0, sunset - fmax(sunrise, hod_start));
		op_time += fmax(0.0, fmin(sunset, hod_end) - sunrise);
	}
	return op_time;
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


void solarfield_availability::simulate(bool(*callback)(float prg, const char *msg), std::string *results_file_name)
{

	//set up problem scaling
	//always scale the problem, ensures sampling repeatability
	double hscale = (double)m_settings.n_helio_sim;
	double problem_scale = (float)m_settings.n_helio / hscale;
	int n_helio_s = (int)hscale;

	double ts = m_settings.step;

	int nhours = m_settings.n_years * 8760;
	int nsteps = (int)(nhours / ts);

	int n_components = (int)m_settings.helio_components.size();

	std::default_random_engine rand(m_settings.seed);

	if (m_settings.is_allow_multiple_per_helio)
		m_settings.is_trade_repairs = false;	

	
	//--- Set integer # staff per year
	std::vector<int> n_staff;
	int n_staff_max = 0;
	for (size_t y = 0; y < m_settings.n_om_staff.size(); y++)
	{
		n_staff.push_back((int)ceil(m_settings.n_om_staff[y]));
		n_staff_max = std::max(n_staff_max, n_staff[y]);
	}

	//--- Initialize results
	initialize_results();


	//--- Randomly sample heliostats if number simulated != number in field
	std::vector<int> select;
	select.reserve(m_settings.n_helio);
	if (n_helio_s != m_settings.n_helio)
	{
		for (int h = 0; h < m_settings.n_helio; h++)
			select.push_back(h);
		std::random_shuffle(select.begin(), select.end());
	}


	//--- Initialize heliostats
	heliostat_field field;
	field.m_helios.reserve(n_helio_s);
	double sum_performance = 0.0;
	for (int h = 0; h < n_helio_s; h++)
	{
		solarfield_heliostat *hel = new solarfield_heliostat;

		int i = h;
		if (n_helio_s < m_settings.n_helio)
			i = select[h];
		else if (n_helio_s > m_settings.n_helio)
			i = select[h % m_settings.n_helio];

		sum_performance += m_settings.helio_performance[i];

		hel->initialize(m_settings.helio_components, rand, problem_scale, m_settings.helio_performance[i]);
		field.m_helios.push_back(hel);

		if (m_settings.is_tracking)
			hel->initialize_repair_time_tracking();
	}

	//--- Initialize repair staff 
	std::vector<double> staff_fraction(n_staff_max, 1.0);
	solarfield_repair_staff staff(n_staff_max, m_settings.max_hours_per_day, m_settings.max_hours_per_week, staff_fraction);
	staff.m_queue.reserve(n_helio_s);
	staff.m_repair_order = m_settings.repair_order;
	staff.m_is_prioritize_partial_repairs = m_settings.is_prioritize_partial_repairs;


	//--- Set daily operating hours
	std::vector<double> daily_sunrise, daily_sunset;
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




	//----------------------------------------------------------
	std::vector<int> staff_avail, staff_not_avail, failed_components;
	solarfield_heliostat *hel;
	double n_this_year = 0.0;
	double yearly_avail = 0.0;
	double total_time_this_year = 0.0;

	int update_per = (int)(nsteps / 50.);

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
		double sunrise = daily_sunrise[doy];
		double sunset = daily_sunset[doy];
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
					// Identify failed components and account for time waiting if other heliostats were repaired first
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



	//fill in the return data
	double hours_worked = 0.;
	double repairs_made = 0;
	double max_hours = 0.;
	for (size_t s = 0; s < staff.m_members.size(); s++)
	{
		hours_worked += staff.m_members[s]->m_hours_worked;
		repairs_made += staff.m_members[s]->m_n_repairs_completed*problem_scale;
		max_hours += staff.m_members[s]->m_fraction * m_settings.n_years * m_settings.max_hours_per_week * (8760. / 168.);
	}

	m_results.n_repairs = repairs_made;
	m_results.staff_utilization = hours_worked / max_hours;


	//--- Total failures and repairs per component
	for (int h = 0; h < n_helio_s; h++)
	{
		hel = field.m_helios[h];

		for (int c = 0; c < n_components; c++)
		{
			m_results.n_repairs_per_component[c] += hel->get_repairs_per_component()[c] * problem_scale;
			m_results.n_failures_per_component[c] += hel->get_failures_per_component()[c] * problem_scale;
		}
	}


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
		for (size_t s = 0; s<staff.m_members.size(); s++)
			ofs << ",staff_" << s;
		ofs << "\n";

		//summary
		ofs << "hrs_worked";
		for (size_t s = 0; s<staff.m_members.size(); s++)
			ofs << "," << staff.m_members.at(s)->m_hours_worked;
		ofs << "\nrepairs";
		for (size_t s = 0; s<staff.m_members.size(); s++)
			ofs << "," << staff.m_members.at(s)->m_n_repairs_completed*problem_scale;
		ofs << "\n";
		ofs << "\n";

		//--failures and repairs per component
		ofs << " ";
		for (size_t c = 0; c<n_components; c++)
			ofs << ",component_" << c;
		ofs << "\nfailures";
		for (size_t c = 0; c < n_components; c++)
			ofs << "," << m_results.n_failures_per_component[c];
		ofs << "\nrepairs";
		for (size_t c = 0; c < n_components; c++)
			ofs << "," << m_results.n_repairs_per_component[c];
		ofs << "\n";
		ofs << "\n";


		//-- yearly-average results
		ofs << "year,avg_avail";
		if (m_settings.is_tracking)
		{
			for (size_t c = 0; c < n_components; c++)
				ofs << ",component_" << c << "_failures";
			for (size_t c = 0; c < n_components; c++)
				ofs << ",component_" << c << "_repair_time";
		}
		ofs << "\n";

		for (int y = 0; y < m_settings.n_years; y++)
		{
			ofs << y << "," << m_results.yearly_avg_avail[y];
			if (m_settings.is_tracking)
			{
				for (size_t c = 0; c < n_components; c++)
					ofs << "," << m_results.failures_per_year[c][y];
				for (size_t c = 0; c < n_components; c++)
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
		for (int t = 0; t<nsteps; t++)
		{
			ofs << t*ts << "," << m_results.avail_schedule.at(t);
			if (m_settings.is_tracking)
				ofs << "," << m_results.queue_size_vs_time[t] << "," << m_results.queue_time_vs_time[t];
			ofs << "\n";
		}
		ofs.close();
	}


	m_sim_available = true;
}
