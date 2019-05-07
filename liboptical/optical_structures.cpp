#include "optical_structures.h"
#include <iostream>


opt_heliostat::opt_heliostat()
{
    refl_base = 1.;
    soil_loss = 1.;
    age_hours = 0;
    soil_history = 0;
    refl_history = 0;
};

opt_heliostat::~opt_heliostat()
{
    if( soil_history != 0 )
        delete [] soil_history;
    if( refl_history != 0 )
        delete [] refl_history;
}

opt_crew::opt_crew()
{
    current_heliostat = -1;
    carryover_wash_time = 0.;
    replacements_made = 0;
    hours_this_week = 0.;
    hours_today = 0.;
	is_active = false;
}

void opt_settings::print()
{
	std::cerr << "Settings: \n"
		<< "n_helio:  " << n_helio << "\n"
		<< "n_wash_crews:  " << n_wash_crews << "\n"
		<< "wash_units_per_hour:  " << wash_units_per_hour << "\n"
		<< "hours_per_day:  " << hours_per_day << "\n"
		<< "hours_per_week:  " << hours_per_week << "\n"
		<< "use_fixed_replacement_threshold:  " << use_fixed_replacement_threshold << "\n"
		<< "use_mean_replacement_threshold:  " << use_mean_replacement_threshold << "\n"
		<< "replacement_threshold:  " << replacement_threshold << "\n"
		<< "soil_loss_per_hr:  " << soil_loss_per_hr << "\n"
		<< "degr_loss_per_hr:  " << degr_loss_per_hr << "\n"
		<< "degr_accel_per_year:  " << degr_accel_per_year << "\n"
		<< "annual_profit_per_kwh:  " << annual_profit_per_kwh << "\n"
		<< "heliostat_refurbish_cost:  " << heliostat_refurbish_cost << "\n"
		<< "n_hr_sim:  " << n_hr_sim << "\n"
		<< "n_hr_warmup:  " << n_hr_warmup << "\n"
		<< "soil_sim_interval:  " << soil_sim_interval << "\n"
		<< "refl_sim_interval:  " << refl_sim_interval << "\n"
		<< "seed:  " << seed << "\n";
}
