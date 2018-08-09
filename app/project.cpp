#include "project.h"
#include <wx/wx.h>
#include <limits>


variables::variables()
{
	/* 
	Initialize members
	*/
	double dnan = std::numeric_limits<double>::quiet_NaN();
	double dmax = -std::numeric_limits<double>::infinity();
	double dmin = -dmax;
	
    h_tower.set(dnan, dmin, dmax, "h_tower", lk::vardata_t::NUMBER, "Tower height", "m", "Variables");
	rec_height.set(dnan, dmin, dmax, "rec_height", lk::vardata_t::NUMBER, "Receiver height", "m", "Variables");
	D_rec.set(dnan, dmin, dmax, "D_rec", lk::vardata_t::NUMBER, "Receiver diameter", "m", "Variables");
	design_eff.set(dnan, dmin, dmax, "design_eff", lk::vardata_t::NUMBER, "Design efficiency", "-", "Variables");
	dni_des.set(dnan, dmin, dmax, "dni_des", lk::vardata_t::NUMBER, "Design point DNI", "W/m2", "Variables");
	P_ref.set(dnan, dmin, dmax, "P_ref", lk::vardata_t::NUMBER, "Design gross power", "kW", "Variables");
	solarm.set(dnan, dmin, dmax, "solarm", lk::vardata_t::NUMBER, "Solar multiple", "-", "Variables");
	tshours.set(dnan, dmin, dmax, "tshours", lk::vardata_t::NUMBER, "Hours stored at full load operation", "hr", "Variables");
	degr_replace_limit.set(dnan, dmin, dmax, "degr_replace_limit", lk::vardata_t::NUMBER, "Mirror degradation replacement limit", "-", "Variables");
	om_staff.set(-1, -999, 999, "om_staff", lk::vardata_t::NUMBER, "Number of o&m staff", "-", "Variables");
	n_wash_crews.set(-1, -999, 999, "n_wash_crews", lk::vardata_t::NUMBER, "Number of wash crews", "-", "Variables");
	N_panels.set(-1, -999, 999, "N_panels", lk::vardata_t::NUMBER, "Number of receiver panels", "-", "Variables");

    (*this)["h_tower"] = &h_tower;
    (*this)["rec_height"] = &rec_height;
    (*this)["D_rec"] = &D_rec;
    (*this)["design_eff"] = &design_eff;
    (*this)["dni_des"] = &dni_des;
    (*this)["P_ref"] = &P_ref;
    (*this)["solarm"] = &solarm;
    (*this)["tshours"] = &tshours;
    (*this)["degr_replace_limit"] = &degr_replace_limit;
    (*this)["om_staff"] = &om_staff;
    (*this)["n_wash_crews"] = &n_wash_crews;
    (*this)["N_panels"] = &N_panels;

};

parameters::parameters()
{
	/* 
	Initialize members
	*/

	print_messages.set( true, "print_messages", lk::vardata_t::NUMBER, false, "Print full output", "-", "Settings" );
	check_max_flux.set( true, "check_max_flux", lk::vardata_t::NUMBER, false, "Check max flux", "-", "Settings" );
	is_optimize.set( false, "is_optimize", lk::vardata_t::NUMBER, false );
	is_dispatch.set( false, "is_dispatch", lk::vardata_t::NUMBER, false, "Optimize dispatch", "-", "Settings" );
	is_ampl_engine.set( false, "is_ampl_engine", lk::vardata_t::NUMBER, false, "Use AMPL optimizer", "-", "Settings");
	is_stochastic_disp.set( false, "is_stochastic_disp", lk::vardata_t::NUMBER, false, "Run stochastic dispatch", "-", "Settings" );

	ampl_data_dir.set( "", "ampl_data_dir", lk::vardata_t::STRING, false, "AMPL data folder", "-", "Settings" );
	solar_resource_file.set( "", "solar_resource_file", lk::vardata_t::STRING, false, "Solar resource file", "-", "Settings" );

	disp_steps_per_hour.set( 1, "disp_steps_per_hour", lk::vardata_t::NUMBER, false, "Dispatch time steps per hour", "-", "Settings" );
	avail_seed.set( 123, "avail_seed", lk::vardata_t::NUMBER, false, "Random number generator seed", "-", "Heliostat availability|Parameters" );
	plant_lifetime.set( 30, "plant_lifetime", lk::vardata_t::NUMBER, false, "Plant lifetime", "yr", "Financial parameters" );
	finance_period.set( 25, "finance_period", lk::vardata_t::NUMBER, false, "Finance period", "yr", "Financial parameters" );
	ppa_multiplier_model.set( 1, "ppa_multiplier_model", lk::vardata_t::NUMBER, false, "PPA multiplier model", "-", "Financial parameters" );

	rec_ref_cost.set( 1.03e+008, "rec_ref_cost", lk::vardata_t::NUMBER, false, "Receiver reference cost", "$", "Financial parameters" );
	rec_ref_area.set( 1571., "rec_ref_area", lk::vardata_t::NUMBER, false, "Receiver reference area", "m2", "Financial parameters");
	tes_spec_cost.set( 24., "tes_spec_cost", lk::vardata_t::NUMBER, false, "TES specific cost", "$/kWht", "Financial parameters" );
	tower_fixed_cost.set( 3.e6, "tower_fixed_cost", lk::vardata_t::NUMBER, false, "Tower fixed cost", "$", "Financial parameters");
	tower_exp.set( 0.0113, "tower_exp", lk::vardata_t::NUMBER, false, "Tower cost scaling exponent", "-", "Financial parameters");
	heliostat_spec_cost.set( 145., "heliostat_spec_cost", lk::vardata_t::NUMBER, false, "Heliostat specific cost", "$/m2", "Financial parameters" );
	site_spec_cost.set( 16., "site_spec_cost", lk::vardata_t::NUMBER, false, "Site specific cost", "$/m2", "Financial parameters");
	land_spec_cost.set( 10000, "land_spec_cost", lk::vardata_t::NUMBER, false, "Land specific cost", "$/acre", "Financial parameters" );
	c_cps0.set( 0., "c_cps0", lk::vardata_t::NUMBER, false, "Power cycle cost coef - const", "-", "Financial parameters" );
	c_cps1.set( 1440., "c_cps1", lk::vardata_t::NUMBER, false, "Power cycle cost coef - slope", "-", "Financial parameters" );
	om_staff_cost.set( 75, "om_staff_cost", lk::vardata_t::NUMBER, false, "O&M staff cost rate", "$/hr", "Financial parameters");
	wash_crew_cost.set( 65. + 10. + 25, "wash_crew_cost", lk::vardata_t::NUMBER, false, "Wash crew cost rate", "$/hr", "Financial parameters" );
	heliostat_refurbish_cost.set( 144. * 25 + 90 * 4., "heliostat_refurbish_cost", lk::vardata_t::NUMBER, false, "Mirror replacement cost", "$", "Optical degradation|Parameters" );
	helio_mtf.set( 12000, "helio_mtf", lk::vardata_t::NUMBER, false, "Heliostat mean time to failure", "hr", "Heliostat availability|Parameters" );
	heliostat_repair_cost.set( 300, "heliostat_repair_cost", lk::vardata_t::NUMBER, false, "Heliostat repair cost", "$", "Heliostat availability|Parameters" );
	om_staff_max_hours_week.set( 35, "om_staff_max_hours_week", lk::vardata_t::NUMBER, false, "Max O&M staff hours per week", "hr", "Heliostat availability|Parameters" );
	n_heliostats_sim.set( 1000, "n_heliostats_sim", lk::vardata_t::NUMBER, false, "Number of simulated heliostats", "-", "Heliostat availability|Parameters" );
	wash_units_per_hour.set( 45., "wash_units_per_hour", lk::vardata_t::NUMBER, false, "Heliostat wash rate", "1/crew-hr", "Optical degradation|Parameters" );
	wash_crew_max_hours_week.set( 70., "wash_crew_max_hours_week", lk::vardata_t::NUMBER, false, "Wash crew max hours per week", "hr", "Optical degradation|Parameters" );
	degr_per_hour.set( 1.e-7, "degr_per_hour", lk::vardata_t::NUMBER, false, "Reflectivity degradation rate", "1/hr", "Optical degradation|Parameters" );
	degr_accel_per_year.set( 0.125, "degr_accel_per_year", lk::vardata_t::NUMBER, false, "Refl. degradation acceleration", "1/yr", "Optical degradation|Parameters" );
	degr_seed.set( 123, "degr_seed", lk::vardata_t::NUMBER, false, "Random number generator seed", "-", "Optical degradation|Parameters" );
	soil_per_hour.set( 6.e-4, "soil_per_hour", lk::vardata_t::NUMBER, false, "Mean soiling rate", "1/hr", "Optical degradation|Parameters" );
	adjust_constant.set( 4, "adjust:constant", lk::vardata_t::NUMBER, false, "Misc fixed power loss", "%", "Financial parameters" );
	helio_reflectance.set( 0.95, "helio_reflectance", lk::vardata_t::NUMBER, false, "Initial mirror reflectance", "-", "Optical degradation|Parameters" );
	disp_rsu_cost.set( 950., "disp_rsu_cost", lk::vardata_t::NUMBER, false, "Receiver startup cost", "$", "Simulation|Parameters" );
	disp_csu_cost.set( 10000., "disp_csu_cost", lk::vardata_t::NUMBER, false, "Power block startup cost", "$", "Simulation|Parameters");
	disp_pen_delta_w.set( 0.1, "disp_pen_delta_w", lk::vardata_t::NUMBER, false, "Power block ramp penalty", "$/delta-kW", "Simulation|Parameters" );
	rec_su_delay.set( 0.2, "rec_su_delay", lk::vardata_t::NUMBER, false, "Receiver min startup time", "hr", "Simulation|Parameters" );
	rec_qf_delay.set( 0.25, "rec_qf_delay", lk::vardata_t::NUMBER, false, "Receiver min startup energy", "MWh/MWh", "Simulation|Parameters" );
	startup_time.set( 0.5, "startup_time", lk::vardata_t::NUMBER, false, "Power block min startup time", "hr", "Simulation|Parameters" );
	startup_frac.set( 0.5, "startup_frac", lk::vardata_t::NUMBER, false, "Power block startup energy", "MWh/MWh", "Simulation|Parameters" );
	v_wind_max.set( 15., "v_wind_max", lk::vardata_t::NUMBER, false, "Max operational wind velocity", "m/s", "Simulation|Parameters" );
	flux_max.set(1000., "flux_max", lk::vardata_t::NUMBER, false, "Maximum receiver flux", "kW/m2", "Simulation|Parameters");
	
	std::vector< double > pval = { 0., 7., 200., 12000. };
	c_ces.set( pval, "c_ces", lk::vardata_t::VECTOR, false );

	std::vector< double > pvalts(8760, 1.);
	dispatch_factors_ts.set( pvalts, "dispatch_factors_ts", lk::vardata_t::VECTOR, false );
	dispatch_factors_ts.set( pvalts, "user_sf_avail", lk::vardata_t::VECTOR, false );
	
    (*this)["print_messages"] = &print_messages;
    (*this)["check_max_flux"] = &check_max_flux;
    (*this)["is_optimize"] = &is_optimize;
    (*this)["is_dispatch"] = &is_dispatch;
    (*this)["is_ampl_engine"] = &is_ampl_engine;
    (*this)["is_stochastic_disp"] = &is_stochastic_disp;
    (*this)["ampl_data_dir"] = &ampl_data_dir;
    (*this)["solar_resource_file"] = &solar_resource_file;
    (*this)["disp_steps_per_hour"] = &disp_steps_per_hour;
    (*this)["avail_seed"] = &avail_seed;
    (*this)["plant_lifetime"] = &plant_lifetime;
    (*this)["finance_period"] = &finance_period;
    (*this)["ppa_multiplier_model"] = &ppa_multiplier_model;
    (*this)["rec_ref_cost"] = &rec_ref_cost;
    (*this)["rec_ref_area"] = &rec_ref_area;
    (*this)["tes_spec_cost"] = &tes_spec_cost;
    (*this)["tower_fixed_cost"] = &tower_fixed_cost;
    (*this)["tower_exp"] = &tower_exp;
    (*this)["heliostat_spec_cost"] = &heliostat_spec_cost;
    (*this)["site_spec_cost"] = &site_spec_cost;
    (*this)["land_spec_cost"] = &land_spec_cost;
    (*this)["c_cps0"] = &c_cps0;
    (*this)["c_cps1"] = &c_cps1;
    (*this)["om_staff_cost"] = &om_staff_cost;
    (*this)["wash_crew_cost"] = &wash_crew_cost;
    (*this)["heliostat_refurbish_cost"] = &heliostat_refurbish_cost;
    (*this)["helio_mtf"] = &helio_mtf;
    (*this)["heliostat_repair_cost"] = &heliostat_repair_cost;
    (*this)["om_staff_max_hours_week"] = &om_staff_max_hours_week;
    (*this)["n_heliostats_sim"] = &n_heliostats_sim;
    (*this)["wash_units_per_hour"] = &wash_units_per_hour;
    (*this)["wash_crew_max_hours_week"] = &wash_crew_max_hours_week;
    (*this)["degr_per_hour"] = &degr_per_hour;
    (*this)["degr_accel_per_year"] = &degr_accel_per_year;
    (*this)["degr_seed"] = &degr_seed;
    (*this)["soil_per_hour"] = &soil_per_hour;
    (*this)["adjust_constant"] = &adjust_constant;
    (*this)["helio_reflectance"] = &helio_reflectance;
    (*this)["disp_rsu_cost"] = &disp_rsu_cost;
    (*this)["disp_csu_cost"] = &disp_csu_cost;
    (*this)["disp_pen_delta_w"] = &disp_pen_delta_w;
    (*this)["rec_su_delay"] = &rec_su_delay;
    (*this)["rec_qf_delay"] = &rec_qf_delay;
    (*this)["startup_time"] = &startup_time;
    (*this)["startup_frac"] = &startup_frac;
    (*this)["v_wind_max"] = &v_wind_max;
    (*this)["flux_max"] = &flux_max;
    (*this)["c_ces"] = &c_ces;
    (*this)["dispatch_factors_ts"] = &dispatch_factors_ts;
	(*this)["user_sf_avail"] = &user_sf_avail;
}

