#include "project.h"
#include <limits>

 int double_scale(double val, int *scale)
 {
	 if( val == 0. )
	 {
		*scale = 0;
	 	return 0;
	 }

	 int power = (int)std::ceil( std::log10(val) );
	 *scale = SIGNIF_FIGURE - power;

     int ival = (int)(val*std::pow(10, *scale));

    std::string sival = std::to_string(ival);

    for(size_t i=sival.length()-1; i>0; i++)
    {
        if( sival.back() == '0')
        {
            sival.pop_back();
            ival /= 10;
            *scale -= 1;
        }
        else
            break;
    }

	 return ival;
 }
 
 double double_unscale(int val, int power)
 {
	 return (double)(val)*pow(10,-power);
 }


variables::variables()
{
	/* 
	Initialize members
	*/
	double dnan = std::numeric_limits<double>::quiet_NaN();
	double dmax = -std::numeric_limits<double>::infinity();
	double dmin = -dmax;
	
    h_tower.set(                          dnan,      dmin,      dmax,            "h_tower",                                     "Tower height",        "m",      "Variables", false, false);
    rec_height.set(                       dnan,      dmin,      dmax,         "rec_height",                                  "Receiver height",        "m",      "Variables", false, false);
    D_rec.set(                            dnan,      dmin,      dmax,              "D_rec",                                "Receiver diameter",        "m",      "Variables", false, false);
    design_eff.set(                       dnan,      dmin,      dmax,         "design_eff",                                "Design efficiency",        "-",      "Variables", false, false);
    dni_des.set(                          dnan,      dmin,      dmax,            "dni_des",                                 "Design point DNI",     "W/m2",      "Variables", false, false);
    P_ref.set(                            dnan,      dmin,      dmax,              "P_ref",                               "Design gross power",       "kW",      "Variables", false, false);
    solarm.set(                           dnan,      dmin,      dmax,             "solarm",                                   "Solar multiple",        "-",      "Variables", false, false);
    tshours.set(                          dnan,      dmin,      dmax,            "tshours",              "Hours stored at full load operation",       "hr",      "Variables", false, false);
    degr_replace_limit.set(               dnan,      dmin,      dmax, "degr_replace_limit",             "Mirror degradation replacement limit",        "-",      "Variables", false, false);
    om_staff.set(                           -1,      -999,       999,           "om_staff",                              "Number of o&m staff",        "-",      "Variables", false, true);
    //n_wash_crews.set(                       -1,      -999,       999,       "n_wash_crews",                             "Number of wash crews",        "-",      "Variables", false, true);
    N_panels.set(                           -1,      -999,       999,           "N_panels",                        "Number of receiver panels",        "-",      "Variables", false, true);


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
    //(*this)["n_wash_crews"] = &n_wash_crews;
    (*this)["N_panels"] = &N_panels;

};

parameters::parameters()
{
	/* 
	Initialize members
	*/

    print_messages.set(                   true,               "print_messages",      false,                                "Print full output",           "-",                          "Settings" );
    check_max_flux.set(                   true,               "check_max_flux",      false,                                   "Check max flux",           "-",                          "Settings" );
    is_optimize.set(                     false,                  "is_optimize",      false );
    is_dispatch.set(                     false,                  "is_dispatch",      false,                                "Optimize dispatch",           "-",                          "Settings" );
    is_ampl_engine.set(                  false,               "is_ampl_engine",      false,                               "Use AMPL optimizer",           "-",                          "Settings" );
    is_stochastic_disp.set(              false,           "is_stochastic_disp",      false,                          "Run stochastic dispatch",           "-",                          "Settings" );
    current_standby.set(                 false,              "current_standby",      false,                     "Start power cycle in standby",           "-",                  "Cycle|Parameters" );

	std::string empty_string = "";
    ampl_data_dir.set(                      "",                "ampl_data_dir",      false,                                 "AMPL data folder",           "-",                          "Settings" );
    solar_resource_file.set(      empty_string,          "solar_resource_file",      false,                              "Solar resource file",           "-",                          "Settings" );

    disp_steps_per_hour.set(                 1,          "disp_steps_per_hour",      false,                     "Dispatch time steps per hour",           "-",                          "Settings" );
    avail_seed.set(                        123,                   "avail_seed",      false,                     "Random number generator seed",           "-", "Heliostat availability|Parameters" );
    plant_lifetime.set(                     30,               "plant_lifetime",      false,                                   "Plant lifetime",          "yr",              "Financial|Parameters" );
    finance_period.set(                     25,               "finance_period",      false,                                   "Finance period",          "yr",              "Financial|Parameters" );
    ppa_multiplier_model.set(                1,         "ppa_multiplier_model",      false,                             "PPA multiplier model",           "-",              "Financial|Parameters" );
    num_condenser_trains.set(                2,         "num_condenser_trains",      false,                       "Number of condenser trains",           "-",                  "Cycle|Parameters" );
    fans_per_train.set(                     30,               "fans_per_train",      false,               "Number of fans per condenser train",           "-",                  "Cycle|Parameters" );
    radiators_per_train.set(                 2,          "radiators_per_train",      false,          "Number of radiators per condenser train",           "-",                  "Cycle|Parameters" );
    num_salt_steam_trains.set(               2,        "num_salt_steam_trains",      false,                   "Number of salt-to-steam trains",           "-",                  "Cycle|Parameters" );
    num_fwh.set(                             6,                      "num_fwh",      false,                      "Number of feedwater heaters",           "-",                  "Cycle|Parameters" );
    num_salt_pumps.set(                      4,               "num_salt_pumps",      false,                         "Number of hot salt pumps",           "-",                  "Cycle|Parameters" );
	num_salt_pumps_required.set(             3,      "num_salt_pumps_required",      false,  "Number of hot salt pumps to operate at capacity",           "-",                  "Cycle|Parameters" );
	num_water_pumps.set(                     2,              "num_water_pumps",      false,                 "Number of boiler and water pumps",           "-",                  "Cycle|Parameters" );
    num_turbines.set(                        1,                 "num_turbines",      false,               "Number of turbine-generator shafts",           "-",                  "Cycle|Parameters" );
    num_scenarios.set(                       1,                "num_scenarios",      false,                              "Number of scenarios",           "-",                  "Cycle|Parameters" );
	wash_vehicle_life.set(                  10,            "wash_vehicle_life",      false,                           "Wash vehicle lifetime",           "yr",    "Optical degradation|Parameters" );

    rec_ref_cost.set(                1.03e+008,                 "rec_ref_cost",      false,                          "Receiver reference cost",           "$",              "Financial|Parameters" );
    rec_ref_area.set(                    1571.,                 "rec_ref_area",      false,                          "Receiver reference area",          "m2",              "Financial|Parameters" );
    tes_spec_cost.set(                     24.,                "tes_spec_cost",      false,                                "TES specific cost",      "$/kWht",              "Financial|Parameters" );
    tower_fixed_cost.set(                 3.e6,             "tower_fixed_cost",      false,                                 "Tower fixed cost",           "$",              "Financial|Parameters" );
    tower_exp.set(                      0.0113,                    "tower_exp",      false,                      "Tower cost scaling exponent",           "-",              "Financial|Parameters" );
    heliostat_spec_cost.set(              145.,          "heliostat_spec_cost",      false,                          "Heliostat specific cost",        "$/m2",              "Financial|Parameters" );
    site_spec_cost.set(                    16.,               "site_spec_cost",      false,                               "Site specific cost",        "$/m2",              "Financial|Parameters" );
    land_spec_cost.set(                  10000,               "land_spec_cost",      false,                               "Land specific cost",      "$/acre",              "Financial|Parameters" );
    c_cps0.set(                             0.,                       "c_cps0",      false,                    "Power cycle cost coef - const",           "-",              "Financial|Parameters" );
    c_cps1.set(                          1440.,                       "c_cps1",      false,                    "Power cycle cost coef - slope",           "-",              "Financial|Parameters" );
    om_staff_cost.set(                      75,                "om_staff_cost",      false,                              "O&M staff cost rate",        "$/hr",              "Financial|Parameters" );
    wash_crew_cost.set(                    50.,               "wash_crew_cost",      false,                              "Wash crew cost rate",        "$/hr",              "Financial|Parameters" );
    heliostat_refurbish_cost.set(        23.06,     "heliostat_refurbish_cost",      false,                          "Mirror replacement cost",       "$/m^2",    "Optical degradation|Parameters" );
    om_staff_max_hours_week.set(            35,      "om_staff_max_hours_week",      false,                     "Max O&M staff hours per week",          "hr", "Heliostat availability|Parameters" );
    n_heliostats_sim.set(                 1000,             "n_heliostats_sim",      false,                   "Number of simulated heliostats",           "-", "Heliostat availability|Parameters" );
    wash_rate.set(                       3680.,                    "wash_rate",      false,                              "Heliostat wash rate", "m^2/crew-hr", "Optical degradation|Parameters" );
    wash_crew_max_hours_week.set(          70.,     "wash_crew_max_hours_week",      false,                     "Wash crew max hours per week",          "hr",    "Optical degradation|Parameters" );
	wash_crew_max_hours_day.set(           10.,      "wash_crew_max_hours_day",      false,                      "Wash crew max hours per day",          "hr",    "Optical degradation|Parameters");
	wash_crew_capital_cost.set(        100000.,       "wash_crew_capital_cost",      false,                  "Capital cost per wash crew hire",           "$",    "Optical degradation|Parameters" );
	price_per_kwh.set(                     0.1,                "price_per_kwh",      false,      "Estimated revenue per kWh delivered to grid",           "$",    "Optical degradation|Parameters" );
	TES_powercycle_eff.set(              0.376,           "TES_powercycle_eff",      false,      "Estimated efficiency of power block and TES",           "-",    "Optical degradation|Parameters" );
	degr_per_hour.set(                3.139e-6,                "degr_per_hour",      false,                    "Reflectivity degradation rate",        "1/hr",    "Optical degradation|Parameters" );
    degr_accel_per_year.set(                0.,          "degr_accel_per_year",      false,                   "Refl. degradation acceleration",        "1/yr",    "Optical degradation|Parameters" );
    degr_seed.set(                         123,                    "degr_seed",      false,                     "Random number generator seed",           "-",    "Optical degradation|Parameters" );
    soil_per_hour.set(                  1.5e-4,                "soil_per_hour",      false,                                "Mean soiling rate",        "1/hr",    "Optical degradation|Parameters" );
    adjust_constant.set(                     4,              "adjust:constant",      false,                            "Misc fixed power loss",           "%",              "Financial|Parameters" );
    helio_reflectance.set(                0.95,            "helio_reflectance",      false,                       "Initial mirror reflectance",           "-",    "Optical degradation|Parameters" );
    disp_rsu_cost.set(                    950.,                "disp_rsu_cost",      false,                            "Receiver startup cost",           "$",             "Simulation|Parameters" );
    disp_csu_cost.set(                  10000.,                "disp_csu_cost",      false,                         "Power block startup cost",           "$",             "Simulation|Parameters" );
    disp_pen_delta_w.set(                  0.1,             "disp_pen_delta_w",      false,                         "Power block ramp penalty",  "$/delta-kW",            "Simulation|Parameters" );
    rec_su_delay.set(                      0.2,                 "rec_su_delay",      false,                        "Receiver min startup time",          "hr",             "Simulation|Parameters" );
    rec_qf_delay.set(                     0.25,                 "rec_qf_delay",      false,                      "Receiver min startup energy",     "MWh/MWh",             "Simulation|Parameters" );
    startup_time.set(                      0.5,                 "startup_time",      false,                     "Power block min startup time",          "hr",             "Simulation|Parameters" );
    startup_frac.set(                      0.5,                 "startup_frac",      false,                       "Power block startup energy",     "MWh/MWh",             "Simulation|Parameters" );
    v_wind_max.set(                        15.,                   "v_wind_max",      false,                    "Max operational wind velocity",         "m/s",             "Simulation|Parameters" );
    flux_max.set(                        1000.,                     "flux_max",      false,                            "Maximum receiver flux",       "kW/m2",             "Simulation|Parameters" );
    maintenance_interval.set(             1.e6,         "maintenance_interval",      false,      "Runtime duration between maintenance events",           "h",                  "Cycle|Parameters" );
    maintenance_duration.set(             168.,         "maintenance_duration",      false,                   "Duration of maintenance events",           "h",                  "Cycle|Parameters" );
    downtime_threshold.set(                 24,           "downtime_threshold",      false,                "Downtime threshold for warm start",           "h",                  "Cycle|Parameters" );
    hours_to_maintenance.set(             1.e6,         "hours_to_maintenance",      false,   "Runtime duration before next maintenance event",           "h",                  "Cycle|Parameters" );
    temp_threshold.set(                    20.,               "temp_threshold",      false,     "Ambient temperature threshold for condensers",     "Celsius",                  "Cycle|Parameters" );
    shutdown_capacity.set(                 0.3,            "shutdown_capacity",      false,            "Threshold capacity to shut plant down",          "-",                  "Cycle|Parameters" );
    no_restart_capacity.set(               0.8,          "no_restart_capacity",      false,   "Threshold capacity for maintenance on shutdown",           "-",                  "Cycle|Parameters" );
	shutdown_efficiency.set(               0.7,          "shutdown_efficiency",      false,          "Threshold efficiency to shut plant down",           "-",                  "Cycle|Parameters" );
	no_restart_efficiency.set(             0.9,        "no_restart_efficiency",      false, "Threshold efficiency for maintenance on shutdown",           "-",                  "Cycle|Parameters" );
	cycle_hourly_labor_cost.set(           50.,       "cycle_hourly_labor_cost",     false,       "Hourly cost for repair of cycle components",           "h",                  "Cycle|Parameters" );

	std::vector< double > pval = { 0., 7., 200., 12000. };
    c_ces.set(                            pval,                        "c_ces",      false );

	std::vector< double > pvalts(8760, 1.);
    dispatch_factors_ts.set(            pvalts,          "dispatch_factors_ts",      false );

	std::vector< double > c_eff_cold = { 0., 1., 1. };
    condenser_eff_cold.set(         c_eff_cold,           "condenser_eff_cold",      false );

	std::vector< double > c_eff_hot = { 0., 0.95, 1. };
    condenser_eff_hot.set(           c_eff_hot,            "condenser_eff_hot",      false );


	// Availability parameters
	std::string rp = "mean_repair_time";
    helio_repair_priority.set(              rp,        "helio_repair_priority",      false,                        "Heliostat repair priority",           "-", "Heliostat availability|Parameters" );
    avail_model_timestep.set(               24,         "avail_model_timestep",      false,                      "Availability model timestep",          "hr", "Heliostat availability|Parameters" );

	std::vector< double > shape = { 1. };
	std::vector< double > scale = { 12000. };
	std::vector< double > mtr = { 2. };
	std::vector< double > repair_cost = { 300. };
    helio_comp_weibull_shape.set(        shape,     "helio_comp_weibull_shape",      false,             "Helio component Weibull shape params",           "-", "Heliostat availability|Parameters" );
    helio_comp_weibull_scale.set(        scale,     "helio_comp_weibull_scale",      false,             "Helio component Weibull scale params",          "hr", "Heliostat availability|Parameters" );
    helio_comp_mtr.set(                    mtr,               "helio_comp_mtr",      false,              "Helio component mean time to repair",          "hr", "Heliostat availability|Parameters" );
    helio_comp_repair_cost.set(    repair_cost,       "helio_comp_repair_cost",      false,                      "Helio component repair cost",           "$", "Heliostat availability|Parameters" );

	// Clustering parameters
    is_use_clusters.set(                 false,              "is_use_clusters",      false,                                    "Use clusters?",           "-",                          "Settings" );
    n_clusters.set(                         30,                   "n_clusters",      false,                               "Number of clusters",           "-",                          "Settings" );

    cluster_ndays.set(                       2,                "cluster_ndays",      false );
    cluster_nprev.set(                       1,                "cluster_nprev",      false );
    is_run_continuous.set(                true,            "is_run_continuous",      false );
	std::string ca = "affinity_propagation";
    cluster_algorithm.set(                  ca,            "cluster_algorithm",      false );

	std::vector<double> default_feature_wts = { 1.0, 0.5, 0.5, 0.25, 0.75, 0.5, 0.5, 0.5, 0.0 };
	std::vector<double> default_feature_divs = { 8, 4, 4, 4, 8, 4, 4, 4, 1 };
    clustering_feature_weights.set(  default_feature_wts,   "clustering_feature_weights",       true );
    clustering_feature_divisions.set( default_feature_wts,      "clustering_feature_divs",       true );

	// Cycle availability and simulation integration
	is_cycle_ssc_integration.set(false, "is_cycle_ssc_integration", false, "Integrate cycle failure and dispatch optimization models?", "-", "Settings");
	is_reoptimize_at_repairs.set(false, "is_reoptimize_at_repairs", false, "Re-optimize at cycle repairs?", "-", "Settings");
	is_reoptimize_at_failures.set(false, "is_reoptimize_at_failures", false, "Re-optimize at cycle failures?", "-", "Settings");
	is_use_target_heuristic.set(true, "is_use_target_heuristic", false, "Use heuristic to adjust dispatch targets during cycle failures?", "-", "Settings");
	cycle_nyears.set(1, "cycle_nyears", false, "Cycle availability number of years simulated", "-", "Settings");



    (*this)["print_messages"] = &print_messages;
    (*this)["check_max_flux"] = &check_max_flux;
    (*this)["is_optimize"] = &is_optimize;
    (*this)["is_dispatch"] = &is_dispatch;
    (*this)["is_ampl_engine"] = &is_ampl_engine;
    (*this)["is_stochastic_disp"] = &is_stochastic_disp;
	(*this)["current_standby"] = &current_standby;
	(*this)["ampl_data_dir"] = &ampl_data_dir;
    (*this)["solar_resource_file"] = &solar_resource_file;
    (*this)["disp_steps_per_hour"] = &disp_steps_per_hour;
    (*this)["avail_seed"] = &avail_seed;
    (*this)["plant_lifetime"] = &plant_lifetime;
    (*this)["finance_period"] = &finance_period;
    (*this)["ppa_multiplier_model"] = &ppa_multiplier_model;
	(*this)["num_condenser_trains"] = &num_condenser_trains;
	(*this)["fans_per_train"] = &fans_per_train;
	(*this)["radiators_per_train"] = &radiators_per_train;
	(*this)["num_salt_steam_trains"] = &num_salt_steam_trains;
	(*this)["num_fwh"] = &num_fwh;
	(*this)["num_salt_pumps"] = &num_salt_pumps;
	(*this)["num_water_pumps"] = &num_water_pumps;
	(*this)["num_turbines"] = &num_turbines;
	(*this)["num_scenarios"] = &num_scenarios;
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

    (*this)["om_staff_max_hours_week"] = &om_staff_max_hours_week;
    (*this)["n_heliostats_sim"] = &n_heliostats_sim;
    (*this)["wash_rate"] = &wash_rate;
    (*this)["wash_crew_max_hours_week"] = &wash_crew_max_hours_week;
	(*this)["wash_crew_max_hours_day"] = &wash_crew_max_hours_day;
	(*this)["wash_crew_capital_cost"] = &wash_crew_capital_cost;
	(*this)["price_per_kwh"] = &price_per_kwh;
	(*this)["TES_powercycle_eff"] = &TES_powercycle_eff;
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
	(*this)["maintenance_interval"] = &maintenance_interval;
	(*this)["maintenance_duration"] = &maintenance_duration;
	(*this)["downtime_threshold"] = &downtime_threshold;
	(*this)["hours_to_maintenance"] = &hours_to_maintenance;
	(*this)["temp_threshold"] = &temp_threshold;
	(*this)["shutdown_capacity"] = &shutdown_capacity;
	(*this)["no_restart_capacity"] = &no_restart_capacity;
	(*this)["no_restart_efficiency"] = &no_restart_efficiency;
	(*this)["cycle_hourly_labor_cost"] = &cycle_hourly_labor_cost;
    (*this)["c_ces"] = &c_ces;
    (*this)["dispatch_factors_ts"] = &dispatch_factors_ts;
	(*this)["condenser_eff_cold"] = &condenser_eff_cold;
	(*this)["condenser_eff_hot"] = &condenser_eff_hot;

	(*this)["helio_repair_priority"] = &helio_repair_priority;
	(*this)["avail_model_timestep"] = &avail_model_timestep;
	(*this)["helio_comp_weibull_shape"] = &helio_comp_weibull_shape;
	(*this)["helio_comp_weibull_scale"] = &helio_comp_weibull_scale;
	(*this)["helio_comp_mtr"] = &helio_comp_mtr;
	(*this)["helio_comp_repair_cost"] = &helio_comp_repair_cost;

	(*this)["is_use_clusters"] = &is_use_clusters;
	(*this)["n_clusters"] = &n_clusters;
	(*this)["cluster_ndays"] = &cluster_ndays;
	(*this)["cluster_nprev"] = &cluster_nprev;
	(*this)["is_run_continuous"] = &is_run_continuous;
	(*this)["cluster_algorithm"] = &cluster_algorithm;
	(*this)["clustering_feature_weights"] = &clustering_feature_weights;
	(*this)["clustering_feature_divisions"] = &clustering_feature_divisions;

	(*this)["is_cycle_ssc_integration"] = &is_cycle_ssc_integration;
	(*this)["is_reoptimize_at_repairs"] = &is_reoptimize_at_repairs;
	(*this)["is_reoptimize_at_failures"] = &is_reoptimize_at_failures;
	(*this)["is_use_target_heuristic"] = &is_use_target_heuristic;
	(*this)["cycle_nyears"] = &cycle_nyears;

}

