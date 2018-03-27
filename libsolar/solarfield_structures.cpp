#include "solarfield_structures.h"


solarfield_heliostat::solarfield_heliostat()
{
    operating = true;
    repair_remain = 0.;
    number_failure = 0;
};

solarfield_staff::solarfield_staff()
{
    queue.clear();
    queue_length.clear();
    repairs_made = 0;
    hours_worked = 0;
    hours_this_week = 0;
    hours_today = 0;
}