design_outputs::design_outputs()
{
	/* 
	Set up output members
	*/

	double nan = std::numeric_limits<double>::quiet_NaN();
	number_heliostats.set(-1, "number_heliostats", lk::vardata_t::NUMBER, true, "Heliostat count", "-", "Design|Outputs");
	area_sf.set(nan, "area_sf", lk::vardata_t::NUMBER, true, "Total heliostat area", "m2", "Design|Outputs");
	base_land_area.set(nan, "base_land_area", lk::vardata_t::NUMBER, true, "Solar field land area", "acre", "Design|Outputs");
	land_area.set(nan, "land_area", lk::vardata_t::NUMBER, true, "Total land area", "acre", "Design|Outputs");
	flux_max_observed.set(nan, "flux_max_observed", lk::vardata_t::NUMBER, true, "Max observed flux", "kW/m2", "Design|Outputs");
	cost_rec_tot.set(nan, "cost_rec_tot", lk::vardata_t::NUMBER, true, "Receiver cost", "$", "Design|Costs");
	cost_sf_tot.set(nan, "cost_sf_tot", lk::vardata_t::NUMBER, true, "Solar field cost", "$", "Design|Costs");
	cost_sf_real.set(nan, "", lk::vardata_t::NUMBER, true, "Solar field cost (real)", "$", "Design|Costs");
	cost_tower_tot.set(nan, "cost_tower_tot", lk::vardata_t::NUMBER, true, "Tower cost", "$", "Design|Costs");
	cost_land_tot.set(nan, "cost_land_tot", lk::vardata_t::NUMBER, true, "Land cost", "$", "Design|Costs");
	cost_land_real.set(nan, "", lk::vardata_t::NUMBER, true, "Land cost (real)", "$", "Design|Costs");
	cost_site_tot.set(nan, "cost_site_tot", lk::vardata_t::NUMBER, true, "Site cost", "$", "Design|Costs");
	h_tower_opt.set(nan, "h_tower_opt", lk::vardata_t::NUMBER, true);
	rec_height_opt.set(nan, "rec_height_opt", lk::vardata_t::NUMBER, true);
	rec_aspect_opt.set(nan, "rec_aspect_opt", lk::vardata_t::NUMBER, true);

	std::vector< std::vector< double > > empty_mat;
	opteff_table.set(empty_mat, "opteff_table", lk::vardata_t::VECTOR, true);
	flux_table.set(empty_mat, "flux_table", lk::vardata_t::VECTOR, true);
	heliostat_positions.set(empty_mat, "heliostat_positions", lk::vardata_t::VECTOR, true);

    (*this)["number_heliostats"] = &number_heliostats;
    (*this)["area_sf"] = &area_sf;
    (*this)["base_land_area"] = &base_land_area;
    (*this)["land_area"] = &land_area;
    (*this)["h_tower_opt"] = &h_tower_opt;
    (*this)["rec_height_opt"] = &rec_height_opt;
    (*this)["rec_aspect_opt"] = &rec_aspect_opt;
    (*this)["cost_rec_tot"] = &cost_rec_tot;
    (*this)["cost_sf_tot"] = &cost_sf_tot;
    (*this)["cost_sf_real"] = &cost_sf_real;
    (*this)["cost_tower_tot"] = &cost_tower_tot;
    (*this)["cost_land_tot"] = &cost_land_tot;
    (*this)["cost_land_real"] = &cost_land_real;
    (*this)["cost_site_tot"] = &cost_site_tot;
    (*this)["flux_max_observed"] = &flux_max_observed;
    (*this)["opteff_table"] = &opteff_table;
    (*this)["flux_table"] = &flux_table;
    (*this)["heliostat_positions"] = &heliostat_positions;

}

solarfield_outputs::solarfield_outputs()
{
	/* 
	Set up output members
	*/

	double nan = std::numeric_limits<double>::quiet_NaN();
	n_repairs.set(nan, "n_repairs", lk::vardata_t::NUMBER, true, "Number of heliostat repairs", "-", "Heliostat availability|Outputs");
	staff_utilization.set(nan, "staff_utilization", lk::vardata_t::NUMBER, true, "Staff utilization", "-", "Heliostat availability|Outputs");
	heliostat_repair_cost_y1.set(nan, "heliostat_repair_cost_y1", lk::vardata_t::NUMBER, true, "Heliostat repair cost (year 1)", "$", "Heliostat availability|Outputs");
	heliostat_repair_cost.set(nan, "heliostat_repair_cost", lk::vardata_t::NUMBER, true);

	std::vector< double > empty_vec;
	avail_schedule.set(empty_vec, "avail_schedule", lk::vardata_t::VECTOR, true);

    (*this)["n_repairs"] = &n_repairs;
    (*this)["staff_utilization"] = &staff_utilization;
    (*this)["heliostat_repair_cost_y1"] = &heliostat_repair_cost_y1;
    (*this)["heliostat_repair_cost"] = &heliostat_repair_cost;
    (*this)["avail_schedule"] = &avail_schedule;

}

optical_outputs::optical_outputs()
{
	/* 
	Set up output members
	*/

	double nan = std::numeric_limits<double>::quiet_NaN();
	std::vector< double > empty_vec;
	
	n_replacements.set(nan, "n_replacements", lk::vardata_t::NUMBER, true, "Mirror replacements", "-", "Optical degradation|Outputs" );
	heliostat_refurbish_cost.set(nan, "heliostat_refurbish_cost", lk::vardata_t::NUMBER, true, "Mirror replacement cost", "$", "Optical degradation|Outputs" );
	heliostat_refurbish_cost_y1.set(nan, "heliostat_refurbish_cost_y1", lk::vardata_t::NUMBER, true, "Mirror replacement cost (year 1)", "$", "Optical degradation|Outputs" );
	avg_soil.set(nan, "avg_soil", lk::vardata_t::NUMBER, true, "Average lifetime soiling", "-", "Optical degradation|Outputs" );
	avg_degr.set(nan, "avg_degr", lk::vardata_t::NUMBER, true, "Average lifetime degradation", "-", "Optical degradation|Outputs" );

	soil_schedule.set(empty_vec, "soil_schedule", lk::vardata_t::VECTOR, true, "Soiling time series", "-", "Optical degradation|Outputs" );
	degr_schedule.set(empty_vec, "degr_schedule", lk::vardata_t::VECTOR, true, "Degradation time series", "-", "Optical degradation|Outputs" );
	repl_schedule.set(empty_vec, "repl_schedule", lk::vardata_t::VECTOR, true, "Mirror repl. time series", "-", "Optical degradation|Outputs" );
	repl_total.set(empty_vec, "repl_total", lk::vardata_t::VECTOR, true );

    (*this)["n_replacements"] = &n_replacements;
    (*this)["heliostat_refurbish_cost"] = &heliostat_refurbish_cost;
    (*this)["heliostat_refurbish_cost_y1"] = &heliostat_refurbish_cost_y1;
    (*this)["avg_soil"] = &avg_soil;
    (*this)["avg_degr"] = &avg_degr;

    (*this)["soil_schedule"] = &soil_schedule;
    (*this)["degr_schedule"] = &degr_schedule;
    (*this)["repl_schedule"] = &repl_schedule;
    (*this)["repl_total"] = &repl_total;

}

simulation_outputs::simulation_outputs()
{
	double nan = std::numeric_limits<double>::quiet_NaN();
	std::vector< double > empty_vec;

	generation_arr.set(empty_vec, "generation_arr", lk::vardata_t::VECTOR, true, "Net power generation", "kWe", "Simulation|Outputs");
	solar_field_power_arr.set(empty_vec, "solar_field_power_arr", lk::vardata_t::VECTOR, true, "Solarfield thermal power", "kWt", "Simulation|Outputs");
	tes_charge_state.set(empty_vec, "e_ch_tes", lk::vardata_t::VECTOR, true, "Thermal storage charge state", "MWht", "Simulation|Outputs");
	dni_arr.set(empty_vec, "dni_arr", lk::vardata_t::VECTOR, true, "Direct normal irradiation", "W/m2", "Simulation|Outputs");
	price_arr.set(empty_vec, "price_arr", lk::vardata_t::VECTOR, true, "Price signal", "-", "Simulation|Outputs");
	
	dni_templates.set(empty_vec, "dni_templates", lk::vardata_t::VECTOR, true, "DNI clusters", "W/m2", "Simulation|Outputs");
	price_templates.set(empty_vec, "price_templates", lk::vardata_t::VECTOR, true, "Price clusters", "-", "Simulation|Outputs");

	annual_generation.set(nan, "annual_generation", lk::vardata_t::NUMBER, true, "Annual total generation", "GWhe", "Simulation|Outputs");
	annual_revenue.set(nan, "annual_revenue", lk::vardata_t::NULLVAL, true, "Annual revenue units", "-", "Simulation|Outputs");

	(*this)["generation_arr"] = &generation_arr;
	(*this)["solar_field_power_arr"] = &solar_field_power_arr;
	(*this)["tes_charge_state"] = &tes_charge_state;
	(*this)["dni_arr"] = &dni_arr;
	(*this)["price_arr"] = &price_arr;
	(*this)["dni_templates"] = &dni_templates;
	(*this)["price_templates"] = &price_templates;
	(*this)["annual_generation"] = &annual_generation;
	(*this)["annual_revenue"] = &annual_revenue;
}

