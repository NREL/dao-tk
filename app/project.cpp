#include "project.h"
#include <wx/wx.h>
#include <limits>

#include <iostream>
#include <fstream>
#include <sstream>

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

    std::string sival = wxString::Format("%d", ival).ToStdString();

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
	
    h_tower.set(                          dnan,      dmin,      dmax,            "h_tower",                                     "Tower height",        "m",                         "Variables" );
    rec_height.set(                       dnan,      dmin,      dmax,         "rec_height",                                  "Receiver height",        "m",                         "Variables" );
    D_rec.set(                            dnan,      dmin,      dmax,              "D_rec",                                "Receiver diameter",        "m",                         "Variables" );
    design_eff.set(                       dnan,      dmin,      dmax,         "design_eff",                                "Design efficiency",        "-",                         "Variables" );
    dni_des.set(                          dnan,      dmin,      dmax,            "dni_des",                                 "Design point DNI",     "W/m2",                         "Variables" );
    P_ref.set(                            dnan,      dmin,      dmax,              "P_ref",                               "Design gross power",       "kW",                         "Variables" );
    solarm.set(                           dnan,      dmin,      dmax,             "solarm",                                   "Solar multiple",        "-",                         "Variables" );
    tshours.set(                          dnan,      dmin,      dmax,            "tshours",              "Hours stored at full load operation",       "hr",                         "Variables" );
    degr_replace_limit.set(               dnan,      dmin,      dmax, "degr_replace_limit",             "Mirror degradation replacement limit",        "-",                         "Variables" );
    om_staff.set(                           -1,      -999,       999,           "om_staff",                              "Number of o&m staff",        "-",                         "Variables" );
    n_wash_crews.set(                       -1,      -999,       999,       "n_wash_crews",                             "Number of wash crews",        "-",                         "Variables" );
    N_panels.set(                           -1,      -999,       999,           "N_panels",                        "Number of receiver panels",        "-",                         "Variables" );

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

    print_messages.set(                   true,               "print_messages",      false,                                "Print full output",        "-",                          "Settings" );
    check_max_flux.set(                   true,               "check_max_flux",      false,                                   "Check max flux",        "-",                          "Settings" );
    is_optimize.set(                     false,                  "is_optimize",      false );
    is_dispatch.set(                     false,                  "is_dispatch",      false,                                "Optimize dispatch",        "-",                          "Settings" );
    is_ampl_engine.set(                  false,               "is_ampl_engine",      false,                               "Use AMPL optimizer",        "-",                          "Settings" );
    is_stochastic_disp.set(              false,           "is_stochastic_disp",      false,                          "Run stochastic dispatch",        "-",                          "Settings" );
    current_standby.set(                 false,              "current_standby",      false,                     "Start power cycle in standby",        "-",                  "Cycle|Parameters" );
	stop_cycle_at_first_failure.set(     false,  "stop_cycle_at_first_failure",      false,   "Stop cycle model after first new failure event",        "-",                  "Cycle|Parameters" );
	stop_cycle_at_first_repair.set(      false,   "stop_cycle_at_first_repair",      false,    "Stop cycle model after first new repair event",        "-",                  "Cycle|Parameters" );

	std::string empty_string = "";
    ampl_data_dir.set(                      "",                "ampl_data_dir",      false,                                 "AMPL data folder",        "-",                          "Settings" );
    solar_resource_file.set(      empty_string,          "solar_resource_file",      false,                              "Solar resource file",        "-",                          "Settings" );

    disp_steps_per_hour.set(                 1,          "disp_steps_per_hour",      false,                     "Dispatch time steps per hour",        "-",                          "Settings" );
    avail_seed.set(                        123,                   "avail_seed",      false,                     "Random number generator seed",        "-", "Heliostat availability|Parameters" );
    plant_lifetime.set(                     30,               "plant_lifetime",      false,                                   "Plant lifetime",       "yr",              "Financial|Parameters" );
    finance_period.set(                     25,               "finance_period",      false,                                   "Finance period",       "yr",              "Financial|Parameters" );
    ppa_multiplier_model.set(                1,         "ppa_multiplier_model",      false,                             "PPA multiplier model",        "-",              "Financial|Parameters" );
    num_condenser_trains.set(                2,         "num_condenser_trains",      false,                       "Number of condenser trains",        "-",                  "Cycle|Parameters" );
    fans_per_train.set(                     30,               "fans_per_train",      false,               "Number of fans per condenser train",        "-",                  "Cycle|Parameters" );
    radiators_per_train.set(                 2,          "radiators_per_train",      false,          "Number of radiators per condenser train",        "-",                  "Cycle|Parameters" );
    num_salt_steam_trains.set(               2,        "num_salt_steam_trains",      false,                   "Number of salt-to-steam trains",        "-",                  "Cycle|Parameters" );
    num_fwh.set(                             6,                      "num_fwh",      false,                      "Number of feedwater heaters",        "-",                  "Cycle|Parameters" );
    num_salt_pumps.set(                      4,               "num_salt_pumps",      false,                         "Number of hot salt pumps",        "-",                  "Cycle|Parameters" );
	num_salt_pumps_required.set(             3,      "num_salt_pumps_required",      false,  "Number of hot salt pumps to operate at capacity",        "-",                  "Cycle|Parameters" );
	num_water_pumps.set(                     2,              "num_water_pumps",      false,                 "Number of boiler and water pumps",        "-",                  "Cycle|Parameters" );
    num_turbines.set(                        1,                 "num_turbines",      false,               "Number of turbine-generator shafts",        "-",                  "Cycle|Parameters" );
    read_periods.set(                        0,                 "read_periods",      false,                      "Number of read-only periods",        "-",                  "Cycle|Parameters" );
    sim_length.set(                         48,                   "sim_length",      false,            "Number of periods in cycle simulation",        "-",                  "Cycle|Parameters" );
    num_scenarios.set(                       1,                "num_scenarios",      false,                              "Number of scenarios",        "-",                  "Cycle|Parameters" );

    rec_ref_cost.set(                1.03e+008,                 "rec_ref_cost",      false,                          "Receiver reference cost",        "$",              "Financial|Parameters" );
    rec_ref_area.set(                    1571.,                 "rec_ref_area",      false,                          "Receiver reference area",       "m2",              "Financial|Parameters" );
    tes_spec_cost.set(                     24.,                "tes_spec_cost",      false,                                "TES specific cost",   "$/kWht",              "Financial|Parameters" );
    tower_fixed_cost.set(                 3.e6,             "tower_fixed_cost",      false,                                 "Tower fixed cost",        "$",              "Financial|Parameters" );
    tower_exp.set(                      0.0113,                    "tower_exp",      false,                      "Tower cost scaling exponent",        "-",              "Financial|Parameters" );
    heliostat_spec_cost.set(              145.,          "heliostat_spec_cost",      false,                          "Heliostat specific cost",     "$/m2",              "Financial|Parameters" );
    site_spec_cost.set(                    16.,               "site_spec_cost",      false,                               "Site specific cost",     "$/m2",              "Financial|Parameters" );
    land_spec_cost.set(                  10000,               "land_spec_cost",      false,                               "Land specific cost",   "$/acre",              "Financial|Parameters" );
    c_cps0.set(                             0.,                       "c_cps0",      false,                    "Power cycle cost coef - const",        "-",              "Financial|Parameters" );
    c_cps1.set(                          1440.,                       "c_cps1",      false,                    "Power cycle cost coef - slope",        "-",              "Financial|Parameters" );
    om_staff_cost.set(                      75,                "om_staff_cost",      false,                              "O&M staff cost rate",     "$/hr",              "Financial|Parameters" );
    wash_crew_cost.set(                   100.,               "wash_crew_cost",      false,                              "Wash crew cost rate",     "$/hr",              "Financial|Parameters" );
    heliostat_refurbish_cost.set(        3960.,     "heliostat_refurbish_cost",      false,                          "Mirror replacement cost",        "$",    "Optical degradation|Parameters" );
    om_staff_max_hours_week.set(            35,      "om_staff_max_hours_week",      false,                     "Max O&M staff hours per week",       "hr", "Heliostat availability|Parameters" );
    n_heliostats_sim.set(                 1000,             "n_heliostats_sim",      false,                   "Number of simulated heliostats",        "-", "Heliostat availability|Parameters" );
    wash_units_per_hour.set(               45.,          "wash_units_per_hour",      false,                              "Heliostat wash rate","1/crew-hr",    "Optical degradation|Parameters" );
    wash_crew_max_hours_week.set(          70.,     "wash_crew_max_hours_week",      false,                     "Wash crew max hours per week",       "hr",    "Optical degradation|Parameters" );
    degr_per_hour.set(                   1.e-7,                "degr_per_hour",      false,                    "Reflectivity degradation rate",     "1/hr",    "Optical degradation|Parameters" );
    degr_accel_per_year.set(             0.125,          "degr_accel_per_year",      false,                   "Refl. degradation acceleration",     "1/yr",    "Optical degradation|Parameters" );
    degr_seed.set(                         123,                    "degr_seed",      false,                     "Random number generator seed",        "-",    "Optical degradation|Parameters" );
    soil_per_hour.set(                   6.e-4,                "soil_per_hour",      false,                                "Mean soiling rate",     "1/hr",    "Optical degradation|Parameters" );
    adjust_constant.set(                     4,              "adjust:constant",      false,                            "Misc fixed power loss",        "%",              "Financial|Parameters" );
    helio_reflectance.set(                0.95,            "helio_reflectance",      false,                       "Initial mirror reflectance",        "-",    "Optical degradation|Parameters" );
    disp_rsu_cost.set(                    950.,                "disp_rsu_cost",      false,                            "Receiver startup cost",        "$",             "Simulation|Parameters" );
    disp_csu_cost.set(                  10000.,                "disp_csu_cost",      false,                         "Power block startup cost",        "$",             "Simulation|Parameters" );
    disp_pen_delta_w.set(                  0.1,             "disp_pen_delta_w",      false,                         "Power block ramp penalty","$/delta-kW",            "Simulation|Parameters" );
    rec_su_delay.set(                      0.2,                 "rec_su_delay",      false,                        "Receiver min startup time",       "hr",             "Simulation|Parameters" );
    rec_qf_delay.set(                     0.25,                 "rec_qf_delay",      false,                      "Receiver min startup energy",  "MWh/MWh",             "Simulation|Parameters" );
    startup_time.set(                      0.5,                 "startup_time",      false,                     "Power block min startup time",       "hr",             "Simulation|Parameters" );
    startup_frac.set(                      0.5,                 "startup_frac",      false,                       "Power block startup energy",  "MWh/MWh",             "Simulation|Parameters" );
    v_wind_max.set(                        15.,                   "v_wind_max",      false,                    "Max operational wind velocity",      "m/s",             "Simulation|Parameters" );
    flux_max.set(                        1000.,                     "flux_max",      false,                            "Maximum receiver flux",    "kW/m2",             "Simulation|Parameters" );
    maintenance_interval.set(            5000.,         "maintenance_interval",      false,      "Runtime duration between maintenance events",        "h",                  "Cycle|Parameters" );
    maintenance_duration.set(             168.,         "maintenance_duration",      false,                   "Duration of maintenance events",        "h",                  "Cycle|Parameters" );
    downtime_threshold.set(                 24,           "downtime_threshold",      false,                "Downtime threshold for warm start",        "h",                  "Cycle|Parameters" );
    steplength.set(                         1.,                   "steplength",      false,                    "Simulation time period length",        "h",                  "Cycle|Parameters" );
    hours_to_maintenance.set(            5000.,         "hours_to_maintenance",      false,   "Runtime duration before next maintenance event",        "h",                  "Cycle|Parameters" );
    power_output.set(                       0.,                 "power_output",      false,                       "Initial power cycle output",        "W",                  "Cycle|Parameters" );
    capacity.set(                      500000.,                     "capacity",      false,                             "Power cycle capacity",        "W",                  "Cycle|Parameters" );
    temp_threshold.set(                    20.,               "temp_threshold",      false,     "Ambient temperature threshold for condensers",  "Celsius",                  "Cycle|Parameters" );
    time_online.set(                        0.,                  "time_online",      false,              "Initial power cycle output duration",        "h",                  "Cycle|Parameters" );
    time_in_standby.set(                    0.,              "time_in_standby",      false,               "Initial power cycle time in stndby",        "h",                  "Cycle|Parameters" );
    downtime.set(                           0.,                     "downtime",      false,                     "Initial power cycle downtime",        "h",                  "Cycle|Parameters" );
    shutdown_capacity.set(                 0.3,            "shutdown_capacity",      false,            "Threshold capacity to shut plant down",        "-",                  "Cycle|Parameters" );
    no_restart_capacity.set(               0.8,          "no_restart_capacity",      false,   "Threshold capacity for maintenance on shutdown",        "-",                  "Cycle|Parameters" );
	shutdown_efficiency.set(               0.7,          "shutdown_efficiency",      false,          "Threshold efficiency to shut plant down",        "-",                  "Cycle|Parameters" );
	no_restart_efficiency.set(             0.9,        "no_restart_efficiency",      false, "Threshold efficiency for maintenance on shutdown",        "-",                  "Cycle|Parameters" );
	cycle_hourly_labor_cost.set(           50.,       "cycle_hourly_labor_cost",     false,       "Hourly cost for repair of cycle components",        "h",                  "Cycle|Parameters" );

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
    helio_repair_priority.set(              rp,        "helio_repair_priority",      false,                        "Heliostat repair priority",        "-", "Heliostat availability|Parameters" );
    avail_model_timestep.set(               24,         "avail_model_timestep",      false,                      "Availability model timestep",       "hr", "Heliostat availability|Parameters" );

	std::vector< double > shape = { 1. };
	std::vector< double > scale = { 12000. };
	std::vector< double > mtr = { 2. };
	std::vector< double > repair_cost = { 300. };
    helio_comp_weibull_shape.set(        shape,     "helio_comp_weibull_shape",      false,             "Helio component Weibull shape params",        "-", "Heliostat availability|Parameters" );
    helio_comp_weibull_scale.set(        scale,     "helio_comp_weibull_scale",      false,             "Helio component Weibull scale params",       "hr", "Heliostat availability|Parameters" );
    helio_comp_mtr.set(                    mtr,               "helio_comp_mtr",      false,              "Helio component mean time to repair",       "hr", "Heliostat availability|Parameters" );
    helio_comp_repair_cost.set(    repair_cost,       "helio_comp_repair_cost",      false,                      "Helio component repair cost",        "$", "Heliostat availability|Parameters" );

	// Clustering parameters
    is_use_clusters.set(                 false,              "is_use_clusters",      false,                                    "Use clusters?",        "-",                          "Settings" );
    n_clusters.set(                         30,                   "n_clusters",      false,                               "Number of clusters",        "-",                          "Settings" );

    cluster_ndays.set(                       2,                "cluster_ndays",      false );
    cluster_nprev.set(                       1,                "cluster_nprev",      false );
    is_hard_partitions.set(               true,           "is_hard_partitions",      false );
    is_run_continuous.set(                true,            "is_run_continuous",      false );
	std::string ca = "affinity_propagation";
    cluster_algorithm.set(                  ca,            "cluster_algorithm",      false );

	std::vector<double> default_feature_wts = { 1.0, 0.5, 0.5, 0.0, 1.0, 0.5, 0.5, 0.25, 0.25 };
	std::vector<double> default_feature_divs = { 8, 4, 4, 1, 8, 4, 4, 4, 4 };
    clustering_feature_weights.set(  default_feature_wts,   "clustering_feature_weights",       true );
    clustering_feature_divisions.set( default_feature_wts,      "clustering_feature_divs",       true );

	// Cycle availability and simulation integration
	is_cycle_ssc_integration.set(false, "is_cycle_ssc_integration", false, "Integrate cycle failure and dispatch optimization models?", "-", "Settings");
	is_reoptimize_at_repairs.set(true, "is_reoptimize_at_repairs", false, "Re-optimize at cycle repairs?", "-", "Settings");
	is_reoptimize_at_failures.set(false, "is_reoptimize_at_failures", false, "Re-optimize at cycle failures?", "-", "Settings");
	is_use_target_heuristic.set(false, "is_use_target_heuristic", false, "Use heuristic to adjust dispatch targets due to cycle failures?", "-", "Settings");


    (*this)["print_messages"] = &print_messages;
    (*this)["check_max_flux"] = &check_max_flux;
    (*this)["is_optimize"] = &is_optimize;
    (*this)["is_dispatch"] = &is_dispatch;
    (*this)["is_ampl_engine"] = &is_ampl_engine;
    (*this)["is_stochastic_disp"] = &is_stochastic_disp;
	(*this)["current_standby"] = &current_standby;
	(*this)["stop_cycle_at_first_failure"] = &stop_cycle_at_first_failure;
	(*this)["stop_cycle_at_first_repair"] = &stop_cycle_at_first_repair;
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
	(*this)["read_periods"] = &read_periods;
	(*this)["sim_length"] = &sim_length;
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
	(*this)["maintenance_interval"] = &maintenance_interval;
	(*this)["maintenance_duration"] = &maintenance_duration;
	(*this)["downtime_threshold"] = &downtime_threshold;
	(*this)["steplength"] = &steplength;
	(*this)["hours_to_maintenance"] = &hours_to_maintenance;
	(*this)["power_output"] = &power_output;
	(*this)["capacity"] = &capacity;
	(*this)["temp_threshold"] = &temp_threshold;
	(*this)["time_online"] = &time_online;
	(*this)["time_in_standby"] = &time_in_standby;
	(*this)["downtime"] = &downtime;
	(*this)["shutdown_capacity"] = &shutdown_capacity;
	(*this)["no_restart_capacity"] = &no_restart_capacity;
	(*this)["cycle_hourly_labor_cost"] = &cycle_hourly_labor_cost;
    (*this)["c_ces"] = &c_ces;
    (*this)["dispatch_factors_ts"] = &dispatch_factors_ts;
	(*this)["user_sf_avail"] = &user_sf_avail;
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
	(*this)["is_hard_partitions"] = &is_hard_partitions;
	(*this)["is_run_continuous"] = &is_run_continuous;
	(*this)["cluster_algorithm"] = &cluster_algorithm;
	(*this)["clustering_feature_weights"] = &clustering_feature_weights;
	(*this)["clustering_feature_divisions"] = &clustering_feature_divisions;

	(*this)["is_cycle_ssc_integration"] = &is_cycle_ssc_integration;
	(*this)["is_reoptimize_at_repairs"] = &is_reoptimize_at_repairs;
	(*this)["is_reoptimize_at_failures"] = &is_reoptimize_at_failures;
	(*this)["is_use_target_heuristic"] = &is_use_target_heuristic;

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
	
    n_replacements.set(                    nan,               "n_replacements",       true,                              "Mirror replacements",        "-",       "Optical degradation|Outputs" );
    heliostat_refurbish_cost.set(          nan,     "heliostat_refurbish_cost",       true,                          "Mirror replacement cost",        "$",       "Optical degradation|Outputs" );
    heliostat_refurbish_cost_y1.set(       nan,  "heliostat_refurbish_cost_y1",       true,                 "Mirror replacement cost (year 1)",        "$",       "Optical degradation|Outputs" );
    avg_soil.set(                          nan,                     "avg_soil",       true,                         "Average lifetime soiling",        "-",       "Optical degradation|Outputs" );
    avg_degr.set(                          nan,                     "avg_degr",       true,                     "Average lifetime degradation",        "-",       "Optical degradation|Outputs" );

    soil_schedule.set(               empty_vec,                "soil_schedule",       true,                              "Soiling time series",        "-",       "Optical degradation|Outputs" );
    degr_schedule.set(               empty_vec,                "degr_schedule",       true,                          "Degradation time series",        "-",       "Optical degradation|Outputs" );
    repl_schedule.set(               empty_vec,                "repl_schedule",       true,                         "Mirror repl. time series",        "-",       "Optical degradation|Outputs" );
    repl_total.set(                  empty_vec,                   "repl_total",       true );

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
	turbine_efficiency.set(        nan,                     "turbine_efficiency",  true,                               "Current turbine efficiency",  "-",  "Cycle|Outputs" );
	turbine_capacity.set(          nan,                       "turbine_capacity",  true,                                 "Current turbine capacity",  "-",  "Cycle|Outputs" );
	expected_time_to_next_cycle_failure.set(          nan,    "expected_time_to_next_cycle_failure",  true,   "Expected operating hours before next component failure",  "h",  "Cycle|Outputs" );
	expected_starts_to_next_cycle_failure.set(          nan,  "expected_starts_to_next_cycle_failure",  true,  "Expected number of starts before next component failure",  "-",  "Cycle|Outputs" );

	(*this)["cycle_efficiency"] = &cycle_efficiency;
	(*this)["cycle_capacity"] = &cycle_capacity;
	(*this)["cycle_labor_cost"] = &cycle_labor_cost;
	(*this)["turbine_efficiency"] = &turbine_efficiency;
	(*this)["turbine_capacity"] = &turbine_capacity;
	(*this)["expected_time_to_next_cycle_failure"] = &expected_time_to_next_cycle_failure;
	(*this)["expected_starts_to_next_cycle_failure"] = &expected_starts_to_next_cycle_failure;
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

	cycle_capacity_avail.set(		empty_vec,		"cycle_capacity_avail",			  true,				"Available fraction of cycle capacity",		   "-",				   "Simulation|Outputs");
	cycle_efficiency_avail.set(		empty_vec,		"cycle_efficiency_avail",		  true,			  "Available fraction of cycle efficiency",		   "-",				   "Simulation|Outputs");
	

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
	(*this)["cycle_capacity_avail"] = &cycle_capacity_avail;
	(*this)["cycle_efficiency_avail"] = &cycle_efficiency_avail;
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
		case CALLING_SIM::CYCLE_AVAIL:
			if (!is_design_valid)
			{
				(*error_msg).append("Error: Attempting to run cycle availability without a valid solar field design.\n");
				return false;
			}
			else
			{
				if (!is_cycle_avail_valid)
					(*error_msg).append("Notice: Simulating performance without valid cycle availability model output.\n");
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
				(*error_msg).append(wxString::Format( "Error: Cannot calculate system financial metrics without a valid %s.\n",
													is_design_valid ? (is_simulation_valid ? "cost simulation" : "plant performance simulation") : "solar field design").c_str() );
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

	//construct the merged data map
	std::vector<lk::varhash_t*> struct_pointers = { &m_variables, &m_parameters, &m_design_outputs,
		&m_solarfield_outputs, &m_optical_outputs, &m_cycle_outputs, &m_simulation_outputs, &m_objective_outputs };

	_merged_data.clear();

	for (size_t i = 0; i < struct_pointers.size(); i++)
	{
        lk::varhash_t *this_varhash = struct_pointers.at(i);

        for (lk::varhash_t::iterator it = this_varhash->begin(); it != this_varhash->end(); it++)
		{
            _merged_data[(*it).first] = it->second;
		}
	}

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
        (void*)&m_variables, (void*)&m_parameters, //(void*)&m_cluster_parameters,
        (void*)&m_design_outputs, (void*)&m_optical_outputs, (void*)&m_solarfield_outputs,
		(void*)&m_cycle_outputs, (void*)&m_simulation_outputs, (void*)&m_objective_outputs }; // 

    return rvec;
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
	if (!Validate(Project::CALLING_SIM::CYCLE_AVAIL, &error_msg))
	{
		message_handler(error_msg.c_str());
		return false;
	}
	

	PowerCycle pc = PowerCycle();
	WELLFiveTwelve gen(0);
	pc.AssignGenerator(&gen);

	//Simulation parameters
	pc.SetSimulationParameters(
		m_parameters.read_periods.as_integer(),
		m_parameters.sim_length.as_integer(),
		m_parameters.steplength.as_integer(),
		1.e-8,
		false,
		m_parameters.num_scenarios.as_integer(),
		m_parameters.cycle_hourly_labor_cost.as_number(),
		m_parameters.stop_cycle_at_first_failure.as_boolean(),
		m_parameters.stop_cycle_at_first_repair.as_boolean()
	);
	
	//Plant Components
	std::vector < double > c_eff_cold;
	std::vector < double > c_eff_hot;
	for (int i = 0; i < m_parameters.num_condenser_trains.as_integer()+1; i++)
	{
		c_eff_cold.push_back( m_parameters.condenser_eff_cold.vec()->at(i).as_number() );
		c_eff_hot.push_back( m_parameters.condenser_eff_hot.vec()->at(i).as_number() );
	}
	
	pc.GeneratePlantComponents(
		m_parameters.num_condenser_trains.as_integer(),
		m_parameters.fans_per_train.as_integer(),
		m_parameters.radiators_per_train.as_integer(),
		m_parameters.num_salt_steam_trains.as_integer(),
		m_parameters.num_fwh.as_integer(),
		m_parameters.num_salt_pumps.as_integer(),
		m_parameters.num_salt_pumps_required.as_integer(),
		m_parameters.num_water_pumps.as_integer(),
		m_parameters.num_turbines.as_integer(),
		c_eff_cold,
		c_eff_hot
	);

	//Plant Parameters
	pc.SetPlantAttributes(
		m_parameters.maintenance_interval.as_number(),
		m_parameters.maintenance_duration.as_number(),
		m_parameters.downtime_threshold.as_number(),
		m_parameters.hours_to_maintenance.as_number(),
		m_parameters.power_output.as_number(),
		m_parameters.current_standby.as_boolean(),
		m_parameters.capacity.as_number(),
		m_parameters.temp_threshold.as_number(),
		m_parameters.time_online.as_number(),
		m_parameters.time_in_standby.as_number(),
		m_parameters.downtime.as_number(),
		m_parameters.shutdown_capacity.as_number(),
		m_parameters.no_restart_capacity.as_number(),
		m_parameters.shutdown_efficiency.as_number(),
		m_parameters.no_restart_efficiency.as_number()
	);

	pc.Initialize();

	//Assign Dispatch  
	//ajz: needs to be updated to pull these params from SSC after S() is called
	std::unordered_map < std::string, std::vector < double > > dispatch;
	dispatch["standby"] = {};
	dispatch["cycle_power"] = {};
	dispatch["ambient_temperature"] = {};

	for (int i = 0; i < m_parameters.sim_length.as_integer(); i++)
	{
		dispatch["cycle_power"].push_back(m_parameters.cycle_power.vec()->at(i).as_number());
		dispatch["standby"].push_back(m_parameters.standby.vec()->at(i).as_number());
		dispatch["ambient_temperature"].push_back(m_parameters.ambient_temperature.vec()->at(i).as_number());
	}
	
	pc.SetDispatch(dispatch);

	pc.Simulate(false,false,false);

	//Annualize repair cost
	//double ann_fact = 8760. / (double)(pc.GetSteplength() * pc.GetWriteInterval());

	//pc.m_results.avg_labor_cost = calc_real_dollars(pc.m_results.avg_labor_cost) * ann_fact;

	//Assign results to structure
	pc.GetSummaryResults();
	m_cycle_outputs.cycle_capacity.empty_vector();
	m_cycle_outputs.cycle_efficiency.empty_vector();

	m_cycle_outputs.cycle_labor_cost.assign(pc.m_results.avg_labor_cost);
	m_cycle_outputs.cycle_capacity.assign_vector( pc.m_results.cycle_capacity[0] );
	m_cycle_outputs.cycle_efficiency.assign_vector( pc.m_results.cycle_efficiency[0] );
	m_cycle_outputs.expected_starts_to_next_cycle_failure.assign(pc.m_results.expected_starts_to_failure);
	m_cycle_outputs.expected_time_to_next_cycle_failure.assign(pc.m_results.expected_time_to_failure);
	m_cycle_outputs.num_failures.assign(pc.m_results.failure_event_labels.size());

	is_cycle_avail_valid = true;

	return is_cycle_avail_valid;
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

	//--- Initialize cycle availability model and check for incompatible model settings
	PowerCycle pc;
	WELLFiveTwelve gen(0);

	if (m_parameters.is_cycle_ssc_integration.as_boolean())
	{

		//Plant Components
		std::vector < double > c_eff_cold;
		std::vector < double > c_eff_hot;
		for (int i = 0; i < m_parameters.num_condenser_trains.as_integer() + 1; i++)
		{
			c_eff_cold.push_back(m_parameters.condenser_eff_cold.vec()->at(i).as_number());
			c_eff_hot.push_back(m_parameters.condenser_eff_hot.vec()->at(i).as_number());
		}

		pc.GeneratePlantComponents(
			m_parameters.num_condenser_trains.as_integer(),
			m_parameters.fans_per_train.as_integer(),
			m_parameters.radiators_per_train.as_integer(),
			m_parameters.num_salt_steam_trains.as_integer(),
			m_parameters.num_fwh.as_integer(),
			m_parameters.num_salt_pumps.as_integer(),
			m_parameters.num_salt_pumps_required.as_integer(),
			m_parameters.num_water_pumps.as_integer(),
			m_parameters.num_turbines.as_integer(),
			c_eff_cold,
			c_eff_hot);

		pc.AssignGenerator(&gen);

		pc.SetSimulationParameters(0, 48, 1, 1.e-8, false, m_parameters.num_scenarios.as_integer(), m_parameters.cycle_hourly_labor_cost.as_number(), true);  // called here only to set number of scenarios, duration/step length will be overwritten later

		double capacity = m_variables.P_ref.as_number() * 1.e6;

		// call once to set number of scenarios 
		pc.SetPlantAttributes(m_parameters.maintenance_interval.as_number(), m_parameters.maintenance_duration.as_number(), m_parameters.downtime_threshold.as_number(),
			m_parameters.hours_to_maintenance.as_number(), 0.0, 0, capacity, m_parameters.temp_threshold.as_number(),
			0.0, 0.0, 0.0, m_parameters.shutdown_capacity.as_number(), m_parameters.no_restart_capacity.as_number());  

		pc.Initialize();

		// call again to set hours to maintenance (reset to maintenance interval in Initialize())
		pc.SetPlantAttributes(m_parameters.maintenance_interval.as_number(), m_parameters.maintenance_duration.as_number(), m_parameters.downtime_threshold.as_number(),
			m_parameters.hours_to_maintenance.as_number(), 0.0, 0, capacity, m_parameters.temp_threshold.as_number(),
			0.0, 0.0, 0.0, m_parameters.shutdown_capacity.as_number(), m_parameters.no_restart_capacity.as_number());  
		
		// update "initial" cycle state
		pc.StoreCycleState(); 

		if (!m_parameters.is_dispatch.as_boolean())
		{
			message_handler("Integration of cycle availability and plant simulation models is only available when dispatch optimization is enabled.");
			return false;
		}

		if (m_parameters.num_scenarios.as_integer() > 1)
		{
			message_handler(wxString::Format("Integration of cycle availability and plant simulation models is only available for a single scenario. Parameter 'num_scenarios' is currently %d. \n", m_parameters.num_scenarios.as_integer()));
			return false;
		}

		if (m_parameters.is_use_clusters.as_boolean() && cluster_outputs.exemplars.size() > 0 && !m_parameters.is_hard_partitions.as_boolean())
		{
			message_handler("Cluster-based simulation is specified with integration of cycle availability and plant simulation models. The existing set of clusters will be recomputed wtih hard weighting");
			cluster_outputs.exemplars.clear();
		}

	}




	//--- Run simulation
	is_simulation_valid = simulate_system(pc);
	
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
	double heliostat_wash_cost_y1 = m_parameters.wash_crew_cost.as_number() * m_variables.n_wash_crews.as_number() * m_parameters.wash_crew_max_hours_week.as_number()*52.;
	double heliostat_wash_cost = calc_real_dollars(heliostat_wash_cost_y1, false, true);


	m_explicit_outputs.cost_receiver_real.assign(e_rec_real);
	m_explicit_outputs.cost_tower_real.assign(e_tower_real);
	m_explicit_outputs.cost_plant_real.assign(e_plant_real);
	m_explicit_outputs.cost_tes_real.assign(e_tes_real);

	m_explicit_outputs.heliostat_om_labor_y1.assign(heliostat_om_labor_y1);
	m_explicit_outputs.heliostat_om_labor_real.assign(heliostat_om_labor);

	m_explicit_outputs.heliostat_wash_cost_y1.assign(heliostat_wash_cost_y1);
	m_explicit_outputs.heliostat_wash_cost_real.assign(heliostat_wash_cost);

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

	// Simulation
	if (!is_simulation_valid)
	{
		is_cycle_avail_valid = false;
		is_financial_valid = false;
		S();
	}
	else
		message_handler("Using existing annual performance results in objective function");

	// Cycle efficiency/capacity
	if (!is_cycle_avail_valid)
	{
		is_financial_valid = false;
		C();
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

	double cap_cost = m_objective_outputs.cost_receiver_real.as_number() +
					m_objective_outputs.cost_tower_real.as_number() +
					m_objective_outputs.cost_plant_real.as_number() +
					m_objective_outputs.cost_tes_real.as_number() +
					m_objective_outputs.cost_land_real.as_number() +
					m_objective_outputs.cost_sf_real.as_number();

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
	cluster.inputs.hard_partitions = m_parameters.is_hard_partitions.as_boolean();

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

	return true;
}


