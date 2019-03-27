#include "solarfield_heliostat.h"
#include <iostream>

helio_component_inputs::helio_component_inputs()
{
	m_beta = m_eta = m_mean_repair_time = m_min_repair_time = m_max_repair_time = m_repair_cost = std::numeric_limits<double>::quiet_NaN();
	m_is_good_as_new = true;
}

helio_component_inputs::helio_component_inputs(double beta, double eta, double mean_repair, double min_repair, double max_repair, bool good_as_new, double repair_cost)
{
	m_beta = beta;
	m_eta = eta;
	m_mean_repair_time = mean_repair;
	m_min_repair_time = min_repair;
	m_max_repair_time = max_repair;
	m_is_good_as_new = good_as_new;
	m_repair_cost = repair_cost;
}




solarfield_helio_component::solarfield_helio_component()
{
	m_properties.m_beta = std::numeric_limits<double>::quiet_NaN();
	m_properties.m_eta = std::numeric_limits<double>::quiet_NaN();

	m_properties.m_mean_repair_time = std::numeric_limits<double>::quiet_NaN();
	m_properties.m_min_repair_time = std::numeric_limits<double>::quiet_NaN();
	m_properties.m_max_repair_time = std::numeric_limits<double>::quiet_NaN();
	m_properties.m_is_good_as_new = true;

	m_properties.m_repair_cost = 0.0;
}

solarfield_helio_component::solarfield_helio_component(const helio_component_inputs &inputs)
{
	m_properties.m_beta = inputs.m_beta;
	m_properties.m_eta = inputs.m_eta;

	m_properties.m_mean_repair_time = inputs.m_mean_repair_time;
	m_properties.m_min_repair_time = inputs.m_min_repair_time;
	m_properties.m_max_repair_time = inputs.m_max_repair_time;
	m_properties.m_is_good_as_new = inputs.m_is_good_as_new;

	m_properties.m_repair_cost = inputs.m_repair_cost;

}


double solarfield_helio_component::gen_lifetime(double age, WELLFiveTwelve &gen)
{
	double tf = std::numeric_limits<double>::quiet_NaN();  

	double r = gen.getVariate();

	if (m_properties.m_beta == 1.0)
		tf = -m_properties.m_eta * log(1.0 - r);
	else
	{
		if (m_properties.m_is_good_as_new)
			tf = m_properties.m_eta * pow(-log(1.0 - r), 1. / m_properties.m_beta);  // Time to "first" failure
		else
			tf = m_properties.m_eta * pow(pow(age / m_properties.m_eta, m_properties.m_beta) - log(1.0 - r), 1. / m_properties.m_beta) - age;  // Time to "next" failure
	}

	return tf;
}

double solarfield_helio_component::gen_repair_time(WELLFiveTwelve &gen)
{
	double r = gen.getVariate();
	double time = -m_properties.m_mean_repair_time * log(1.0 - r);  // Exponential distribution of repair times

	time = fmax(time, m_properties.m_min_repair_time);
	time = fmin(time, m_properties.m_max_repair_time);

	return time;
}



double solarfield_helio_component::get_mean_repair_time()
{
	return m_properties.m_mean_repair_time;
}




solarfield_heliostat::solarfield_heliostat()
{
	m_status = OPERATIONAL;
	m_n_components = 0;

	m_scale = 1.0;
	m_performance = 1.0;

	m_time_repairing = 0.0;
	m_time_failed = 0.0;
	m_time_operating = 0.0;

	m_n_failures.clear();
	m_n_repairs.clear();

	m_is_track_repair_time = false;
	m_repair_time_per_component.clear();

}


void solarfield_heliostat::initialize(std::vector<solarfield_helio_component*> components, WELLFiveTwelve &gen, double scale, double performance)
{
	m_n_components = (int)components.size();
	m_status = OPERATIONAL;
	m_scale = scale;
	m_performance = performance;

	m_time_repairing = 0.0;
	m_time_failed = 0.0;
	m_time_operating = 0.0;

	m_n_failures.assign(m_n_components, 0);
	m_n_repairs.assign(m_n_components, 0);

	m_components = components;
	m_lifetimes.assign(m_n_components, 0);
	for (int c = 0; c < m_n_components; c++)
	{
		m_lifetimes[c] = m_components.at(c)->gen_lifetime(0, gen);
	}

	update_failure_time();
	
	return;
}

void solarfield_heliostat::initialize_repair_time_tracking()
{
	m_is_track_repair_time = true;
	m_repair_time_per_component.resize(m_n_components);
}



int solarfield_heliostat::get_n_components()
{
	return m_n_components;
}

unsigned int solarfield_heliostat::get_operational_state()
{
	return m_status;
}

double solarfield_heliostat::get_performance()
{
	return m_performance;
}

double solarfield_heliostat::get_op_time_to_next_failure()
{
	return m_time_to_next_failure;
}

void solarfield_heliostat::update_failure_time()
{
	m_next_component_to_fail = 0;
	m_time_to_next_failure = m_lifetimes.at(0);
	for (int i=1; i<m_n_components; i++)
		if (m_lifetimes.at(i) < m_time_to_next_failure)
		{
			m_time_to_next_failure = m_lifetimes.at(i);
			m_next_component_to_fail = i;
		}
	//std::cerr << "heliostat fail time: " << m_time_to_next_failure << "\n";
}

unsigned int solarfield_heliostat::get_next_component_to_fail()
{
	return m_next_component_to_fail;
}

std::vector<solarfield_helio_component*>  solarfield_heliostat::get_components()
{
	return m_components;
}

double solarfield_heliostat::get_repair_time()
{
	return m_repair_time;
}

void solarfield_heliostat::fail(double time, WELLFiveTwelve &gen)
{
	m_status = FAILED;
	m_time_operating += m_time_to_next_failure;
	m_n_failures[m_next_component_to_fail] += 1;
	
	for (int c = 0; c < m_n_components; c++)
		m_lifetimes[c] -= m_time_to_next_failure;

	m_time_of_last_event = time;
	m_lifetimes[m_next_component_to_fail] = m_components.at(m_next_component_to_fail)->gen_lifetime(m_time_operating, gen);
	m_repair_time = m_components.at(m_next_component_to_fail)->gen_repair_time(gen);
	update_failure_time();
}

void solarfield_heliostat::start_repair(double time)
{
	m_status = REPAIRING;
	m_time_failed += (time - m_time_of_last_event);
	m_time_of_last_event = time;	
}

void solarfield_heliostat::end_repair(double time, int idx)
{
	m_status = OPERATIONAL;
	m_repair_time = 0.;
	m_time_repairing += (time - m_time_of_last_event);
	m_time_of_last_event = time;
}

std::vector<int> solarfield_heliostat::get_failures_per_component()
{
	return m_n_failures;
}

std::vector<int> solarfield_heliostat::get_repairs_per_component()
{
	return m_n_repairs;
}

std::vector<double>* solarfield_heliostat::get_repair_time_tracking()
{
	return &m_repair_time_per_component;
}


void heliostat_field::add_component(const helio_component_inputs &inputs)
{
	solarfield_helio_component *comp = new solarfield_helio_component(inputs);
	m_components.push_back(comp);
	return;
}