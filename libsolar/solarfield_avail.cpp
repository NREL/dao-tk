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

float* solarfield_availability::get_avail_schedule(int *length)
{
    *length = m_results.n_avail_schedule;

    return m_results.avail_schedule;
}

void solarfield_availability::get_summary_results(int* n_repairs, float* staff_utilization)
{
    *n_repairs = m_results.n_repairs;
    *staff_utilization = m_results.staff_utilization;
    return;
}

void solarfield_availability::simulate(std::string *results_file_name)
{

	//set up problem scaling
	//always scale the problem, ensures sampling repeatability
	double hscale = (double)m_settings.n_helio_sim;
	double problem_scale = (float)m_settings.n_helio / hscale;
	int n_helio_s = (int)hscale;


	std::vector< solarfield_heliostat > helios(n_helio_s, solarfield_heliostat());
	std::vector< solarfield_staff > staff(m_settings.n_om_staff, solarfield_staff());

	int staff_last_assigned = -1; //index tracking which staff was most recently assigned a task

	std::vector< double > avail(m_settings.n_hr_sim);

	double outage_time = 0.;

	int update_per = (int)(m_settings.n_hr_sim / 20.);

	//random generators
	std::default_random_engine rand(m_settings.seed);
	std::chi_squared_distribution<double> chi2_2p2(2.2);
	std::default_random_engine rand2(m_settings.seed + 1234);
	std::uniform_real_distribution<double> ureal(0., 1.0);
	//---------

	for (int t = 0; t<m_settings.n_hr_sim; t++)
	{

		//log the availability for this time step before doing repair operations
		int nop = 0;
		for (int i = 0; i<n_helio_s; i++)
			nop += helios.at(i).operating ? 1 : 0;
		avail[t] = float(nop) / float(n_helio_s);

		//determine which heliostats fail this time step
		int hod = (t / 24. - t / 24) * 24;

		//generate new failures
		//only during operating hours
		std::vector< solarfield_heliostat *> new_fails;
		if (hod > 6.9 && hod < 19)
		{
			for (int i = 0; i<n_helio_s; i++)
			{
				if (ureal(rand2) < 1. / (float)m_settings.mf)
					new_fails.push_back(&helios.at(i));
			}
		}

		//generate failure times for all new fails
		std::vector<double> times;
		for (size_t i = 0; i<new_fails.size(); i++)
		{
			double ft = std::fmax(m_settings.rep_min, std::fmin(chi2_2p2(rand), m_settings.rep_max))*problem_scale;
			times.push_back(ft);
			outage_time += ft;
		}

		//update?
		if (t / float(update_per) == t / update_per)
			std::cout << t / float(m_settings.n_hr_sim)*100. << " %\n";

		//Add the failure time to the heliostats
		for (size_t i = 0; i<new_fails.size(); i++)
		{
			solarfield_heliostat *h = new_fails.at(i);

			if (h->operating)
			{
				//n_helio_out += 1;
				h->repair_remain += times[i];
				h->operating = false;
				h->number_failure += 1;
			}

			//assign this heliostat to a staff member
			staff_last_assigned += 1;
			if (staff_last_assigned == (int)staff.size())
				staff_last_assigned = 0;

			staff[staff_last_assigned].queue.push_back(h);
		}

		int how = (t / (24 * 7.) - t / (24 * 7)) * 24 * 7;  //hour of the week
		if (how == 0)  //reset for new week
		{
			for (size_t j = 0; j<staff.size(); j++)
				staff.at(j).hours_this_week = 0;
		}

		if (hod == 0)
		{
			for (size_t j = 0; j<staff.size(); j++)
				staff.at(j).hours_today = 0.;
		}

		//handle repairs
		for (size_t i = 0; i<staff.size(); i++)
		{
			solarfield_staff *s = &staff.at(i);

			//limit hours worked
			if (s->hours_this_week < m_settings.hr_prod && s->hours_today < 9)
			{

				if (s->queue.size() == 0)
				{
					//try to take jobs from other staff if possible
					std::vector<int> sco;
					for (size_t j = 0; j<staff.size(); j++)
						sco.push_back(j);  //integer array of length (staff)

					std::random_shuffle(sco.begin(), sco.end());    //randomly shuffle the array

					for (size_t si = 0; si<sco.size(); si++)   //Hi Jim - who I randomly bumped into...
					{
						if (&staff[si] == s)
							continue;
						if (staff[si].queue.size() > 1) //can I help you with anything?
						{
							s->queue.push_back(staff[si].queue.back());
							staff[si].queue.pop_back();
							break;
						}
					}
				}
				//check again for any queued jobs
				if (s->queue.size() > 0)
				{
					double step_remain = 1.;

					while (step_remain > 0.)
					{

						//Do work on current job
						if (s->queue.size() == 0)
							break;
						solarfield_heliostat *hcur = s->queue[0];
						double rr_save = hcur->repair_remain;
						hcur->repair_remain += -step_remain;

						if (hcur->repair_remain <= 0)
						{
							step_remain += -rr_save;
							hcur->repair_remain = 0.;
							hcur->operating = true;
							s->queue.erase(s->queue.begin());
							s->repairs_made++;
						}
						else
						{
							step_remain = 0.;
						}
					}
					s->hours_worked += 1;
					s->hours_this_week += 1;
					s->hours_today += 1;
				}
			}
			//track the total queue length remaining for this staff
			double ql = 0.;
			for (size_t hi = 0; hi<s->queue.size(); hi++)
				ql += s->queue.at(hi)->repair_remain;

			s->queue_length.push_back(ql);
		}
	}


	//fill in the return data
	double hours_worked = 0.;
	int repairs_made = 0;
	for (size_t s = 0; s < staff.size(); s++)
	{
		hours_worked += staff.at(s).hours_worked;
		repairs_made += staff.at(s).repairs_made*problem_scale;
	}

	m_results.n_repairs = repairs_made;
	m_results.staff_utilization = (float)hours_worked * 168. / (float)(m_settings.hr_prod * m_settings.n_hr_sim * staff.size());

	m_results.avail_schedule = new float[m_settings.n_hr_sim];

	for (int i = 0; i<m_settings.n_hr_sim; i++)
		m_results.avail_schedule[i] = avail.at(i);
	m_results.n_avail_schedule = m_settings.n_hr_sim;

	//if a file name is provided, write to that file
	if (results_file_name != 0)
	{
		//write the files
		std::ofstream ofs;
		ofs.open(*results_file_name, std::ofstream::out);

		ofs.clear();

		//header
		ofs << "hour,avail";
		for (size_t s = 0; s<staff.size(); s++)
			ofs << ",staff_" << s;
		ofs << "\n";

		//summary
		ofs << "hrs_worked,";
		for (size_t s = 0; s<staff.size(); s++)
			ofs << "," << staff.at(s).hours_worked;
		ofs << "\nrepairs,";
		for (size_t s = 0; s<staff.size(); s++)
			ofs << "," << staff.at(s).repairs_made*problem_scale;
		ofs << "\n";

		//header
		ofs << "hour,avail";
		for (size_t s = 0; s<staff.size(); s++)
			ofs << ",staff_" << s;
		ofs << "\n";

		//hourly data
		for (int t = 0; t<m_settings.n_hr_sim; t++)
		{
			ofs << t << "," << avail.at(t);
			for (size_t s = 0; s<staff.size(); s++)
				ofs << "," << staff.at(s).queue_length.at(t);
			ofs << "\n";
		}

		ofs.close();
	}

	m_sim_available = true;
}
