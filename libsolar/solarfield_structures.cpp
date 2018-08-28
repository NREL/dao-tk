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

	is_fix_hours = true;	
	sunrise = 6.9;
	sunset = 19.;

	seed = 123;
	repair_order = MEAN_REPAIR_TIME;
	is_tracking = false;

	is_allow_multiple_per_helio = false;
	is_trade_repairs = true;
	is_prioritize_partial_repairs = true;
	is_split_operation = true;

}