design_outputs::design_outputs()
{
	/* 
	Set up output members
	*/

	double nan = std::numeric_limits<double>::quiet_NaN();
    number_heliostats.set(                  -1,            "number_heliostats",       true,                                  "Heliostat count",        "-",                    "Design|Outputs" );
    area_sf.set(                           nan,                      "area_sf",       true,                             "Total heliostat area",       "m2",                    "Design|Outputs" );
    base_land_area.set(                    nan,               "base_land_area",       true,                            "Solar field land area",     "acre",                    "Design|Outputs" );
    land_area.set(                         nan,                    "land_area",       true,                                  "Total land area",     "acre",                    "Design|Outputs" );
    flux_max_observed.set(                 nan,            "flux_max_observed",       true,                                "Max observed flux",    "kW/m2",                    "Design|Outputs" );
    cost_rec_tot.set(                      nan,                 "cost_rec_tot",       true,                                    "Receiver cost",        "$",                      "Design|Costs" );
    cost_sf_tot.set(                       nan,                  "cost_sf_tot",       true,                                 "Solar field cost",        "$",                      "Design|Costs" );
    cost_sf_real.set(                      nan,                             "",       true,                          "Solar field cost (real)",        "$",                      "Design|Costs" );
    cost_tower_tot.set(                    nan,               "cost_tower_tot",       true,                                       "Tower cost",        "$",                      "Design|Costs" );
    cost_land_tot.set(                     nan,                "cost_land_tot",       true,                                        "Land cost",        "$",                      "Design|Costs" );
    cost_land_real.set(                    nan,                             "",       true,                                 "Land cost (real)",        "$",                      "Design|Costs" );
    cost_site_tot.set(                     nan,                "cost_site_tot",       true,                                        "Site cost",        "$",                      "Design|Costs" );
    h_tower_opt.set(                       nan,                  "h_tower_opt",       true );
    rec_height_opt.set(                    nan,               "rec_height_opt",       true );
    rec_aspect_opt.set(                    nan,               "rec_aspect_opt",       true );

	std::vector< std::vector< double > > empty_mat;
    opteff_table.set(                empty_mat,                 "opteff_table",       true );
    flux_table.set(                  empty_mat,                   "flux_table",       true );
    heliostat_positions.set(         empty_mat,          "heliostat_positions",       true );

	std::vector< std::vector< double > > empty_vec;
	annual_helio_energy.set(         empty_vec,          "annual_helio_energy",       true );

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
	(*this)["annual_helio_energy"] = &annual_helio_energy;
}

solarfield_outputs::solarfield_outputs()
{
	/* 
	Set up output members
	*/

	double nan = std::numeric_limits<double>::quiet_NaN();

    n_repairs.set(                         nan,                    "n_repairs",       true,                      "Number of heliostat repairs",        "-",    "Heliostat availability|Outputs" );
    staff_utilization.set(                 nan,            "staff_utilization",       true,                                "Staff utilization",        "-",    "Heliostat availability|Outputs" );
    heliostat_repair_cost_y1.set(          nan,     "heliostat_repair_cost_y1",       true,                   "Heliostat repair cost (year 1)",        "$",    "Heliostat availability|Outputs" );
    heliostat_repair_cost.set(             nan,        "heliostat_repair_cost",       true );
    avg_avail.set(                         nan,                    "avg_avail",       true,                    "Average lifetime availability",        "-",    "Heliostat availability|Outputs" );

	std::vector< double > empty_vec;
    avail_schedule.set(              empty_vec,               "avail_schedule",       true );
    n_repairs_per_component.set(     empty_vec,      "n_repairs_per_component",       true,       "Average annual heliostat component repairs",        "-",    "Heliostat availability|Outputs" );

    (*this)["n_repairs"] = &n_repairs;
    (*this)["staff_utilization"] = &staff_utilization;
    (*this)["heliostat_repair_cost_y1"] = &heliostat_repair_cost_y1;
    (*this)["heliostat_repair_cost_real"] = &heliostat_repair_cost;
    (*this)["avail_schedule"] = &avail_schedule;
	(*this)["avg_avail"] = &avg_avail;
	(*this)["n_repairs_per_component"] = &n_repairs_per_component;

}

optical_outputs::optical_outputs()
{
	/* 
	Set up output members
	*/

	double nan = std::numeric_limits<double>::quiet_NaN();
	std::vector< double > empty_vec;
	n_wash_crews.set(                      nan,                 "n_wash_crews",       true,                       "Number of wash crews hired",        "-"        "Optical degradation|Outputs" );
    n_replacements.set(                    nan,               "n_replacements",       true,                              "Mirror replacements",        "-",       "Optical degradation|Outputs" );
    heliostat_refurbish_cost.set(          nan,     "heliostat_refurbish_cost",       true,                          "Mirror replacement cost",        "$",       "Optical degradation|Outputs" );
    heliostat_refurbish_cost_y1.set(       nan,  "heliostat_refurbish_cost_y1",       true,                 "Mirror replacement cost (year 1)",        "$",       "Optical degradation|Outputs" );
    avg_soil.set(                          nan,                     "avg_soil",       true,                         "Average lifetime soiling",        "-",       "Optical degradation|Outputs" );
    avg_degr.set(                          nan,                     "avg_degr",       true,                     "Average lifetime degradation",        "-",       "Optical degradation|Outputs" );

    soil_schedule.set(               empty_vec,                "soil_schedule",       true,                              "Soiling time series",        "-",       "Optical degradation|Outputs" );
    degr_schedule.set(               empty_vec,                "degr_schedule",       true,                          "Degradation time series",        "-",       "Optical degradation|Outputs" );
    repl_schedule.set(               empty_vec,                "repl_schedule",       true,                         "Mirror repl. time series",        "-",       "Optical degradation|Outputs" );
    repl_total.set(                  empty_vec,                   "repl_total",       true );

	(*this)["n_wash_crews"] = &n_wash_crews;
    (*this)["n_replacements"] = &n_replacements;
    (*this)["heliostat_refurbish_cost_real"] = &heliostat_refurbish_cost;
    (*this)["heliostat_refurbish_cost_y1"] = &heliostat_refurbish_cost_y1;
    (*this)["avg_soil"] = &avg_soil;
    (*this)["avg_degr"] = &avg_degr;

    (*this)["soil_schedule"] = &soil_schedule;
    (*this)["degr_schedule"] = &degr_schedule;
    (*this)["repl_schedule"] = &repl_schedule;
    (*this)["repl_total"] = &repl_total;

}

cycle_outputs::cycle_outputs()
{
	std::vector< double > empty_vec;

    cycle_efficiency.set(    empty_vec,                       "cycle_efficiency",  true,                             "Cycle efficiency time series",  "-",  "Cycle|Outputs" );
    cycle_capacity.set(      empty_vec,                         "cycle_capacity",  true,                               "Cycle capacity time series",  "-",  "Cycle|Outputs" );
	cycle_labor_cost.set(          nan,                       "cycle_labor_cost",  true,              "Expected labor costs for power cycle repair",  "$",  "Cycle|Outputs" );
	num_failures.set(		nan,								"num_failures",		true,					"Average number of annual cycle failures", "",	 "Cycle|Outputs");

	(*this)["cycle_efficiency"] = &cycle_efficiency;
	(*this)["cycle_capacity"] = &cycle_capacity;
	(*this)["cycle_labor_cost"] = &cycle_labor_cost;
	(*this)["num_failures"] = &num_failures;
}

simulation_outputs::simulation_outputs()
{
	double nan = std::numeric_limits<double>::quiet_NaN();
	std::vector< double > empty_vec;

    generation_arr.set(              empty_vec,               "generation_arr",       true,                             "Net power generation",      "kWe",                "Simulation|Outputs" );
	solar_field_power_arr.set(       empty_vec,        "solar_field_power_arr",       true,                         "Solarfield thermal power",      "kWt",                "Simulation|Outputs" );
    tes_charge_state.set(            empty_vec,                     "e_ch_tes",       true,                     "Thermal storage charge state",     "MWht",                "Simulation|Outputs" );
    dni_arr.set(                     empty_vec,                      "dni_arr",       true,                        "Direct normal irradiation",     "W/m2",                "Simulation|Outputs" );
    price_arr.set(                   empty_vec,                    "price_arr",       true,                                     "Price signal",        "-",                "Simulation|Outputs" );
	
    dni_templates.set(               empty_vec,                "dni_templates",       true,                                     "DNI clusters",     "W/m2",                "Simulation|Outputs" );
    price_templates.set(             empty_vec,              "price_templates",       true,                                   "Price clusters",        "-",                "Simulation|Outputs" );


    annual_generation.set(                 nan,            "annual_generation",       true,                          "Annual total generation",     "GWhe",                "Simulation|Outputs" );
    annual_revenue_units.set(              nan,         "annual_revenue_units",       true,                             "Annual revenue units",        "-",                "Simulation|Outputs" );

    annual_rec_starts.set(                 nan,            "annual_rec_starts",       true,                           "Annual receiver starts",        "-",                "Simulation|Outputs" );
    annual_cycle_starts.set(               nan,          "annual_cycle_starts",       true,                              "Annual cycle starts",        "-",                "Simulation|Outputs" );
    annual_cycle_ramp.set(                 nan,            "annual_cycle_ramp",       true,                             "Annual cycle ramping",      "GWe",                "Simulation|Outputs" );
    cycle_ramp_index.set(                  nan,             "cycle_ramp_index",       true,                                 "Cycle ramp index",        "%",                "Simulation|Outputs" );

    annual_rec_starts_disp.set(            nan,       "annual_rec_starts_disp",       true,        "Annual receiver starts from dispatch soln",        "-",                "Simulation|Outputs" );
    annual_cycle_starts_disp.set(          nan,     "annual_cycle_starts_disp",       true,           "Annual cycle starts from dispatch soln",        "-",                "Simulation|Outputs" );
    annual_cycle_ramp_disp.set(            nan,       "annual_cycle_ramp_disp",       true,          "Annual cycle ramping from dispatch soln",      "GWe",                "Simulation|Outputs" );
    cycle_ramp_index_disp.set(             nan,        "cycle_ramp_index_disp",       true,              "Cycle ramp index from dispatch soln",        "%",                "Simulation|Outputs" );

	
	// outputs needs for cycle availability model
	gross_gen.set(empty_vec, "gross_gen", true);
	q_pb.set(empty_vec, "q_pb", true);
	tamb.set(empty_vec, "tamb", true);
	is_standby.set(empty_vec, "is_standby", true);


	(*this)["generation_arr"] = &generation_arr;
	(*this)["solar_field_power_arr"] = &solar_field_power_arr;
	(*this)["tes_charge_state"] = &tes_charge_state;
	(*this)["dni_arr"] = &dni_arr;
	(*this)["price_arr"] = &price_arr;
	(*this)["dni_templates"] = &dni_templates;
	(*this)["price_templates"] = &price_templates;
	(*this)["annual_generation"] = &annual_generation;
	(*this)["annual_revenue_units"] = &annual_revenue_units;
	(*this)["annual_rec_starts"] = &annual_rec_starts;
	(*this)["annual_cycle_starts"] = &annual_cycle_starts;
	(*this)["annual_cycle_ramp"] = &annual_cycle_ramp;
	(*this)["cycle_ramp_index"] = &cycle_ramp_index;
	(*this)["annual_rec_starts_disp"] = &annual_rec_starts_disp;
	(*this)["annual_cycle_starts_disp"] = &annual_cycle_starts_disp;
	(*this)["annual_cycle_ramp_disp"] = &annual_cycle_ramp_disp;
	(*this)["cycle_ramp_index_disp"] = &cycle_ramp_index_disp;

	(*this)["gross_gen"] = &gross_gen;
	(*this)["q_pb"] = &q_pb;
	(*this)["tamb"] = &tamb;
	(*this)["is_standby"] = &is_standby;

}

explicit_outputs::explicit_outputs()
{
	double nan = std::numeric_limits<double>::quiet_NaN();
    cost_receiver_real.set(                nan,           "cost_receiver_real",       true );
    cost_tower_real.set(                   nan,              "cost_tower_real",       true );
    cost_plant_real.set(                   nan,              "cost_plant_real",       true );
    cost_tes_real.set(                     nan,                "cost_tes_real",       true );

    heliostat_om_labor_y1.set(             nan,        "heliostat_om_labor_y1",       true );
    heliostat_om_labor_real.set(           nan,           "heliostat_om_labor",       true );
    heliostat_wash_cost_y1.set(            nan,       "heliostat_wash_cost_y1",       true );
    heliostat_wash_cost_real.set(          nan,          "heliostat_wash_cost",       true );
	heliostat_wash_capital_cost.set(       nan,  "heliostat_wash_capital_cost",       true );

	(*this)["heliostat_om_labor_y1"] = &heliostat_om_labor_y1;
	(*this)["heliostat_wash_cost_y1"] = &heliostat_wash_cost_y1;

}

