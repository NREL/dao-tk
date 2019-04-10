#ifndef _SOLARFIELD_OPT
#define _SOLARFIELD_OPT

#include "./solarfield_avail.h"
#include "./solarfield_heliostat.h"
#include "./solarfield_staff.h"
#include "./solarfield_structures.h"

class solarfield_opt
{
	bool m_sim_available;
public:
	solarfield_availability m_sfa;
	solarfield_opt_settings m_settings;
	solarfield_results m_results;

	void initialize();
	double calculate_rev_loss();
	double calculate_labor_cost();
	double calculate_repair_cost();
	void optimize_staff(bool(*callback)(float prg, const char *msg) = 0, std::string *results_file_name = 0);

};


#endif
