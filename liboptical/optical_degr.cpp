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

	n_helio_s = (int)m_solar_data.num_mirror_groups;
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
	opt_crew crew;
	for (int i = 0; i < m_settings.n_wash_crews; i++)
	{
		crew = opt_crew();
		crew.start_heliostat = m_wc_results.assignments.at(i);
		crew.end_heliostat = m_wc_results.assignments.at(i+1);
		crew.current_heliostat = crew.start_heliostat * 1;
		crews.push_back(crew);
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
	soiling_dist = GammaProcessDist(0, .25, 4 * m_settings.soil_loss_per_hr * m_settings.soil_sim_interval, "linear");
	GammaProcessDist degr_dist;
	degr_dist = GammaProcessDist(log(.05), m_settings.degr_accel_per_year / 8760., 20. * m_settings.degr_loss_per_hr * m_settings.refl_sim_interval, "exponential");
	
	//---------

	int this_heliostat; //current heliostat index

	//initialize where each crew starts in the field
	int inc = (int)((float)n_helio_s / (float)crews.size());
	for (size_t c = 0; c<crews.size(); c++)
		crews.at(c).current_heliostat = c * inc;
	int n_replacements_cumu = 0;

	//create the degradation rate by age
	double degr_accel = (1. + m_settings.degr_accel_per_year / 8760.);
	std::vector< float > degr_mult_by_age(m_settings.n_hr_sim);
	degr_mult_by_age[0] = degr_accel;
	for (int t = 1; t<m_settings.n_hr_sim; t++)
		degr_mult_by_age[t] = degr_mult_by_age[t - 1] * degr_accel;

	double *current_var;
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
				double ss = soiling_dist.GetVariate(h->age_hours, 1, soil_gen) / m_settings.soil_sim_interval;
				h->soil_loss_rate = ss;
			}
		}

		//at each degradation interval, refresh hourly degradation rates
		if (t % m_settings.refl_sim_interval == 0)
		{
			for (std::vector<opt_heliostat>::iterator h = helios.begin(); h != helios.end(); h++)
			{

				//degrade each heliostat
				double dd = degr_dist.GetVariate(h->age_hours, 1, degr_gen) / m_settings.refl_sim_interval * degr_mult_by_age[h->age_hours];
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
				if (crew->carryover_wash_time > 0.)
				{
					units_washed_remain += -crew->carryover_wash_time;

					if (units_washed_remain <= 0.)
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

					if (units_washed_remain <= 0.)
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
				if (helios.at(this_heliostat).refl_base < m_settings.replacement_threshold)
				{
					crew->replacements_made++;
					n_replacements_t += m_solar_data.num_mirrors_by_group[this_heliostat];
					n_replacements_cumu += m_solar_data.num_mirrors_by_group[this_heliostat];
					helios.at(this_heliostat).age_hours = 0;
					helios.at(this_heliostat).refl_base = 1.;
					helios.at(this_heliostat).soil_loss = 1.; //even if washing doesn't finish, repair happens
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
		
		for (size_t i = 0; i<helios.size(); i++)
		{
			refl_ave += helios.at(i).refl_base * m_solar_data.mirror_eff[i];
			soil_ave += helios.at(i).soil_loss * m_solar_data.mirror_eff[i];

		}

		soil.at(t) = soil_ave / m_solar_data.total_mirror_eff;
		degr.at(t) = refl_ave / m_solar_data.total_mirror_eff;
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

	m_results.n_replacements = replacements_made;

	m_results.soil_schedule = new float[m_settings.n_hr_sim];
	m_results.degr_schedule = new float[m_settings.n_hr_sim];
	m_results.repl_schedule = new float[m_settings.n_hr_sim];
	m_results.repl_total = new float[m_settings.n_hr_sim];

	m_results.avg_degr = 0.;
	m_results.avg_soil = 0.;

	for (int i = 0; i<m_settings.n_hr_sim; i++)
	{
		float s = soil.at(i);
		float d = degr.at(i);

		m_results.soil_schedule[i] = s;
		m_results.degr_schedule[i] = d;
		m_results.repl_schedule[i] = repr.at(i);
		m_results.repl_total[i] = repr_cum.at(i);

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

	if (trace_file_name != 0)
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