financial_outputs::financial_outputs()
{
	double nan = std::numeric_limits<double>::quiet_NaN();

    lcoe_nom.set(                          nan,                     "lcoe_nom",       true );
    lcoe_real.set(                         nan,                    "lcoe_real",       true );
    ppa.set(                               nan,                          "ppa",       true );
    project_return_aftertax_npv.set(       nan,  "project_return_aftertax_npv",       true );
    project_return_aftertax_irr.set(       nan,  "project_return_aftertax_irr",       true );
    total_installed_cost.set(              nan,         "total_installed_cost",       true );


	(*this)["project_return_aftertax_npv"] = &project_return_aftertax_npv;
	(*this)["project_return_aftertax_irr"] = &project_return_aftertax_irr;
	(*this)["total_installed_cost"] = &total_installed_cost;
}

objective_outputs::objective_outputs()
{
	double nan = std::numeric_limits<double>::quiet_NaN();

    cost_receiver_real.set(                nan,           "cost_receiver_real",       true,                                    "Receiver cost",        "$",                 "Objective|Outputs" );
    cost_tower_real.set(                   nan,              "cost_tower_real",       true,                                       "Tower cost",        "$",                 "Objective|Outputs" );
    cost_plant_real.set(                   nan,              "cost_plant_real",       true,                                       "Plant cost",        "$",                 "Objective|Outputs" );
    cost_tes_real.set(                     nan,                "cost_tes_real",       true,                                         "TES cost",        "$",                 "Objective|Outputs" );
    cost_land_real.set(                    nan,               "cost_land_real",       true,                                        "Land cost",        "$",                 "Objective|Outputs" );
    cost_sf_real.set(                      nan,                 "cost_sf_real",       true,                             "Heliostat field cost",        "$",                 "Objective|Outputs" );
    cap_cost_real.set(                     nan,                "cap_cost_real",       true,                               "Total Capital cost",        "$",                 "Objective|Outputs" );

    rec_start_cost_real.set(               nan,          "rec_start_cost_real",       true,                              "Receiver start cost",        "$",                 "Objective|Outputs" );
    cycle_start_cost_real.set(             nan,        "cycle_start_cost_real",       true,                                 "Cycle start cost",        "$",                 "Objective|Outputs" );
    cycle_ramp_cost_real.set(              nan,         "cycle_ramp_cost_real",       true,                                  "Cycle ramp cost",        "$",                 "Objective|Outputs" );
	cycle_repair_cost_real.set(            nan,       "cycle_repair_cost_real",       true,                                "Cycle repair cost",        "$",                 "Objective|Outputs" );
	heliostat_repair_cost_real.set(        nan,   "heliostat_repair_cost_real",       true,                            "Heliostat repair cost",        "$",                 "Objective|Outputs" );
    heliostat_om_labor_real.set(           nan,      "heliostat_om_labor_real",       true,                         "Heliostat O&M labor cost",        "$",                 "Objective|Outputs" );
    heliostat_wash_cost_real.set(          nan,     "heliostat_wash_cost_real",       true,                          "Heliostat O&M wash cost",        "$",                 "Objective|Outputs" );
    heliostat_refurbish_cost_real.set(     nan,"heliostat_refurbish_cost_real",       true,                         "Heliostat refurbish cost",        "$",                 "Objective|Outputs" );
	heliostat_wash_capital_cost.set(       nan,  "heliostat_wash_capital_cost",       true,                      "Heliostat wash vehicle cost",        "$",                 "Objective|Outputs" );
	om_cost_real.set(                      nan,                 "om_cost_real",       true,                                   "Total O&M cost",        "$",                 "Objective|Outputs" );
	
    sales.set(                             nan,                        "sales",       true,                                            "Sales",        "$",                 "Objective|Outputs" );
    cash_flow.set(                         nan,                    "cash_flow",       true,                                        "Cash flow",        "$",                 "Objective|Outputs" );
    ppa.set(                               nan,                          "ppa",       true,                               "PPA price (year 1)",   "c/kWhe",                 "Objective|Outputs" );
    lcoe_nom.set(                          nan,                     "lcoe_nom",       true,                                   "LCOE (nominal)",   "c/kWhe",                 "Objective|Outputs" );
    lcoe_real.set(                         nan,                    "lcoe_real",       true,                                      "LCOE (real)",   "c/kWhe",                 "Objective|Outputs" );
	
	(*this)["cost_receiver_real"] = &cost_receiver_real;
	(*this)["cost_tower_real"] = &cost_tower_real;
	(*this)["cost_plant_real"] = &cost_plant_real;
	(*this)["cost_tes_real"] = &cost_tes_real;
	(*this)["cost_land_real"] = &cost_land_real;
	(*this)["cost_sf_real"] = &cost_sf_real;
	(*this)["cap_cost_real"] = &cap_cost_real;
	(*this)["rec_start_cost_real"] = &rec_start_cost_real;
	(*this)["cycle_start_cost_real"] = &cycle_start_cost_real;
	(*this)["cycle_ramp_cost_real"] = &cycle_ramp_cost_real;
	(*this)["cycle_repair_cost_real"] = &cycle_repair_cost_real;
	(*this)["heliostat_repair_cost_real"] = &heliostat_repair_cost_real;
	(*this)["heliostat_om_labor_real"] = &heliostat_om_labor_real;
	(*this)["heliostat_wash_cost_real"] = &heliostat_wash_cost_real;
	(*this)["heliostat_refurbish_cost_real"] = &heliostat_refurbish_cost_real;
	(*this)["om_cost_real"] = &om_cost_real;
	(*this)["sales"] = &sales;
	(*this)["cash_flow"] = &cash_flow;
	(*this)["ppa"] = &ppa;
	(*this)["lcoe_nom"] = &lcoe_nom;
	(*this)["lcoe_real"] = &lcoe_real;
	

}

