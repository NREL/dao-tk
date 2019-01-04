#include "project.h"



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
	ssc_data_set_number( m_ssc_data, "degr_replace_limit", (ssc_number_t)0.7);
	//parameters
	ssc_data_set_string( m_ssc_data, "ampl_data_dir", " ");
	ssc_data_set_number( m_ssc_data, "is_stochastic_disp", 0 );
	ssc_data_set_number( m_ssc_data, "q_rec_standby", (ssc_number_t)9e9 );
	ssc_data_set_number( m_ssc_data, "eta_map_aod_format",  0 );
	ssc_data_set_number( m_ssc_data, "eta_map_aod_format",  0 );
	ssc_data_set_number( m_ssc_data, "ppa_multiplier_model",  0 );
	ssc_data_set_number( m_ssc_data, "field_model_type",  2 );
	ssc_data_set_number( m_ssc_data, "gross_net_conversion_factor", (ssc_number_t)0.90 );
	ssc_data_set_number( m_ssc_data, "helio_width", (ssc_number_t)12.2 );
	ssc_data_set_number( m_ssc_data, "helio_height", (ssc_number_t)12.2 );
	ssc_data_set_number( m_ssc_data, "helio_optical_error_mrad", (ssc_number_t)1.523 );
	ssc_data_set_number( m_ssc_data, "helio_active_fraction", (ssc_number_t)0.99 );
	ssc_data_set_number( m_ssc_data, "dens_mirror", (ssc_number_t)0.97 );
	ssc_data_set_number( m_ssc_data, "helio_reflectance", (ssc_number_t)0.9 );
	ssc_data_set_number( m_ssc_data, "rec_absorptance", (ssc_number_t)0.94 );
	ssc_data_set_number( m_ssc_data, "rec_hl_perm2", (ssc_number_t)30 );
	ssc_data_set_number( m_ssc_data, "land_max", (ssc_number_t)9.5 );
	ssc_data_set_number( m_ssc_data, "land_min", (ssc_number_t)0.75 );
	ssc_data_set_number( m_ssc_data, "dni_des", (ssc_number_t)950 );
	ssc_data_set_number( m_ssc_data, "p_start", (ssc_number_t)0.025 );
	ssc_data_set_number( m_ssc_data, "p_track", (ssc_number_t)0.055 );
	ssc_data_set_number( m_ssc_data, "hel_stow_deploy", (ssc_number_t)8 );
	ssc_data_set_number( m_ssc_data, "v_wind_max", (ssc_number_t)15 );
	ssc_data_set_number( m_ssc_data, "c_atm_0", (ssc_number_t)0.006789 );
	ssc_data_set_number( m_ssc_data, "c_atm_1", (ssc_number_t)0.1046 );
	ssc_data_set_number( m_ssc_data, "c_atm_2", (ssc_number_t)-0.017 );
	ssc_data_set_number( m_ssc_data, "c_atm_3", (ssc_number_t)0.002845 );
	ssc_data_set_number( m_ssc_data, "n_facet_x",  2 );
	ssc_data_set_number( m_ssc_data, "n_facet_y",  8 );
	ssc_data_set_number( m_ssc_data, "focus_type",  1 );
	ssc_data_set_number( m_ssc_data, "cant_type",  1 );
	ssc_data_set_number( m_ssc_data, "n_flux_days",  8 );
	ssc_data_set_number( m_ssc_data, "delta_flux_hrs",  2 );
	ssc_data_set_number( m_ssc_data, "water_usage_per_wash", (ssc_number_t)0.7 );
	ssc_data_set_number( m_ssc_data, "washing_frequency",  63 );
	ssc_data_set_number( m_ssc_data, "check_max_flux",  0 );
	ssc_data_set_number( m_ssc_data, "sf_excess",  1 );
	ssc_data_set_number( m_ssc_data, "tower_fixed_cost", (ssc_number_t)3000000 );
	ssc_data_set_number( m_ssc_data, "tower_exp", (ssc_number_t)0.0113 );
	ssc_data_set_number( m_ssc_data, "rec_ref_cost", (ssc_number_t)103000000 );
	ssc_data_set_number( m_ssc_data, "rec_ref_area", (ssc_number_t)1571 );
	ssc_data_set_number( m_ssc_data, "rec_cost_exp", (ssc_number_t)0.7 );
	ssc_data_set_number( m_ssc_data, "site_spec_cost", (ssc_number_t)16 );
	ssc_data_set_number( m_ssc_data, "heliostat_spec_cost", (ssc_number_t)145 );
	ssc_data_set_number( m_ssc_data, "plant_spec_cost", (ssc_number_t)1100 );
	ssc_data_set_number( m_ssc_data, "bop_spec_cost", (ssc_number_t)340 );
	ssc_data_set_number( m_ssc_data, "tes_spec_cost", (ssc_number_t)24 );
	ssc_data_set_number( m_ssc_data, "land_spec_cost", (ssc_number_t)10000 );
	ssc_data_set_number( m_ssc_data, "contingency_rate", (ssc_number_t)7 );
	ssc_data_set_number( m_ssc_data, "sales_tax_rate", (ssc_number_t)5 );
	ssc_data_set_number( m_ssc_data, "sales_tax_frac", (ssc_number_t)80 );
	ssc_data_set_number( m_ssc_data, "cost_sf_fixed", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "fossil_spec_cost", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "is_optimize", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "opt_init_step", (ssc_number_t)0.06 );
	ssc_data_set_number( m_ssc_data, "opt_max_iter",  200 );
	ssc_data_set_number( m_ssc_data, "opt_conv_tol", (ssc_number_t)0.001 );
	ssc_data_set_number( m_ssc_data, "opt_flux_penalty", (ssc_number_t)0.25 );
	ssc_data_set_number( m_ssc_data, "opt_algorithm",  1 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.per_acre", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.percent", (ssc_number_t)13 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.per_watt", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.epc.fixed", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.plm.percent", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.plm.per_watt", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.cost.plm.fixed", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "csp.pt.sf.fixed_land_area", (ssc_number_t)45 );
	ssc_data_set_number( m_ssc_data, "csp.pt.sf.land_overhead_factor", (ssc_number_t)1 );
	ssc_data_set_number( m_ssc_data, "T_htf_cold_des", (ssc_number_t)290 );
	ssc_data_set_number( m_ssc_data, "T_htf_hot_des", (ssc_number_t)574 );
	ssc_data_set_number( m_ssc_data, "P_ref", (ssc_number_t)115 );
	ssc_data_set_number( m_ssc_data, "design_eff", (ssc_number_t)0.412 );
	ssc_data_set_number( m_ssc_data, "tshours", (ssc_number_t)10 );
	ssc_data_set_number( m_ssc_data, "solarm", (ssc_number_t)2.4 );
	ssc_data_set_number( m_ssc_data, "receiver_type",  0 );
	ssc_data_set_number( m_ssc_data, "N_panel_pairs",  10 );
	ssc_data_set_number( m_ssc_data, "d_tube_out", (ssc_number_t)40 );
	ssc_data_set_number( m_ssc_data, "th_tube", (ssc_number_t)1.25 );
	ssc_data_set_number( m_ssc_data, "mat_tube",  2 );
	ssc_data_set_number( m_ssc_data, "rec_htf",  17 );
	ssc_data_set_number( m_ssc_data, "Flow_type",  1 );
	ssc_data_set_number( m_ssc_data, "epsilon", (ssc_number_t)0.88 );
	ssc_data_set_number( m_ssc_data, "hl_ffact",  1 );
	ssc_data_set_number( m_ssc_data, "f_rec_min", (ssc_number_t)0.25 );
	ssc_data_set_number( m_ssc_data, "rec_su_delay", (ssc_number_t)0.2 );
	ssc_data_set_number( m_ssc_data, "rec_qf_delay", (ssc_number_t)0.25 );
	ssc_data_set_number( m_ssc_data, "csp.pt.rec.max_oper_frac", (ssc_number_t)1.2 );
	ssc_data_set_number( m_ssc_data, "eta_pump", (ssc_number_t)0.85 );
	ssc_data_set_number( m_ssc_data, "piping_loss", (ssc_number_t)10200 );
	ssc_data_set_number( m_ssc_data, "piping_length_mult", (ssc_number_t)2.6 );
	ssc_data_set_number( m_ssc_data, "piping_length_const", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "rec_d_spec", (ssc_number_t)15 );
	ssc_data_set_number( m_ssc_data, "csp.pt.tes.init_hot_htf_percent", (ssc_number_t)30 );
	ssc_data_set_number( m_ssc_data, "h_tank", (ssc_number_t)20 );
	ssc_data_set_number( m_ssc_data, "cold_tank_max_heat", (ssc_number_t)15 );
	ssc_data_set_number( m_ssc_data, "u_tank", (ssc_number_t)0.4 );
	ssc_data_set_number( m_ssc_data, "tank_pairs",  1 );
	ssc_data_set_number( m_ssc_data, "cold_tank_Thtr", (ssc_number_t)280 );
	ssc_data_set_number( m_ssc_data, "h_tank_min", (ssc_number_t)1 );
	ssc_data_set_number( m_ssc_data, "hot_tank_Thtr", (ssc_number_t)500 );
	ssc_data_set_number( m_ssc_data, "hot_tank_max_heat", (ssc_number_t)30 );
	ssc_data_set_number( m_ssc_data, "tc_fill", (ssc_number_t)8 );
	ssc_data_set_number( m_ssc_data, "tc_void", (ssc_number_t)0.25 );
	ssc_data_set_number( m_ssc_data, "t_dis_out_min", (ssc_number_t)500 );
	ssc_data_set_number( m_ssc_data, "t_ch_out_max", (ssc_number_t)400 );
	ssc_data_set_number( m_ssc_data, "nodes",  100 );
	ssc_data_set_number( m_ssc_data, "pc_config",  0 );
	ssc_data_set_number( m_ssc_data, "pb_pump_coef", (ssc_number_t)0.55 );
	ssc_data_set_number( m_ssc_data, "startup_time", (ssc_number_t)0.5 );
	ssc_data_set_number( m_ssc_data, "startup_frac", (ssc_number_t)0.5 );
	ssc_data_set_number( m_ssc_data, "cycle_max_frac", (ssc_number_t)1.05 );
	ssc_data_set_number( m_ssc_data, "cycle_cutoff_frac", (ssc_number_t)0.2 );
	ssc_data_set_number( m_ssc_data, "q_sby_frac", (ssc_number_t)0.2 );
	ssc_data_set_number( m_ssc_data, "dT_cw_ref", (ssc_number_t)10 );
	ssc_data_set_number( m_ssc_data, "T_amb_des", (ssc_number_t)42 );
	ssc_data_set_number( m_ssc_data, "P_boil", (ssc_number_t)100 );
	ssc_data_set_number( m_ssc_data, "CT",  2 );
	ssc_data_set_number( m_ssc_data, "T_approach", (ssc_number_t)5 );
	ssc_data_set_number( m_ssc_data, "T_ITD_des", (ssc_number_t)16 );
	ssc_data_set_number( m_ssc_data, "P_cond_ratio", (ssc_number_t)1.0028 );
	ssc_data_set_number( m_ssc_data, "pb_bd_frac", (ssc_number_t)0.02 );
	ssc_data_set_number( m_ssc_data, "P_cond_min", (ssc_number_t)2 );
	ssc_data_set_number( m_ssc_data, "n_pl_inc",  8 );
	ssc_data_set_number( m_ssc_data, "tech_type",  1 );
	ssc_data_set_number( m_ssc_data, "time_start",  0 );
	ssc_data_set_number( m_ssc_data, "time_stop",  31536000 );
	ssc_data_set_number( m_ssc_data, "pb_fixed_par", (ssc_number_t)0.0055 );
	ssc_data_set_number( m_ssc_data, "aux_par", (ssc_number_t)0.023 );
	ssc_data_set_number( m_ssc_data, "aux_par_f", (ssc_number_t)1 );
	ssc_data_set_number( m_ssc_data, "aux_par_0", (ssc_number_t)0.483 );
	ssc_data_set_number( m_ssc_data, "aux_par_1", (ssc_number_t)0.571 );
	ssc_data_set_number( m_ssc_data, "aux_par_2", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "bop_par", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "bop_par_f", (ssc_number_t)1 );
	ssc_data_set_number( m_ssc_data, "bop_par_0", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "bop_par_1", (ssc_number_t)0.483 );
	ssc_data_set_number( m_ssc_data, "bop_par_2", (ssc_number_t)0 );
	ssc_data_set_number( m_ssc_data, "is_dispatch",  0 );
	ssc_data_set_number( m_ssc_data, "disp_horizon",  48 );
	ssc_data_set_number( m_ssc_data, "disp_frequency",  24 );
	ssc_data_set_number( m_ssc_data, "disp_max_iter",  35000 );
	ssc_data_set_number( m_ssc_data, "disp_timeout", (ssc_number_t)5 );
	ssc_data_set_number( m_ssc_data, "disp_mip_gap", (ssc_number_t)0.001 );
	ssc_data_set_number( m_ssc_data, "disp_time_weighting", (ssc_number_t)0.99 );
	ssc_data_set_number( m_ssc_data, "disp_rsu_cost",  950 );
	ssc_data_set_number( m_ssc_data, "disp_csu_cost",  10000 );
	ssc_data_set_number( m_ssc_data, "disp_pen_delta_w", (ssc_number_t)0.1 );
    ssc_data_set_number( m_ssc_data, "fc_gamma", (ssc_number_t)0.0 );
	ssc_data_set_number( m_ssc_data, "is_wlim_series",  0 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor1", (ssc_number_t)2.064 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor2", (ssc_number_t)1.2 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor3",  1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor4", (ssc_number_t)1.1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor5", (ssc_number_t)0.8 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor6", (ssc_number_t)0.7 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor7",  1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor8",  1 );
	ssc_data_set_number( m_ssc_data, "dispatch_factor9",  1 );
	ssc_data_set_number( m_ssc_data, "is_dispatch_series",  0 );
	ssc_data_set_number( m_ssc_data, "rec_height", (ssc_number_t)21.6 );
	ssc_data_set_number( m_ssc_data, "D_rec", (ssc_number_t)17.65 );
	ssc_data_set_number( m_ssc_data, "h_tower", (ssc_number_t)193.46 );
	ssc_data_set_number( m_ssc_data, "land_area_base", (ssc_number_t)1847.037 );
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
	ssc_number_t p_f_turb_tou_periods[9] = { (ssc_number_t)1.05, 1, 1, 1, 1, 1, 1, 1, 1 };
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
		p_wlim_series[ii] = (ssc_number_t)1.e37;
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