bool Project::simulate_system(PowerCycle &pc)
{
	int nr;

	double nan = std::numeric_limits<double>::quiet_NaN();

	//--- Get time step size from current ssc inputs
	ssc_number_t wf_steps_per_hour;
	ssc_data_get_number(m_ssc_data, "time_steps_per_hour", &wf_steps_per_hour);
	int nperday = (int)wf_steps_per_hour * 24;
	int nrec = (int)wf_steps_per_hour * 8760;


	//--- Initialize results
	double annual_generation, revenue_units, rec_starts, cycle_starts, cycle_ramp, max_generation;
	std::vector<double> cycle_capacity, cycle_efficiency;

	//std::vector<std::string> ssc_keys = { "gen", "Q_thermal", "e_ch_tes",  "beam", "pricing_mult", "disp_qsfprod_expected", "disp_wpb_expected" };
	
	std::vector<std::string> ssc_keys = { "gen", "P_cycle", "P_out_net", "e_ch_tes", "T_tes_hot", "T_tes_cold", "Q_thermal", "q_pb", "q_pc_startup", "beam", "tdry", "pricing_mult", 
										  "disp_qsfprod_expected", "disp_wpb_expected", "q_dot_pc_target", "q_dot_pc_max", "is_rec_su_allowed" , "is_pc_su_allowed", "is_pc_sb_allowed" };
	
	int n_hourly_keys = (int)ssc_keys.size();
	unordered_map < std::string, std::vector<double>> collect_ssc_data, full_data, empty_map;
	for (int i = 0; i < n_hourly_keys; i++)
	{
		std::string key = ssc_keys[i];
		collect_ssc_data[key].assign(nrec, nan);
		full_data[key].assign(nrec, nan);
	}



	//--- Run full simulation
	if (!m_parameters.is_use_clusters.as_boolean())
	{

		// Full annual simulation without integration with cycle failure model
		if (!m_parameters.is_cycle_ssc_integration.as_boolean())
		{

			ssc_module_exec_set_print(m_parameters.print_messages.as_boolean());
			ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");

			if (!ssc_module_exec_with_handler(mod_mspt, m_ssc_data, ssc_progress_handler, 0))
			{
				message_handler("SSC simulation failed");
				ssc_module_free(mod_mspt);
				return false;
			}

			for (int i = 0; i < n_hourly_keys; i++)
			{
				std::string key = ssc_keys[i];
				ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
				for (int r = 0; r < nr; r++)
					full_data[key].at(r) = p_data[r];
			}

			ssc_module_free(mod_mspt);
		}


		// Full annual simulation integrated with cycle failure model
		else
		{
			full_data.clear();
			integrate_cycle_and_simulation(pc, 0., 8760., 168., false, empty_map, full_data, cycle_capacity, cycle_efficiency);
		}

	}
	
	//--- Run simulations of only cluster exemplars
	else
	{

		ssc_module_exec_set_print(m_parameters.print_messages.as_boolean());
		ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");

		//--- Set cluster simulation parameters
		cluster_sim csim;
		csim.set_default_inputs();
		csim.inputs.days.nnext = int(m_parameters.is_dispatch.as_boolean());
		csim.inputs.is_run_continuous = m_parameters.is_run_continuous.as_boolean();
		csim.inputs.days.ncount = m_parameters.cluster_ndays.as_integer();
		csim.inputs.days.nprev = m_parameters.cluster_nprev.as_integer();

		if (m_parameters.is_cycle_ssc_integration.as_boolean())
		{
			message_handler("Integration of cycle availability and plant simulation models was selected. 'is_run_continuous' for simulation of clusters will be reset to false");
			csim.inputs.is_run_continuous = false;
			csim.inputs.is_combine_consecutive = false;
		}

		//--- Set up clusters unless already defined
		if (abs(m_parameters.n_clusters.as_integer() - (int)cluster_outputs.exemplars.size()) > 1 )
		{
			if (!setup_clusters())
				return false;
		}
		else
			message_handler(wxString::Format("Using existing set of %d cluster exemplars", cluster_outputs.ncluster).ToStdString().c_str());

		csim.inputs.cluster_results = cluster_outputs;
		csim.assign_first_last(metric_outputs);
		int ncl = csim.inputs.cluster_results.ncluster;


		//--- Get the solar field availability
		ssc_number_t *p_sf = ssc_data_get_array(m_ssc_data, "sf_adjust:hourly", &nr);
		std::vector<double>sfavail;
		for (int i = 0; i < nr; i++)
			sfavail.push_back(1. - p_sf[i]/100.);


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

			for (int i = 0; i < n_hourly_keys; i++)
			{
				std::string key = ssc_keys[i];
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


				int doy_full = d1;
				int doy_sim = nprev;

				// Don't integrate with cycle availability model -> fill in exemplar time block in full year arry with ssc solution
				if (!m_parameters.is_cycle_ssc_integration.as_boolean())
				{
					for (int i = 0; i < n_hourly_keys; i++)
					{
						std::string key = ssc_keys[i];
						ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
						for (int h = 0; h < nperday*ncount; h++)
							collect_ssc_data[key].at(doy_full*nperday + h) = p_data[doy_sim*nperday + h];
					}
				}

				// Integrate with cycle availability model -> re-run availability model (and ssc if failures occur) starting from this ssc solution for all groups within this cluster, 
				// Fill in exemplar time block in full year array with average of all cycle/ssc model realizations
				else  
				{
					std::unordered_map<std::string, std::vector<double>> initial_soln, integrated_soln;
					for (int k = 0; k < n_hourly_keys; k++)
					{
						std::string key = ssc_keys[k];
						ssc_number_t *p_data = ssc_data_get_array(m_ssc_data, key.c_str(), &nr);
						for (int h = 0; h < nperday*ncount; h++)
						{
							initial_soln[key].push_back(p_data[doy_sim*nperday + h]);
							collect_ssc_data[key].at(doy_full*nperday + h) = 0.0;
						}
					}


					int npercluster = cluster_outputs.count[g];
					for (int j = 0; j < npercluster; j++)
					{
						bool use_stored_state = true;
						if (g == 0 && j == 0)
							use_stored_state = false;

						integrate_cycle_and_simulation(pc, doy_full*24., (doy_full + ncount) * 24, ncount, use_stored_state, initial_soln, integrated_soln, cycle_capacity, cycle_efficiency);
						
						for (int k = 0; k < n_hourly_keys; k++)
						{
							for (int h = 0; h < nperday*ncount; h++) 
								collect_ssc_data[ssc_keys[k]].at(doy_full*nperday + h) += integrated_soln[ssc_keys[k]][h] / (double)npercluster;  
						}

					}

				}




			}
		}


		//--- Compute full annual array from array containing simulated values at cluster-exemplar time blocks
		for (int k = 0; k < n_hourly_keys; k++)
		{
			std::string key = ssc_keys[k];
			csim.compute_annual_array_from_clusters(collect_ssc_data[key], full_data[key]);
		}

		ssc_module_free(mod_mspt);

		//message_handler(wxString::Format("Cluster wcss = %.4f", csim.inputs.cluster_results.wcss));
		//message_handler(wxString::Format("Cluster wcss based on exemplar location = %.4f", csim.inputs.cluster_results.wcss_to_exemplars));
		//message_handler("Cluster exemplars,  weights:");
		//for (int i = 0; i < csim.inputs.cluster_results.ncluster; i++)
		//	message_handler(wxString::Format("%d, %.6f", csim.inputs.cluster_results.exemplars.at(i), csim.inputs.cluster_results.weights.at(i)));

	}

	

	//--- Sum annual generation and revenue
	annual_generation = 0.0;
	revenue_units = 0.0;
	max_generation = 0.0;
	for (int i = 0; i < (int)full_data["gen"].size(); i++)
	{
		annual_generation += full_data["gen"].at(i) / (double)wf_steps_per_hour;
		revenue_units += full_data["gen"].at(i) * m_parameters.dispatch_factors_ts.vec()->at(i).as_number() / (double)wf_steps_per_hour;
		max_generation = fmax(max_generation, full_data["gen"].at(i));
	}
	//message_handler(wxString::Format("Annual generation (GWhe) = %.4f",annual_generation / 1.e6).c_str());
	//message_handler(wxString::Format("Annual revenue units (GWhe) = %.4f", revenue_units / 1.e6).c_str());


	//--- Calculate annual receiver starts, cycle starts, and cycle ramping
	rec_starts = 0.0;
	cycle_starts = 0.0;
	cycle_ramp = 0.0;

	double disp_rec_starts = 0.0;
	double disp_cycle_starts = 0.0;
	double disp_cycle_ramp = 0.0;

	for (int i = 1; i < (int)full_data["gen"].size(); i++)
	{
		if (full_data["Q_thermal"][i] > 0.0 && full_data["Q_thermal"][i - 1] <= 0.0)
			rec_starts += 1;

		if (full_data["gen"][i] > 0.0 && full_data["gen"][i - 1] <= 0.0)
			cycle_starts += 1;

		if (full_data["gen"][i] > 0.0 && full_data["gen"][i] > full_data["gen"][i - 1])
			cycle_ramp += full_data["gen"][i] - full_data["gen"][i - 1]; //kWe

		// Dispatched results for comparison
		if (m_parameters.is_dispatch.as_boolean())
		{
			if (full_data["disp_qsfprod_expected"][i] > 0.0 && full_data["disp_qsfprod_expected"][i - 1] <= 0.0)
				disp_rec_starts += 1;

			if (full_data["disp_wpb_expected"][i] > 0.0 && full_data["disp_wpb_expected"][i - 1] <= 0.0)
				disp_cycle_starts += 1;

			if (full_data["disp_wpb_expected"][i] > 0.0 && full_data["disp_wpb_expected"][i] > full_data["disp_wpb_expected"][i - 1])
				disp_cycle_ramp += (full_data["disp_wpb_expected"][i] - full_data["disp_wpb_expected"][i - 1])*1000.;   //"disp_wpb_expected" in MWe
		}
	}
	double cycle_ramp_index = cycle_ramp / max_generation / 365.;
	double disp_cycle_ramp_index = 0.0;
	if (m_parameters.is_dispatch.as_boolean())
		disp_cycle_ramp_index = disp_cycle_ramp / max_generation / 365.;



	//--- Set simulation outputs
	m_simulation_outputs.generation_arr.assign_vector( full_data["gen"] );
	m_simulation_outputs.solar_field_power_arr.assign_vector( full_data["Q_thermal"]);
	m_simulation_outputs.tes_charge_state.assign_vector( full_data["e_ch_tes"] );
	m_simulation_outputs.dni_arr.assign_vector( full_data["beam"] );
	m_simulation_outputs.price_arr.assign_vector( full_data["pricing_mult"] );

	m_simulation_outputs.annual_generation.assign( annual_generation*1.e-6 );
	m_simulation_outputs.annual_revenue_units.assign( revenue_units*1.e-6);
	m_simulation_outputs.annual_rec_starts.assign(rec_starts);
	m_simulation_outputs.annual_cycle_starts.assign(cycle_starts);
	m_simulation_outputs.annual_cycle_ramp.assign(cycle_ramp*1.e-6);
	m_simulation_outputs.cycle_ramp_index.assign(cycle_ramp_index*100.);

	m_simulation_outputs.annual_rec_starts_disp.assign(disp_rec_starts);
	m_simulation_outputs.annual_cycle_starts_disp.assign(disp_cycle_starts);
	m_simulation_outputs.annual_cycle_ramp_disp.assign(disp_cycle_ramp*1.e-6);
	m_simulation_outputs.cycle_ramp_index_disp.assign(disp_cycle_ramp_index*100.);

	m_simulation_outputs.cycle_capacity_avail.assign_vector(cycle_capacity);
	m_simulation_outputs.cycle_efficiency_avail.assign_vector(cycle_efficiency);

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