optimization_outputs::optimization_outputs()
{
	double nan = std::numeric_limits<double>::quiet_NaN();
	std::vector< double > empty_vec_d;
    std::vector< std::vector<double> > empty_mat;
    	
    eta_i.set(empty_mat, "obj_function_lower_b", true, "Lower bound on objective at evaluation points", "-", "Optimization|Outputs");
    secants_i.set(empty_vec_d, "obj_function_secants", true, "Objective function secants", "-", "Optimization|Outputs");
    feas_secants_i.set(empty_vec_d, "obj_function_secants_f", true, "Feasible objective function secants", "-", "Optimization|Outputs");
    eval_order.set(empty_vec_d, "obj_eval_order", true, "Objective function evaluation order", "-", "Optimization|Outputs");
    wall_time_i.set(empty_vec_d, "obj_wall_time", true, "Clock time for objective function evaluation", "-", "Optimization|Outputs");

    (*this)["obj_function_lower_b"] = &eta_i;
    (*this)["obj_function_secants"] = &secants_i;
    (*this)["obj_function_secants_f"] = &feas_secants_i;
    (*this)["obj_eval_order"] = &eval_order;
    (*this)["obj_wall_time"] = &wall_time_i;
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
			switch(v->type() )
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
            (*error_msg).append((std::stringstream() << "Variable or parameter has invalid data: " << v->nice_name << " (" << v->name << ")\n").str());
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
		case CALLING_SIM::CYCLE_AVAIL:
			if (!is_design_valid)
			{
				(*error_msg).append("Error: Attempting to run cycle availability without a valid solar field design.\n");
				return false;
			}

			if (!is_simulation_valid)
			{
				(*error_msg).append("Error: Attempting to simulated cycle availability without a time-series performance simulation.\n");
				return false;
			}
			break;
		case CALLING_SIM::SIMULATION:
			if( !is_design_valid )
			{
				(*error_msg).append("Error: Attempting to run time-series performance simulation without a valid solar field design.\n");
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
			if( !( is_design_valid && is_simulation_valid && is_explicit_valid) )
			{
                (*error_msg).append((std::stringstream() << "Error: Cannot calculate system financial metrics without a valid " <<
                    (is_design_valid ? (is_simulation_valid ? "cost simulation" : "plant performance simulation") : "solar field design") << ".\n").str()
                );
                    
				return false;
			}

			if (!is_sf_avail_valid)
				(*error_msg).append("Notice: Calculating O&M costs without valid solar field availability model output.\n");
			if (!is_sf_optical_valid)
				(*error_msg).append("Notice:  Calculating O&M costs without valid solar field soiling/degradation model output.\n");

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
	is_explicit_valid = false;
	is_financial_valid = false;
	is_cycle_avail_valid = false;

	initialize_ssc_project();

    //ssc_to_lk_hash(m_ssc_data, m_parameters);
    ssc_to_lk_hash(m_ssc_data, m_variables);

	parameters default_params;
	lk_hash_to_ssc(m_ssc_data, default_params);
	ssc_to_lk_hash(m_ssc_data, m_parameters);

	cluster_outputs.clear();

}

Project::Project()
{
	m_ssc_data = 0;
	Initialize();

    std::vector< void* > struct_pointers = GetDataObjects();

	_merged_data.clear();

	for (size_t i = 0; i < struct_pointers.size(); i++)
	{
        lk::varhash_t *this_varhash = static_cast<lk::varhash_t*>( struct_pointers.at(i) );

        for (lk::varhash_t::iterator it = this_varhash->begin(); it != this_varhash->end(); it++)
		{
            _merged_data[(*it).first] = it->second;
		}
	}

    /*
    Keep track of which methods need to be called when a specific variable value changes
        D()  :  Solar field layout and design
        M()  :  Heliostat mechanical availability
        C()  :  Cycle availability 
        O()  :  Optical degradation and soiling
        S()  :  Production simulation
        E()  :  Explicit cost calculations
        F()  :  Financial model calculations
        Z()  :  Rolled-up objective function
    */

    ObjectiveMethodPtr
        pd = &Project::D,
        pm = &Project::M,
        pc = &Project::C,
        po = &Project::O,
        ps = &Project::S,
        pe = &Project::E,
        pf = &Project::F;

    m_variables.h_tower.triggers = 
        m_variables.rec_height.triggers =
        m_variables.D_rec.triggers =
        m_variables.design_eff.triggers =
        m_variables.dni_des.triggers =
        m_variables.solarm.triggers = { "D", "M", "O", "S", "E", "F" }; //{ &Project::D, &Project::M, &Project::O, &Project::S, &Project::E, &Project::F };
    m_variables.P_ref.triggers = {"D", "M", "C", "O", "S", "E", "F"};  //{ &Project::D, &Project::M, &Project::C, &Project::O, &Project::S, &Project::E, &Project::F };
    m_variables.tshours.triggers = { "S", "E", "F"}; // { &Project::S, &Project::E, &Project::F };
    m_variables.degr_replace_limit.triggers = { "O", "S", "E", "F"}; // { &Project::O, &Project::S, &Project::E, &Project::F };
    m_variables.om_staff.triggers = { "M", "S", "E", "F" }; // { &Project::M, &Project::S, &Project::E, &Project::F };
    //m_variables.n_wash_crews.triggers = { "O", "S", "E", "F" }; // { &Project::O, &Project::S, &Project::E, &Project::F };
    m_variables.N_panels.triggers = { "S", "E", "F" }; // {&Project::S, &Project::E, &Project::F };

    _all_method_pointers.clear();
    _all_method_pointers["D"] = pd;
    _all_method_pointers["M"] = pm;
    _all_method_pointers["C"] = pc;
    _all_method_pointers["O"] = po;
    _all_method_pointers["S"] = ps;
    _all_method_pointers["E"] = pe;
    _all_method_pointers["F"] = pf;

    _all_method_names.clear();
    _all_method_names = { "D", "M", "C", "O", "S", "E", "F" };

    add_documentation();
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

std::vector< void* > Project::GetDataObjects()
{
    std::vector< void* > rvec = {
        (void*)&m_variables, 
        (void*)&m_parameters, 
        //(void*)&m_cluster_parameters,
        (void*)&m_design_outputs, 
        (void*)&m_optical_outputs, 
        (void*)&m_solarfield_outputs,
        (void*)&m_simulation_outputs, 
        //(void*)&m_cycle_outputs,
        (void*)&m_objective_outputs,
        (void*)&m_optimization_outputs
    }; 

    return rvec;
}

bool Project::CallMethodByName(const std::string &method)
{
    return _all_method_pointers.at(method).Run(this);
}

std::vector<std::string> Project::GetAllMethodNames()
{
    return _all_method_names;
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


double Project::calc_real_dollars(const double &dollars, bool is_revenue, bool is_labor, bool one_time_exp, int num_years)
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
	else if (one_time_exp)
	{
		double r = 1 + inflation_rate;
		double tot = dollars * std::pow(1. / (1 + r), num_years);
		return tot;
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
		message_handler( (std::stringstream() << "The solar resource file could not be located (Design module). The specified path is:\n" << m_parameters.solar_resource_file.as_string() ).str().c_str() );
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
		int nr, nc;
		ssc_number_t *p_hel = ssc_data_get_matrix(m_ssc_data, "heliostat_positions", &nr, &nc);
		ssc_data_set_matrix(m_ssc_data, "helio_positions", p_hel, nr, nc);

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
	
	// obtain annual energy by heliostat
	std::vector< double > ann_e = {};
	ssc_number_t *ann = ssc_data_get_array(m_ssc_data, "annual_helio_energy", &nr);
	for (int i = 0; i < nr; i++)
	{
		ann_e.push_back((double)ann[i]);
	}
	
	ssc_module_free(mod_solarpilot);

	//assign outputs and return
	
	//Land cost
	m_design_outputs.cost_land_real.assign( calc_real_dollars( m_parameters.land_spec_cost.as_number() * m_design_outputs.land_area.as_number() ) ); 
	//solar field cost
	m_design_outputs.cost_sf_real.assign( calc_real_dollars( (m_parameters.heliostat_spec_cost.as_number() + m_parameters.site_spec_cost.as_number())*m_design_outputs.area_sf.as_number() ) );
	//Annual output by heliostat
	m_design_outputs.annual_helio_energy.empty_vector();
	m_design_outputs.annual_helio_energy.assign_vector(ann_e);

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

	// error if invalid design
	std::string error_msg;
	if (!Validate(Project::CALLING_SIM::HELIO_AVAIL, &error_msg))
	{
		message_handler(error_msg.c_str());
		return false;
	}

	//error if trying to simulate with no heliostats
	if (m_design_outputs.number_heliostats.as_integer() <= 1)
	{
		message_handler("Error: Empty layout in field availability simulation.");
		return false;
	}

	solarfield_availability sfa;

	sfa.m_settings.n_years = m_parameters.plant_lifetime.as_integer();
	sfa.m_settings.step = m_parameters.avail_model_timestep.as_number();
	
	sfa.m_settings.n_om_staff.assign(sfa.m_settings.n_years, m_variables.om_staff.as_number());
	sfa.m_settings.max_hours_per_day = 9.;
	sfa.m_settings.max_hours_per_week = m_parameters.om_staff_max_hours_week.as_number();

	sfa.m_settings.n_helio = m_design_outputs.number_heliostats.as_integer();
	sfa.m_settings.n_helio_sim = m_parameters.n_heliostats_sim.as_integer();
	sfa.m_settings.seed = m_parameters.avail_seed.as_integer();

	sfa.m_settings.is_fix_hours = false;
	std::string weatherfile = m_parameters.solar_resource_file.as_string();
	s_location loc(weatherfile);
	sfa.m_settings.location = loc;


	//-- Repair prioritization
	std::string repair_priority = m_parameters.helio_repair_priority.as_string();
	if (repair_priority == "failure_order")
		sfa.m_settings.repair_order = FAILURE_ORDER;
	//else if (repair_priority == "performance")
	//	sfa.m_settings.repair_order = PERFORMANCE;
	else if (repair_priority == "repair_time")
		sfa.m_settings.repair_order = REPAIR_TIME;
	else if (repair_priority == "mean_repair_time")
		sfa.m_settings.repair_order = MEAN_REPAIR_TIME;
	else if (repair_priority == "random")
		sfa.m_settings.repair_order = RANDOM;
	else
	{
		message_handler("Specified helio_repair_priority not recognized. Valid inputs are 'random', 'failure_order', 'repair_time', 'mean_repair_time'");
		return false;
	}


	//-- Heliostat components
	bool good_as_new = true;
	double min_rep = 0.;
	double max_rep = 1000.;
	sfa.m_settings.helio_components.clear();
	int ncomp = (int) m_parameters.helio_comp_weibull_shape.vec()->size();

	if ((int)m_parameters.helio_comp_weibull_scale.vec()->size() != ncomp || (int)m_parameters.helio_comp_mtr.vec()->size() != ncomp || (int)m_parameters.helio_comp_repair_cost.vec()->size() != ncomp)
	{
		message_handler("Unequal number of heliostat components are specified in helio_comp_weibull_scale, helio_comp_weibull_shape, helio_comp_mtr, and helio_comp_repair_cost.");
		return false;
	}

	for (int c = 0; c < ncomp; c++)
	{
		double shape = m_parameters.helio_comp_weibull_shape.vec()->at(c).as_number();
		double scale = m_parameters.helio_comp_weibull_scale.vec()->at(c).as_number();
		double mtr = m_parameters.helio_comp_mtr.vec()->at(c).as_number();
		double rep_cost = m_parameters.helio_comp_repair_cost.vec()->at(c).as_number();
		helio_component_inputs comp(shape, scale, mtr, min_rep, max_rep, good_as_new, rep_cost);
		sfa.m_settings.helio_components.push_back(comp);
	}


	sfa.m_settings.is_tracking = false;
	sfa.m_settings.helio_performance.assign(sfa.m_settings.n_helio, 1.0);
	sfa.simulate(sim_progress_handler);


	//Calculate staff cost and repair cost
	sfa.m_results.heliostat_repair_cost_y1 = 0.0;
	for (int c = 0; c < sfa.m_settings.helio_components.size(); c++)
		sfa.m_results.heliostat_repair_cost_y1 += sfa.m_settings.helio_components[c].m_repair_cost * ( sfa.m_results.n_repairs_per_component[c] / sfa.m_settings.n_years); // Average yearly repair cost

	
	//lifetime costs
	//treat heliostat repair costs as consuming reserve equipment paid for at the project outset
	sfa.m_results.heliostat_repair_cost = calc_real_dollars(sfa.m_results.heliostat_repair_cost_y1);

    //assign outputs to project structure
    m_solarfield_outputs.n_repairs.assign( sfa.m_results.n_repairs / sfa.m_settings.n_years);
    m_solarfield_outputs.staff_utilization.assign( sfa.m_results.staff_utilization );
    m_solarfield_outputs.heliostat_repair_cost_y1.assign( sfa.m_results.heliostat_repair_cost_y1 );
    m_solarfield_outputs.heliostat_repair_cost.assign( sfa.m_results.heliostat_repair_cost );
	m_solarfield_outputs.avg_avail.assign(sfa.m_results.avg_avail);
    m_solarfield_outputs.avail_schedule.assign_vector( sfa.m_results.avail_schedule);

	std::vector<double> n_per_comp;
	for (int c = 0; c < ncomp; c++)
		n_per_comp.push_back( sfa.m_results.n_repairs_per_component[c] / sfa.m_settings.n_years );
	
	m_solarfield_outputs.n_repairs_per_component.assign_vector(n_per_comp);


	is_sf_avail_valid = true;
	return true;
}

bool Project::C()
{
	/*
	The cycle efficiency and capacity problem

	Returns a dict with keys:
	cycle_efficiency    Time series of mean cycle efficiency 
	cycle_capacity      Time series of mean cycle capacity
	*/
	
	// error if invalid design
	std::string error_msg;
	bool ok = Validate(Project::CALLING_SIM::CYCLE_AVAIL, &error_msg);
	message_handler(error_msg.c_str());
	if (!ok)
		return false;

	if (is_cycle_avail_valid) // cycle availability results already exist
	{
		if (m_parameters.is_cycle_ssc_integration.as_boolean())
		{
			message_handler("Notice: Cycle availability was already simulated during performance simulation and will not be repeated.\n");
			return true;
		}
		else
			message_handler("Notice: Cycle availability was already simulated. Previous results will be discarded.\n");
	}



	int ny = m_parameters.cycle_nyears.as_integer();  // number of years to simulated cycle failures (same dispatch profiles will be used in all years)
	int ns = m_parameters.num_scenarios.as_integer(); // number of scenarios. Final capacity/efficiency arrays will be averaged over scenarios

	//--- Check that simulation outputs are complete
	int nrec = (int)m_simulation_outputs.gross_gen.vec()->size();  
	if (nrec == 0 || m_simulation_outputs.q_pb.vec()->size() != nrec || m_simulation_outputs.tamb.vec()->size() != nrec || m_simulation_outputs.is_standby.vec()->size() != nrec)
	{
		message_handler("Incomplete set of simulation data. Cycle availability will not be simulated.\n");
		is_cycle_avail_valid = false;
		return false;
	}

	//--- Initialize cycle model
	PowerCycle pc = PowerCycle();
	WELLFiveTwelve gen1(0);
	WELLFiveTwelve gen2(0);
	WELLFiveTwelve gen3(0);
	pc.AssignGenerators(&gen1, &gen2, &gen3);

	initialize_cycle_model(pc);

	double steplength = 8760. / (double)nrec;
	pc.SetSimulationParameters(0, nrec*ny, steplength, 1.e-8, false, ns, m_parameters.cycle_hourly_labor_cost.as_number(), false, false);

	//Assign Dispatch  
	std::unordered_map < std::string, std::vector < double > > dispatch;
	dispatch["cycle_power"].assign(nrec*ny, 0);
	dispatch["thermal_power"].assign(nrec*ny, 0);
	dispatch["ambient_temperature"].assign(nrec*ny, 0);
	dispatch["standby"].assign(nrec*ny, 0);
	for (int i = 0; i < nrec; i++)
	{
		double gross_gen = m_simulation_outputs.gross_gen.vec()->at(i).as_number() * 1.e6; // [W]
		double q_pb = m_simulation_outputs.q_pb.vec()->at(i).as_number() * 1.e6; // [W]
		double tamb = m_simulation_outputs.tamb.vec()->at(i).as_number();  // [C]
		double standby = m_simulation_outputs.is_standby.vec()->at(i).as_number();

		for (int y = 0; y < ny; y++)
		{
			dispatch["cycle_power"].at(y*nrec + i) = gross_gen;
			dispatch["thermal_power"].at(y*nrec + i) = q_pb;
			dispatch["ambient_temperature"].at(y*nrec + i) = tamb;
			dispatch["standby"].at(y*nrec + i) = standby;
		}
	}

	
	pc.SetDispatch(dispatch);

	pc.Simulate(false,false,false);


	//--- Average number of failure events over all scenarios
	double nf = 0.0;
	/*
	std::unordered_map < int, std::vector < std::string > > failure_event_labels;
	for (int s = 0; s < ns; s++)
		nf += pc.m_results.failure_event_labels[s].size() / (double)ns;
	*/

	// for now count failures from capacity/efficiency results
	for (int s = 0; s < ns; s++)
	{
		for (int i = 1; i < nrec*ny; i++)
		{
			if (pc.m_results.cycle_capacity[s][i] < pc.m_results.cycle_capacity[s][i - 1] || pc.m_results.cycle_efficiency[s][i] < pc.m_results.cycle_efficiency[s][i - 1])
				nf += 1;
		}
	}
	nf /= (double)ns;

	



	//--- Calculate yearly-average hourly cycle capacity/efficiency (averaged over all scenarios)
	std::vector<double> avg_cycle_capacity(nrec, 0.0);
	std::vector<double> avg_cycle_efficiency(nrec, 0.0);
	for (int i = 0; i < nrec; i++)
	{
		for (int y = 0; y < ny; y++)
		{
			avg_cycle_capacity.at(i) += pc.m_results.avg_cycle_capacity.at(y*nrec + i) / (double)ny;
			avg_cycle_efficiency.at(i) += pc.m_results.avg_cycle_efficiency.at(y*nrec + i) / (double)ny;
		}
	}

	double nfailures_per_year = nf / (double)ny;
	double avg_labor_cost_per_year = pc.m_results.avg_labor_cost / (double)ny;

	//--- Assign results to structure
	save_cycle_outputs(avg_cycle_capacity, avg_cycle_efficiency, nfailures_per_year, avg_labor_cost_per_year);


	//--- De-rate generation/revenue in simulation outputs based on average cycle availability and efficiency
	double qdes = m_variables.P_ref.as_number() / m_variables.design_eff.as_number();
	std::vector<double> gen_mod(nrec, 0.0);
	for (int i = 0; i < nrec; i++)
	{
		gen_mod[i] = m_simulation_outputs.generation_arr.vec()->at(i).as_number() * avg_cycle_efficiency[i];  //lost efficiency

		// de-rate by lost capacity if operating below currently available level
		double f = m_simulation_outputs.q_pb.vec()->at(i).as_number() / qdes;
		if (avg_cycle_capacity[i] < 0.999 && f>avg_cycle_capacity[i])
			gen_mod[i] *= (avg_cycle_capacity[i]/f);
	}
		

	double annual_gen, annual_rev, cycle_starts, cycle_ramp, cycle_ramp_index;
	accumulate_annual_results(gen_mod, annual_gen, annual_rev, cycle_starts, cycle_ramp, cycle_ramp_index);

	m_simulation_outputs.generation_arr.assign_vector(gen_mod);
	m_simulation_outputs.annual_generation.assign(annual_gen*1.e-6);
	m_simulation_outputs.annual_revenue_units.assign(annual_rev*1.e-6);
	m_simulation_outputs.annual_cycle_starts.assign(cycle_starts);
	m_simulation_outputs.annual_cycle_ramp.assign(cycle_ramp*1.e-6);
	m_simulation_outputs.cycle_ramp_index.assign(cycle_ramp_index * 100);


	is_cycle_avail_valid = true;

	return is_cycle_avail_valid;
}

bool Project::O()
{
	/*
	The heliostat field soiling and degradation problem

	update 10/26: now runs optimization model to calculate optimal number and
	allocation of wash crews to heliostat groups

	Returns a dict with keys :
	soil_steady             Steady - state availability
	n_repairs               Total number of repairs made
	heliostat_refurbish_cost  Cost to refurbish heliostats($ lifetime)
	heliostat_refurbish_cost_y1  Cost "" in year 1 ($ / year)
	*/

	WashCrewOptimizer wc;
	wc = WashCrewOptimizer();
	LinearSoilingFunc f(m_parameters.soil_per_hour.as_number() * 24);
	wc.AssignSoilingFunction(&f);
	wc.Initialize();

	//obtain heliostat information from SSC
	int nr, nc;
	ssc_number_t num_heliostats, term_int_rate, helio_width, helio_height;
	ssc_data_get_number(m_ssc_data, "number_heliostats", &num_heliostats);
	ssc_data_get_number(m_ssc_data, "inflation_rate", &term_int_rate);
	ssc_data_get_number(m_ssc_data, "helio_width", &helio_width);
	ssc_data_get_number(m_ssc_data, "helio_height", &helio_height);
	ssc_number_t *helio_positions = ssc_data_get_matrix(m_ssc_data, "heliostat_positions", &nr, &nc);
	ssc_number_t *helio_ids = ssc_data_get_array(m_ssc_data, "helio_ids", &nr);

	//mirror size comes from heliostat width * height
	//wc.m_solar_data.mirror_size = h_width * h_height;

	//load ssc data into settings, solar field info	
	wc.m_solar_data.num_mirror_groups = num_heliostats;
	wc.m_solar_data.x_pos = new double[num_heliostats];
	wc.m_solar_data.y_pos = new double[num_heliostats];
	wc.m_solar_data.mirror_output = new double[num_heliostats];
	wc.m_solar_data.names = new int[num_heliostats];
	for (int i = 0; i < num_heliostats; i++)
	{
		wc.m_solar_data.x_pos[i] = helio_positions[2*i];
		wc.m_solar_data.y_pos[i] = helio_positions[2*i+1];
		wc.m_solar_data.mirror_output[i] = m_design_outputs.annual_helio_energy.vec()->at(i).as_number() / 1000;  //convert from Wh to kWh
		wc.m_solar_data.names[i] = helio_ids[i];
	}
	//additional settings information from parameters
	wc.m_settings.capital_cost_per_crew = m_parameters.wash_crew_capital_cost.as_number();
	wc.m_settings.heliostat_size = (double)(helio_width * helio_height);
	wc.m_settings.crew_hours_per_week = m_parameters.wash_crew_max_hours_week.as_number();
	wc.m_settings.discount_rate = term_int_rate * 0.01;
	wc.m_settings.hourly_cost_per_crew = m_parameters.wash_crew_cost.as_number();
	wc.m_settings.num_years = m_parameters.plant_lifetime.as_number();
	wc.m_settings.price_per_kwh = m_parameters.price_per_kwh.as_number();
	wc.m_settings.system_efficiency = m_parameters.TES_powercycle_eff.as_number();
	wc.m_settings.wash_rate = m_parameters.wash_rate.as_number();
	wc.m_settings.vehicle_life = m_parameters.wash_vehicle_life.as_integer();
	wc.m_settings.use_uniform_assignment = true;
	wc.m_settings.max_num_crews = 10;
	wc.OptimizeWashCrews();
	while (wc.m_settings.max_num_crews == wc.m_results.num_wash_crews)
	{
		wc.m_settings.max_num_crews *= 2;
		wc.OptimizeWashCrews();
	}

	optical_degradation od;

	//transfer results and heliostat data from wash crew optimization model
	od.m_wc_results = wc.m_results;
	od.m_solar_data = wc.m_solution_data;
	od.m_settings.annual_profit_per_kwh = (
		wc.m_settings.profit_per_kwh / wc.m_settings.annual_multiplier
		);

	od.m_settings.n_hr_sim = m_parameters.finance_period.as_integer() * 8760;
	od.m_settings.n_wash_crews = wc.m_results.num_wash_crews;
	od.m_settings.n_helio = m_design_outputs.number_heliostats.as_integer();
	od.m_settings.degr_loss_per_hr = m_parameters.degr_per_hour.as_number();
	od.m_settings.degr_accel_per_year = m_parameters.degr_accel_per_year.as_number();
	//od.m_settings.replacement_threshold = m_variables.degr_replace_limit.as_number();
	od.m_settings.heliostat_refurbish_cost = m_parameters.heliostat_refurbish_cost.as_number() * wc.m_settings.heliostat_size;
	od.m_settings.soil_loss_per_hr = m_parameters.soil_per_hour.as_number();
	od.m_settings.wash_units_per_hour = m_parameters.wash_rate.as_number() / wc.m_settings.heliostat_size;
	od.m_settings.hours_per_week = m_parameters.wash_crew_max_hours_week.as_number();
	od.m_settings.hours_per_day = m_parameters.wash_crew_max_hours_day.as_number();
	od.m_settings.seed = m_parameters.degr_seed.as_integer();
	od.m_settings.refl_sim_interval = 168;
	od.m_settings.soil_sim_interval = 24;
	od.m_settings.use_mean_replacement_threshold = false;
	od.m_settings.use_fixed_replacement_threshold = false;

	od.simulate(sim_progress_handler);

	double ann_fact = 8760. / (double)od.m_settings.n_hr_sim;
	
	od.m_results.heliostat_refurbish_cost_y1 = (
		od.m_results.n_replacements * od.m_settings.heliostat_refurbish_cost
		);
	
	//Annualize
	od.m_results.n_replacements *= ann_fact;
	
	od.m_results.heliostat_refurbish_cost = calc_real_dollars( od.m_results.heliostat_refurbish_cost_y1 ) * ann_fact;

    //assign results to structure
	m_optical_outputs.n_wash_crews.assign(wc.m_results.num_wash_crews);
    m_optical_outputs.n_replacements.assign(od.m_results.n_replacements);
    m_optical_outputs.heliostat_refurbish_cost.assign(od.m_results.heliostat_refurbish_cost);
    m_optical_outputs.heliostat_refurbish_cost_y1.assign(od.m_results.heliostat_refurbish_cost_y1);
    m_optical_outputs.avg_soil.assign(od.m_results.avg_soil);
    m_optical_outputs.avg_degr.assign(od.m_results.avg_degr);

    m_optical_outputs.soil_schedule.assign_vector( od.m_results.soil_schedule, od.m_results.n_schedule );
	m_optical_outputs.repl_schedule.assign_vector(od.m_results.repl_schedule, od.m_results.n_schedule);
	m_optical_outputs.degr_schedule.assign_vector(od.m_results.degr_schedule, od.m_results.n_schedule);
	m_optical_outputs.repl_total.assign_vector(od.m_results.repl_total, od.m_results.n_schedule);

	is_sf_optical_valid = true;
	return true;
}

bool Project::S()
{
	//--- error if invalid design
	std::string error_msg;
	bool ok = Validate(Project::CALLING_SIM::SIMULATION, &error_msg);
	message_handler(error_msg.c_str());
	if (!ok)
		return false;


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
	double sim_ts = 1. / (double)wf_steps_per_hour;

	ssc_data_set_number(m_ssc_data, "time_steps_per_hour", wf_steps_per_hour);
	ssc_data_set_number(m_ssc_data, "disp_mip_gap", 0.02);

	//--- Set the solar field availability schedule
	std::vector<double> avail(nrec, 1.);
	std::vector<double> soil(nrec, 1.);
	std::vector<double> degr(nrec, 1.);

	if (is_sf_avail_valid)
	{
		int avail_ts = m_parameters.avail_model_timestep.as_integer();
		calc_avg_annual_schedule(avail_ts, sim_ts, m_solarfield_outputs.avail_schedule, avail);
	}
	if (is_sf_optical_valid)
	{
		int op_ts = 1.0;  // assume 1hr time step in optical model
		calc_avg_annual_schedule(op_ts, sim_ts, m_optical_outputs.soil_schedule, soil);
		calc_avg_annual_schedule(op_ts, sim_ts, m_optical_outputs.degr_schedule, degr);
	}

	ssc_number_t *p_sf = new ssc_number_t[nrec];
	for (int i = 0; i<nrec; i++)
		p_sf[i] = 100.*(1. - avail[i]*soil[i]*degr[i]);
	ssc_data_set_array(m_ssc_data, "sf_adjust:hourly", p_sf, nrec);
	delete p_sf;


	//--- Set ssc parameters
	ssc_number_t val, nhel, helio_height, helio_width, dens_mirror;
	ssc_data_set_number(m_ssc_data, "P_ref", m_variables.P_ref.as_number());
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

	int nr, nc;
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

	ssc_data_set_number(m_ssc_data, "allow_controller_exceptions", 0);




	//--- Run ssc simulation.  Cycle availability model will be run separately 
	unordered_map < std::string, std::vector<double>> ssc_soln;
	if (!m_parameters.is_cycle_ssc_integration.as_boolean())
	{
		if (m_parameters.is_use_clusters.as_boolean())
			is_simulation_valid = simulate_clusters(ssc_soln);
		else  
		{
			ssc_module_exec_set_print(m_parameters.print_messages.as_boolean());
			ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");

			if (!ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0))
			{
				message_handler("SSC simulation failed");
				ssc_module_free(mod_mspt);
				is_simulation_valid = false;
				return false;
			}
			ssc_module_free(mod_mspt);
			std::vector<std::string> ssc_keys = { "gen", "P_cycle", "q_pb", "q_dot_pc_startup", "Q_thermal", "e_ch_tes", "beam", "tdry", "pricing_mult", "disp_qsfprod_expected", "disp_wpb_expected"};
			ssc_soln = ssc_data_to_map(m_ssc_data, ssc_keys);
			is_simulation_valid = true;
		}
		is_simulation_valid = save_simulation_outputs(ssc_soln);
	}



	//--- Run ssc simulation including re-evaluation or re-dispatch at points of cycle failure/repair. 
	PowerCycle pc;
	WELLFiveTwelve gen1(0);
	WELLFiveTwelve gen2(0);
	WELLFiveTwelve gen3(0);
	pc.AssignGenerators(&gen1, &gen2, &gen3);

	if (m_parameters.is_cycle_ssc_integration.as_boolean())
	{

		if (!m_parameters.is_dispatch.as_boolean())
		{
			message_handler("Integration of cycle availability and plant simulation models is only available when dispatch optimization is enabled.");
			return false;
		}

		if (m_parameters.num_scenarios.as_integer() > 1 || m_parameters.cycle_nyears.as_integer() > 1)
		{
			message_handler(wxString::Format("Integration of cycle availability and plant simulation models is only available for a single scenario and single year. Parameter 'num_scenarios' and 'cycle_nyears' are currently %d and %d, respectively. \n", m_parameters.num_scenarios.as_integer(), m_parameters.cycle_nyears.as_integer()));
			return false;
		}

		if (m_parameters.is_reoptimize_at_failures.as_boolean() && !m_parameters.is_reoptimize_at_failures.as_boolean())
			message_handler(wxString::Format("'is_reoptimize_at_failures was set to true.  'is_reoptimize_at_repairs' will be reset to true"));


		if (m_parameters.is_use_clusters.as_boolean()) 
			is_simulation_valid = simulate_clusters(ssc_soln);
		else 
		{
			cycle_ssc_integration_inputs inputs;
			plant_state final_state;
			std::vector<double> cycle_capacity, cycle_efficiency;
			int n_failures;
			double labor_cost;
			inputs.horizon = 168.;
			initialize_cycle_model(pc);
			is_simulation_valid = integrate_cycle_and_simulation(pc, inputs, final_state, ssc_soln, cycle_capacity, cycle_efficiency, n_failures, labor_cost);
			save_cycle_outputs(cycle_capacity, cycle_efficiency, n_failures, labor_cost);
			
		}
		is_simulation_valid = save_simulation_outputs(ssc_soln);
		is_cycle_avail_valid = is_simulation_valid;	
	}


	return is_simulation_valid;
}

bool Project::E()
{
	// Receiver cost
	double arec = m_variables.D_rec.as_number() * 3.14159 * m_variables.rec_height.as_number();
	double e_rec = m_parameters.rec_ref_cost.as_number() * pow(arec / m_parameters.rec_ref_area.as_number(), 0.7);
	double e_rec_real = calc_real_dollars(e_rec);
	
	// Tower cost
	double e_tower = m_parameters.tower_fixed_cost.as_number() * exp(m_parameters.tower_exp.as_number() * m_variables.h_tower.as_number());
	double e_tower_real = calc_real_dollars(e_tower);

	// Plant cost
	double del_eff = m_variables.design_eff.as_number() - 0.412;
	double effscale = 0.0;
	for (int i = 0; i < m_parameters.c_ces.vec()->size(); i++)
		effscale += m_parameters.c_ces.vec()->at(i).as_number() * pow(del_eff, i);

	double e_plant = (m_parameters.c_cps0.as_number() + m_parameters.c_cps1.as_number() * m_variables.P_ref.as_number()*1000.) *(1.0 + effscale);
	double e_plant_real = calc_real_dollars(e_plant);

	// TES cost
	double e_tes = m_parameters.tes_spec_cost.as_number() * m_variables.tshours.as_number() * (m_variables.P_ref.as_number() * 1000. / m_variables.design_eff.as_number());
	double e_tes_real = calc_real_dollars(e_tes);

	// OM labor costs
	double heliostat_om_labor_y1 = m_parameters.om_staff_cost.as_number() * m_variables.om_staff.as_number() * m_parameters.om_staff_max_hours_week.as_number()*52.;
	double heliostat_om_labor = calc_real_dollars(heliostat_om_labor_y1, false, true);

	// Washing labor costs
	double heliostat_wash_cost_y1 = m_parameters.wash_crew_cost.as_number() * m_optical_outputs.n_wash_crews.as_number() * m_parameters.wash_crew_max_hours_week.as_number()*52.;
	double heliostat_wash_cost = calc_real_dollars(heliostat_wash_cost_y1, false, true);

	// Washing capital costs
	double heliostat_wash_capital_cost = m_parameters.wash_crew_capital_cost.as_number();
	for (int i = m_parameters.wash_vehicle_life.as_integer(); i < m_parameters.plant_lifetime.as_integer(); i += m_parameters.wash_vehicle_life.as_integer())
	{
		heliostat_wash_capital_cost += calc_real_dollars( m_parameters.wash_crew_capital_cost.as_number(), false, false, true, i );
	}
	heliostat_wash_capital_cost *= m_optical_outputs.n_wash_crews.as_number();


	m_explicit_outputs.cost_receiver_real.assign(e_rec_real);
	m_explicit_outputs.cost_tower_real.assign(e_tower_real);
	m_explicit_outputs.cost_plant_real.assign(e_plant_real);
	m_explicit_outputs.cost_tes_real.assign(e_tes_real);

	m_explicit_outputs.heliostat_om_labor_y1.assign(heliostat_om_labor_y1);
	m_explicit_outputs.heliostat_om_labor_real.assign(heliostat_om_labor);

	m_explicit_outputs.heliostat_wash_cost_y1.assign(heliostat_wash_cost_y1);
	m_explicit_outputs.heliostat_wash_cost_real.assign(heliostat_wash_cost);

	m_explicit_outputs.heliostat_wash_capital_cost.assign(heliostat_wash_capital_cost);

	is_explicit_valid = true;

	return is_explicit_valid;
}

bool Project::F()
{
	// error if invalid design or simulation
	std::string error_msg;
	bool ok = Validate(Project::CALLING_SIM::FINANCE, &error_msg);
	message_handler(error_msg.c_str());
	if (!ok)
		return false;

	// Set ssc generation array from simulation outputs
	size_t n = m_simulation_outputs.generation_arr.vec()->size();
	ssc_number_t *p_gen = new ssc_number_t[n];
	for (size_t i = 0; i < n; i++)
		p_gen[i] = m_simulation_outputs.generation_arr.vec()->at(i).as_number();
	ssc_data_set_array(m_ssc_data, "gen", p_gen, n);
	delete[] p_gen;


	// om_costs
	double om_cost = 0.0;
	om_cost += m_explicit_outputs.heliostat_om_labor_y1.as_number();
	om_cost += m_explicit_outputs.heliostat_wash_cost_y1.as_number();

	om_cost += m_simulation_outputs.annual_rec_starts.as_number() * m_parameters.disp_rsu_cost.as_number();
	om_cost += m_simulation_outputs.annual_cycle_starts.as_number() * m_parameters.disp_csu_cost.as_number();
	om_cost += m_simulation_outputs.annual_cycle_ramp.as_number() * 1.e6 * m_parameters.disp_pen_delta_w.as_number();

	if (is_sf_avail_valid)
		om_cost += m_solarfield_outputs.heliostat_repair_cost_y1.as_number();
	if (is_sf_optical_valid)
		om_cost += m_optical_outputs.heliostat_refurbish_cost_y1.as_number();

	ssc_number_t *p_om = new ssc_number_t[1];
	p_om[0] = om_cost;
	ssc_data_set_array(m_ssc_data, "om_fixed", p_om, 1);
	delete[] p_om;


	// run financial model
	ssc_module_exec_set_print(m_parameters.print_messages.as_boolean());
	ssc_module_t mod_fin = ssc_module_create("singleowner");

	if (!ssc_module_exec_with_handler(mod_fin, m_ssc_data, ssc_progress_handler, 0))
	{
		message_handler("Financial simulation failed");
		ssc_module_free(mod_fin);
		return false;
	}

	// collect outputs
	ssc_number_t val;
	ssc_data_get_number(m_ssc_data, "lcoe_nom", &val);
	m_financial_outputs.lcoe_nom.assign(val);

	ssc_data_get_number(m_ssc_data, "lcoe_real", &val);
	m_financial_outputs.lcoe_real.assign(val);

	ssc_data_get_number(m_ssc_data, "ppa", &val);
	m_financial_outputs.ppa.assign(val);

	ssc_data_get_number(m_ssc_data, "project_return_aftertax_npv", &val);
	m_financial_outputs.project_return_aftertax_npv.assign(val);

	ssc_data_get_number(m_ssc_data, "project_return_aftertax_irr", &val);
	m_financial_outputs.project_return_aftertax_irr.assign(val);

	ssc_data_get_number(m_ssc_data, "total_installed_cost", &val);
	m_financial_outputs.total_installed_cost.assign(val);

	ssc_module_free(mod_fin);

	is_financial_valid = true;

	return is_financial_valid;
}

bool Project::Z()
{

	// Check for existing simulations 
	// Design
	if (!is_design_valid)
	{
		is_sf_avail_valid = false;
		is_sf_optical_valid = false;
		is_simulation_valid = false;
		is_cycle_avail_valid = false;
		is_explicit_valid = false;
		is_financial_valid = false;
		D();
	}
	else
		message_handler("Using existing solar field design in objective function");

	// Availability
	if (!is_sf_avail_valid)
	{
		is_sf_optical_valid = false;
		is_simulation_valid = false;
		is_cycle_avail_valid = false;
		is_financial_valid = false;
		M();
	}		
	else
		message_handler("Using existing heliostat field availability results in objective function");


	// Degradiation/soiling
	if (!is_sf_optical_valid)
	{
		is_simulation_valid = false;
		is_cycle_avail_valid = false;
		is_financial_valid = false;
		O();
	}
	else
		message_handler("Using existing heliostat field soiling/degradation results in objective function");

	// Simulation and cycle availability
	if (!is_simulation_valid)
	{
		is_cycle_avail_valid = false;
		is_financial_valid = false;
		S();
	}
	else
		message_handler("Using existing annual performance and cycle availability results in objective function");

	// Cycle efficiency/capacity
	if (!is_cycle_avail_valid)
	{
		is_financial_valid = false;
		C();		// simulates cycle availabiltiy and de-rates generation/revenue based on availability (averaged over scenarios and simulated years)
	}
	else
		message_handler("Using existing cycle availability results in objective function");
	

	// Explicit cost terms
	if (!is_explicit_valid)
	{
		is_financial_valid = false;
		E();
	}
	else
		message_handler("Using existing cost results in objective function");


	// Financial simulation
	if (!is_financial_valid)
		F();
	else
		message_handler("Using existing financial results in objective function");


	//-- Capital costs -> all already defined in outputs from E() and D()
	m_objective_outputs.cost_receiver_real.assign(m_explicit_outputs.cost_receiver_real.as_number()); // E
	m_objective_outputs.cost_tower_real.assign(m_explicit_outputs.cost_tower_real.as_number());		  // E
	m_objective_outputs.cost_plant_real.assign(m_explicit_outputs.cost_plant_real.as_number());		  // E
	m_objective_outputs.cost_tes_real.assign(m_explicit_outputs.cost_tes_real.as_number());			  // E
	m_objective_outputs.cost_land_real.assign(m_design_outputs.cost_land_real.as_number());			  // D
	m_objective_outputs.cost_sf_real.assign(m_design_outputs.cost_sf_real.as_number());				  // D
	m_objective_outputs.heliostat_wash_capital_cost.assign(m_explicit_outputs.heliostat_wash_capital_cost.as_number()); //E

	double cap_cost = m_objective_outputs.cost_receiver_real.as_number() +
					m_objective_outputs.cost_tower_real.as_number() +
					m_objective_outputs.cost_plant_real.as_number() +
					m_objective_outputs.cost_tes_real.as_number() +
					m_objective_outputs.cost_land_real.as_number() +
					m_objective_outputs.cost_sf_real.as_number() + 
					m_objective_outputs.heliostat_wash_capital_cost.as_number();

	m_objective_outputs.cap_cost_real.assign(cap_cost);


	//-- O&M costs
	m_objective_outputs.heliostat_om_labor_real.assign(m_explicit_outputs.heliostat_om_labor_real.as_number());				// E
	m_objective_outputs.heliostat_wash_cost_real.assign(m_explicit_outputs.heliostat_wash_cost_real.as_number());			// E
	m_objective_outputs.heliostat_repair_cost_real.assign(m_solarfield_outputs.heliostat_repair_cost.as_number());		// M
	m_objective_outputs.heliostat_refurbish_cost_real.assign(m_optical_outputs.heliostat_refurbish_cost.as_number());	// O

	double rec_start_cost_y1 = m_simulation_outputs.annual_rec_starts.as_number() * m_parameters.disp_rsu_cost.as_number();
	double cycle_start_cost_y1 = m_simulation_outputs.annual_cycle_starts.as_number() * m_parameters.disp_csu_cost.as_number();
	double cycle_ramp_cost_y1 = m_simulation_outputs.annual_cycle_ramp.as_number() * 1.e6 * m_parameters.disp_pen_delta_w.as_number();

	m_objective_outputs.rec_start_cost_real.assign(calc_real_dollars(rec_start_cost_y1));
	m_objective_outputs.cycle_start_cost_real.assign(calc_real_dollars(cycle_start_cost_y1));
	m_objective_outputs.cycle_ramp_cost_real.assign(calc_real_dollars(cycle_ramp_cost_y1));

	m_objective_outputs.cycle_repair_cost_real.assign(m_cycle_outputs.cycle_labor_cost.as_number());  // C
	
	double om_cost = m_objective_outputs.heliostat_om_labor_real.as_number() +
					m_objective_outputs.heliostat_wash_cost_real.as_number() +
					m_objective_outputs.heliostat_repair_cost_real.as_number() +
					m_objective_outputs.heliostat_refurbish_cost_real.as_number() +
					m_objective_outputs.rec_start_cost_real.as_number() +
					m_objective_outputs.cycle_start_cost_real.as_number() +
					m_objective_outputs.cycle_ramp_cost_real.as_number() +
					m_objective_outputs.cycle_repair_cost_real.as_number();
	
	m_objective_outputs.om_cost_real.assign(om_cost);

	//-- Revenue
	ssc_number_t ppa_price_input;
	ssc_data_get_number(m_ssc_data, "ppa_price_input", &ppa_price_input);
	double rev = m_simulation_outputs.annual_revenue_units.as_number() * 1.e6 * ppa_price_input;  
	double sales = calc_real_dollars(rev, true);
	m_objective_outputs.sales.assign(sales);

	// Cash flow
	m_objective_outputs.cash_flow.assign(sales - cap_cost - om_cost);

	// Financial model results
	m_objective_outputs.ppa.assign(m_financial_outputs.ppa.as_number());				// F
	m_objective_outputs.lcoe_nom.assign(m_financial_outputs.lcoe_nom.as_number());		// F
	m_objective_outputs.lcoe_real.assign(m_financial_outputs.lcoe_real.as_number());	// F


	return true;

}

bool Project::setup_clusters()
{

	//-- Set up metrics for cluster creation
	clustering_metrics metrics; 
	metrics.set_default_inputs();
	metrics.inputs.nsimdays = m_parameters.cluster_ndays.as_integer();
	metrics.inputs.stowlimit = m_parameters.v_wind_max.as_number();

	metrics.inputs.weather_files.clear();
	std::string weatherfile = m_parameters.solar_resource_file.as_string();
	metrics.inputs.weather_files.push_back(weatherfile);
	metrics.inputs.is_price_files = false;
    metrics.inputs.prices.clear();
    metrics.inputs.prices.resize( m_parameters.dispatch_factors_ts.vec()->size() );
    for (size_t i = 0; i < m_parameters.dispatch_factors_ts.vec()->size(); i++)
        metrics.inputs.prices.at(i) = m_parameters.dispatch_factors_ts.vec()->at(i).as_number();

	if (m_parameters.clustering_feature_weights.vec()->size() != 9 || m_parameters.clustering_feature_weights.vec()->size() != 9)
		message_handler("Incomplete set of clustering feature weighting factors or divisions were specified. Default values will be used");
	else
	{
		std::vector<std::string> feature_order = { "dni", "dni_prev", "dni_next", "clearsky", "price", "price_prev", "price_next", "tdry", "wspd" };
		for (int i = 0; i < feature_order.size(); i++)
		{
			metrics.inputs.features[feature_order[i]].weight = m_parameters.clustering_feature_weights.vec()->at(i).as_number();
			metrics.inputs.features[feature_order[i]].divisions = m_parameters.clustering_feature_divisions.vec()->at(i).as_integer();
		}
	}

	if (!m_parameters.is_dispatch.as_boolean())  // don't include price if not using dispatch optimization
	{
		metrics.inputs.features["price"].weight = 0.0;
		metrics.inputs.features["price_prev"].weight = 0.0;
		metrics.inputs.features["price_next"].weight = 0.0;
	}



	//--- Get the solar field availability
	int nrec;
	ssc_number_t *p_sf = ssc_data_get_array(m_ssc_data, "sf_adjust:hourly", &nrec);
	metrics.inputs.sfavail.clear();
	for (int i = 0; i < nrec; i++)
		metrics.inputs.sfavail.push_back(1. - p_sf[i] / 100.);

	
	//-- Set up clustering parameters
	cluster_alg cluster;
	cluster.set_default_inputs();
	cluster.inputs.ncluster = m_parameters.n_clusters.as_integer();
	cluster.inputs.hard_partitions = true;

	std::string ca = m_parameters.cluster_algorithm.as_string();
	if (ca == "affinity_propagation")
		cluster.inputs.alg = AFFINITY_PROPAGATION;
	else if (ca == "kmeans")
		cluster.inputs.alg = KMEANS;
	else if (ca == "random")
		cluster.inputs.alg = RANDOM_SELECTION;
	else
	{
		message_handler("Cluster setup failed because algorithm is not recognized. Valid inputs are 'affinity_propagation', 'kmeans', or 'random'. Suggested input is 'affinity_propagation'");
		return false;
	}

	//--- Calculate metrics and create clusters
	metrics.calc_metrics();
	metric_outputs = metrics.results;

	cluster.create_clusters(metrics.results.data);
	cluster_outputs = cluster.results;


	//message_handler(wxString::Format("Cluster wcss = %.4f", cluster_outputs.wcss));
	//message_handler(wxString::Format("Cluster wcss based on exemplar location = %.4f", cluster_outputs.wcss_to_exemplars));
	//message_handler("Cluster exemplars,  weights:");
	//for (int i = 0; i < cluster_outputs.ncluster; i++)
	//	message_handler(wxString::Format("%d, %.6f", cluster_outputs.exemplars.at(i), cluster_outputs.weights.at(i)));

	return true;
}


bool Project::simulate_clusters(std::unordered_map<std::string, std::vector<double>> &ssc_soln)
{
	// Run simulation of cluster exemplars and aggregate into full-year output arrays


	int nr;
	double nan = std::numeric_limits<double>::quiet_NaN();
	ssc_soln.clear();

	//--- Set cluster simulation parameters
	cluster_sim csim;
	csim.set_default_inputs();
	csim.inputs.days.nnext = int(m_parameters.is_dispatch.as_boolean());
	csim.inputs.is_run_continuous = m_parameters.is_run_continuous.as_boolean();
	csim.inputs.days.ncount = m_parameters.cluster_ndays.as_integer();
	csim.inputs.days.nprev = m_parameters.cluster_nprev.as_integer();



	//--- Initialize ssc results
	ssc_module_exec_set_print(m_parameters.print_messages.as_boolean());
	ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");

	ssc_number_t wf_steps_per_hour;
	ssc_data_get_number(m_ssc_data, "time_steps_per_hour", &wf_steps_per_hour);
	int nperday = (int)wf_steps_per_hour * 24;
	int nrec = (int)wf_steps_per_hour * 8760;

	unordered_map < std::string, std::vector<double>> collect_ssc_data;
	std::vector<std::string> ssc_keys = { "gen", "P_cycle", "q_pb", "q_dot_pc_startup", "Q_thermal", "e_ch_tes", "beam", "tdry", "pricing_mult", "disp_qsfprod_expected", "disp_wpb_expected" };
	if (m_parameters.is_cycle_ssc_integration.as_boolean())
	{
		std::vector<std::string> more_keys = { "P_out_net", "T_tes_hot","T_tes_cold","q_dot_pc_target","q_dot_pc_max","is_rec_su_allowed","is_pc_su_allowed","is_pc_sb_allowed" };
		ssc_keys.insert(ssc_keys.end(), more_keys.begin(), more_keys.end());
	}

	
	//--- Set up clusters unless already defined
	if (abs(m_parameters.n_clusters.as_integer() - (int)cluster_outputs.exemplars.size()) > 1 )
	{
		if (!setup_clusters())
			return false;
	}
	else
		message_handler(wxString::Format("Using existing set of %d cluster exemplars", cluster_outputs.ncluster));

	csim.inputs.cluster_results = cluster_outputs;
	csim.assign_first_last(metric_outputs);
	int ncl = csim.inputs.cluster_results.ncluster;


	//--- Get the solar field availability and calculate cluster-average availability
	ssc_number_t *p_sf = ssc_data_get_array(m_ssc_data, "sf_adjust:hourly", &nr);
	std::vector<double>sfavail;
	for (int i = 0; i < nr; i++)
		sfavail.push_back(1. - p_sf[i]/100.);

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



	//==============================================================

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
				avg_prev_dni += metric_outputs.daily_dni.at(d, 0) / (double)ng;
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
		if (!ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0))
		{
			message_handler("SSC simulation failed");
			ssc_module_free(mod_mspt);
			return false;
		}

		collect_ssc_data = ssc_data_to_map(m_ssc_data, ssc_keys);

		ssc_data_unassign(m_ssc_data, "select_simulation_days");
	}



	//==============================================================

	//--- Option 2: Run simulation of each cluster exemplar independently in discrete limited time-duration simulations
	if (!csim.inputs.is_run_continuous)
	{
		for (size_t i = 0; i < ssc_keys.size(); i++)
			collect_ssc_data[ssc_keys[i]].assign(nrec, nan);

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
				double prev_dni = metric_outputs.daily_dni.at(d, 0);
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


			// Run simulation 
			if (!ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0))
			{
				message_handler("SSC simulation failed");
				ssc_module_free(mod_mspt);
				return false;
			}

			// Fill in exemplar time block in full year array with ssc solution
			int doy_full = d1;
			int doy_sim = nprev;
			for (size_t k = 0; k < ssc_keys.size(); k++)
			{
				std::string key = ssc_keys[k];
				ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
				for (int h = 0; h < nperday*ncount; h++)
					collect_ssc_data[key].at(doy_full*nperday + h) = p_data[doy_sim*nperday + h];
			}

		}
	}

	ssc_module_free(mod_mspt);
	
	

	//--- Compute full annual array from array containing simulated values at cluster-exemplar time blocks
	std::unordered_map < std::string, std::vector<double> >::iterator it;
	for (it = collect_ssc_data.begin(); it != collect_ssc_data.end(); it++)
		csim.compute_annual_array_from_clusters(it->second, ssc_soln[it->first]);

	ssc_soln["beam_clusters"] = ssc_soln["beam"];
	ssc_soln["pricing_mult_clusters"] = ssc_soln["pricing_mult"];


	//-- Read in full-year weather/price for results if necessary (if running discrete increments "beam", "tdry", and "pricing_mult" contain only values at the cluster exemplars)
	ssc_soln["beam"] = collect_ssc_data["beam"];
	ssc_soln["tdry"] = collect_ssc_data["tdry"];
	ssc_soln["pricing_mult"] = collect_ssc_data["pricing_mult"];
	if (!csim.inputs.is_run_continuous) 
	{
		ssc_data_t ssc_data_wf = ssc_data_create();
		ssc_module_t mod_wf = ssc_module_create("wfreader");
		ssc_data_set_string(ssc_data_wf, "file_name", m_parameters.solar_resource_file.as_string().c_str());
		ssc_module_exec(mod_wf, ssc_data_wf);
		std::vector<std::string> wfkeys = { "beam", "tdry" };
		std::unordered_map<std::string, std::vector<double>> wf = ssc_data_to_map(ssc_data_wf, wfkeys);
		ssc_soln["beam"] = wf["beam"];
		ssc_soln["tdry"] = wf["tdry"];

		ssc_module_free(mod_wf);
		ssc_data_free(ssc_data_wf);

		ssc_soln["pricing_mult"].clear();
		for (size_t i = 0; i < m_parameters.dispatch_factors_ts.vec()->size(); i++)
			ssc_soln["pricing_mult"].push_back(m_parameters.dispatch_factors_ts.vec()->at(i).as_number());
	}


	//--- Integrate ssc solution with cycle availability model?
	if (m_parameters.is_cycle_ssc_integration.as_boolean())
	{
		std::unordered_map<std::string, std::vector<double>> modified_soln;
		if (!integrate_cycle_and_clusters(ssc_soln, csim.inputs.skip_first.index[0], csim.inputs.skip_last.index[0], modified_soln))
			return false;

		for (it = modified_soln.begin(); it != modified_soln.end(); it++)
		{
			std::string key = it->first;
			if (key != "beam" && key != "tdry" && key != "pricing_mult" && key != "beam_clusters" && key != "pricing_mult_clusters")
				ssc_soln[it->first] = it->second;
		}

		modified_soln.clear();
	}

	return true;
}

