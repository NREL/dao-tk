#include "solarfield_structures.h"
#include "solarfield_staff.h"



solarfield_settings::solarfield_settings()
{
	n_years = 30;
	step = 4.;

	max_hours_per_day = 9.;		
	max_hours_per_week = 40.;	

	n_helio = std::numeric_limits<int>::quiet_NaN();
	n_helio_sim = 8000;

	is_fix_hours = false;	
	sunrise = 6.9;
	sunset = 19.;

	seed = 123;
	repair_order = PERF_OVER_MRT;
	is_tracking = false;

	is_trade_repairs = true;

}

bool operator<(const solarfield_event & e1, const solarfield_event & e2)
{
	return e1.priority < e2.priority;
}
