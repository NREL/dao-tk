#include "solarfield_heliostat.h"

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

	m_status = OPERATIONAL;
	m_time_to_next_failure = std::numeric_limits<double>::quiet_NaN();
	m_repair_time_remaining = 0.0;
	m_operational_age = 0.0;
	m_age_at_last_failure = 0.0;

	m_scale = 1.0;
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

	m_status = OPERATIONAL;
	m_time_to_next_failure = std::numeric_limits<double>::quiet_NaN();
	m_repair_time_remaining = 0.0;
	m_operational_age = 0.0;
	m_age_at_last_failure = 0.0;

	m_scale = 1.0;
}

void solarfield_helio_component::set_scale(double scale)
{
	m_scale = scale;
	return;
}

void solarfield_helio_component::set_time_to_next_failure(WELLFiveTwelve &gen)
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
			tf = m_properties.m_eta * pow(pow(m_age_at_last_failure / m_properties.m_eta, m_properties.m_beta) - log(1.0 - r), 1. / m_properties.m_beta) - m_age_at_last_failure;  // Time to "next" failure
	}

	m_time_to_next_failure = tf;
	return;
}

void solarfield_helio_component::set_time_to_repair(WELLFiveTwelve &gen)
{
	double r = gen.getVariate();
	double time = -m_properties.m_mean_repair_time * log(1.0 - r);  // Exponential distribution of repair times

	time = fmax(time, m_properties.m_min_repair_time);
	time = fmin(time, m_properties.m_max_repair_time);

	m_repair_time_remaining = time * m_scale;
	return;
}

bool solarfield_helio_component::test_for_failure(double timestep)
{
	double time_remain = m_time_to_next_failure - timestep;
	return time_remain <= 0;
}

void solarfield_helio_component::fail(WELLFiveTwelve &gen)
{
	m_status = FAILED;
	m_age_at_last_failure = m_operational_age;
	set_time_to_repair(gen);
	set_time_to_next_failure(gen);
	return;
}

void solarfield_helio_component::operate(double timestep)
{
	m_status = OPERATIONAL;
	m_time_to_next_failure -= timestep;
	m_operational_age += timestep;
	return;
}

void solarfield_helio_component::repair(double repair_time)
{
	m_status = REPAIRING;
	m_repair_time_remaining -= repair_time;
	if (m_repair_time_remaining <= 1.e-6)
	{
		m_status = OPERATIONAL;
		m_repair_time_remaining = 0.0;
		if (m_properties.m_is_good_as_new)
			m_operational_age = 0.0;
	}
	return;
}

unsigned int solarfield_helio_component::get_operational_state()
{
	return m_status;
}

double solarfield_helio_component::get_repair_time()
{
	return m_repair_time_remaining;
}

double solarfield_helio_component::get_time_to_next_failure()
{
	return m_time_to_next_failure;
}

double solarfield_helio_component::get_mean_repair_time()
{
	return m_properties.m_mean_repair_time;
}

double solarfield_helio_component::get_operational_age()
{
	return m_operational_age;
}




solarfield_heliostat::solarfield_heliostat()
{
	m_status = OPERATIONAL;
	m_n_components = 0;

	m_scale = 1.0;
	m_performance = 1.0;
	m_mean_repair_time = 0.0;
	m_time_to_next_failure = 0;
	m_repair_time_remaining = 0.0;

	m_time_repairing = 0.0;
	m_time_failed = 0.0;
	m_time_operating = 0.0;

	m_components.clear();
	m_n_failures.clear();
	m_n_repairs.clear();

	m_is_track_repair_time = false;
	m_repair_time_per_component.clear();

}

void solarfield_heliostat::add_component(const helio_component_inputs &inputs)
{
	solarfield_helio_component *comp = new solarfield_helio_component(inputs);
	comp->set_scale(m_scale);
	m_components.push_back(comp);
	return;
}