std::unordered_map<std::string, std::vector<double>> Project::ssc_data_to_map(const ssc_data_t & ssc_data, std::vector<std::string> keys)
{
	int nr;
	unordered_map < std::string, std::vector<double>> ssc_data_map;
	for (size_t i = 0; i < keys.size(); i++)
	{
		std::string key = keys[i];
		ssc_number_t *p_data = ssc_data_get_array(ssc_data, key.c_str(), &nr);
		for (int r = 0; r < nr; r++)
			ssc_data_map[key].push_back(p_data[r]);
	}
	return ssc_data_map;
}

bool Project::accumulate_annual_results(const std::vector<double> &soln, double &sum, double &summult_price, double &starts, double &ramp, double &ramp_index)
{
	// Calculate annual total, annual total of value*price, # starts, total ramping, and ramping index from annual solution array
	sum = 0.0;
	summult_price = 0.0;
	starts = 0.0;
	ramp = 0.0;
	double maxval = 0.0;

	int nrec = (int)soln.size();
	double steps_per_hour = nrec / 8760.;
	int nrec_price = (int)m_parameters.dispatch_factors_ts.vec()->size();
	if (nrec == 0 || nrec != nrec_price)
		return false;


	for (int d = 0; d < 365; d++)
	{
		int i0 = 0;
		if (d == 0 || (m_parameters.is_use_clusters.as_boolean() && (d - 1) % m_parameters.cluster_ndays.as_integer()))  // skip first point of some days when using clustering because of discontinuities with previous exemplar
			i0 = 1;

		for (int i = 0; i < 24 * steps_per_hour; i++)
		{
			int p = d * 24 * steps_per_hour + i;

			sum += soln[p] / steps_per_hour;
			summult_price += soln[p] * m_parameters.dispatch_factors_ts.vec()->at(p).as_number() / steps_per_hour;
			maxval = fmax(maxval, soln[p]);

			if (i>=i0 && soln[p] > 0.0)
			{
				if (soln[p - 1] <= 0.0)
					starts += 1;

				if (soln[p] > soln[p - 1])
					ramp += soln[p] - soln[p - 1]; //kWe
			}

		}
	}

	ramp_index = ramp / maxval / 365.;

	return true;
}

