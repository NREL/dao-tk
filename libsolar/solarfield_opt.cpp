#include "./solarfield_opt.h"
#include "./solarfield_avail.h"
#include <iostream>

void solarfield_opt::initialize()
{
	m_sfa.initialize();
}

double solarfield_opt::calculate_rev_loss()
{
	double ann_revenue = m_sfa.m_settings.sum_performance * m_settings.price_per_kwh * m_settings.system_efficiency;
	double factor = 1.0;
	double rev_loss = 0.;
	for (size_t t = 0; t < m_sfa.m_settings.n_years; t++)
	{
		rev_loss += ann_revenue * factor * (1-m_sfa.m_results.yearly_avg_avail[t]);
		factor *= (1.0 - m_settings.revenue_discount_rate);
	}
	return rev_loss;
}

double solarfield_opt::calculate_labor_cost()
{
	double ann_cost = 52 * m_sfa.m_settings.max_hours_per_week * m_settings.hourly_cost_per_staff * m_sfa.m_settings.n_om_staff;
	double total_cost = 0.;
	for (size_t t = 0; t < m_sfa.m_settings.n_years; t++)
	{
		total_cost += ann_cost;
		ann_cost *= (1.0 - m_settings.labor_discount_rate);
	}
	return total_cost;
}

double solarfield_opt::calculate_repair_cost()
{
	double factor = 1.0;
	double total_cost = 0.;
	for (size_t t = 0; t < m_sfa.m_settings.n_years; t++)
	{
		total_cost += factor * m_sfa.m_results.repair_cost_per_year[t];
		factor *= (1.0 - m_settings.repair_discount_rate);
	}
	return total_cost;
}

void solarfield_opt::optimize_staff(bool(*)(float prg, const char *msg), std::string *)
{
	WELLFiveTwelve gen(0);
	gen.assignStates(m_sfa.m_settings.seed % 100);
	m_sfa.assign_generator(gen);
	double total_cost;
	double best_cost = INFINITY;
	m_sfa.m_settings.n_om_staff = 1;
	while (m_sfa.m_settings.n_om_staff <= m_settings.max_num_staff)
	{
		gen.assignStates(m_sfa.m_settings.seed % 100);
		m_sfa.initialize();
		m_sfa.simulate();
		total_cost = calculate_rev_loss() + calculate_labor_cost() + calculate_repair_cost();
		std::cerr << "num staff: " << m_sfa.m_settings.n_om_staff << "\n";
		std::cerr << "avg_avail: " << m_sfa.m_results.avg_avail << "\n";
		std::cerr << "total_cost: " << total_cost << "\n";
		if (total_cost > best_cost)
			break;
		best_cost = total_cost;
		m_results = m_sfa.m_results;
		m_sfa.m_settings.n_om_staff++;
	}
	m_sfa.m_settings.n_om_staff--;
	std::cerr << "final cost: " << best_cost << "\n";
	std::cerr << "optimal # of ft staff: " << m_sfa.m_settings.n_om_staff << "\n";
	std::cerr << "avg_avail: " << m_results.avg_avail << "\n";

}