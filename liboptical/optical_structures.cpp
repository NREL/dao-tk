#include "optical_structures.h"


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
	max_hours_per_week = std::numeric_limits<double>::quiet_NaN(); 
	max_hours_per_day = std::numeric_limits<double>::quiet_NaN();
}