bool Project::save_simulation_outputs(std::unordered_map < std::string, std::vector<double>> &ssc_soln)
{
	// Save results from ssc simulation in simulation_outputs

	double annual_generation, revenue_units, cycle_starts, cycle_ramp, cycle_ramp_index, rec_starts;
	double disp_cycle_starts, disp_cycle_ramp, disp_cycle_ramp_index, disp_rec_starts;

	annual_generation = revenue_units = cycle_starts = cycle_ramp = cycle_ramp_index = rec_starts = std::numeric_limits<double>::quiet_NaN();
	disp_cycle_starts = disp_cycle_ramp = disp_cycle_ramp_index = disp_rec_starts = std::numeric_limits<double>::quiet_NaN();


	//--- Check for completeness of data in ssc_soln
	std::vector<std::string> keys = { "gen", "P_cycle", "Q_thermal", "e_ch_tes", "q_pb", "q_dot_pc_startup", "beam", "tdry", "pricing_mult" };
	if (m_parameters.is_dispatch.as_boolean())
	{
		keys.push_back("disp_wpb_expected");
		keys.push_back("disp_qsfprod_expected");
	}
	if (m_parameters.is_use_clusters.as_boolean())
	{
		keys.push_back("beam_clusters");
		keys.push_back("pricing_mult_clusters");
	}
	
	unordered_map < std::string, std::vector<double>>::iterator it = ssc_soln.find("gen");
	int nrec = (int)it->second.size();
	for (size_t k = 0; k < keys.size(); k++)
	{
		it = ssc_soln.find(keys[k]);
		if (it == ssc_soln.end() || it->second.size() != nrec)
		{
			message_handler("Incomplete set of data in ssc solution. Simulation results will not be saved.\n");
			return false;
		}
	}



	//--- Annual results
	double sum, sum_price, starts, ramp, ramp_index;

	accumulate_annual_results(ssc_soln["gen"], sum, sum_price, starts, ramp, ramp_index);
	annual_generation = sum * 1.e-6;
	revenue_units = sum_price * 1.e-6;
	cycle_starts = starts;
	cycle_ramp = ramp * 1.e-6;
	cycle_ramp_index = ramp_index * 100;
	
	accumulate_annual_results(ssc_soln["Q_thermal"], sum, sum_price, starts, ramp, ramp_index);
	rec_starts = starts;

	accumulate_annual_results(ssc_soln["disp_wpb_expected"], sum, sum_price, starts, ramp, ramp_index);
	disp_cycle_starts = starts;
	disp_cycle_ramp = ramp*1.e-3;  //GW
	disp_cycle_ramp_index = ramp_index*100;

	accumulate_annual_results(ssc_soln["disp_qsfprod_expected"], sum, sum_price, starts, ramp, ramp_index);
	disp_rec_starts = starts;

	// Check if cycle is in standby
	std::vector<double> standby(nrec, 0);
	for (int i = 0; i > nrec; i++)
	{
		if (ssc_soln["P_cycle"][i] < 1.e-6 && ssc_soln["q_pb"][i] > 0.0 && ssc_soln["q_dot_pc_startup"][i] < ssc_soln["q_pb"][i])  // Thermal energy going to power block, but no electrical output
			standby[i] = 1.0;
	}


	//--- Set simulation outputs
	m_simulation_outputs.generation_arr.assign_vector(ssc_soln["gen"]);
	m_simulation_outputs.solar_field_power_arr.assign_vector(ssc_soln["Q_thermal"]);
	m_simulation_outputs.tes_charge_state.assign_vector(ssc_soln["e_ch_tes"]);
	m_simulation_outputs.dni_arr.assign_vector(ssc_soln["beam"]);
	m_simulation_outputs.price_arr.assign_vector(ssc_soln["pricing_mult"]);

	m_simulation_outputs.gross_gen.assign_vector(ssc_soln["P_cycle"]);
	m_simulation_outputs.q_pb.assign_vector(ssc_soln["q_pb"]);
	m_simulation_outputs.tamb.assign_vector(ssc_soln["tdry"]);
	m_simulation_outputs.is_standby.assign_vector(standby);
	if (m_parameters.is_use_clusters.as_boolean())
	{
		m_simulation_outputs.dni_templates.assign_vector(ssc_soln["beam_clusters"]);
		m_simulation_outputs.price_templates.assign_vector(ssc_soln["pricing_mult_clusters"]);
	}

	m_simulation_outputs.annual_generation.assign(annual_generation);
	m_simulation_outputs.annual_revenue_units.assign(revenue_units);
	m_simulation_outputs.annual_rec_starts.assign(rec_starts);
	m_simulation_outputs.annual_cycle_starts.assign(cycle_starts);
	m_simulation_outputs.annual_cycle_ramp.assign(cycle_ramp);
	m_simulation_outputs.cycle_ramp_index.assign(cycle_ramp_index);

	m_simulation_outputs.annual_rec_starts_disp.assign(disp_rec_starts);
	m_simulation_outputs.annual_cycle_starts_disp.assign(disp_cycle_starts);
	m_simulation_outputs.annual_cycle_ramp_disp.assign(disp_cycle_ramp);
	m_simulation_outputs.cycle_ramp_index_disp.assign(disp_cycle_ramp_index);


	return true;
}

