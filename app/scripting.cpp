
#include <lk/env.h>
//#include <fstream>
//#include <algorithm> 

#include "scripting.h"

#include "../liboptical/optical_degr.h"
#include "../liboptical/optical_structures.h"


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

void _optical_simulation(lk::invoke_t &cxt)
{
	LK_DOC("simulate_optical_deg", "Simulate the baseline optical reflectance over time, including soiling and degradation. "
		"Table keys include: n_helio, n_wash_crews, wash_units_per_hour, hours_per_day, hours_per_week, replacement_threshold, "
		"soil_loss_per_hr, degr_loss_per_hr, degr_accel_per_year, rng_seed."
		, "(table:inputs):table");
	settings S;
	Results R;

	lk::varhash_t *h = cxt.arg(0).hash();

	S.n_helio = 8000;
	if( h->find("n_helio") != h->end() )
		S.n_helio = h->at("n_helio")->as_integer();

	S.n_wash_crews = 3;
	if ( h->find("n_wash_crews") != h->end() )
		S.n_wash_crews = h->at("n_wash_crews")->as_integer();

	S.wash_units_per_hour = 45;
	if ( h->find("wash_units_per_hour") != h->end() )
		S.wash_units_per_hour = h->at("wash_units_per_hour")->as_number();
	
	S.hours_per_day = 10.;
	if ( h->find("hours_per_day") != h->end() )
		S.hours_per_day = h->at("hours_per_day")->as_number();

	S.hours_per_week = 70;
	if ( h->find("hours_per_week") != h->end() )
		S.hours_per_week = h->at("hours_per_week")->as_number();

	S.replacement_threshold = 0.771;
	if (h->find("replacement_threshold") != h->end())
		S.replacement_threshold = h->at("replacement_threshold")->as_number();

	S.soil_loss_per_hr = 1.e-4;
	if (h->find("soil_loss_per_hr") != h->end())
		S.soil_loss_per_hr = h->at("soil_loss_per_hr")->as_number();

	S.degr_loss_per_hr = 1.e-7;
	if (h->find("degr_loss_per_hr") != h->end())
		S.degr_loss_per_hr = h->at("degr_loss_per_hr")->as_number();

	S.degr_accel_per_year = 0.125;
	if (h->find("degr_accel_per_year") != h->end())
		S.degr_accel_per_year = h->at("degr_accel_per_year")->as_number();

	S.n_hr_sim = 219000;
	if (h->find("n_hr_sim") != h->end())
		S.n_hr_sim = h->at("n_hr_sim")->as_number();

	int rng_seed = 123;
	if (h->find("rng_seed") != h->end())
		rng_seed = h->at("rng_seed")->as_integer();

	//std::string resfile = "results.csv";
	//std::string tracefile = "trace.csv";
	simulate(S, R, rng_seed); // , &resfile, &tracefile);

	//process and return results



}