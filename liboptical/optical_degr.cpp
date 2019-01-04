#include "optical_structures.h"
#include "optical_degr.h"
#include "wash_opt_structure.h"
#include "./../libcycle/distributions.h"
#include "./../libcycle/well512.h"

#include <random>
#include <vector>
#include <algorithm>
#include <string>

#include <iostream>
#include <fstream>
#include <stdio.h>

optical_degradation::optical_degradation()
{
	//initialize
	m_sim_available = false;
}

float* optical_degradation::get_soiling_schedule(int *length)
{
	*length = m_results.n_schedule;

	return m_results.soil_schedule;
}

float* optical_degradation::get_degradation_schedule(int *length)
{
	*length = m_results.n_schedule;

	return m_results.degr_schedule;
}

float* optical_degradation::get_replacement_schedule(int *length)
{
	*length = m_results.n_schedule;

	return m_results.repl_schedule;
}

float* optical_degradation::get_replacement_totals(int *length)
{
	*length = m_results.n_schedule;

	return m_results.repl_total;
}

//------------------------------------------


double optical_degradation::get_replacement_threshold(
	double mirror_output, 
	int num_mirrors
)
{
	double rev_loss_rate = (
		mirror_output * m_settings.annual_profit_per_kwh / 8760
		);
	double refurb_cost = m_settings.heliostat_refurbish_cost * num_mirrors;
	if (m_settings.degr_accel_per_year < DBL_EPSILON)
	{
		double time_threshold = sqrt(
			2.0 * refurb_cost /
			(rev_loss_rate * m_settings.degr_loss_per_hr)
		);
		//std::cerr << "rev loss " << rev_loss_rate << " time " << time_threshold << " loss " << ((refurb_cost /time_threshold) + (rev_loss_rate * m_settings.degr_loss_per_hr * time_threshold/2.)) << "\n";
		return 1.0 - time_threshold * m_settings.degr_loss_per_hr;
	}
	else
	{
	
		//use bisection to obtain the optimal time interval.
		double b, c, logc, z_lo, z_med, z_hi, lo, med, hi, interval, min_t, max_t;
		
		b = rev_loss_rate * m_settings.degr_loss_per_hr;
		c = (1 + m_settings.degr_accel_per_year);
		logc = log(c)/8760.;
		interval = m_settings.n_hr_sim * 0.5 * 100;
		lo = 1.;
		min_t = lo * 1.0;
		hi = m_settings.n_hr_sim*100;
		max_t = hi * 1.0;
		med = hi / 2.0;
		z_lo = (
			(b*std::pow(c, lo / 8760.) / logc)
			- (b / (lo*logc*logc)) * (std::pow(c, lo / 8760.) - 1)
			+ (refurb_cost / lo)
			);
		z_med = (
			(b*std::pow(c, med / 8760.) / logc)
			- (b / (med*logc*logc)) * (std::pow(c, med / 8760.) - 1)
			+ (refurb_cost / med)
			);
		z_hi= (
			(b*std::pow(c, hi / 8760.) / logc)
			- (b / (hi*logc*logc)) * (std::pow(c, hi / 8760.) - 1)
			+ (refurb_cost / hi)
			);
		while (interval > 24)
		{
			interval *= 0.5;
			if (z_lo >= z_med && z_med >= z_hi)
			{
				if (hi + interval > max_t)
				{
					z_lo = z_med;
					lo = med;
					med = hi - interval;
					z_med = (
						(b*std::pow(c, med / 8760.) / logc)
						- (b / (med*logc*logc)) * (std::pow(c, med / 8760.) - 1)
						+ (refurb_cost / med)
						);
				}
				else
				{
					z_med = z_hi;
					med = hi;
					hi = med + interval;
					lo = med - interval;
					z_lo = (
						(b*std::pow(c, lo / 8760.) / logc)
						- (b / (lo*logc*logc)) * (std::pow(c, lo / 8760.) - 1)
						+ (refurb_cost / lo)
						);
					z_hi = (
						(b*std::pow(c, hi / 8760.) / logc)
						- (b / (hi*logc*logc)) * (std::pow(c, hi / 8760.) - 1)
						+ (refurb_cost / hi)
						);
				}
			}
			else if (z_lo <= z_med && z_med <= z_hi)
			{
				if (lo - interval < min_t)
				{
					z_hi = z_med;
					hi = med;
					med = lo + interval;
					z_med = (
						(b*std::pow(c, med / 8760.) / logc)
						- (b / (med*logc*logc)) * (std::pow(c, med / 8760.) - 1)
						+ (refurb_cost / med)
						);
				}
				else
				{
					z_med = z_lo;
					med = lo;
					lo = med - interval;
					hi = med + interval;
					z_lo = (
						(b*std::pow(c, lo / 8760.) / logc)
						- (b / (lo*logc*logc)) * (std::pow(c, lo / 8760.) - 1)
						+ (refurb_cost / lo)
						);
					z_hi = (
						(b*std::pow(c, hi / 8760.) / logc)
						- (b / (hi*logc*logc)) * (std::pow(c, hi / 8760.) - 1)
						+ (refurb_cost / hi)
						);
				}
			}
			else
			{
				lo = med - interval;
				hi = med + interval;
				z_lo = (
					(b*std::pow(c, lo / 8760.) / logc)
					- (b / (lo*logc*logc)) * (std::pow(c, lo / 8760.) - 1)
					+ (refurb_cost / lo)
					);
				z_hi = (
					(b*std::pow(c, hi / 8760.) / logc)
					- (b / (hi*logc*logc)) * (std::pow(c, hi / 8760.) - 1)
					+ (refurb_cost / hi)
					);
			}
		}
		//bisection gives the optimal time interval, evaluate loss function at this
		//rate to obtain the optimal replacement threshold
		if (z_lo <= z_med && z_lo <= z_hi)
		{
			return 1.0 - m_settings.degr_loss_per_hr * lo * std::pow(c, lo / 8760);
		}
		else if (z_hi <= z_med && z_hi <= z_lo)
		{
			return 1.0 - m_settings.degr_loss_per_hr * hi * std::pow(c, hi / 8760);
		}
		else
		{
			return 1.0 - m_settings.degr_loss_per_hr * med * std::pow(c, med / 8760);
		}
	}
}