/* 
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
	// struct CALLING_SIM{ enum E {DESIGN=1, HELIO_AVAIL, HELIO_OPTIC, SIMULATION, EXPLICIT, FINANCE, OBJECTIVE, NULLSIM=0}; };
bool Project::Validate(Project::CALLING_SIM::E sim_type, std::string *error_msg)
{
	/* 
	make sure the appropriate items have been set up for the method that's being called
	*/

	//check all of the variable and parameter values for nan or invalid
	bool any_var_errors = false;
	for( lk::varhash_t::iterator it = _merged_data.begin(); it != _merged_data.end(); it++ )
	{
		data_base *v = static_cast< data_base* >( it->second );
		bool valid_flag = true;
		if( !v->IsInvalidAllowed() )
		{
			switch(v->type)
			{
				case lk::vardata_t::NUMBER:
					if( v->as_number() != v->as_number() )
						valid_flag = false;
					break;
				case lk::vardata_t::VECTOR:
					if( v->vec()->size() == 0 )
						valid_flag = false;
					break;
				case lk::vardata_t::STRING:
					if( v->as_string().empty() )
						valid_flag = false;
					break;
				default:
					break;
			}
		}
		if( ! valid_flag && error_msg != 0)
		{
			(*error_msg).append(wxString::Format("Variable or parameter has invalid data: %s (%s)\n", 
												v->nice_name.c_str(), v->name.c_str()).c_str());
			any_var_errors = true;
		}
	}
	if( any_var_errors )
		return false;


	switch(sim_type)
	{
		case CALLING_SIM::DESIGN:

			break;
		case CALLING_SIM::HELIO_AVAIL:
			if( !is_design_valid )
			{
				(*error_msg).append("Error: Attempting to run heliostat field availability without a valid solar field design.\n");
				return false;
			}
			break;
		case CALLING_SIM::HELIO_OPTIC:
			if( !is_design_valid )
			{
				(*error_msg).append("Error: Attempting to run heliostat field soiling/degradation without a valid solar field design.\n");
				return false;
			}
			break;
		case CALLING_SIM::SIMULATION:
			if( !is_design_valid )
			{
				(*error_msg).append("Error: Attempting to run heliostat field soiling/degradation without a valid solar field design.\n");
				return false;
			}
			else
			{
				if( !is_sf_avail_valid )
					(*error_msg).append("Notice: Simulating performance without valid solar field availability model output.\n");
				if( !is_sf_optical_valid )
					(*error_msg).append("Notice: Simulating performance without valid solar field soiling/degradation model output.\n");
			}
			break;
		case CALLING_SIM::EXPLICIT:
			if( !is_design_valid )
			{
				(*error_msg).append("Error: Cannot calculate system costs without a valid solar field design.\n");
				return false;
			}
			break;
		case CALLING_SIM::FINANCE:
			if( !( is_design_valid && is_simulation_valid ) )
			{
				(*error_msg).append(wxString::Format( "Error: Cannot calculate system financial metrics without a valid %s.\n",
													is_design_valid ? "plant performance simulation" : "solar field design").c_str() );

			}
			break;
		case CALLING_SIM::OBJECTIVE:

			break;
		case CALLING_SIM::NULLSIM:

			break;
	}


	return true;
}

void Project::Initialize()
{
	is_design_valid = false;
	is_sf_avail_valid = false;
	is_sf_optical_valid = false;
	is_simulation_valid = false;

	initialize_ssc_project();

    ssc_to_lk_hash(m_ssc_data, m_parameters);
    ssc_to_lk_hash(m_ssc_data, m_variables);
}

Project::Project()
{
	m_ssc_data = 0;
	Initialize();

	//construct the merged data map
	std::vector<lk::varhash_t*> struct_pointers = { &m_variables, &m_parameters, &m_design_outputs,
		&m_solarfield_outputs, &m_optical_outputs, &m_cycle_outputs, &m_simulation_outputs };

	_merged_data.clear();

	for (size_t i = 0; i < struct_pointers.size(); i++)
	{
        lk::varhash_t *this_varhash = struct_pointers.at(i);

        for (lk::varhash_t::iterator it = this_varhash->begin(); it != this_varhash->end(); it++)
		{
            _merged_data[(*it).first] = it->second;
		}
	}

}

Project::~Project()
{
	if (m_ssc_data)
		ssc_data_free(m_ssc_data);
}

lk::varhash_t *Project::GetMergedData()
{
	return &_merged_data;
}

data_base *Project::GetVarPtr(const char *name)
{

	lk::varhash_t::iterator itfind = _merged_data.find(name);

	if (itfind != _merged_data.end())
		return (data_base*)itfind->second;
	else
		return 0;
}

void Project::ssc_to_lk_hash(ssc_data_t &cxt, lk::varhash_t &vars)
{
    for( lk::varhash_t::iterator v = vars.begin(); v != vars.end(); v++ )
    {
        // unsigned char c = v->second->type();
        int datatype = ssc_data_query(cxt, v->first.c_str());

        switch (datatype)
        {
        case SSC_NUMBER:
            ssc_number_t value;
            ssc_data_get_number(cxt, v->first.c_str(), &value);
            v->second->assign(value);
            break;
        case SSC_STRING:
            v->second->assign( ssc_data_get_string(cxt, v->first.c_str()) );
            break;
        case SSC_ARRAY:
        {
            int nv;
            ssc_number_t *arr = ssc_data_get_array(cxt, v->first.c_str(), &nv);
            v->second->vec()->resize(nv);
            for (int i = 0; i < nv; i++)
                v->second->vec()->at(i).assign( arr[i] );

            break;
        }
        case SSC_MATRIX:
        {
            
            int nr, nc;
            ssc_number_t *arr = ssc_data_get_matrix(cxt, v->first.c_str(), &nr, &nc);

            v->second->empty_vector();
            v->second->vec()->resize(nr);

            for (int i = 0; i < nr; i++)
            {
                v->second->vec()->at(i).empty_vector();
                std::vector<lk::vardata_t> *vpi = v->second->vec()->at(i).vec();
                vpi->resize(nc);

                for (int j = 0; j < nc; j++)
                    vpi->at(j).assign(arr[i*nc + j]);
            }
            
        }
        default:
            break;
        }
    }
}

void Project::lk_hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars)
{
    for( lk::varhash_t::iterator v = vars.begin(); v != vars.end(); v++ )
    {
        unsigned char c = v->second->type();

        switch(c)
        {
            case lk::vardata_t::NUMBER:
                ssc_data_set_number(cxt, v->first.c_str(), v->second->as_number() );
            break;
            case lk::vardata_t::STRING:
                ssc_data_set_string(cxt, v->first.c_str(), v->second->as_string().c_str() );
            break;
            // case lk::vardata_t::HASH:
            // {
                
            //     // ssc_data_set_table(cxt, v->first.c_str(), 

            //     break;
            // }
            case lk::vardata_t::VECTOR:
            {
                //vector needs to contain either another vector or a simple type
                unsigned char cc = v->second->vec()->front().type();
                switch(cc)
                {
                    case lk::vardata_t::NUMBER:
                    {
                        size_t nv = v->second->vec()->size();
                        ssc_number_t *tvec = new ssc_number_t[nv];

                        for(size_t i=0; i<nv; i++)
                            tvec[i] = v->second->vec()->at(i).as_number();
                        
                        ssc_data_set_array(cxt, v->first.c_str(), tvec, nv);

                        delete [] tvec;
                        break;
                    }
                    case lk::vardata_t::VECTOR:
                    {
                        size_t nr, nc;
                        nr = v->second->vec()->size();
                        nc = v->second->vec()->front().vec()->size();

                        ssc_number_t *tvec = new ssc_number_t[nr*nc];

                        for( size_t i=0; i<nr; i++ )
                        {
                            std::vector< lk::vardata_t > *cr = v->second->vec()->at(i).vec();
                            for( size_t j=0; j<nc; j++ )
                                tvec[i*nc + j] = cr->at(j).as_number();
                        }

                        ssc_data_set_matrix(cxt, v->first.c_str(), tvec, nr, nc);

                        delete [] tvec;
                        break;
                    }
                    default:
                        //exception
                    break;
                }

                break;
            }
            default:
            break;
        }
    }
}

void Project::update_calculated_system_values()
{
	//"""
	//	Inputs that you can specify in Dict :
	//------------------------------------------------------------ -
	//	D_rec
	//	design_eff
	//	dni_des
	//	gross_net_conversion_factor
	//	N_panels
	//	P_ref
	//	solarm
	//	tshours
	//	helio_optical_error_mrad

	//	Values that are set by this algorithm:
	//------------------------------------------------------------ -
	//	dni_des_calc
	//	n_flux_x(if less than 12)
	//	nameplate
	//	q_design
	//	q_pb_design
	//	Q_rec_des
	//	rec_aspect
	//	field_model_type
	//	system_capacity
	//	tower_technology
	//	tshours_sf
	//	helio_optical_error

	//	"""    

	////net power(only used as an input to system costs compute module)
	//D["nameplate"] = D["P_ref"] * D["gross_net_conversion_factor"]  //MWe
	ssc_number_t gross_net_conversion_factor;
	ssc_data_get_number(m_ssc_data, "gross_net_conversion_factor", &gross_net_conversion_factor);
	ssc_number_t nameplate = m_variables.P_ref.as_number() * gross_net_conversion_factor;
	ssc_data_set_number(m_ssc_data, "nameplate", nameplate);
	//
	//D["system_capacity"] = D["nameplate"] * 1000.
	ssc_data_set_number(m_ssc_data, "system_capacity", nameplate*1000.);

	//// q_pb_design(informational, not used as a compute module input for mspt)
	ssc_number_t q_pb_design = m_variables.P_ref.as_number() / m_variables.design_eff.as_number();
	//D["q_pb_design"] = float(D["P_ref"]) / float(D["design_eff"])
	ssc_data_set_number(m_ssc_data, "q_pb_design", q_pb_design);

	//// Q_rec_des(only used as in input to solarpilot compute module)
	//D["Q_rec_des"] = D["solarm"] * D["q_pb_design"]
	ssc_data_set_number(m_ssc_data, "Q_rec_des", m_variables.solarm.as_number() * q_pb_design);
	//D["q_design"] = D["Q_rec_des"]
	ssc_data_set_number(m_ssc_data, "q_design", m_variables.solarm.as_number() * q_pb_design);
	
	//// tshours_sf(informational, not used as a compute module input)
	//D["tshours_sf"] = D["tshours"] / D["solarm"]

	////receiver aspect ratio(only used as in input to solarpilot compute module)
	//D["rec_aspect"] = float(D["rec_height"]) / float(D["D_rec"]);
	ssc_data_set_number(m_ssc_data, "rec_aspect", m_variables.rec_height.as_number() / m_variables.D_rec.as_number());

	////always set to MSPT
	//D["tower_technology"] = 0
	ssc_data_set_number(m_ssc_data, "tower_technology", 0.);

	////Flux grid resolution limited by number of panels(only used as in input to solarpilot compute module)
	//D["n_flux_x"] = max(12, D["N_panels"])
	ssc_data_set_number(m_ssc_data, "n_flux_x", m_variables.N_panels.as_number() > 12 ? m_variables.N_panels.as_number() : 12);

	//D["field_model_type"] = 2  // 0 = design field and tower / receiver geometry 1 = design field 2 = user field, calculate performance 3 = user performance maps vs solar position
	ssc_data_set_number(m_ssc_data, "field_model_type", 2);

	//D["helio_optical_error"] = D["helio_optical_error_mrad"] / 1000.  // only used as in input to solarpilot compute module
	//ssc_data_set_number(m_ssc_data, "helio_optical_error", )
	ssc_number_t helio_optical_error_mrad;
	ssc_data_get_number(m_ssc_data, "helio_optical_error_mrad", &helio_optical_error_mrad);
	ssc_data_set_number(m_ssc_data, "helio_optical_error", helio_optical_error_mrad/1000.);

}