bool Project::integrate_cycle_and_simulation(PowerCycle &pc, double start_time, double end_time, double horizon, bool start_stored_pc_state,
											std::unordered_map<std::string, std::vector<double>>&initial_ssc_soln, 
											std::unordered_map<std::string, std::vector<double>> &soln,
											std::vector<double> &capacity, std::vector<double> &efficiency)
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

	Input parameters:
		- start_time, end_time: start and end times for the simulation in hours
		- horizon: time horizon for model integration [hr].  Will be fixed to this value if provided. If a value of zero is provided, the horizon will be set based on expected time to next failure.
		- start_stored_pc_state: start from stored pc initial state?
		- initial_ssc_soln = initial_ssc_results (optional, if empty the initial call to ssc will be performed here). Only needed when using clustering and simulated only a selected subset of days
	Outputs:
		- soln = final ssc solution after integration with the cycle availability model 
		- capacity = final time-series for cycle available capacity
		- efficiency = final time-series for cycle available efficiency
	*/



	double nan = std::numeric_limits<double>::quiet_NaN();
	soln.clear();
	capacity.clear();
	efficiency.clear();

	bool is_use_target_heuristic = m_parameters.is_use_target_heuristic.as_boolean();		// Use heuristics to adjust q_pb_target and is_pc_su_allowed when running from pre-computed targets after capacity reduction
	bool is_reoptimize_at_repairs = m_parameters.is_reoptimize_at_repairs.as_boolean();		// Re-run dispatch optimization and cycle availability each time a component is repaired
	bool is_reoptimize_at_failures = m_parameters.is_reoptimize_at_failures.as_boolean();   // Re-run dispatch optimization and cycle availability each time a component fails 


	
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
	unordered_map < std::string, std::vector<double>> current_soln, pc_dispatch, current_targets;
	std::vector<std::string> pc_keys = { "cycle_power", "ambient_temperature", "standby" };
	std::vector<std::string> ssc_keys = { "gen", "P_cycle", "P_out_net", "e_ch_tes", "T_tes_hot", "T_tes_cold", "Q_thermal", "q_pb", "q_pc_startup", "beam", "tdry", "pricing_mult", "disp_qsfprod_expected", "disp_wpb_expected"};
	std::vector<std::string> disp_target_keys = {"Q_thermal", "e_ch_tes", "q_dot_pc_target", "q_dot_pc_max", "is_rec_su_allowed", "is_pc_su_allowed", "is_pc_sb_allowed" };

	//-- Use provided initial ssc solution, or rerun? Only use provided initial ssc solution if the solution exists for all variables and for all time points between the specified start and end times
	bool use_existing_ssc_soln = true;
	if (initial_ssc_soln.size() == 0)  
		use_existing_ssc_soln = false;
	else
	{
		std::unordered_map < std::string, std::vector<double> >::iterator it;
		for (size_t k = 0; k < ssc_keys.size(); k++)
		{
			it = initial_ssc_soln.find(ssc_keys[k]);
			if (it == initial_ssc_soln.end() || it->second.size() != nsteps_tot)  
				use_existing_ssc_soln = false;
		}
	}


	//--- Initial states
	ssc_number_t tes_charge, tes_thot, tes_thot_des, tes_tcold, tes_tcold_des, is_dispatch;
	bool is_rec_on, is_pc_on, is_pc_standby, is_reoptimize, use_stored_state;
	double capacity_last, efficiency_last;
	std::vector<double> q_pc_target, q_pc_max, is_rec_su_allowed, is_pc_su_allowed, is_pc_sb_allowed;
	ssc_data_get_number(m_ssc_data, "T_htf_hot_des", &tes_thot_des);
	ssc_data_get_number(m_ssc_data, "T_htf_cold_des", &tes_tcold_des);
	ssc_data_get_number(m_ssc_data, "csp.pt.tes.init_hot_htf_percent", &tes_charge);
	ssc_data_get_number(m_ssc_data, "is_dispatch", &is_dispatch);
	tes_thot = tes_thot_des;
	tes_tcold = tes_tcold_des;
	is_rec_on = 0;
	is_pc_on = 0;
	is_pc_standby = 0;
	capacity_last = 1.0;
	efficiency_last = 1.0;
	is_reoptimize = true;
	use_stored_state = start_stored_pc_state;

	ssc_module_exec_set_print(m_parameters.print_messages.as_boolean());
	ssc_module_t mod_mspt = ssc_module_create("tcsmolten_salt");

	
	//--- Loop over model horizons  
	double time_completed = start_time;
	while (time_completed < end_time)  
	{

		//-- Set amount of time to run ssc solution and cycle availability model
		double current_horizon = horizon;
		if (horizon == 0)
		{
			double fop = estimate_capacity_factor(m_variables.solarm.as_number(), m_variables.tshours.as_number());  // rough estimate of annual capacity factor based on configuration
			current_horizon = pc.GetEstimatedMinimumLifetime(fop);
		}

		double hour_end = fmin(end_time, time_completed + current_horizon);
		current_horizon = hour_end - time_completed;
		int nsteps = (int)ceil(current_horizon / steplength);
		
		pc.SetSimulationParameters(0, nsteps, steplength, 1.e-8, false, m_parameters.num_scenarios.as_integer(), m_parameters.cycle_hourly_labor_cost.as_number(), true, true);

		//-- Initialize solutions for this model horizon 
		//-- Start from "full" cycle capacity/efficiency to set dispatch targets unless reoptimizing at repairs (in which case targets will be re-computed at any increase in availability, so no reason not to use current capacity/efficiency in first call)
		is_reoptimize = true;
		double capacity_init = 1.0;
		double efficiency_init = 1.0;
		if (is_reoptimize_at_repairs && capacity_last > 0.0)
		{
			capacity_init = capacity_last;
			efficiency_init = efficiency_last;
		}
		std::vector<double> cycle_capacity(nsteps, capacity_init);
		std::vector<double> cycle_efficiency(nsteps, efficiency_init);

		for (int k = 0; k < (int)pc_keys.size(); k++)
			pc_dispatch[pc_keys[k]].assign(nsteps, nan);

		for (int k = 0; k < (int)ssc_keys.size(); k++)
			current_soln[ssc_keys[k]].assign(nsteps, nan);

		for (int k = 0; k < (int)disp_target_keys.size(); k++)
			current_targets[disp_target_keys[k]].assign(nsteps, nan);

		if (use_existing_ssc_soln)
		{
			for (int k = 0; k < (int)ssc_keys.size(); k++)
			{
				std::string key = ssc_keys[k];
				for (int i = 0; i < nsteps; i++)
					current_soln[key][i] = initial_ssc_soln[key][i];
			}
		}
		

		//-- Alternate between ssc and cycle availability solutions
		int step_now = 0;
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

				if (is_reoptimize)  // Optimize dispatch 
				{
					ssc_data_set_number(m_ssc_data, "is_dispatch", is_dispatch);
					ssc_data_set_number(m_ssc_data, "is_dispatch_targets", false);
					ssc_data_set_number(m_ssc_data, "is_dispatch_constr", true);

					int n = nsteps - step_now;
					ssc_number_t *p_data = new ssc_number_t[n];

					for (int i = 0; i < n; i++)
						p_data[i] = cycle_capacity[step_now + i];
					ssc_data_set_array(m_ssc_data, "disp_cap_constr", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = cycle_efficiency[step_now + i];
					ssc_data_set_array(m_ssc_data, "disp_eff_constr", p_data, n);

					delete[] p_data;

				}

				else   // Set targets for cycle operation from previous optimization
				{
					ssc_data_set_number(m_ssc_data, "is_dispatch", false);
					ssc_data_set_number(m_ssc_data, "is_dispatch_targets", true);
					ssc_data_set_number(m_ssc_data, "is_dispatch_constr", false);

					
					int n = (int)q_pc_target.size();
					ssc_number_t *p_data = new ssc_number_t[n];

					for (int i = 0; i < n; i++)
						p_data[i] = q_pc_target[i];
					ssc_data_set_array(m_ssc_data, "q_pc_target_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = q_pc_max[i];
					ssc_data_set_array(m_ssc_data, "q_pc_max_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = is_rec_su_allowed[i];
					ssc_data_set_array(m_ssc_data, "is_rec_su_allowed_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = is_pc_su_allowed[i];
					ssc_data_set_array(m_ssc_data, "is_pc_su_allowed_in", p_data, n);

					for (int i = 0; i < n; i++)
						p_data[i] = is_pc_sb_allowed[i];
					ssc_data_set_array(m_ssc_data, "is_pc_sb_allowed_in", p_data, n);

					delete[] p_data;
					
				}


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
							current_targets[key][step_now + r] = p_data[r];
					}
				}

			}
			
			std::vector<double>cycle_capacity_in = cycle_capacity;
			std::vector<double>cycle_efficiency_in = cycle_efficiency;

			//--- Find next failure/repair event
			bool is_repair, is_failure;
			int next_start_pt = -1;

			if (step_now == 0 && (cycle_capacity[0] - capacity_last > 0.01 || cycle_efficiency[0] - efficiency_last > 0.01))  // Capacity/efficiency used in ssc at first step is too high... treat like a failure
			{
				next_start_pt = 0;
				is_failure = true;
				is_repair = false;
				cycle_capacity.assign(nsteps, capacity_last);
				cycle_efficiency.assign(nsteps, efficiency_last);
			}
			else if (step_now == 0 && (cycle_capacity[0] - capacity_last < -0.01 || cycle_efficiency[0] - efficiency_last < -0.01))   // Capacity/efficiency used in ssc at first step is too low... treat like a repair
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
				int prev_repair = pc.m_results.period_of_last_repair[sc];

				for (int i = step_now; i < nsteps; i++)
				{
					pc_dispatch["cycle_power"][i] = current_soln["P_cycle"][i]* 1.e6;		// cycle power output [W]
					if (cycle_capacity[i] < 0.999)  // limit dispatch input to cycle model to avoid mismatch between power output in simulated / read-only periods
						pc_dispatch["cycle_power"][i] = fmin(pc_dispatch["cycle_power"][i], m_variables.P_ref.as_number() * cycle_capacity[i] * 1.e6);
					
					pc_dispatch["ambient_temperature"][i] = current_soln["tdry"][i];
					pc_dispatch["standby"][i] = 0;
					if (current_soln["P_cycle"][i] < 1.e-6 && current_soln["q_pb"][i] > 0.0 && current_soln["q_pc_startup"][i] < current_soln["q_pb"][i])  // Thermal energy going to power block, but no electrical output
						pc_dispatch["standby"][i] = 1.0;
				}

				pc.SetDispatch(pc_dispatch, true);
				pc.Simulate(false, use_stored_state);
				use_stored_state = true;  // Use stored state for all PC calls after first call	

				cycle_capacity = pc.m_results.cycle_capacity[sc];		
				cycle_efficiency = pc.m_results.cycle_efficiency[sc];	

				int next_failure = pc.m_results.period_of_last_failure[sc];
				int next_repair = pc.m_results.period_of_last_repair[sc];
				

				if (next_repair > prev_repair) 
					next_repair += 1;  // repairs are reported from cycle model in the time step before capacity/efficiency increase

				if (next_failure <= step_now)
					next_failure = nsteps + 1000;
				if (next_repair <= step_now)
					next_repair = nsteps + 1000;

				next_start_pt = std::min(nsteps, std::min(next_failure, next_repair));
				is_repair = (next_repair == next_start_pt);
				is_failure = (!is_repair && next_failure == next_start_pt);


				if (next_start_pt == nsteps)
				{
					capacity_last = cycle_capacity.back();
					efficiency_last = cycle_efficiency.back();
				}

				if (next_repair == nsteps) // repair occurred at last time point in this horizon... call cycle model again to store state and manually increase capacity/efficiency for beginning of next horizon
				{
					pc.Simulate(false, use_stored_state);
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


			
			//--- Write current solution to file		
			std::ofstream ofs;
			std::string filename = "C:/Users/jmartine/Desktop/cycle_ssc_results/time" + std::to_string(time_completed) + "_step" + std::to_string(step_now) + ".txt";
			ofs.open(filename, std::ofstream::out);
			ofs.clear();
			ofs << "PC target (MWt), PC energy (MWt), TES (MWht), Receiver (MWt), Cycle gross output (MWe), Capacity, Efficiency\n";
			for (int i = 0; i < nsteps; i++)
			{
				ofs << current_targets["q_dot_pc_target"][i] << ", "
					<< current_soln["q_pb"][i] << ", "
					<< current_soln["e_ch_tes"][i] << ", "
					<< current_soln["Q_thermal"][i] << ", "
					<< (current_soln["P_cycle"][i] * cycle_efficiency_in[i]) << ", "
					<< cycle_capacity[i] << ", "
					<< cycle_efficiency[i] <<"\n";
			}
			ofs.close();
			

			


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
			else if (soln["q_pb"].back() > 0.0 && soln["q_pc_startup"].back() < soln["q_pb"].back())
				is_pc_standby = true;


			//--- Set dispatch targets if next ssc call will not involve re-optimization
			double e_ch_tes_adj = nan;
			if (!is_reoptimize && !use_existing_ssc_soln && next_start_pt < nsteps)
			{

				int n = nsteps - next_start_pt;
				q_pc_target.assign(n, 0);
				q_pc_max.assign(n, 0);
				is_rec_su_allowed.assign(n, 0);
				is_pc_su_allowed.assign(n, 0);
				is_pc_sb_allowed.assign(n, 0);

				double op_cutoff = 0.999*(q_pc_des * cycle_cutoff_frac);
				double stby_cutoff = 0.999*(q_pc_des * q_sby_frac);

				for (int i = 0; i < n; i++)
				{
					int p = next_start_pt + i; 

					q_pc_target[i] = current_targets["q_dot_pc_target"][p];
					q_pc_max[i] = current_targets["q_dot_pc_max"][p];
					is_rec_su_allowed[i] = current_targets["is_rec_su_allowed"][p];
					is_pc_su_allowed[i] = current_targets["is_pc_su_allowed"][p];
					is_pc_sb_allowed[i] = current_targets["is_pc_sb_allowed"][p];

					if (cycle_capacity[p] < 0.999)  // Modify targets based on reduced cycle capacity
					{
						q_pc_target[i] = fmin(q_pc_target[i], q_pc_des * cycle_capacity[p]);  
						q_pc_max[i] = q_pc_des * cycle_capacity[p];

						if (q_pc_target[i] < op_cutoff)
							is_pc_su_allowed[i] = 0;

						if (q_pc_target[i] < stby_cutoff)
							is_pc_sb_allowed[i] = 0;

						if (is_pc_su_allowed[i] == 0 && is_pc_sb_allowed[i] == 0)
							q_pc_target[i] = 0.0;
					}

					// Adjust q_pb_target and is_pc_su_allowed in cases where additional TES is present due to outages
					if (is_use_target_heuristic)  
					{
						if (i == 0)
							e_ch_tes_adj = current_targets["e_ch_tes"][p];
						else
						{
							double e_ch_tes_prev = current_targets["e_ch_tes"][p - 1];
							double tes_extra = fmax(0.0, e_ch_tes_adj - e_ch_tes_prev);  // extra TES relative to original dispatch solution without cycle failrues

							if (tes_extra > 0.0)
							{
								double q_pb_added = 0.0;

								// new target -> keep running as close as possible to previous target, or at maximum if constrained capacity changes
								double new_target = q_pc_target[i - 1];
								if (abs(cycle_capacity[p] - cycle_capacity[p - 1]) > 0.01)
									new_target = q_pc_max[i];

								// power block was already planned to run during this timestep but was supposed to ramp down 
								if (is_pc_su_allowed[i] == 1 && q_pc_target[i - 1] - q_pc_target[i] > 0)
									q_pb_added = fmin(0.98*tes_extra / steplength, new_target - q_pc_target[i]);

								// power block was not planned to run during this timestep but was running during previous timestep and extra TES is sufficient for at least minimum operation
								else if ((is_pc_su_allowed[i] + is_pc_sb_allowed[i] == 0) && (is_pc_su_allowed[i - 1] == 1) && (0.98*tes_extra / steplength > q_pc_des*cycle_cutoff_frac))
									q_pb_added = fmin(0.98*tes_extra / steplength, new_target);

								q_pc_target[i] += q_pb_added;

								if (q_pc_target[i] > 0.0 && is_pc_sb_allowed[i] == 0)
									is_pc_su_allowed[i] = 1;
							}


							// calculate approximate expected TES charge state at end of this step
							double expected_tes_change = current_targets["e_ch_tes"][p] - current_targets["e_ch_tes"][p - 1];
							e_ch_tes_adj = e_ch_tes_adj + expected_tes_change + steplength * (current_targets["q_dot_pc_target"][p] - q_pc_target[i]);  // Approximate TES charge state using new targets

							if (e_ch_tes_adj > tes_capacity)
							{
								double q_thermal = current_targets["Q_thermal"][p];  // expected receiver output
								double q_decrease = (e_ch_tes_adj - tes_capacity)/steplength;	// Decrease in receiver output required to keep TES within limits
								if (q_thermal > 0.0 && current_targets["Q_thermal"][p - 1] > 0.0 && (q_thermal - q_decrease) < q_rec_des*f_rec_min)  // Does receiver need to be shut off? (only for steps without receiver startup)
									q_decrease = q_thermal;

								e_ch_tes_adj -= q_decrease * steplength; 
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


	return true;

}

double Project::estimate_capacity_factor(double sm, double tes)
{
	// rough correlation between annual capacity factor and solar multiple, tes size
	sm = fmax(0.6, fmin(sm, 3.0));
	tes = fmax(2.0, fmin(tes, 16.0));

	double cf;
	if (sm <= 1.5)
		cf = -2.3 + 29.3*sm - 0.3*tes + 0.05*sm*tes;
	else
		cf = 7.4 + 28.0*sm - 0.86*tes + 2.1*sm*tes - 6.6*sm*sm - 0.137*tes*tes;

	return cf / 100.;

}