void solarfield_heliostat::initialize(const std::vector<helio_component_inputs> & components, WELLFiveTwelve &gen, double scale, double performance)
{
	m_n_components = (int)components.size();
	m_status = OPERATIONAL;
	m_scale = scale;
	m_performance = performance;
	m_mean_repair_time = 0.0;
	m_repair_time_remaining = 0.0;

	m_time_repairing = 0.0;
	m_time_failed = 0.0;
	m_time_operating = 0.0;

	m_n_failures.assign(m_n_components, 0);
	m_n_repairs.assign(m_n_components, 0);

	m_time_to_next_failure = 1.e10;
	for (int c = 0; c < m_n_components; c++)
	{
		add_component(components[c]);
		m_components[c]->set_time_to_next_failure(gen);
		m_time_to_next_failure = fmin(m_time_to_next_failure, m_components[c]->get_time_to_next_failure());
	}

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

double solarfield_heliostat::get_total_repair_time()
{
	return m_repair_time_remaining;
}

double solarfield_heliostat::get_time_to_next_failure()
{
	return m_time_to_next_failure;
}

double solarfield_heliostat::get_performance()
{
	return m_performance;
}

double solarfield_heliostat::get_mean_repair_time()
{
	return m_mean_repair_time;
}

std::vector<solarfield_helio_component *> solarfield_heliostat::get_components()
{
	return m_components;
}

std::vector<int> solarfield_heliostat::get_failed_components()
{
	std::vector<int> failed_components;
	unsigned int status;
	for (int c = 0; c < m_n_components; c++)
	{
		status = m_components[c]->get_operational_state();
		if (status == FAILED || status == REPAIRING)
			failed_components.push_back(c);
	}
	return failed_components;
}



void solarfield_heliostat::fail(double timestep, WELLFiveTwelve &gen)
{
	m_status = FAILED;
	m_repair_time_remaining = 0.0;
	m_mean_repair_time = 0.0;
	m_time_to_next_failure = 1.e10;
	for (int c = 0; c < m_n_components; c++)
	{
		if (m_components[c]->test_for_failure(timestep))
		{
			m_n_failures[c] += 1;
			m_components[c]->fail(gen);
			m_repair_time_remaining += m_components[c]->get_repair_time();
			m_mean_repair_time += m_components[c]->get_mean_repair_time();
		}
		m_time_to_next_failure = fmin(m_time_to_next_failure, m_components[c]->get_time_to_next_failure());
	}

	return;
}

void solarfield_heliostat::operate(double timestep)
{
	m_status = OPERATIONAL;
	m_time_operating += timestep;
	m_time_to_next_failure -= timestep;
	for (int c = 0; c < m_n_components; c++)
		m_components[c]->operate(timestep);
	return;
}

int solarfield_heliostat::repair(double timestep)
{

	m_status = REPAIRING;
	double time_remaining = timestep;
	int comp_repairs_completed = 0;

	if (m_is_track_repair_time)
		m_repair_time_per_component.assign(m_n_components, 0.0);

	int c = 0;
	while (time_remaining > 0 && c < m_n_components)
	{
		unsigned int state = m_components[c]->get_operational_state();
		if (state == FAILED || state == REPAIRING)
		{
			double repair_time = fmin(time_remaining, m_components[c]->get_repair_time());
			m_components[c]->repair(repair_time);

			m_time_repairing += repair_time;
			time_remaining -= repair_time;
			m_repair_time_remaining -= repair_time;

			if (m_components[c]->get_operational_state() == OPERATIONAL)
			{
				m_n_repairs[c] += 1;
				comp_repairs_completed += 1;
			}

			if (m_is_track_repair_time)
				m_repair_time_per_component[c] = repair_time;
		}
		c++;
	}

	if (m_repair_time_remaining <= 1.e-6)  
		m_status = OPERATIONAL;

	return comp_repairs_completed;
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