void Project::update_calculated_values_post_layout()
{
	//#------------solar field (only 'n_hel' and 'A_sf' used as inputs for mspt or system costs compute module)
	//D['n_hel'] = len(D['helio_positions'])
	int N_hel;
	{
		int nc;
		ssc_data_get_matrix(m_ssc_data, "helio_positions", &N_hel, &nc);
		ssc_data_set_number(m_ssc_data, "N_hel", N_hel);
	}
	//
	//D['csp.pt.sf.heliostat_area'] = D['helio_height'] * D['helio_width'] * D['dens_mirror']
	//
	//D['csp.pt.sf.total_reflective_area'] = D['n_hel'] * D['csp.pt.sf.heliostat_area']
	//
	//D['csp.pt.sf.total_land_area'] = D['csp.pt.sf.fixed_land_area'] + D['land_area_base'] * D['csp.pt.sf.land_overhead_factor']
	//
	//D['A_sf'] = D['helio_width'] * D['helio_height'] * D['dens_mirror'] * D['n_hel']
	//D['helio_area_tot'] = D['A_sf']
	//
	//D['field_control'] = 1
	//
	//D['V_wind_10'] = 0
	//
	//#------------parasitics (informational, not used as a compute module input)
	//#parasitic BOP
	//D['csp.pt.par.calc.bop'] = 
	//D['bop_par'] * D['bop_par_f'] * (D['bop_par_0'] + D['bop_par_1'] + 
	//	D['bop_par_2'])*D['P_ref']
	//#Aux parasitic
	//D['csp.pt.par.calc.aux'] = 
	//D['aux_par'] * D['aux_par_f'] * (D['aux_par_0'] + D['aux_par_1'] + 
	//	D['aux_par_2'])*D['P_ref']
	//
	//
	//#------------receiver max mass flow rate (informational, not used as a compute module input)
	//#Receiver average temperature
	//D['csp.pt.rec.htf_t_avg'] = (D['T_htf_cold_des'] + D['T_htf_hot_des']) / 2.
	//
	//#htf specific heat
	//D['csp.pt.rec.htf_c_avg'] = htf_cp(D['csp.pt.rec.htf_t_avg'])
	//
	//#maximum flow rate to the receiver
	//D['csp.pt.rec.max_flow_to_rec'] = 
	//(D['csp.pt.rec.max_oper_frac'] * D['Q_rec_des'] * 1e6) 
	/// (D['csp.pt.rec.htf_c_avg'] * 1e3*(D['T_htf_hot_des'] - D['T_htf_cold_des']))
	//
	//#max flow rate in kg / hr
	//D['m_dot_htf_max'] = D['csp.pt.rec.max_flow_to_rec'] * 60 * 60
	//
	//
	//#------------piping length and piping loss (informational, not used as a compute module input for mspt)
	//#Calculate the thermal piping length
	//D['piping_length'] = D['h_tower'] * D['piping_length_mult'] + D['piping_length_const']
	//
	//#total piping length
	//D['piping_loss_tot'] = D['piping_length'] * D['piping_loss'] / 1000.
	//
	//#------------TES (informational, not used as a compute module input)
	//#update data object with D items
	//D['W_dot_pb_des'] = D['P_ref']       #[MWe]
	//D['eta_pb_des'] = D['design_eff']       #[-]
	//D['tes_hrs'] = D['tshours']       #[hrs]
	//D['T_HTF_hot'] = D['T_htf_hot_des']       #[C]
	//D['T_HTF_cold'] = D['T_htf_cold_des']       #[C]
	//D['TES_HTF_code'] = D['rec_htf']       #[-]
	//D['TES_HTF_props'] = [[]]
	//
	//set_ssc_data_from_dict(ssc_api, ssc_data, D)
	//#use the built in calculations for sizing TES
	//tescalcs = ssc_api.module_create("ui_tes_calcs")
	//ret = ssc_api.module_exec(tescalcs, ssc_data)
	//#---Collect calculated values
	//#TES thermal capacity at design
	//D['q_tes_des'] = ssc_api.data_get_number(ssc_data, 'q_tes_des')
	//#Available single temp storage volume
	//D['vol_one_temp_avail'] = ssc_api.data_get_number(ssc_data, 'vol_one_temp_avail')
	//#Total single temp storage volume
	//D['vol_one_temp_total'] = ssc_api.data_get_number(ssc_data, 'vol_one_temp_total')
	//#Single tank diameter
	//D['d_tank'] = ssc_api.data_get_number(ssc_data, 'd_tank')
	//#Estimated tank heat loss to env.
	//D['q_dot_loss'] = ssc_api.data_get_number(ssc_data, 'q_dot_loss')
	//D['dens'] = ssc_api.data_get_number(ssc_data, 'HTF_dens')
	//ssc_api.module_free(tescalcs)
	//
	//#------------capital costs (informational, not used as a compute module input for mspt)
	//# Receiver Area
	//D['csp.pt.cost.receiver.area'] = D['rec_height'] * D['D_rec'] * 3.1415926
	//
	//# Storage Capacity
	//D['csp.pt.cost.storage_mwht'] = D['P_ref'] / D['design_eff'] * D['tshours']
	//
	//# Total land area
	//D['csp.pt.cost.total_land_area'] = D['csp.pt.sf.total_land_area']
	//
	//D['H_rec'] = D['rec_height']
	//D['csp.pt.cost.power_block_per_kwe'] = D['plant_spec_cost']
	//D['csp.pt.cost.plm.per_acre'] = D['land_spec_cost']
	//D['csp.pt.cost.fixed_sf'] = D['cost_sf_fixed']
	//
	//set_ssc_data_from_dict(ssc_api, ssc_data, D)
	//#init cost
	//cost = ssc_api.module_create("cb_mspt_system_costs")
	//#do cost calcs
	//ret = ssc_api.module_exec(cost, ssc_data)
	//if ret == 0:
	//print "Cost model failed"
	//
	//#collect calculated values
	//D['csp.pt.cost.site_improvements'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.site_improvements')
	//D['csp.pt.cost.heliostats'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.heliostats')
	//D['csp.pt.cost.tower'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.tower')
	//D['csp.pt.cost.receiver'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.receiver')
	//D['csp.pt.cost.storage'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.storage')
	//D['csp.pt.cost.power_block'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.power_block')
	//D['csp.pt.cost.bop'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.bop')
	//D['csp.pt.cost.fossil'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.fossil')
	//D['ui_direct_subtotal'] = ssc_api.data_get_number(ssc_data, 'ui_direct_subtotal')
	//D['csp.pt.cost.contingency'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.contingency')
	//D['total_direct_cost'] = ssc_api.data_get_number(ssc_data, 'total_direct_cost')
	//D['csp.pt.cost.epc.total'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.epc.total')
	//D['csp.pt.cost.plm.total'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.plm.total')
	//D['csp.pt.cost.sales_tax.total'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.sales_tax.total')
	//D['total_indirect_cost'] = ssc_api.data_get_number(ssc_data, 'total_indirect_cost')
	//D['total_installed_cost'] = ssc_api.data_get_number(ssc_data, 'total_installed_cost')
	//D['csp.pt.cost.installed_per_capacity'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.installed_per_capacity')
	//
	//ssc_api.module_free(cost)

}