void Project::calc_avg_annual_schedule(double original_ts, double new_ts, const parameter &full_sch, std::vector<double> &output_sch)
{
    // Convert full multiple-year availability, soiling, or degradation schedule into avg annual schedule for simulation
    // original_ts = timestep used in availability model
    // new_ts = timestep needed for simulation

    int nsteps = full_sch.vec()->size();
    int ny = (int)floor(original_ts * nsteps / 8760);
    double n = ceil(8760. / (double)original_ts);  // number points per year
    int n_int = (int)floor(n);

    // Calculate annual availability schedule averaged over all simulated years
    std::vector<double> avg_sched(n_int, 0.0);
    for (int y = 0; y < ny; y++)
    {
        int p1 = (int)floor(y*n);
        for (int p = 0; p < n_int; p++)
        {
            double avail_pt;
            if (p1 + p <= nsteps - 1)
                avail_pt = full_sch.vec()->at(p1 + p).as_number();
            else
                avail_pt = full_sch.vec()->at(nsteps - 1).as_number();

            avg_sched[p] += avail_pt / (double)ny;
        }
    }

    // Translate to new time step
    output_sch.clear();
    int nrec = (int)ceil(8760. / new_ts);
    for (int i = 0; i < nrec; i++)
    {
        int j = floor(i*new_ts / original_ts);

        if (j <= avg_sched.size() - 1)
            output_sch.push_back(avg_sched[j]);
        else
            output_sch.push_back(avg_sched.back());
    }

    return;
}




void Project::initialize_cycle_model(PowerCycle &pc)
{

	std::vector < double > c_eff_cold;
	std::vector < double > c_eff_hot;
	for (int i = 0; i < m_parameters.num_condenser_trains.as_integer() + 1; i++)
	{
		c_eff_cold.push_back(m_parameters.condenser_eff_cold.vec()->at(i).as_number());
		c_eff_hot.push_back(m_parameters.condenser_eff_hot.vec()->at(i).as_number());
	}

	pc.GeneratePlantComponents(m_parameters.num_condenser_trains.as_integer(), m_parameters.fans_per_train.as_integer(), m_parameters.radiators_per_train.as_integer(),
		m_parameters.num_salt_steam_trains.as_integer(), m_parameters.num_fwh.as_integer(), m_parameters.num_salt_pumps.as_integer(),
		m_parameters.num_salt_pumps_required.as_integer(), m_parameters.num_water_pumps.as_integer(), m_parameters.num_turbines.as_integer(),
		c_eff_cold, c_eff_hot);

	pc.m_sim_params.num_scenarios = m_parameters.num_scenarios.as_integer();
	
	pc.Initialize(0.0);
	
	double capacity = m_variables.P_ref.as_number() * 1.e6;
	double thermal_capacity = capacity / m_variables.design_eff.as_number() * 1.e6;
	
	pc.SetPlantAttributes(m_parameters.maintenance_interval.as_number(), 
						  m_parameters.maintenance_duration.as_number(), 
						  m_parameters.downtime_threshold.as_number(),
						  m_parameters.hours_to_maintenance.as_number(), 
						  0.0, 
						  0.0, 
						  0, 
						  capacity, 
						  thermal_capacity, 
						  m_parameters.temp_threshold.as_number(),
						  0.0, 
						  0.0, 
						  0.0, 
						  m_parameters.shutdown_capacity.as_number(),
						  m_parameters.no_restart_capacity.as_number(),
						  m_parameters.shutdown_efficiency.as_number(), 
						  m_parameters.no_restart_efficiency.as_number());


	pc.StoreCycleState();

	return;
}

void Project::save_cycle_outputs(std::vector<double> &capacity, std::vector<double> &efficiency, double n_failures, double yearly_labor_cost)
{

	m_cycle_outputs.cycle_capacity.assign_vector(capacity);
	m_cycle_outputs.cycle_efficiency.assign_vector(efficiency);
	m_cycle_outputs.num_failures.assign(n_failures);

	double labor_cost = calc_real_dollars(yearly_labor_cost);
	m_cycle_outputs.cycle_labor_cost.assign(labor_cost);

	return;
}

