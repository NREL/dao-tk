
#include <lk/env.h>
//#include <fstream>
//#include <algorithm> 

#include "scripting.h"

#include "../liboptical/optical_degr.h"
#include "../liboptical/optical_structures.h"
#include "../libsolar/solarfield_avail.h"
#include "../libsolar/solarfield_structures.h"


void _test(lk::invoke_t &cxt)
{
	LK_DOC("test", "Test description.", "(void):null");

	//do something
}


void _power_cycle(lk::invoke_t &cxt)
{
	LK_DOC("power_cycle_avail", "Simulate a power cycle failure series.", "(void):null");

	//do something
}

void _simulate_optical(lk::invoke_t &cxt)
{
	LK_DOC("simulate_optical", "Simulate the baseline optical reflectance over time, including soiling and degradation. "
		"Table keys include: n_helio, n_wash_crews, wash_units_per_hour, hours_per_day, hours_per_week, replacement_threshold, "
		"soil_loss_per_hr, degr_loss_per_hr, degr_accel_per_year, n_hr_sim, rng_seed."
		, "(table:inputs):table");
	
	optical_degradation OD;
	
	lk::varhash_t *h = cxt.arg(0).hash();

	OD.m_settings.n_helio = 8000;
	if( h->find("n_helio") != h->end() )
		OD.m_settings.n_helio = h->at("n_helio")->as_integer();

	OD.m_settings.n_wash_crews = 3;
	if ( h->find("n_wash_crews") != h->end() )
		OD.m_settings.n_wash_crews = h->at("n_wash_crews")->as_integer();

	OD.m_settings.wash_units_per_hour = 45;
	if ( h->find("wash_units_per_hour") != h->end() )
		OD.m_settings.wash_units_per_hour = h->at("wash_units_per_hour")->as_number();
	
	OD.m_settings.hours_per_day = 10.;
	if ( h->find("hours_per_day") != h->end() )
		OD.m_settings.hours_per_day = h->at("hours_per_day")->as_number();

	OD.m_settings.hours_per_week = 70;
	if ( h->find("hours_per_week") != h->end() )
		OD.m_settings.hours_per_week = h->at("hours_per_week")->as_number();

	OD.m_settings.replacement_threshold = 0.771;
	if (h->find("replacement_threshold") != h->end())
		OD.m_settings.replacement_threshold = h->at("replacement_threshold")->as_number();

	OD.m_settings.soil_loss_per_hr = 1.e-4;
	if (h->find("soil_loss_per_hr") != h->end())
		OD.m_settings.soil_loss_per_hr = h->at("soil_loss_per_hr")->as_number();

	OD.m_settings.degr_loss_per_hr = 1.e-7;
	if (h->find("degr_loss_per_hr") != h->end())
		OD.m_settings.degr_loss_per_hr = h->at("degr_loss_per_hr")->as_number();

	OD.m_settings.degr_accel_per_year = 0.125;
	if (h->find("degr_accel_per_year") != h->end())
		OD.m_settings.degr_accel_per_year = h->at("degr_accel_per_year")->as_number();

	OD.m_settings.n_hr_sim = 219000;
	if (h->find("n_hr_sim") != h->end())
		OD.m_settings.n_hr_sim = h->at("n_hr_sim")->as_number();

	OD.m_settings.seed = 123;
	if (h->find("rng_seed") != h->end())
		OD.m_settings.seed = h->at("rng_seed")->as_integer();

	OD.simulate();

	return;

}

void _simulate_solarfield(lk::invoke_t &cxt)
{
	LK_DOC("simulate_solarfield", "Simulate the solar field availability over time due to heliostat failures. "
		"Table keys include: "
		"mean_time_to_failure, n_helio_actual, n_helio_simulated, n_om_staff, n_hr_sim, rng_seed, repair_min_downtime, "
		"rep_max_downtime, staff_productive_hr_week."
		, "(table:inputs):table");

	solarfield_availability SA;

	lk::varhash_t *H = cxt.arg(0).hash();



	SA.m_settings.mf = 12000;
	if (H->find("mean_time_to_failure") != H->end())
		SA.m_settings.mf = H->at("mean_time_to_failure")->as_integer();

	SA.m_settings.n_helio = 9264;
	if (H->find("n_helio_actual") != H->end())
		SA.m_settings.n_helio = H->at("n_helio_actual")->as_integer();

	SA.m_settings.n_helio_sim = 1000;
	if (H->find("n_helio_simulated") != H->end())
		SA.m_settings.n_helio_sim = H->at("n_helio_simulated")->as_integer();

	SA.m_settings.n_om_staff = 5;
	if (H->find("n_om_staff") != H->end())
		SA.m_settings.n_om_staff = H->at("n_om_staff")->as_integer();

	SA.m_settings.n_hr_sim = 105120;
	if (H->find("n_hr_sim") != H->end())
		SA.m_settings.n_hr_sim = H->at("n_hr_sim")->as_integer();

	SA.m_settings.seed = 123;
	if (H->find("rng_seed") != H->end())
		SA.m_settings.seed = H->at("rng_seed")->as_integer();

	SA.m_settings.rep_min = 1; //[hr]
	if (H->find("repair_min_downtime") != H->end())
		SA.m_settings.rep_min = H->at("repair_min_downtime")->as_number();

	SA.m_settings.rep_max = 100; //[hr]
	if (H->find("rep_max_downtime") != H->end())
		SA.m_settings.rep_max = H->at("rep_max_downtime")->as_number();

	SA.m_settings.hr_prod = 35;
	if (H->find("staff_productive_hr_week") != H->end())
		SA.m_settings.hr_prod = H->at("staff_productive_hr_week")->as_number();
	
	SA.simulate();

	return;

}