void Project::initialize_ssc_project()
{
	//free data structures if already allocated
	if (m_ssc_data)
		ssc_data_free(m_ssc_data);

	//create a new data structures
	m_ssc_data = ssc_data_create();

	//set project initial and default values

	ssc_number_t smax = std::numeric_limits<ssc_number_t>::max();
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//       MSPT parameters and defaults
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	
	//initialize some of the variables here. They aren't actually SSC variables..
	ssc_data_set_number( m_ssc_data, "om_staff", 5);
	ssc_data_set_number( m_ssc_data, "n_wash_crews", 5);
	ssc_data_set_number( m_ssc_data, "degr_replace_limit", 0.7);
	//parameters
	ssc_data_set_string( m_ssc_data, "ampl_data_dir", " ");
	ssc_data_set_number( m_ssc_data, "is_stochastic_disp", 0 );
	ssc_data_set_number( m_ssc_data, "q_rec_standby",  9e9 );
	ssc_data_set_number( m_ssc_data, "eta_map_aod_format",  0 );
	ssc_data_set_number( m_ssc_data, "eta_map_aod_format",  0 );
	ssc_data_set_number( m_ssc_data, "ppa_multiplier_model",  0 );
	ssc_data_set_number( m_ssc_data, "field_model_type",  2 );
	ssc_data_set_number( m_ssc_data, "gross_net_conversion_factor",  0.90 );
	ssc_data_set_number( m_ssc_data, "helio_width",  12.2 );
	ssc_data_set_number( m_ssc_data, "helio_height",  12.2 );
	ssc_data_set_number( m_ssc_data, "helio_optical_error_mrad",  1.523 );
	ssc_data_set_number( m_ssc_data, "helio_active_fraction",  0.99 );
	ssc_data_set_number( m_ssc_data, "dens_mirror",  0.97 );
	ssc_data_set_number( m_ssc_data, "helio_reflectance",  0.9 );
	ssc_data_set_number( m_ssc_data, "rec_absorptance",  0.94 );
	ssc_data_set_number( m_ssc_data, "rec_hl_perm2",  30 );
	ssc_data_set_number( m_ssc_data, "land_max",  9.5 );
	ssc_data_set_number( m_ssc_data, "land_min",  0.75 );
	ssc_data_set_number( m_ssc_data, "dni_des",  950 );
	ssc_data_set_number( m_ssc_data, "p_start",  0.025 );
	ssc_data_set_number( m_ssc_data, "p_track",  0.055 );
	ssc_data_set_number( m_ssc_data, "hel_stow_deploy",  8 );
	ssc_data_set_number( m_ssc_data, "v_wind_max",  15 );
	ssc_data_set_number( m_ssc_data, "c_atm_0",  0.006789 );
	ssc_data_set_number( m_ssc_data, "c_atm_1",  0.1046 );
	ssc_data_set_number( m_ssc_data, "c_atm_2",  -0.017 );
	ssc_data_set_number( m_ssc_data, "c_atm_3",  0.002845 );
	ssc_data_set_number( m_ssc_data, "n_facet_x",  2 );
	ssc_data_set_number( m_ssc_data, "n_facet_y",  8 );
	ssc_data_set_number( m_ssc_data, "focus_type",  1 );
	ssc_data_set_number( m_ssc_data, "cant_type",  1 );
	ssc_data_set_number( m_ssc_data, "n_flux_days",  8 );
	ssc_data_set_number( m_ssc_data, "delta_flux_hrs",  2 );
	ssc_data_set_number( m_ssc_data, "water_usage_per_wash",  0.7 );
	ssc_data_set_number( m_ssc_data, "washing_frequency",  63 );
	ssc_data_set_number( m_ssc_data, "check_max_flux",  0 );
	ssc_data_set_number( m_ssc_data, "sf_excess",  1 );
	ssc_data_set_number( m_ssc_data, "tower_fixed_cost",  3000000 );
	ssc_data_set_number( m_ssc_data, "tower_exp",  0.0113 );
	ssc_data_set_number( m_ssc_data, "rec_ref_cost",  103000000 );
	ssc_data_set_number( m_ssc_data, "rec_ref_area",  1571 );
	ssc_data_set_number( m_ssc_data, "rec_cost_exp",  0.7 );
	ssc_data_set_number( m_ssc_data, "site_spec_cost",  16 );
	ssc_data_set_number( m_ssc_data, "heliostat_spec_cost",  145 );
	ssc_data_set_number( m_ssc_data, "plant_spec_cost",  1100 );
	ssc_data_set_number( m_ssc_data, "bop_spec_cost",  340 );
	ssc_data_set_number( m_ssc_data, "tes_spec_cost",  24 );
	ssc_data_set_number( m_ssc_data, "land_spec_cost",  10000 );
	ssc_data_set_number( m_ssc_data, "contingency_rate",  7 );
	ssc_data_set_number( m_ssc_data, "sales_tax_rate",  5 );
	ssc_data_set_number( m_ssc_data, "sales_tax_frac",  80 );
	ssc_data_set_number( m_ssc_data, "cost_sf_fixed",  0 );
	ssc_data_set_number( m_ssc_data, "fossil_spec_cost",  0 );
	ssc_data_set_number( m_ssc_data, "is_optimize",  0 );
	ssc_data_set_number( m_ssc_data, "opt_init_step",  0.06 );
	ssc_data_set_number( m_ssc_data, "opt_max_iter",  200 );
	ssc_data_set_number( m_ssc_data, "opt_conv_tol",  0.001 );
	ssc_data_set_number( m_ssc_data, "opt_flux_penalty",  0.25 );
	ssc_data_set_number( m_ssc_data, "opt_algorithm",  1 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.per_acre",  0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.percent",  13 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.per_watt",  0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.fixed",  0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.plm.percent",  0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.plm.per_watt",  0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.plm.fixed",  0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.sf.fixed_land_area",  45 );
	ssc_data_set_number( m_ssc_data, "csp.pt.sf.land_overhead_factor",  1 );
	ssc_data_set_number( m_ssc_data, "T_htf_cold_des",  290 );
	ssc_data_set_number( m_ssc_data, "T_htf_hot_des",  574 );
	ssc_data_set_number( m_ssc_data, "P_ref",  115 );
	ssc_data_set_number( m_ssc_data, "design_eff",  0.412 );
	ssc_data_set_number( m_ssc_data, "tshours",  10 );
	ssc_data_set_number( m_ssc_data, "solarm",  2.4 );
	ssc_data_set_number( m_ssc_data, "receiver_type",  0 );
	ssc_data_set_number( m_ssc_data, "N_panels",  20 );
	ssc_data_set_number( m_ssc_data, "d_tube_out",  40 );
	ssc_data_set_number( m_ssc_data, "th_tube",  1.25 );
	ssc_data_set_number( m_ssc_data, "mat_tube",  2 );
	ssc_data_set_number( m_ssc_data, "rec_htf",  17 );
	ssc_data_set_number( m_ssc_data, "Flow_type",  1 );
	ssc_data_set_number( m_ssc_data, "epsilon",  0.88 );
	ssc_data_set_number( m_ssc_data, "hl_ffact",  1 );
	ssc_data_set_number( m_ssc_data, "f_rec_min",  0.25 );
	ssc_data_set_number( m_ssc_data, "rec_su_delay",  0.2 );
	ssc_data_set_number( m_ssc_data, "rec_qf_delay",  0.25 );
	ssc_data_set_number( m_ssc_data, "csp.pt.rec.max_oper_frac",  1.2 );
	ssc_data_set_number( m_ssc_data, "eta_pump",  0.85 );
	ssc_data_set_number( m_ssc_data, "piping_loss",  10200 );
	ssc_data_set_number( m_ssc_data, "piping_length_mult",  2.6 );
	ssc_data_set_number( m_ssc_data, "piping_length_const",  0 );
	ssc_data_set_number( m_ssc_data, "rec_d_spec",  15 );
	ssc_data_set_number( m_ssc_data, "csp.pt.tes.init_hot_htf_percent",  30 );
	ssc_data_set_number( m_ssc_data, "h_tank",  20 );
	ssc_data_set_number( m_ssc_data, "cold_tank_max_heat",  15 );
	ssc_data_set_number( m_ssc_data, "u_tank",  0.4 );
	ssc_data_set_number( m_ssc_data, "tank_pairs",  1 );
	ssc_data_set_number( m_ssc_data, "cold_tank_Thtr",  280 );
	ssc_data_set_number( m_ssc_data, "h_tank_min",  1 );
	ssc_data_set_number( m_ssc_data, "hot_tank_Thtr",  500 );
	ssc_data_set_number( m_ssc_data, "hot_tank_max_heat",  30 );
	ssc_data_set_number( m_ssc_data, "tc_fill",  8 );
	ssc_data_set_number( m_ssc_data, "tc_void",  0.25 );
	ssc_data_set_number( m_ssc_data, "t_dis_out_min",  500 );
	ssc_data_set_number( m_ssc_data, "t_ch_out_max",  400 );
	ssc_data_set_number( m_ssc_data, "nodes",  100 );
	ssc_data_set_number( m_ssc_data, "pc_config",  0 );
	ssc_data_set_number( m_ssc_data, "pb_pump_coef",  0.55 );
	ssc_data_set_number( m_ssc_data, "startup_time",  0.5 );
	ssc_data_set_number( m_ssc_data, "startup_frac",  0.5 );
	ssc_data_set_number( m_ssc_data, "cycle_max_frac",  1.05 );
	ssc_data_set_number( m_ssc_data, "cycle_cutoff_frac",  0.2 );
	ssc_data_set_number( m_ssc_data, "q_sby_frac",  0.2 );
	ssc_data_set_number( m_ssc_data, "dT_cw_ref",  10 );
	ssc_data_set_number( m_ssc_data, "T_amb_des",  42 );
	ssc_data_set_number( m_ssc_data, "P_boil",  100 );
	ssc_data_set_number( m_ssc_data, "CT",  2 );
	ssc_data_set_number( m_ssc_data, "T_approach",  5 );
	ssc_data_set_number( m_ssc_data, "T_ITD_des",  16 );
	ssc_data_set_number( m_ssc_data, "P_cond_ratio",  1.0028 );
	ssc_data_set_number( m_ssc_data, "pb_bd_frac",  0.02 );
	ssc_data_set_number( m_ssc_data, "P_cond_min",  2 );
	ssc_data_set_number( m_ssc_data, "n_pl_inc",  8 );
	ssc_data_set_number( m_ssc_data, "tech_type",  1 );
	ssc_data_set_number( m_ssc_data, "time_start",  0 );
	ssc_data_set_number( m_ssc_data, "time_stop",  31536000 );
	ssc_data_set_number( m_ssc_data, "pb_fixed_par",  0.0055 );
	ssc_data_set_number( m_ssc_data, "aux_par",  0.023 );
	ssc_data_set_number( m_ssc_data, "aux_par_f",  1 );
	ssc_data_set_number( m_ssc_data, "aux_par_0",  0.483 );
	ssc_data_set_number( m_ssc_data, "aux_par_1",  0.571 );
	ssc_data_set_number( m_ssc_data, "aux_par_2",  0 );
	ssc_data_set_number( m_ssc_data, "bop_par",  0 );
	ssc_data_set_number( m_ssc_data, "bop_par_f",  1 );
	ssc_data_set_number( m_ssc_data, "bop_par_0",  0 );
	ssc_data_set_number( m_ssc_data, "bop_par_1",  0.483 );
	ssc_data_set_number( m_ssc_data, "bop_par_2",  0 );
	ssc_data_set_number( m_ssc_data, "is_dispatch",  0 );
	ssc_data_set_number( m_ssc_data, "disp_horizon",  48 );
	ssc_data_set_number( m_ssc_data, "disp_frequency",  24 );
	ssc_data_set_number( m_ssc_data, "disp_max_iter",  35000 );
	ssc_data_set_number( m_ssc_data, "disp_timeout",  5 );
	ssc_data_set_number( m_ssc_data, "disp_mip_gap",  0.001 );
	ssc_data_set_number( m_ssc_data, "disp_time_weighting",  0.99 );
	ssc_data_set_number( m_ssc_data, "disp_rsu_cost",  950 );
	ssc_data_set_number( m_ssc_data, "disp_csu_cost",  10000 );
	ssc_data_set_number( m_ssc_data, "disp_pen_delta_w",  0.1 );
	ssc_data_set_number( m_ssc_data, "is_wlim_series",  0 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor1",  2.064 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor2",  1.2 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor3",  1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor4",  1.1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor5",  0.8 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor6",  0.7 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor7",  1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor8",  1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor9",  1 );
	ssc_data_set_number( m_ssc_data, "is_dispatch_series",  0 );
	ssc_data_set_number( m_ssc_data, "rec_height",  21.6 );
	ssc_data_set_number( m_ssc_data, "D_rec",  17.65 );
	ssc_data_set_number( m_ssc_data, "h_tower",  193.46 );
	ssc_data_set_number( m_ssc_data, "land_area_base",  1847.037 );
	ssc_data_set_number( m_ssc_data, "const_per_interest_rate1",  4 );
	ssc_data_set_number( m_ssc_data, "const_per_interest_rate2",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_interest_rate3",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_interest_rate4",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_interest_rate5",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_months1",  24 );
	ssc_data_set_number( m_ssc_data, "const_per_months2",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_months3",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_months4",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_months5",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_percent1",  100 );
	ssc_data_set_number( m_ssc_data, "const_per_percent2",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_percent3",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_percent4",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_percent5",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_upfront_rate1",  1 );
	ssc_data_set_number( m_ssc_data, "const_per_upfront_rate2",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_upfront_rate3",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_upfront_rate4",  0 );
	ssc_data_set_number( m_ssc_data, "const_per_upfront_rate5",  0 );
	ssc_data_set_number( m_ssc_data, "adjust:constant",  4 );
	ssc_data_set_number( m_ssc_data, "sf_adjust:constant",  0 );
	ssc_data_set_number( m_ssc_data, "ud_T_amb_des",  43 );
	ssc_data_set_number( m_ssc_data, "ud_f_W_dot_cool_des",  0 );
	ssc_data_set_number( m_ssc_data, "ud_m_dot_water_cool_des",  0 );
	ssc_data_set_number( m_ssc_data, "ud_T_htf_low",  500 );
	ssc_data_set_number( m_ssc_data, "ud_T_htf_high",  580 );
	ssc_data_set_number( m_ssc_data, "ud_T_amb_low",  0 );
	ssc_data_set_number( m_ssc_data, "ud_T_amb_high",  55 );
	ssc_data_set_matrix(m_ssc_data, "fc_dni_scenarios", 0, 0, 0 );
	ssc_data_set_matrix(m_ssc_data, "fc_price_scenarios", 0, 0, 0 );
	ssc_data_set_matrix(m_ssc_data, "fc_tamb_scenarios", 0, 0, 0 );
	ssc_number_t p_F_wc[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ssc_data_set_array(m_ssc_data, "F_wc", p_F_wc, 9 );
	ssc_number_t p_field_fl_props[9] = { 1, 7, 0, 0, 0, 0, 0, 0, 0 };
	ssc_data_set_matrix(m_ssc_data, "field_fl_props", p_field_fl_props, 1, 9 );
	ssc_number_t p_f_turb_tou_periods[9] = { 1.05, 1, 1, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_array(m_ssc_data, "f_turb_tou_periods", p_f_turb_tou_periods, 9 );
	ssc_number_t p_weekday_schedule[288] = 
	{ 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
		3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 
		3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 
		3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 
		3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
		6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5 
	};
	ssc_number_t p_weekend_schedule[288] =
	{
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
	};
	ssc_data_set_matrix(m_ssc_data, "weekday_schedule", p_weekday_schedule, 12, 24 );
	ssc_data_set_matrix(m_ssc_data, "weekend_schedule", p_weekend_schedule, 12, 24 );
	ssc_data_set_matrix(m_ssc_data, "dispatch_sched_weekday", p_weekday_schedule, 12, 24);
	ssc_data_set_matrix(m_ssc_data, "dispatch_sched_weekend", p_weekend_schedule, 12, 24);
	ssc_number_t p_dispatch_series[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "dispatch_series", p_dispatch_series, 1 );
	ssc_data_set_number(m_ssc_data, "sf_adjust:constant", 0 );
	ssc_number_t p_sf_adjustperiods[1] = { 0 };
	ssc_data_set_matrix(m_ssc_data, "sf_adjust:periods", p_sf_adjustperiods, 1, 1 );
	ssc_number_t p_sf_adjusthourly[8760];
	for (size_t ii = 0; ii < 8760; ii++)
		p_sf_adjusthourly[ii] = 0.;
	ssc_data_set_array(m_ssc_data, "sf_adjust:hourly", p_sf_adjusthourly, 8760 );
	ssc_number_t p_dispatch_factors_ts[8760];
	for (size_t ii = 0; ii < 8760; ii++)
		p_dispatch_factors_ts[ii] = 1.;
	ssc_data_set_array(m_ssc_data, "dispatch_factors_ts", p_dispatch_factors_ts, 8760 );
	ssc_number_t p_wlim_series[8760];
	for (size_t ii = 0; ii < 8760; ii++)
		p_wlim_series[ii] = 1.e37;
	ssc_data_set_array(m_ssc_data, "wlim_series", p_wlim_series, 8760 );
	ssc_number_t p_helio_positions[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "helio_positions", p_helio_positions, 1 );


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//		       Financial parameters and defaults
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------

	ssc_data_set_number(m_ssc_data, "analysis_period", 25 );
	ssc_number_t p_federal_tax_rate[1] = { 35 };
	ssc_data_set_array(m_ssc_data, "federal_tax_rate", p_federal_tax_rate, 1 );
	ssc_number_t p_state_tax_rate[1] = { 7 };
	ssc_data_set_array(m_ssc_data, "state_tax_rate", p_state_tax_rate, 1 );
	ssc_data_set_number(m_ssc_data, "property_tax_rate", 0 );
	ssc_data_set_number(m_ssc_data, "prop_tax_cost_assessed_percent", 100 );
	ssc_data_set_number(m_ssc_data, "prop_tax_assessed_decline", 0 );
	ssc_data_set_number(m_ssc_data, "real_discount_rate", 5.5 );
	ssc_data_set_number(m_ssc_data, "inflation_rate", 2.5 );
	ssc_data_set_number(m_ssc_data, "insurance_rate", 0.5 );
	ssc_data_set_number(m_ssc_data, "system_capacity", 103500 );
	ssc_number_t p_om_fixed[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "om_fixed", p_om_fixed, 1 );
	ssc_data_set_number(m_ssc_data, "om_fixed_escal", 0 );
	ssc_number_t p_om_production[1] = { 3.5 };
	ssc_data_set_array(m_ssc_data, "om_production", p_om_production, 1 );
	ssc_data_set_number(m_ssc_data, "om_production_escal", 0 );
	ssc_number_t p_om_capacity[1] = { 66 };
	ssc_data_set_array(m_ssc_data, "om_capacity", p_om_capacity, 1 );
	ssc_data_set_number(m_ssc_data, "om_capacity_escal", 0 );
	ssc_number_t p_om_fuel_cost[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "om_fuel_cost", p_om_fuel_cost, 1 );
	ssc_data_set_number(m_ssc_data, "om_fuel_cost_escal", 0 );
	ssc_data_set_number(m_ssc_data, "itc_fed_amount", 0 );
	ssc_data_set_number(m_ssc_data, "itc_fed_amount_deprbas_fed", 1 );
	ssc_data_set_number(m_ssc_data, "itc_fed_amount_deprbas_sta", 1 );
	ssc_data_set_number(m_ssc_data, "itc_sta_amount", 0 );
	ssc_data_set_number(m_ssc_data, "itc_sta_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "itc_sta_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "itc_fed_percent", 30 );
	ssc_data_set_number(m_ssc_data, "itc_fed_percent_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "itc_fed_percent_deprbas_fed", 1 );
	ssc_data_set_number(m_ssc_data, "itc_fed_percent_deprbas_sta", 1 );
	ssc_data_set_number(m_ssc_data, "itc_sta_percent", 0 );
	ssc_data_set_number(m_ssc_data, "itc_sta_percent_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "itc_sta_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "itc_sta_percent_deprbas_sta", 0 );
	ssc_number_t p_ptc_fed_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "ptc_fed_amount", p_ptc_fed_amount, 1 );
	ssc_data_set_number(m_ssc_data, "ptc_fed_term", 10 );
	ssc_data_set_number(m_ssc_data, "ptc_fed_escal", 0 );
	ssc_number_t p_ptc_sta_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "ptc_sta_amount", p_ptc_sta_amount, 1 );
	ssc_data_set_number(m_ssc_data, "ptc_sta_term", 10 );
	ssc_data_set_number(m_ssc_data, "ptc_sta_escal", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_amount", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_amount", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_amount", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_amount", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_percent", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_percent_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "ibi_fed_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_fed_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_percent", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_percent_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "ibi_sta_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_sta_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_percent", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_percent_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "ibi_uti_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_uti_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_percent", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_percent_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "ibi_oth_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "ibi_oth_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_fed_amount", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_fed_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "cbi_fed_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_fed_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_fed_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_fed_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_sta_amount", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_sta_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "cbi_sta_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_sta_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_sta_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_sta_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_uti_amount", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_uti_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "cbi_uti_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_uti_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_uti_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_uti_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_oth_amount", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_oth_maxvalue", smax );
	ssc_data_set_number(m_ssc_data, "cbi_oth_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_oth_tax_sta", 1 );
	ssc_data_set_number(m_ssc_data, "cbi_oth_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_data, "cbi_oth_deprbas_sta", 0 );
	ssc_number_t p_pbi_fed_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "pbi_fed_amount", p_pbi_fed_amount, 1 );
	ssc_data_set_number(m_ssc_data, "pbi_fed_term", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_fed_escal", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_fed_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "pbi_fed_tax_sta", 1 );
	ssc_number_t p_pbi_sta_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "pbi_sta_amount", p_pbi_sta_amount, 1 );
	ssc_data_set_number(m_ssc_data, "pbi_sta_term", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_sta_escal", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_sta_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "pbi_sta_tax_sta", 1 );
	ssc_number_t p_pbi_uti_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "pbi_uti_amount", p_pbi_uti_amount, 1 );
	ssc_data_set_number(m_ssc_data, "pbi_uti_term", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_uti_escal", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_uti_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "pbi_uti_tax_sta", 1 );
	ssc_number_t p_pbi_oth_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "pbi_oth_amount", p_pbi_oth_amount, 1 );
	ssc_data_set_number(m_ssc_data, "pbi_oth_term", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_oth_escal", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_oth_tax_fed", 1 );
	ssc_data_set_number(m_ssc_data, "pbi_oth_tax_sta", 1 );
	ssc_number_t p_degradation[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "degradation", p_degradation, 1 );
	ssc_number_t p_roe_input[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "roe_input", p_roe_input, 1 );
	ssc_data_set_number(m_ssc_data, "loan_moratorium", 0 );
	ssc_data_set_number(m_ssc_data, "system_use_recapitalization", 0 );
	ssc_data_set_number(m_ssc_data, "system_use_lifetime_output", 0 );
	ssc_data_set_number(m_ssc_data, "total_installed_cost", 704234496 );
	ssc_data_set_number(m_ssc_data, "reserves_interest", 1.75 );
	ssc_data_set_number(m_ssc_data, "equip1_reserve_cost", 0 );
	ssc_data_set_number(m_ssc_data, "equip1_reserve_freq", 12 );
	ssc_data_set_number(m_ssc_data, "equip2_reserve_cost", 0 );
	ssc_data_set_number(m_ssc_data, "equip2_reserve_freq", 15 );
	ssc_data_set_number(m_ssc_data, "equip3_reserve_cost", 0 );
	ssc_data_set_number(m_ssc_data, "equip3_reserve_freq", 3 );
	ssc_data_set_number(m_ssc_data, "equip_reserve_depr_sta", 0 );
	ssc_data_set_number(m_ssc_data, "equip_reserve_depr_fed", 0 );
	ssc_data_set_number(m_ssc_data, "salvage_percentage", 0 );
	ssc_data_set_number(m_ssc_data, "ppa_soln_mode", 0 );
	ssc_data_set_number(m_ssc_data, "ppa_price_input", 0.12999999523162842 );
	ssc_data_set_number(m_ssc_data, "ppa_escalation", 1 );
	ssc_data_set_number(m_ssc_data, "construction_financing_cost", 35211724 );
	ssc_data_set_number(m_ssc_data, "term_tenor", 18 );
	ssc_data_set_number(m_ssc_data, "term_int_rate", 7 );
	ssc_data_set_number(m_ssc_data, "dscr", 1.2999999523162842 );
	ssc_data_set_number(m_ssc_data, "dscr_reserve_months", 6 );
	ssc_data_set_number(m_ssc_data, "debt_percent", 50 );
	ssc_data_set_number(m_ssc_data, "debt_option", 1 );
	ssc_data_set_number(m_ssc_data, "payment_option", 0 );
	ssc_data_set_number(m_ssc_data, "cost_debt_closing", 450000 );
	ssc_data_set_number(m_ssc_data, "cost_debt_fee", 2.75 );
	ssc_data_set_number(m_ssc_data, "months_working_reserve", 6 );
	ssc_data_set_number(m_ssc_data, "months_receivables_reserve", 0 );
	ssc_data_set_number(m_ssc_data, "cost_other_financing", 0 );
	ssc_data_set_number(m_ssc_data, "flip_target_percent", 11 );
	ssc_data_set_number(m_ssc_data, "flip_target_year", 20 );
	ssc_data_set_number(m_ssc_data, "depr_alloc_macrs_5_percent", 90 );
	ssc_data_set_number(m_ssc_data, "depr_alloc_macrs_15_percent", 1.5 );
	ssc_data_set_number(m_ssc_data, "depr_alloc_sl_5_percent", 0 );
	ssc_data_set_number(m_ssc_data, "depr_alloc_sl_15_percent", 2.5 );
	ssc_data_set_number(m_ssc_data, "depr_alloc_sl_20_percent", 3 );
	ssc_data_set_number(m_ssc_data, "depr_alloc_sl_39_percent", 0 );
	ssc_data_set_number(m_ssc_data, "depr_alloc_custom_percent", 0 );
	ssc_number_t p_depr_custom_schedule[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "depr_custom_schedule", p_depr_custom_schedule, 1 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta_macrs_5", 1 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta_macrs_15", 1 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta_sl_5", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta_sl_15", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta_sl_20", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta_sl_39", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_sta_custom", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed_macrs_5", 1 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed_macrs_15", 1 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed_sl_5", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed_sl_15", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed_sl_20", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed_sl_39", 0 );
	ssc_data_set_number(m_ssc_data, "depr_bonus_fed_custom", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_sta_macrs_5", 1 );
	ssc_data_set_number(m_ssc_data, "depr_itc_sta_macrs_15", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_sta_sl_5", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_sta_sl_15", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_sta_sl_20", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_sta_sl_39", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_sta_custom", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_fed_macrs_5", 1 );
	ssc_data_set_number(m_ssc_data, "depr_itc_fed_macrs_15", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_fed_sl_5", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_fed_sl_15", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_fed_sl_20", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_fed_sl_39", 0 );
	ssc_data_set_number(m_ssc_data, "depr_itc_fed_custom", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_fed_for_ds", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_sta_for_ds", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_uti_for_ds", 0 );
	ssc_data_set_number(m_ssc_data, "pbi_oth_for_ds", 0 );
	ssc_data_set_number(m_ssc_data, "depr_stabas_method", 1 );
	ssc_data_set_number(m_ssc_data, "depr_fedbas_method", 1 );

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//		       Construction financing parameters and defaults
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	ssc_data_set_number(m_ssc_data, "total_installed_cost", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_interest_rate1", 4. );
	ssc_data_set_number(m_ssc_data, "const_per_interest_rate2", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_interest_rate3", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_interest_rate4", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_interest_rate5", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_months1", 24 );
	ssc_data_set_number(m_ssc_data, "const_per_months2", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_months3", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_months4", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_months5", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_percent1", 100. );
	ssc_data_set_number(m_ssc_data, "const_per_percent2", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_percent3", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_percent4", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_percent5", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_upfront_rate1", 1. );
	ssc_data_set_number(m_ssc_data, "const_per_upfront_rate2", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_upfront_rate3", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_upfront_rate4", 0. );
	ssc_data_set_number(m_ssc_data, "const_per_upfront_rate5", 0. );

}

double Project::calc_real_dollars(const double &dollars, bool is_revenue, bool is_labor)
{
	/* 
	Calculate lifetime dollar amount expressed in today's dollar value.
	Account for interest on debt and closing costs if not revenue or labor.
	The effective interest rate subtracts inflation.
	Assume labor is indexed to inflation.
	*/

	ssc_number_t inflation_rate; 
	ssc_data_get_number(m_ssc_data, "inflation_rate", &inflation_rate);
	inflation_rate *= .01;
	
	if (is_revenue)
	{
		ssc_number_t ppa_escalation; 
		ssc_data_get_number(m_ssc_data, "ppa_escalation", &ppa_escalation);
		ppa_escalation *= .01;

		double r = 1. + ppa_escalation - inflation_rate;

		double c = 0.;
		double cy = dollars;

		for (int i = 0; i < m_parameters.plant_lifetime.as_integer(); i++)
		{
			cy *= r;
			c += cy;
		}

		return c;
	}
	else if (is_labor)
	{
		return dollars * m_parameters.plant_lifetime.as_integer();
	}
	else
	{
		int analysis_period = m_parameters.finance_period.as_integer();

		ssc_number_t debt_percent;
		ssc_data_get_number(m_ssc_data, "debt_percent", &debt_percent);
		debt_percent *= .01;

		ssc_number_t cost_debt_fee;
		ssc_data_get_number(m_ssc_data, "cost_debt_fee", &cost_debt_fee);
		cost_debt_fee *= .01;

		ssc_number_t term_int_rate;
		ssc_data_get_number(m_ssc_data, "term_int_rate", &term_int_rate);
		term_int_rate *= .01;

		double pv = dollars * debt_percent; // present value of debt
		double dp = pv * cost_debt_fee;		// debt financing cost

		double r = (term_int_rate - inflation_rate);	// annual interest rate
		int n = analysis_period;

		double c = r * pv / (1. - pow(1. + r, -n)) * n;

		double tot = c + (dollars - pv) + dp;

		return tot;
	}

}


bool Project::D()
{
	/*
	run only the design
	
	usr_vars is a dict containing name : value pairs.The name must be a valid
	ssc variable from vartab.py, and corresponding value must match the var
	type given in the same file.
	*/
	
	ssc_module_exec_set_print(m_parameters.print_messages.as_boolean()); //0 = no, 1 = yes(print progress updates)
	
	//change any defaults
	ssc_data_set_number(m_ssc_data, "calc_fluxmaps", 1.);
	
	//#Check to make sure the weather file exists
	FILE *fp = fopen(m_parameters.solar_resource_file.as_string().c_str(), "r");
	if (fp == NULL)
	{
		message_handler(wxString::Format("The solar resource file could not be located (Design module). The specified path is:\n%s", 
            m_parameters.solar_resource_file.as_string().c_str()).c_str());
		return false;;
	}
	fclose(fp);

	update_calculated_system_values();
	ssc_data_set_matrix(m_ssc_data, "heliostat_positions_in", (ssc_number_t*)0, 0, 0);
	
    lk_hash_to_ssc(m_ssc_data, m_variables);
    lk_hash_to_ssc(m_ssc_data, m_parameters);

	//Run design to get field layout
	ssc_module_t mod_solarpilot = ssc_module_create("solarpilot");
	ssc_module_exec_with_handler(mod_solarpilot, m_ssc_data, ssc_progress_handler, 0);
	
	//Collect calculated data
    ssc_to_lk_hash(m_ssc_data, m_design_outputs);
	
	ssc_data_set_number(m_ssc_data, "calc_fluxmaps", 0.);
	
	//update values
	{
		int nr, nc;
		ssc_number_t *p_hel = ssc_data_get_matrix(m_ssc_data, "heliostat_positions", &nr, &nc);
		ssc_data_set_matrix(m_ssc_data, "helio_positions", p_hel, nr, nc);
	}
	ssc_number_t val;
	ssc_data_get_number(m_ssc_data, "area_sf", &val);
	ssc_data_set_number(m_ssc_data, "A_sf", val);

	//a successful layout will have solar field area
	if (val < 0.1 || val == std::numeric_limits<double>::quiet_NaN())
		return false;
	
	ssc_data_get_number(m_ssc_data, "base_land_area", &val);
	ssc_data_set_number(m_ssc_data, "land_area_base", val);
	
	update_calculated_values_post_layout();
	// use this layout
	ssc_data_set_number(m_ssc_data, "field_model_type", 2.);
	
	ssc_module_free(mod_solarpilot);

	//assign outputs and return
	
	//Land cost
	m_design_outputs.cost_land_real.assign( calc_real_dollars( m_parameters.land_spec_cost.as_number() * m_design_outputs.land_area.as_number() ) ); 
	//solar field cost
	m_design_outputs.cost_sf_real.assign( calc_real_dollars( (m_parameters.heliostat_spec_cost.as_number() + m_parameters.site_spec_cost.as_number())*m_design_outputs.area_sf.as_number() ) );

	is_design_valid = true;
	return true;
}


bool Project::M()
{
	/*
	The heliostat field availability problem

	Returns a dict with keys :
	avail_steady            Steady - state availability
	n_repairs               Total number of repairs made per year
	staff_utilization       Fractional staff utilization factor(1. = all staff always working)
	heliostat_repair_cost   Total cost of heliostat repairs($ lifetime)
	heliostat_repair_cost_y1 Total "" in first year($ / year)
	*/

	solarfield_availability sfa;

	sfa.m_settings.mf = m_parameters.helio_mtf.as_number();
	sfa.m_settings.rep_min = 1.;
	sfa.m_settings.rep_max = 100.;
	sfa.m_settings.n_helio = m_design_outputs.number_heliostats.as_integer();
	sfa.m_settings.n_om_staff = m_variables.om_staff.as_integer();
	sfa.m_settings.hr_prod = m_parameters.om_staff_max_hours_week.as_number();
	sfa.m_settings.n_hr_sim = 8760 * 12;
	sfa.m_settings.seed = m_parameters.avail_seed.as_integer();
	sfa.m_settings.n_helio_sim = 1000;

	//error if trying to simulate with no heliostats
	if (m_design_outputs.number_heliostats.as_integer() <= 1)
	{
		message_handler("Error: Empty layout in field availability simulation.");
		return false;
	}

	sfa.simulate(sim_progress_handler);

	//Calculate staff cost and repair cost
	double ann_fact = 8760. / (double)sfa.m_settings.n_hr_sim;

	sfa.m_results.n_repairs *= ann_fact;	//annual repairs
	sfa.m_results.heliostat_repair_cost_y1 = sfa.m_results.n_repairs * m_parameters.heliostat_repair_cost.as_number();
	
	//lifetime costs
	//treat heliostat repair costs as consuming reserve equipment paid for at the project outset
	sfa.m_results.heliostat_repair_cost = calc_real_dollars(sfa.m_results.heliostat_repair_cost_y1);

    //assign outputs to project structure
    m_solarfield_outputs.n_repairs.assign( sfa.m_results.n_repairs );
    m_solarfield_outputs.staff_utilization.assign( sfa.m_results.staff_utilization );
    m_solarfield_outputs.heliostat_repair_cost_y1.assign( sfa.m_results.heliostat_repair_cost_y1 );
    m_solarfield_outputs.heliostat_repair_cost.assign( sfa.m_results.heliostat_repair_cost );
    m_solarfield_outputs.avail_schedule.assign_vector( sfa.m_results.avail_schedule, sfa.m_results.n_avail_schedule );

	is_sf_avail_valid = true;
	return true;
}

bool Project::O()
{
	/*
	The heliostat field soiling and degradation problem

	Returns a dict with keys :
	soil_steady             Steady - state availability
	n_repairs               Total number of repairs made
	heliostat_refurbish_cost  Cost to refurbish heliostats($ lifetime)
	heliostat_refurbish_cost_y1  Cost "" in year 1 ($ / year)
	*/
	optical_degradation od;

	od.m_settings.n_hr_sim = 25 * 8760;
	od.m_settings.n_wash_crews = m_variables.n_wash_crews.as_integer();
	od.m_settings.n_helio = m_design_outputs.number_heliostats.as_integer();
	od.m_settings.degr_loss_per_hr = m_parameters.degr_per_hour.as_number();
	od.m_settings.degr_accel_per_year = m_parameters.degr_accel_per_year.as_number();
	od.m_settings.replacement_threshold = m_variables.degr_replace_limit.as_number();
	od.m_settings.soil_loss_per_hr = m_parameters.soil_per_hour.as_number();
	od.m_settings.wash_units_per_hour = m_parameters.wash_units_per_hour.as_number();
	od.m_settings.hours_per_week = m_parameters.wash_crew_max_hours_week.as_number();
	od.m_settings.hours_per_day = 10.;
	od.m_settings.seed = m_parameters.degr_seed.as_integer();

	od.simulate(sim_progress_handler);

	double ann_fact = 8760. / (double)od.m_settings.n_hr_sim;
	
	od.m_results.heliostat_refurbish_cost_y1 =
		od.m_results.n_replacements * m_parameters.heliostat_refurbish_cost.as_number();
	
	//Annualize
	od.m_results.n_replacements *= ann_fact;
	
	od.m_results.heliostat_refurbish_cost = calc_real_dollars( od.m_results.heliostat_refurbish_cost_y1 ) * ann_fact;

    //assign results to structure
    m_optical_outputs.n_replacements.assign(od.m_results.n_replacements);
    m_optical_outputs.heliostat_refurbish_cost.assign(od.m_results.heliostat_refurbish_cost);
    m_optical_outputs.heliostat_refurbish_cost_y1.assign(od.m_results.heliostat_refurbish_cost_y1);
    m_optical_outputs.avg_soil.assign(od.m_results.avg_soil);
    m_optical_outputs.avg_degr.assign(od.m_results.avg_degr);

    m_optical_outputs.soil_schedule.assign_vector( od.m_results.soil_schedule, od.m_results.n_schedule );
    m_optical_outputs.degr_schedule.assign_vector( od.m_results.degr_schedule, od.m_results.n_schedule );
    m_optical_outputs.repl_schedule.assign_vector( od.m_results.repl_schedule, od.m_results.n_schedule );
    m_optical_outputs.repl_total.assign_vector( od.m_results.repl_total, od.m_results.n_schedule );

	is_sf_optical_valid = true;
	return true;
}

bool Project::S()
{
	/* 
	
	*/

	std::vector<double> sfavail(m_parameters.user_sf_avail.vec()->size());
	for(int i=0; i<sfavail.size(); i++)
		sfavail.at(i) = m_parameters.user_sf_avail.vec()->at(i).as_number();

	is_simulation_valid = simulate_system(m_cluster_parameters, sfavail );
	
	return is_simulation_valid;
}

void Project::setup_clusters(const project_cluster_inputs &user_inputs, const std::vector<double> &sfavail, s_metric_outputs &metric_results, s_cluster_outputs &cluster_results)
{

	//--- Set default inputs for all parameters
	clustering_metrics metrics;
	cluster_alg cluster;
	metrics.set_default_inputs();
	cluster.set_default_inputs();

	//--- Set weather / price / solar field availability inputs
	metrics.inputs.weather_files.clear();
	std::string weatherfile = m_parameters.solar_resource_file.as_string();
	metrics.inputs.weather_files.push_back(weatherfile);
	metrics.inputs.is_price_files = false;
    metrics.inputs.prices.clear();
    metrics.inputs.prices.resize( m_parameters.dispatch_factors_ts.vec()->size() );
    for (size_t i = 0; i < m_parameters.dispatch_factors_ts.vec()->size(); i++)
        metrics.inputs.prices.at(i) = m_parameters.dispatch_factors_ts.vec()->at(i).as_number();

	metrics.inputs.sfavail = sfavail;
	metrics.inputs.stowlimit = m_parameters.v_wind_max.as_number();


	//--- Update user-specified inputs if defined
	if (user_inputs.nsim >-1)
		metrics.inputs.nsimdays = user_inputs.nsim;

	if (user_inputs.ncluster >-1)
		cluster.inputs.ncluster = user_inputs.ncluster;

	if (user_inputs.alg >-1)
		cluster.inputs.alg = user_inputs.alg;

	if (!user_inputs.hard_partitions)
		cluster.inputs.hard_partitions = user_inputs.hard_partitions;


	//--- Calculate metrics and create clusters
	metrics.calc_metrics();
	metric_results = metrics.results;

	cluster.create_clusters(metrics.results.data);
	cluster_results = cluster.results;


	return;
}


bool Project::simulate_system(const project_cluster_inputs &user_inputs, const std::vector<double> &sfavail)
{

	int nr, nc;

	//--- Set time step size from weather file
	int nrec = 0;
	FILE *fp = fopen(m_parameters.solar_resource_file.as_string().c_str(), "r");
	char *line;
	char buffer[1024];
	while ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)
		nrec += 1;
	fclose(fp);
	nrec -= 3;
	int wf_steps_per_hour = nrec / 8760;
	int nperday = wf_steps_per_hour * 24;

	//--- Set solar field availability
	nr = (int)sfavail.size();
	ssc_number_t *p_sf = new ssc_number_t[nr];
	for (int i = 0; i<nr; i++)
		p_sf[i] = 100.*(1. - sfavail.at(i));
	ssc_data_set_array(m_ssc_data, "sf_adjust:hourly", p_sf, nr);

	//--- Set ssc parameters
	ssc_number_t val, nhel, helio_height, helio_width, dens_mirror;
	ssc_data_set_number(m_ssc_data, "flux_max", 1000.0);
	ssc_data_set_number(m_ssc_data, "field_model_type", 3);
	ssc_data_set_number(m_ssc_data, "is_ampl_engine", m_parameters.is_ampl_engine.as_boolean());
	if (m_parameters.is_ampl_engine.as_boolean())
		ssc_data_set_string(m_ssc_data, "ampl_data_dir", m_parameters.ampl_data_dir.as_string().c_str());
	else
		ssc_data_set_string(m_ssc_data, "ampl_data_dir", (const char*)(""));		

	ssc_data_get_number(m_ssc_data, "helio_height", &helio_height);
	ssc_data_get_number(m_ssc_data, "helio_width", &helio_width);
	ssc_data_get_number(m_ssc_data, "dens_mirror", &dens_mirror);
	ssc_data_get_number(m_ssc_data, "number_heliostats", &nhel);
	ssc_data_set_number(m_ssc_data, "A_sf_in", helio_height*helio_width*dens_mirror*nhel);
	ssc_data_set_number(m_ssc_data, "N_hel", nhel);
	

	ssc_data_get_number(m_ssc_data, "base_land_area", &val);
	ssc_data_set_number(m_ssc_data, "land_area_base", val);

	ssc_number_t *p_fluxmap = ssc_data_get_matrix(m_ssc_data, "flux_table", &nr, &nc);
	ssc_data_set_matrix(m_ssc_data, "flux_maps", p_fluxmap, nr, nc);

	ssc_number_t *p_opt = ssc_data_get_matrix(m_ssc_data, "opteff_table", &nr, &nc);
	ssc_number_t *p_fluxpos = new ssc_number_t[nr * 2];
	for (int r = 0; r < nr; r++)
	{
		p_opt[r*nc] += 180;
		p_fluxpos[r * 2] = p_opt[r*nc];
		p_fluxpos[r * 2 + 1] = p_opt[r*nc + 1];
	}
	ssc_data_set_matrix(m_ssc_data, "eta_map", p_opt, nr, nc);
	ssc_data_set_matrix(m_ssc_data, "flux_positions", p_fluxpos, nr, 2);


	ssc_module_exec_set_print(m_parameters.print_messages.as_boolean()); 
	ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");


	

	//--- Initialize results
	double annual_generation, revenue_units, total_installed_cost, system_capacity;
	int n_hourly_keys = 5;
	std::string hourly_keys[] = { "gen",  "Q_thermal", "e_ch_tes", "beam", "pricing_mult" };
	unordered_map < std::string, std::vector<double>> collect_ssc_data, full_data;
	for (int i = 0; i < n_hourly_keys; i++)
	{
		std::string key = hourly_keys[i];
		collect_ssc_data[key].assign(nrec, 0.0);
		full_data[key].assign(nrec, 0.0);
	}

	//--- Run full simulation
	if (user_inputs.is_run_full)
	{
		ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0);

		for (int i = 0; i < n_hourly_keys; i++)
		{
			std::string key = hourly_keys[i];
			ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
			for (int r = 0; r < nr; r++)
				full_data[key].at(r) = p_data[r];
		}

	}
	
	//--- Run simulations of only cluster exemplars
	else
	{

		//--- Set default inputs for all clustering parameters
		s_metric_outputs metric_results;
		cluster_sim csim;
		csim.set_default_inputs();
		csim.inputs.days.nnext = int(m_parameters.is_dispatch.as_boolean());

		//--- Update with user-defined inputs if defined
		csim.inputs.is_run_continuous = user_inputs.is_run_continuous;
		if (user_inputs.nsim > -1)
			csim.inputs.days.ncount = user_inputs.nsim;

		if (user_inputs.nprev > -1)
			csim.inputs.days.nprev = user_inputs.nprev;


		//--- Set up clusters
		setup_clusters(user_inputs, sfavail, metric_results, csim.inputs.cluster_results);
		csim.assign_first_last(metric_results);
		int ncl = csim.inputs.cluster_results.ncluster;


		//--- Calculate cluster-average solar-field availability
		matrix<double> avg_sfavail;
		if (csim.inputs.is_clusteravg_sfavail)
		{
			matrix<double> count;
			if (sfavail.size() == (size_t)nrec)
				csim.cluster_avg_from_timeseries(sfavail, avg_sfavail, count);
			else
				csim.inputs.is_clusteravg_sfavail = false;
		}

		//--- Set up arrays of combined simulation groupings
		int ng = ncl;
		if (csim.inputs.is_combine_consecutive)
		{
			csim.combine_consecutive_exemplars();
			ng = (int)csim.inputs.combined.n.size();
		}
		else
		{
			csim.inputs.combined.n.assign(ng, 1);
			csim.inputs.combined.start = csim.inputs.cluster_results.exemplars;
		}





		//--- Option 1: Run full annual continuous simulation but skip calculation on non-exemplar days.  TES hot charge state will be reset to the same value at beginning of each exemplar time block
		if (csim.inputs.is_run_continuous)
		{

			// Select simulation days and set cluster average solar-field availability
			std::vector<bool> select_days = csim.set_all_sim_days();
			std::vector<double> sfavail_sim = sfavail;
			if (csim.inputs.is_clusteravg_sfavail)  
				csim.overwrite_with_cluster_avg_values(sfavail_sim, avg_sfavail, true);


			// Set initial charge state
			double tes_charge = csim.inputs.initial_hot_charge;
			if (csim.inputs.is_initial_charge_heuristic)
			{
				double avg_prev_dni = 0.0;
				for (int g = 0; g < ng; g++)
				{
					int exemplar = csim.inputs.combined.start.at(g);						
					int d = std::max(csim.firstday(exemplar)-csim.inputs.days.nprev-1, 0);	// Day before first simulated day
					avg_prev_dni += metric_results.daily_dni.at(d, 0) / (double)ng;
				}
				tes_charge = csim.initial_charge_heuristic(avg_prev_dni, m_variables.solarm.as_number());
			}


			// Update ssc inputs
			ssc_data_set_number(m_ssc_data, "csp.pt.tes.init_hot_htf_percent", tes_charge);

			nr = (int)sfavail_sim.size();
			ssc_number_t *p_sfavail = new ssc_number_t[nr];
			for (int i = 0; i < nr; i++)
				p_sfavail[i] = 100.*(1. - sfavail_sim.at(i));
			ssc_data_set_array(m_ssc_data, "sf_adjust:hourly", p_sfavail, nr);
			delete[] p_sfavail;

			nr = (int)select_days.size();
			ssc_number_t *p_vals = new ssc_number_t[nr];
			for (int i = 0; i < nr; i++)
				p_vals[i] = select_days.at(i);
			ssc_data_set_array(m_ssc_data, "select_simulation_days", p_vals, nr);
			delete[] p_vals;


			// Run simulation and collect results
			ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0);
			for (int i = 0; i < n_hourly_keys; i++)
			{
				std::string key = hourly_keys[i];
				ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
				for (int r = 0; r < nr; r++)
					collect_ssc_data[key].at(r) = p_data[r];
			}

		}





		//--- Option 2: Run simulation of each cluster exemplar independently in discrete limited time-duration simulations
		else
		{
			for (int g = 0; g < ng; g++)
			{
				int ncount = csim.inputs.combined.n.at(g) * csim.inputs.days.ncount;	  // Number of days counting toward results in this group
				int nsim_nom = ncount + csim.inputs.days.nprev + csim.inputs.days.nnext;  // Nominal total number of days simulated in this group

				int exemplar = csim.inputs.combined.start.at(g);	// First cluster exemplar in this combined group

				int d1 = csim.firstday(exemplar);					// First day to be counted in group g
				int d0 = csim.firstsimday(exemplar);				// First day to be simulated in group g

				int nprev = d1 - d0;									// Actual number of previous days simulated
				int nsim = ncount + nprev + csim.inputs.days.nnext;		// Actual number of total days simulated

				double tstart = d0 * 24 * 3600;
				double tend = std::min((d0 + nsim)*24. * 3600, 8760.*3600.);


				// Update solar field hourly availability to reflect cluster-average values for this exemplar simulation
				std::vector<double> sfavail_sim = sfavail;
				if (csim.inputs.is_clusteravg_sfavail)
				{
					std::vector<double> current_sfavail = csim.compute_combined_clusteravg(g, avg_sfavail);  // Cluster average solar field availability for combined group g
					for (int h = 0; h < nperday*nsim_nom; h++)
					{
						int p = (d1 - csim.inputs.days.nprev) * nperday + h;
						if (p > 0 && p < 365 * nperday)
							sfavail_sim.at(p) = current_sfavail.at(h);
					}
				}

				// Set initial storage charge state
				double tes_charge = csim.inputs.initial_hot_charge;
				if (csim.inputs.is_initial_charge_heuristic)
				{
					int d = std::max((d1 - csim.inputs.days.nprev - 1), 0);  // Day before first simulated day
					double prev_dni = metric_results.daily_dni.at(d, 0);
					tes_charge = csim.initial_charge_heuristic(prev_dni, m_variables.solarm.as_number());
				}


				// Update inputs: 
				ssc_data_set_number(m_ssc_data, "time_start", tstart);
				ssc_data_set_number(m_ssc_data, "time_stop", tend);
				ssc_data_set_number(m_ssc_data, "vacuum_arrays", 1);
				ssc_data_set_number(m_ssc_data, "time_steps_per_hour", wf_steps_per_hour);
				ssc_data_set_number(m_ssc_data, "csp.pt.tes.init_hot_htf_percent", tes_charge);

				nr = (int)sfavail_sim.size();
				ssc_number_t *p_vals = new ssc_number_t[nr];
				for (int i = 0; i < nr; i++)
					p_vals[i] = 100.*(1. - sfavail_sim.at(i));
				ssc_data_set_array(m_ssc_data, "sf_adjust:hourly", p_vals, nr);
				delete[] p_vals;

				// Run simulation and collect results
                if (!ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0))
                {
                    message_handler("SSC simulation failed");
					ssc_module_free(mod_mspt);
 					return false;
                }

				int doy_full = d1;
				int doy_sim = nprev;
				for (int i = 0; i < n_hourly_keys; i++)
				{
					std::string key = hourly_keys[i];
					ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
					for (int h = 0; h < nperday*ncount; h++)
						collect_ssc_data[key].at(doy_full*nperday + h) = p_data[doy_sim*nperday + h];
				}

			}
		}


		//--- Compute full annual array from array containing simulated values at cluster-exemplar time blocks
		for (int k = 0; k < n_hourly_keys; k++)
		{
			std::string key = hourly_keys[k];
			csim.compute_annual_array_from_clusters(collect_ssc_data[key], full_data[key]);
		}


		//message_handler(wxString::Format("Cluster wcss = %.4f", csim.inputs.cluster_results.wcss));
		//message_handler(wxString::Format("Cluster wcss based on exemplar location = %.4f", csim.inputs.cluster_results.wcss_to_exemplars));
		//message_handler("Cluster exemplars,  weights:");
		//for (int i = 0; i < csim.inputs.cluster_results.ncluster; i++)
		//	message_handler(wxString::Format("%d, %.6f", csim.inputs.cluster_results.exemplars.at(i), csim.inputs.cluster_results.weights.at(i)));

	}

	ssc_module_free(mod_mspt);

	//--- Sum annual generation and revenue
	annual_generation = 0.0;
	revenue_units = 0.0;
	for (int i = 0; i < (int)full_data["gen"].size(); i++)
	{
		annual_generation += full_data["gen"].at(i) / (double)wf_steps_per_hour;
		revenue_units += full_data["gen"].at(i) * m_parameters.dispatch_factors_ts.vec()->at(i).as_number() / (double)wf_steps_per_hour;
	}
	message_handler(wxString::Format("Annual generation (GWhe) = %.4f",annual_generation / 1.e6).c_str());
	message_handler(wxString::Format("Annual revenue (GWhe) = %.4f", revenue_units / 1.e6).c_str());

	//"gen",  "Q_thermal", "annual_energy", "e_ch_tes", "beam", "pricing_mult" 
	
	m_simulation_outputs.generation_arr.assign_vector( full_data["gen"] );
	m_simulation_outputs.solar_field_power_arr.assign_vector( full_data["Q_thermal"]);
	m_simulation_outputs.tes_charge_state.assign_vector( full_data["e_ch_tes"] );
	m_simulation_outputs.dni_arr.assign_vector( full_data["beam"] );
	m_simulation_outputs.price_arr.assign_vector( full_data["pricing_mult"] );

	m_simulation_outputs.annual_generation.assign( annual_generation );
	m_simulation_outputs.annual_revenue.assign( revenue_units );

	return true;
}