bool Project::integrate_cycle_and_simulation(PowerCycle &pc, const cycle_ssc_integration_inputs &inputs, 
											plant_state &final_state, std::unordered_map<std::string, std::vector<double>> &soln, 
											std::vector<double> &capacity, std::vector<double> &efficiency, int &n_failures, double &labor_cost)
{
	/*
	Production/dispatch optimization simulation integrated with cycle availability model
	For each designated horizon, model integration follows the framework: 
		(1) Run dispatch optimization and ssc assuming full available cycle capacity/efficiency. 
		(2) Run cycle model until the point of either first failure or first repair
		(3) Re-run ssc from the point of first failure or repair.  
		    If event is a repair (failure) and is_reoptimize_at_repairs (is_reoptimize_at_failures) is true, then ssc will be called with dispatch optimization enabled
			If event is a repair (failure) and is_reoptimize_at_repairs (is_reoptimize_at_failures) is false, then ssc will be re-run using previously-computed dispatch targets (adjusted for actual cycle availability) to accurately track production and TES charge state
			When running from previous dispatch targets, an optional heuristic allows upward adjustment in cycle operational targets when extra TES is available (as compared to that expected from the last solution with dispatch optimization) and the cycle is scheduled to ramp down or shut off
		(4) Repeat (2)/(3) until no new failures/repairs are encountered within the model horizon

	The model integration horizon sets the time span for the initial call to ssc above. Ideally this would be equal to the time to next failure/repair to minimize extra calculations.

	Input parameters (inputs):
		- start_time, end_time: start and end times for the simulation in hours
		- horizon: time horizon for model integration [hr].  
		- initial_state: state of plant at start of simulation
		- use_stored_state: start from stored pc initial state?
		- use_existing_ssc_soln: use provided ssc solution?  If false, the initial call to ssc will be performed here

	Outputs:
		- final_state = final cycle state at the end of the horizon
		- soln = final ssc solution after integration with the cycle availability model 
		- capacity = final time-series for cycle available capacity
		- efficiency = final time-series for cycle available efficiency
	*/



	double nan = std::numeric_limits<double>::quiet_NaN();
	soln.clear();
	capacity.clear();
	efficiency.clear();
	n_failures = 0;
	labor_cost = 0.0;


	bool is_use_target_heuristic = m_parameters.is_use_target_heuristic.as_boolean();		// Use heuristics to adjust q_pb_target and is_pc_su_allowed when running from pre-computed targets after capacity reduction
	bool is_reoptimize_at_repairs = m_parameters.is_reoptimize_at_repairs.as_boolean();		// Re-run dispatch optimization and cycle availability each time a component is repaired
	bool is_reoptimize_at_failures = m_parameters.is_reoptimize_at_failures.as_boolean();   // Re-run dispatch optimization and cycle availability each time a component fails 

	double start_time = inputs.start_time;
	double end_time = inputs.end_time;
	double horizon = inputs.horizon;
	

	//--- Initial states
	ssc_number_t tes_charge, tes_thot, tes_thot_des, tes_tcold, tes_tcold_des, is_dispatch;
	bool is_rec_on, is_pc_on, is_pc_standby, is_reoptimize, use_stored_state, use_existing_ssc_soln;
	double capacity_last, efficiency_last;

	is_reoptimize = true;
	is_dispatch = m_parameters.is_dispatch.as_number();
	ssc_data_get_number(m_ssc_data, "T_htf_hot_des", &tes_thot_des);
	ssc_data_get_number(m_ssc_data, "T_htf_cold_des", &tes_tcold_des);

	use_existing_ssc_soln = inputs.use_existing_ssc_soln;
	use_stored_state = inputs.use_stored_state;
	is_rec_on = inputs.initial_state.is_rec_on;
	is_pc_on = inputs.initial_state.is_pc_on;
	is_pc_standby = inputs.initial_state.is_pc_standby;
	capacity_last = inputs.initial_state.capacity_avail;
	efficiency_last = inputs.initial_state.efficiency_avail;
	tes_charge = inputs.initial_state.tes_charge;
	tes_thot = inputs.initial_state.tes_thot;
	tes_tcold = inputs.initial_state.tes_tcold;

	if (tes_charge != tes_charge)  // use design-point values if initial states are not provided
		ssc_data_get_number(m_ssc_data, "csp.pt.tes.init_hot_htf_percent", &tes_charge);
	if (tes_thot != tes_thot)
		tes_thot = tes_thot_des;
	if (tes_tcold != tes_tcold)
		tes_tcold = tes_tcold_des;



	//--- Get time step and design values from current ssc inputs
	double tes_capacity = (m_variables.P_ref.as_number() / m_variables.design_eff.as_number()) * m_variables.tshours.as_number();  // TES capacity (MWh)
	double q_pc_des = (m_variables.P_ref.as_number() / m_variables.design_eff.as_number());  // Design point cycle thermal inputs (MW)	
	double q_rec_des = m_variables.solarm.as_number() * q_pc_des;		// Design point receiver thermal power (MW)
	ssc_number_t q_sby_frac, cycle_cutoff_frac, f_rec_min, steps_per_hour;
	ssc_data_get_number(m_ssc_data, "q_sby_frac", &q_sby_frac);
	ssc_data_get_number(m_ssc_data, "cycle_cutoff_frac", &cycle_cutoff_frac);
	ssc_data_get_number(m_ssc_data, "f_rec_min", &f_rec_min);
	ssc_data_get_number(m_ssc_data, "time_steps_per_hour", &steps_per_hour);
	double steplength = 1. / (double)steps_per_hour;
	int nsteps_tot = (int)ceil((end_time - start_time) / steplength);


	//--- Initialize results
	unordered_map < std::string, std::vector<double>> current_soln, pc_dispatch, optimized_targets, adjusted_targets;
	std::vector<std::string> pc_keys = { "cycle_power", "thermal_power", "ambient_temperature", "standby" };
	std::vector<std::string> ssc_keys = { "gen", "P_cycle", "P_out_net", "e_ch_tes", "T_tes_hot", "T_tes_cold", "Q_thermal", "q_pb", "q_dot_pc_startup", "beam", "tdry", "pricing_mult", "disp_qsfprod_expected", "disp_wpb_expected"};
	std::vector<std::string> disp_target_keys = {"Q_thermal", "e_ch_tes", "q_dot_pc_target", "q_dot_pc_max", "is_rec_su_allowed", "is_pc_su_allowed", "is_pc_sb_allowed" };

	//-- Check if provided initial solution is complete
	if (use_existing_ssc_soln)
	{
		if (inputs.initial_ssc_soln.size() == 0 || horizon != end_time - start_time)
			use_existing_ssc_soln = false;
		else
		{
			std::unordered_map < std::string, std::vector<double> >::const_iterator it;
			std::string key;
			size_t k = 0;
			while (k < ssc_keys.size() + disp_target_keys.size())
			{
				if (k < ssc_keys.size())
					key = ssc_keys[k];
				else
					key = disp_target_keys[k - ssc_keys.size()];
				
				it = inputs.initial_ssc_soln.find(key);
				if (it == inputs.initial_ssc_soln.end() || it->second.size() != nsteps_tot)
					use_existing_ssc_soln = false;
				k++;
			}
		}
		if (!use_existing_ssc_soln)
			message_handler("Warning: Initial ssc solution is incomplete and will be recomputed");
	}




	ssc_module_exec_set_print(m_parameters.print_messages.as_boolean());
	ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");

	std::vector<std::string> failure_label, failure_component;
	std::vector<double>failure_time, failure_duration;

	//==============================================

	//--- Loop over model horizons  
	double time_completed = start_time;
	while (time_completed < end_time)  
	{

		//-- Set amount of time to run ssc solution and cycle availability model
		double current_horizon = horizon;
		double hour_end = fmin(end_time, time_completed + current_horizon);
		current_horizon = hour_end - time_completed;
		int nsteps = (int)ceil(current_horizon / steplength);
		
		
		pc.SetSimulationParameters(0, nsteps, steplength, 1.e-8, false, m_parameters.num_scenarios.as_integer(), m_parameters.cycle_hourly_labor_cost.as_number(), true, true);


		//-- Initialize solutions for this model horizon 
		is_reoptimize = true;
		double capacity_init = 1.0;
		double efficiency_init = 1.0;
		if (is_reoptimize_at_failures && is_reoptimize_at_repairs && capacity_last > 0.0 && !use_existing_ssc_soln)  
		{
			capacity_init = capacity_last;
			efficiency_init = efficiency_last;
		}
		
		std::vector<double> cycle_capacity(nsteps, capacity_init);
		std::vector<double> cycle_efficiency(nsteps, efficiency_init);

		for (int k = 0; k < (int)pc_keys.size(); k++)
			pc_dispatch[pc_keys[k]].assign(nsteps, nan);

		std::unordered_map < std::string, std::vector<double> >::const_iterator it;
		for (int k = 0; k < (int)ssc_keys.size(); k++)
		{
			current_soln[ssc_keys[k]].assign(nsteps, nan);
			if (use_existing_ssc_soln)
			{
				it = inputs.initial_ssc_soln.find(ssc_keys[k]);
				current_soln[ssc_keys[k]] = it->second;
			}
		}

		for (int k = 0; k < (int)disp_target_keys.size(); k++)
		{
			optimized_targets[disp_target_keys[k]].assign(nsteps, nan);
			adjusted_targets[disp_target_keys[k]].assign(nsteps, nan);
			if (use_existing_ssc_soln)
			{
				it = inputs.initial_ssc_soln.find(disp_target_keys[k]);
				optimized_targets[disp_target_keys[k]] = it->second;
				adjusted_targets[disp_target_keys[k]] = it->second;
			}
		}

		//==============================================

		//-- Alternate between ssc and cycle availability solutions
		int step_now = 0;
		int pc_call = 0;

		while (step_now < nsteps)
		{
			double hour_now = time_completed + step_now * steplength;

			//--- Run ssc 
			if (!use_existing_ssc_soln)
			{
				ssc_data_set_number(m_ssc_data, "csp.pt.tes.init_hot_htf_percent", tes_charge);
				ssc_data_set_number(m_ssc_data, "hot_tank_Tinit", tes_thot);
				ssc_data_set_number(m_ssc_data, "cold_tank_Tinit", tes_tcold);
				ssc_data_set_number(m_ssc_data, "time_start", hour_now*3600.);
				ssc_data_set_number(m_ssc_data, "time_stop", hour_end*3600.);
				ssc_data_set_number(m_ssc_data, "vacuum_arrays", 1);
				ssc_data_set_number(m_ssc_data, "is_rec_on_initial", is_rec_on);
				ssc_data_set_number(m_ssc_data, "is_pc_on_initial", is_pc_on);
				ssc_data_set_number(m_ssc_data, "is_pc_standby_initial", is_pc_standby);

				int n = nsteps - step_now;
				ssc_number_t *p_data = new ssc_number_t[n];

				if (is_reoptimize)  // Optimize dispatch 
				{
					ssc_data_set_number(m_ssc_data, "is_dispatch", is_dispatch);
					ssc_data_set_number(m_ssc_data, "is_dispatch_targets", false);
					ssc_data_set_number(m_ssc_data, "is_dispatch_constr", true);

					for (int i = 0; i < n; i++)
						p_data[i] = cycle_capacity[step_now + i];
					ssc_data_set_array(m_ssc_data, "disp_cap_constr", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = cycle_efficiency[step_now + i];
					ssc_data_set_array(m_ssc_data, "disp_eff_constr", p_data, n);
				}

				else   // Set targets for cycle operation from previous optimization
				{
					ssc_data_set_number(m_ssc_data, "is_dispatch", false);
					ssc_data_set_number(m_ssc_data, "is_dispatch_targets", true);
					ssc_data_set_number(m_ssc_data, "is_dispatch_constr", false);

					for (int i = 0; i < n; i++)
						p_data[i] = adjusted_targets["q_dot_pc_target"][step_now + i];
					ssc_data_set_array(m_ssc_data, "q_pc_target_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = adjusted_targets["q_dot_pc_max"][step_now + i];
					ssc_data_set_array(m_ssc_data, "q_pc_max_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = adjusted_targets["is_rec_su_allowed"][step_now + i];
					ssc_data_set_array(m_ssc_data, "is_rec_su_allowed_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = adjusted_targets["is_pc_su_allowed"][step_now + i];
					ssc_data_set_array(m_ssc_data, "is_pc_su_allowed_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = adjusted_targets["is_pc_sb_allowed"][step_now + i];
					ssc_data_set_array(m_ssc_data, "is_pc_sb_allowed_in", p_data, n);
				}
				delete[] p_data;

				// Run ssc
				if (!ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0))
				{
					message_handler("SSC simulation failed");
					ssc_module_free(mod_mspt);
					return false;
				}

				// Update current solution
				int nr;
				for (int k = 0; k < (int)ssc_keys.size(); k++)
				{
					std::string key = ssc_keys[k];
					ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
					for (int r = 0; r < nr; r++)
						current_soln[key][step_now + r] = p_data[r];
				}

				// Update dispatch operation targets
				if (is_reoptimize)
				{
					for (int k = 0; k < (int)disp_target_keys.size(); k++)
					{
						std::string key = disp_target_keys[k];
						ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
						for (int r = 0; r < nr; r++)
						{
							optimized_targets[key][step_now + r] = p_data[r];
							adjusted_targets[key][step_now + r] = p_data[r];
						}
					}
				}

			}
			
			std::vector<double>cycle_capacity_in = cycle_capacity;
			std::vector<double>cycle_efficiency_in = cycle_efficiency;

			//--- Find next failure/repair event
			bool is_repair, is_failure;
			int next_start_pt = -1;

			if (pc_call == 0 && (cycle_capacity[0] - capacity_last > 0.01 || cycle_efficiency[0] - efficiency_last > 0.01))  // Capacity/efficiency used in ssc at first step is too high... treat like a failure
			{
				next_start_pt = 0;
				is_failure = true;
				is_repair = false;
				cycle_capacity.assign(nsteps, capacity_last);
				cycle_efficiency.assign(nsteps, efficiency_last);
			}
			else if (pc_call == 0 && (cycle_capacity[0] - capacity_last < -0.01 || cycle_efficiency[0] - efficiency_last < -0.01))   // Capacity/efficiency used in ssc at first step is too low... treat like a repair
			{
				next_start_pt = 0;
				is_failure = false;
				is_repair = true;
				cycle_capacity.assign(nsteps, capacity_last);
				cycle_efficiency.assign(nsteps, efficiency_last);
			}
			else  // Run cycle availability model
			{
				int sc = 0; //scenario number
				int prev_failure = pc.m_results.period_of_last_failure[sc];
				int prev_repair = pc.m_results.period_of_last_repair[sc];

				for (int i = step_now; i < nsteps; i++)
				{
					pc_dispatch["cycle_power"][i] = current_soln["P_cycle"][i]* 1.e6;		// cycle power output [W]
					if (cycle_capacity[i] < 0.999)  // limit dispatch input to cycle model to avoid mismatch between power output in simulated / read-only periods
						pc_dispatch["cycle_power"][i] = fmin(pc_dispatch["cycle_power"][i], m_variables.P_ref.as_number() * cycle_capacity[i] * 1.e6);
					
					pc_dispatch["thermal_power"][i] = current_soln["q_pb"][i]*1.e6;			// cycle thermal power input [W]
					pc_dispatch["ambient_temperature"][i] = current_soln["tdry"][i];
					pc_dispatch["standby"][i] = 0;
					if (current_soln["P_cycle"][i] < 1.e-6 && current_soln["q_pb"][i] > 0.0 && current_soln["q_dot_pc_startup"][i] < current_soln["q_pb"][i])  // Thermal energy going to power block, but no electrical output
						pc_dispatch["standby"][i] = 1.0;
				}

				pc.SetDispatch(pc_dispatch, true);
				pc.Simulate(false, use_stored_state);
				use_stored_state = true;  // Use stored state for all PC calls after first call	

				cycle_capacity = pc.m_results.cycle_capacity[sc];		
				cycle_efficiency = pc.m_results.cycle_efficiency[sc];	

				int next_failure = pc.m_results.period_of_last_failure[sc];
				int next_repair = pc.m_results.period_of_last_repair[sc];
				

				pc_call += 1;

				if (next_failure <= prev_failure) // No new failures
					next_failure = nsteps + 1000;

				if (next_repair <= prev_repair) // No new repairs
					next_repair = nsteps + 1000;
				else
					next_repair +=1;  // repairs are reported from cycle model in the time step before capacity/efficiency increase

				next_start_pt = std::min(nsteps, std::min(next_failure, next_repair));
				is_repair = (next_repair == next_start_pt);
				is_failure = (!is_repair && next_failure == next_start_pt);

				if (is_failure)
					n_failures += 1;


				if (next_start_pt == nsteps)
				{
					capacity_last = cycle_capacity.back();
					efficiency_last = cycle_efficiency.back();
					labor_cost += pc.m_results.labor_costs[0];
				}

				if (next_repair == nsteps) // repair occurred at last time point in this horizon... call cycle model again and manually increase capacity/efficiency for beginning of next horizon
				{
					pc.Simulate(false, use_stored_state);
					pc_call += 1;
					capacity_last = 1.0;
					efficiency_last = 1.0;
				}


			}

			//--- Fill in arrays of cycle capacity/efficiency with value at next start point -> next call to ssc will not know when another failure/repair will happen
			if (next_start_pt < nsteps)
			{
				std::fill(cycle_capacity.begin() + next_start_pt, cycle_capacity.end(), cycle_capacity[next_start_pt]);
				std::fill(cycle_efficiency.begin() + next_start_pt, cycle_efficiency.end(), cycle_efficiency[next_start_pt]);
			}

			//--- Accept current solution for all points prior to "next_start_pt"
			for (int k = 0; k < (int)ssc_keys.size(); k++)
			{
				std::string key = ssc_keys[k];
				for (int i = step_now; i < next_start_pt; i++)
				{
					double p_parasitic = current_soln["P_cycle"][i] - current_soln["P_out_net"][i];
					double p_gross_adj = current_soln["P_cycle"][i] * cycle_efficiency[i];  // Reduce gross electricity generation by cycle efficiency penalty (efficiency penalty is not accounted for in ssc solution)
					double p_net_adj = p_gross_adj - p_parasitic;
					double fadj = current_soln["gen"][i] / (current_soln["P_out_net"][i]*1000.);
					
					if (key == "P_cycle")
						soln[key].push_back(p_gross_adj); 
					else if (key == "P_out_net")
						soln[key].push_back(p_net_adj);
					else if (key == "gen")
						soln[key].push_back(p_net_adj*1000.*fadj);
					else
						soln[key].push_back(current_soln[key][i]);
				}
			}

			for (int i = step_now; i < next_start_pt; i++)
			{
				capacity.push_back(cycle_capacity[i]);
				efficiency.push_back(cycle_efficiency[i]);
			}


			//--- What to do in next call to ssc?
			use_existing_ssc_soln = false;
			if (next_start_pt < nsteps)
			{
				is_reoptimize = false;  // run ssc again using previous dispatch targets adjusted for capacity constraints 
				if ((is_repair && is_reoptimize_at_repairs) || (is_failure && is_reoptimize_at_failures))
					is_reoptimize = true;  // re-optimize dispatch using new capacity/efficiency constraints
				else
				{
					// This failure/repair doesn't involve cycle capacity... no need to re-run ssc from dispatch targets, because efficiency changes are not applied in ssc
					if ((next_start_pt == 0 && cycle_capacity[0] == capacity_init) || (next_start_pt > 0 && cycle_capacity[next_start_pt] == cycle_capacity[next_start_pt - 1]))
						use_existing_ssc_soln = true;
				}
			}


			//--- Update initial states for next ssc model calls
			if (next_start_pt > 0)
			{
				tes_thot = soln["T_tes_hot"].back();
				tes_tcold = soln["T_tes_cold"].back();
				tes_charge = (soln["e_ch_tes"].back() / tes_capacity) * ((tes_thot_des - tes_tcold_des) / (tes_thot - tes_tcold_des))* 100.;  // ssc output "e_ch_tes" based on actual hot T and design point cold T

				is_rec_on = false;
				if (soln["Q_thermal"].back() > 0.0)
					is_rec_on = true;

				is_pc_on = false;
				is_pc_standby = false;
				if (soln["P_cycle"].back() > 0.0)
					is_pc_on = true;
				else if (soln["q_pb"].back() > 0.0 && soln["q_dot_pc_startup"].back() < soln["q_pb"].back())
					is_pc_standby = true;
			}



			//--- Set dispatch targets if next ssc call will not involve re-optimization
			double e_ch_tes_adj = nan;
			if (!is_reoptimize && !use_existing_ssc_soln && next_start_pt < nsteps)
			{

				double q_pc_target_0 = adjusted_targets["q_dot_pc_target"][next_start_pt];

				double op_cutoff = 0.999*(q_pc_des * cycle_cutoff_frac);
				double stby_cutoff = 0.999*(q_pc_des * q_sby_frac);

				double cap_max = 0.0;
				for (int p = next_start_pt; p < nsteps; p++)
					cap_max = fmax(cap_max, cycle_capacity[p]);

				for (int p = next_start_pt; p < nsteps; p++)
				{
					// Initialize to targets from optimized solution
					for (int k = 0; k < (int)disp_target_keys.size(); k++)
					{
						std::string key = disp_target_keys[k];
						adjusted_targets[key][p] = optimized_targets[key][p];
					}

					// Modify targets based on reduced cycle capacity
					if (cycle_capacity[p] < 0.999)  
					{
						adjusted_targets["q_dot_pc_target"][p] = fmin(adjusted_targets["q_dot_pc_target"][p], q_pc_des * cycle_capacity[p]);
						adjusted_targets["q_dot_pc_max"][p] = q_pc_des * cycle_capacity[p];

						if (adjusted_targets["q_dot_pc_target"][p] < op_cutoff)
							adjusted_targets["is_pc_su_allowed"][p] = 0;

						if (adjusted_targets["q_dot_pc_target"][p] < stby_cutoff)
							adjusted_targets["is_pc_sb_allowed"][p] = 0;

						if (adjusted_targets["is_pc_su_allowed"][p] == 0 && adjusted_targets["is_pc_sb_allowed"][p] == 0)
							adjusted_targets["q_dot_pc_target"][p] = 0.0;
					}



					// Use heuristic to adjust q_pb_target and is_pc_su_allowed in cases where additional TES is present due to outages
					if (is_use_target_heuristic && cap_max > 0.01)  
					{
						if (p == next_start_pt)  
							adjusted_targets["e_ch_tes"][p] = current_soln["e_ch_tes"][p] + (q_pc_target_0 - adjusted_targets["q_dot_pc_target"][p])*steplength;
						else
						{
							double tes_extra = fmax(0.0, adjusted_targets["e_ch_tes"][p - 1] - optimized_targets["e_ch_tes"][p - 1]);  // extra TES relative to last optimized solution
							
							if (tes_extra > 0.0)
							{
								double q_pb_added = 0.0;

								// new target -> keep running as close as possible to previous target, or at maximum if constrained capacity changes
								double new_target = adjusted_targets["q_dot_pc_target"][p - 1];
								if (abs(cycle_capacity[p] - cycle_capacity[p - 1]) > 0.01)
									new_target = adjusted_targets["q_dot_pc_tmax"][p];

								// power block was already planned to run during this timestep but was supposed to ramp down 
								if (adjusted_targets["is_pc_su_allowed"][p] == 1 && adjusted_targets["q_dot_pc_target"][p - 1] - adjusted_targets["q_dot_pc_target"][p] > 0)
									q_pb_added = fmin(0.95*tes_extra / steplength, new_target - adjusted_targets["q_dot_pc_target"][p]);

								// power block was not planned to run during this timestep but was running during previous timestep and extra TES is sufficient for at least minimum operation
								else if ((adjusted_targets["is_pc_su_allowed"][p] + adjusted_targets["is_pc_sb_allowed"][p] == 0) && (adjusted_targets["is_pc_su_allowed"][p-1] == 1) && (0.98*tes_extra / steplength > q_pc_des*cycle_cutoff_frac))
									q_pb_added = fmin(0.95*tes_extra / steplength, new_target);

								adjusted_targets["q_dot_pc_target"][p] += q_pb_added;

								if (adjusted_targets["q_dot_pc_target"][p] > 0.0 && adjusted_targets["is_pc_sb_allowed"][p] == 0)
									adjusted_targets["is_pc_su_allowed"][p] = 1;
							}


							// calculate approximate expected TES charge state at end of this step
							double expected_tes_change = optimized_targets["e_ch_tes"][p] - optimized_targets["e_ch_tes"][p - 1];
							adjusted_targets["e_ch_tes"][p] = adjusted_targets["e_ch_tes"][p-1] + expected_tes_change + steplength * (optimized_targets["q_dot_pc_target"][p] - adjusted_targets["q_dot_pc_target"][p]);  // Approximate TES charge state using new targets

							if (adjusted_targets["e_ch_tes"][p]> tes_capacity)
							{
								double q_thermal = optimized_targets["Q_thermal"][p];  // expected receiver output
								double q_decrease = (adjusted_targets["e_ch_tes"][p] - tes_capacity)/steplength;	// Decrease in receiver output required to keep TES within limits
								if (q_thermal > 0.0 && optimized_targets["Q_thermal"][p - 1] > 0.0 && (q_thermal - q_decrease) < q_rec_des*f_rec_min)  // Does receiver need to be shut off? (only for steps without receiver startup)
									q_decrease = q_thermal;
								
								adjusted_targets["Q_thermal"][p] = q_thermal - q_decrease;
								adjusted_targets["e_ch_tes"][p] -= q_decrease * steplength;
							}
						}
					}

				}

			}

			step_now = next_start_pt;

		}

		time_completed += current_horizon;
	}

	ssc_module_free(mod_mspt);

	final_state.tes_charge = tes_charge;
	final_state.tes_thot = tes_thot;
	final_state.tes_tcold = tes_tcold;
	final_state.is_rec_on = is_rec_on;
	final_state.is_pc_on = is_pc_on;
	final_state.is_pc_standby = is_pc_standby;
	final_state.capacity_avail = capacity_last;
	final_state.efficiency_avail = efficiency_last;

	return true;

}


bool Project::integrate_cycle_and_clusters(const std::unordered_map<std::string, std::vector<double>> &initial_soln, int first_index, int last_index, std::unordered_map<std::string, std::vector<double>> &final_soln)
{
	// Production/dispatch optimization simulation integrated with cycle availability model when using clustering
	// Simulation run for each group starting from the initial ssc solution previously computed -> ssc is only re-run when failures/repairs occur


	std::unordered_map<std::string, std::vector<double>>::const_iterator it = initial_soln.find("gen");
	int nrec = (int)it->second.size();
	int nperday = nrec / 365;


	//--- Initialize cycle availability model
	PowerCycle pc;
	WELLFiveTwelve gen1(0);
	WELLFiveTwelve gen2(0);
	WELLFiveTwelve gen3(0);
	pc.AssignGenerators(&gen1, &gen2, &gen3);

	initialize_cycle_model(pc);
	std::vector<double> capacity(nrec, 1.0);
	std::vector<double> efficiency(nrec, 1.0);
	int n_failures, nf;
	double labor_cost, lc;
	n_failures = 0;
	labor_cost = 0.0;

	//--- Initialize modified solution
	final_soln.clear();
	std::vector<double> local_cap, local_eff;
	std::unordered_map<std::string, std::vector<double>> local_soln;
	for (it = initial_soln.begin(); it != initial_soln.end(); it++)
		final_soln[it->first].assign(it->second.size(), 0.0);

	plant_state final_state;
	cycle_ssc_integration_inputs inputs;
	inputs.use_existing_ssc_soln = true;
	inputs.use_stored_state = false;

	//--- Loop over days of year
	int ng = (int)cluster_outputs.partition_matrix.nrows;
	int nd = m_parameters.cluster_ndays.as_integer();
	for (int g = 0; g < ng + 2; g++)
	{
		int doy, index;
		double horizon;

		if (g > 0 && g < ng + 1)
		{
			doy = (g - 1)*nd + 1;
			index = cluster_outputs.index.at(g - 1);
			horizon = nd * 24;
		}
		else if (g == 0)
		{
			doy = 0;
			index = first_index;
			horizon = 24;
		}
		else
		{
			doy = ng * nd + 1;
			index = last_index;
			horizon = (365 - doy) * 24;
		}

		int exemplar = cluster_outputs.exemplars[index];
		int doy_exemplar = exemplar * nd + 1;
		inputs.start_time = doy_exemplar * 24.;
		inputs.end_time = inputs.start_time + horizon;
		inputs.horizon = horizon;

		// extract initial solution
		int j = doy * nperday;
		int npts = (horizon / 24) * nperday;
		inputs.initial_ssc_soln.clear();
		for (it = initial_soln.begin(); it != initial_soln.end(); it++)
			inputs.initial_ssc_soln[it->first].assign(it->second.begin() + j, it->second.begin() + j + npts);

		// run 'integrate_cycle_and_simulation' for this time block starting from the existing solution (will not re-run ssc unless failures/repairs occur)
		inputs.initial_state = final_state;
		if (!integrate_cycle_and_simulation(pc, inputs, final_state, local_soln, local_cap, local_eff, nf, lc))
			return false;

		n_failures += nf;
		labor_cost += lc;

		for (int h = 0; h < npts; h++)
		{
			capacity.at(doy*nperday + h) = local_cap.at(h);
			efficiency.at(doy*nperday + h) = local_eff.at(h);
			for (it = local_soln.begin(); it != local_soln.end(); it++)
				final_soln[it->first].at(doy*nperday + h) = it->second.at(h);
		}

		inputs.use_stored_state = true;  // use stored state after g = 0
	}

	save_cycle_outputs(capacity, efficiency, n_failures, labor_cost);

	return true;
}