//------------------------------------------



void optical_degradation::simulate(bool(*callback)(float prg, const char *msg), std::string *results_file_name, std::string *trace_file_name)
{
	//validation
	if (m_settings.n_helio < 1)
	{
		std::cout << "**** Invalid simulation parameter: ****\nNumber of heliostats is " << m_settings.n_helio << "\n";
		return;
	}

	//scale the problem
	int n_helio_s;
	double wash_units_per_hour_s;
	//double problem_scale = 1.;
	//double hscale = 250.;
	bool do_trace = trace_file_name != 0;
	//always scale the problem, ensures sampling repeatability
	//problem_scale = (float)m_settings.n_helio / hscale;

	n_helio_s = m_solar_data.num_mirror_groups;
	wash_units_per_hour_s = m_settings.wash_units_per_hour;
	//-------------------

	std::vector< opt_heliostat > helios(n_helio_s, opt_heliostat());
	if (do_trace)
	{
		for (int i = 0; i<n_helio_s; i++)
		{
			//helios.push_back( opt_heliostat() );
			helios.at(i).refl_history = new double[m_settings.n_hr_sim];
			helios.at(i).soil_history = new double[m_settings.n_hr_sim];
		}
	}

	std::vector< opt_crew > crews;
	opt_crew c;
	for (int i = 0; i < m_settings.n_wash_crews; i++)
	{
		c = opt_crew();
		c.start_heliostat = m_wc_results.assignments.at(i);
		c.end_heliostat = m_wc_results.assignments.at(i+1);
		crews.push_back(c);
	}
		
	//determine replacement thresholds for heliostats.  
	if (m_settings.use_fixed_replacement_threshold)
	{
		for (int i = 0; i < n_helio_s; i++)
		{
			helios.at(i).replacement_threshold = m_settings.replacement_threshold;
		}
	}
	else if (m_settings.use_mean_replacement_threshold)
	{
		int total_mirrors = 0;
		for (int i = 0; i < n_helio_s; i++)
		{
			total_mirrors += m_solar_data.num_mirrors_by_group[i];
			//std::cerr << i << "  " << helios.at(i).replacement_threshold << "  " << m_solar_data.mirror_output[i] <<  "  "   << m_solar_data.num_mirrors_by_group[i] << "\n";
		}
		double mean_threshold = get_replacement_threshold(
			m_solar_data.total_mirror_output / total_mirrors,
			1
		);
		for (int i = 0; i < n_helio_s; i++)
		{
			helios.at(i).replacement_threshold = mean_threshold;
		}
	}
	else
	{
		//at this point, we optimize the threshold for each heliostat.
		for (int i = 0; i < n_helio_s; i++)
		{
			helios.at(i).replacement_threshold = get_replacement_threshold(
				m_solar_data.mirror_output[i],
				m_solar_data.num_mirrors_by_group[i]
			);
			//std::cerr << i << "  " << helios.at(i).replacement_threshold << "  " << m_solar_data.mirror_output[i] <<  "  "   << m_solar_data.num_mirrors_by_group[i] << "\n";
		}
	}

	std::vector< double > soil(m_settings.n_hr_sim);
	std::vector< double > degr(m_settings.n_hr_sim);
	std::vector< int > repr(m_settings.n_hr_sim);
	std::vector< double > repr_cum(m_settings.n_hr_sim);

	//random generators
	unsigned seed1 = m_settings.seed; 
	WELLFiveTwelve soil_gen(seed1 % 100);
	WELLFiveTwelve degr_gen((seed1+50) % 100);
	GammaProcessDist soiling_dist;
	double soil_c = .25 / m_settings.soil_sim_interval;
	double refl_c = .25 / m_settings.refl_sim_interval;
	soiling_dist = GammaProcessDist(0, soil_c, 4 * m_settings.soil_loss_per_hr, "linear");
	GammaProcessDist degr_dist;
	degr_dist = GammaProcessDist(0, refl_c, 4 * m_settings.degr_loss_per_hr, "linear");

	
	
	//---------

	int this_heliostat; //current heliostat index

	//initialize where each crew starts in the field
	for (size_t crew_idx = 0; crew_idx <crews.size(); crew_idx++)
		crews.at(crew_idx).current_heliostat = crews.at(crew_idx).start_heliostat;
	int n_replacements_cumu = 0;

	//create the degradation rate by age
	double degr_accel = std::pow((1. + m_settings.degr_accel_per_year), 1./8760);
	int num_accel_entries = m_settings.n_hr_sim + std::max(m_settings.soil_sim_interval, m_settings.refl_sim_interval) + 1;
	std::vector< double > alpha_t_by_age(num_accel_entries);
	if (m_settings.degr_accel_per_year > DBL_EPSILON)
	{
		alpha_t_by_age[0] = degr_accel;
		for (int t = 1; t < num_accel_entries; t++)
			alpha_t_by_age[t] = alpha_t_by_age[t - 1] * degr_accel;
		for (int t = 1; t < num_accel_entries; t++)
			alpha_t_by_age[t] *= t * refl_c;
	}
	else
	{
		for (int t = 1; t < num_accel_entries; t++)
			alpha_t_by_age[t] = t * refl_c;
	}

	for (int t = 0; t<m_settings.n_hr_sim; t++)
	{

		if (t % (int)(m_settings.n_hr_sim / 50.) == 0 && callback != 0)
		{
			if (!callback((float)t / (float)m_settings.n_hr_sim, "Simulating heliostat field reflectivity"))
				return;
		}

		//at each day, update crew hours worked today
		if (t % 24 == 0)
		{
			for (std::vector<opt_crew>::iterator crew = crews.begin(); crew != crews.end(); crew++)
				crew->hours_today = 0.;
		}

		//at each week, update crew hours worked this week
		if (t % (24 * 7) == 0)
			for (std::vector<opt_crew>::iterator crew = crews.begin(); crew != crews.end(); crew++)
				crew->hours_this_week = 0.;

		//at each soiling interval, refresh hourly soiling rates
		if (t % m_settings.soil_sim_interval == 0)
		{
			for (std::vector<opt_heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
			{
				//soil each heliostat
				double ss = soiling_dist.GetVariate(
					m_settings.soil_sim_interval*soil_c,soil_gen
				);
				h->soil_loss_rate = ss;
			}
		}

		//at each degradation interval, refresh hourly degradation rates
		if (t % m_settings.refl_sim_interval == 0)
		{
			for (std::vector<opt_heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
			{

				//degrade each heliostat
				double dd = degr_dist.GetVariate(
					alpha_t_by_age[h->age_hours+m_settings.refl_sim_interval] 
					- alpha_t_by_age[h->age_hours], degr_gen
				);
				h->refl_loss_rate = dd;
			}
		}

		//at each hour, update the soiling/degradation losses.
		for (std::vector<opt_heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
		{
			h->age_hours++;

			h->refl_base -= h->refl_loss_rate;
			h->refl_base = h->refl_base > 0. ? h->refl_base : 0.;
			
			h->soil_loss -= h->soil_loss_rate;
			h->soil_loss = h->soil_loss > 0. ? h->soil_loss : 0.;

			if (do_trace)
			{
				h->refl_history[t] = h->refl_base;
				h->soil_history[t] = h->soil_loss;
			}
		}


		int n_replacements_t = 0;
		//each crew attends to X number of heliostats
		for (std::vector<opt_crew>::iterator crew = crews.begin(); crew != crews.end(); crew++)
		{

			//check crew availability
			if (crew->hours_this_week >= m_settings.hours_per_week ||
				crew->hours_today >= m_settings.hours_per_day)
				continue;

			//double crew_time = 0.;
			double units_washed_remain = wash_units_per_hour_s;
			while (true)
			{
				//if the current heliostat is out of range, reset to starting heliostat
				if (crew->current_heliostat >= crew->end_heliostat)
				{
					crew->current_heliostat = crew->start_heliostat;
				}

				this_heliostat = crew->current_heliostat;

				bool do_break = false;

				//take care of any remaining time on this heliostat
				if (crew->carryover_wash_time > DBL_EPSILON)
				{
					units_washed_remain += -crew->carryover_wash_time;

					if (units_washed_remain < -DBL_EPSILON)
					{
						crew->carryover_wash_time = -units_washed_remain;
						do_break = true;
					}
					else
					{
						crew->carryover_wash_time = 0.;
						helios.at(crew->current_heliostat).soil_loss = 1.;  //reset
						crew->current_heliostat++;
						if (crew->current_heliostat >= crew->end_heliostat)
						{
							crew->current_heliostat = crew->start_heliostat;
						}
					}
				}
				else 
				{
					units_washed_remain -= m_solar_data.num_mirrors_by_group[crew->current_heliostat];

					if (units_washed_remain < -DBL_EPSILON)
					{
						crew->carryover_wash_time = -units_washed_remain;
						do_break = true;
					}
					else
					{
						crew->carryover_wash_time = 0.;
						helios.at(this_heliostat).soil_loss = 1.;  //reset
						crew->current_heliostat++;
						if (crew->current_heliostat >= crew->end_heliostat)
						{
							crew->current_heliostat = crew->start_heliostat;
						}
					}
				}

				//make repairs as needed; assumes replacement time is about the same as wash time
				if (
					helios.at(this_heliostat).refl_base < 
					helios.at(this_heliostat).replacement_threshold
					)
				{
					crew->replacements_made += m_solar_data.num_mirrors_by_group[this_heliostat];
					n_replacements_t += m_solar_data.num_mirrors_by_group[this_heliostat];
					n_replacements_cumu += m_solar_data.num_mirrors_by_group[this_heliostat];
					helios.at(this_heliostat).age_hours = 0;
					helios.at(this_heliostat).refl_base = 1.;
					helios.at(this_heliostat).soil_loss = 1.; // repair restores soiling losses
				}

				if (do_break)
					break;
			}

			//track time spent
			crew->hours_today++;
			crew->hours_this_week++;
		}

		//log averages
		double refl_ave = 0.;
		double soil_ave = 0.;
		double mirror_energy = 0.;
		for (size_t i = 0; i<helios.size(); i++)
		{
			mirror_energy = m_solar_data.mirror_output[i];
			refl_ave += helios.at(i).refl_base * mirror_energy;
			soil_ave += helios.at(i).soil_loss * mirror_energy;
		}

 		soil.at(t) = soil_ave / m_solar_data.total_mirror_output;
		degr.at(t) = refl_ave / m_solar_data.total_mirror_output;
		repr.at(t) = n_replacements_t;
		repr_cum.at(t) = n_replacements_cumu;
	}


	//fill in the return data
	int replacements_made = 0;
	for (size_t s = 0; s < crews.size(); s++)
	{
		//hours_worked += crews.at(s).hours_worked;
		replacements_made += crews.at(s).replacements_made;
	}

	m_results.n_replacements = (float)replacements_made;

	m_results.soil_schedule = new float[m_settings.n_hr_sim];
	m_results.degr_schedule = new float[m_settings.n_hr_sim];
	m_results.repl_schedule = new float[m_settings.n_hr_sim];
	m_results.repl_total = new float[m_settings.n_hr_sim];

	m_results.avg_degr = 0.;
	m_results.avg_soil = 0.;

	for (int i = 0; i<m_settings.n_hr_sim; i++)
	{
		float s = (float)soil.at(i);
		float d = (float)degr.at(i);

		m_results.soil_schedule[i] = s;
		m_results.degr_schedule[i] = d;
		m_results.repl_schedule[i] = (float)repr.at(i);
		m_results.repl_total[i] = (float)repr_cum.at(i);

		m_results.avg_degr += d;
		m_results.avg_soil += s;
	}

	m_results.avg_degr /= (float)m_settings.n_hr_sim;
	m_results.avg_soil /= (float)m_settings.n_hr_sim;

	m_results.n_schedule = m_settings.n_hr_sim;

	//if a file name is provided, write to that file
	if (results_file_name != 0)
	{
		//write the files
		std::ofstream ofs;
		ofs.open(*results_file_name, std::ofstream::out);

		ofs.clear();

		//summary
		ofs << "\nrepairs by crew,";
		for (size_t s = 0; s<crews.size(); s++)
			ofs << "," << crews.at(s).replacements_made;
		ofs << "\n";


		ofs << "hour,soil,degr,tot,repl,cumu\n";
		//hourly data
		for (int t = 0; t<m_settings.n_hr_sim; t++)
		{
			ofs << t << "," << soil.at(t) << "," << degr.at(t) << "," << soil.at(t)*degr.at(t) << ","
				<< repr.at(t) << "," << repr_cum.at(t) << "\n";
		}

		ofs.close();
	}

	if (do_trace)
	{
		//write the files
		std::ofstream ofs;
		ofs.open(*trace_file_name, std::ofstream::out);

		ofs.clear();

		//headers
		for (size_t i = 0; i<helios.size(); i++)
			ofs << "degr_" << i << ",";
		for (size_t i = 0; i<helios.size(); i++)
			ofs << "soil_" << i << (i<helios.size() - 1 ? "," : "\n");

		for (int t = 0; t<m_settings.n_hr_sim; t += 24 * 7)
		{
			//degradation
			int hct = 0;
			for (std::vector<opt_heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
				ofs << h->refl_history[t] << ",";

			//soiling
			hct = 0;
			for (std::vector<opt_heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
				ofs << h->soil_history[t] << (hct++ < n_helio_s - 1 ? "," : "\n");
		}

		ofs.close();

	}

	m_sim_available = true;
}


