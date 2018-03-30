#include "project.h"
#include <limits>

variables::variables()
{
	/* 
	Initialize members
	*/
	double dnan = std::numeric_limits<double>::quiet_NaN();
	double dmax = -std::numeric_limits<double>::infinity();
	double dmin = -dmax;
	
	h_tower.set(dnan, dmin, dmax, "h_tower", DATATYPE::TYPE_NUMBER);
	rec_height.set(dnan, dmin, dmax, "rec_height", DATATYPE::TYPE_NUMBER);
	D_rec.set(dnan, dmin, dmax, "D_rec", DATATYPE::TYPE_NUMBER);
	design_eff.set(dnan, dmin, dmax, "design_eff", DATATYPE::TYPE_NUMBER);
	dni_des.set(dnan, dmin, dmax, "dni_des", DATATYPE::TYPE_NUMBER);
	P_ref.set(dnan, dmin, dmax, "P_ref", DATATYPE::TYPE_NUMBER);
	solarm.set(dnan, dmin, dmax, "solarm", DATATYPE::TYPE_NUMBER);
	tshours.set(dnan, dmin, dmax, "tshours", DATATYPE::TYPE_NUMBER);
	degr_replace_limit.set(dnan, dmin, dmax, "degr_replace_limit", DATATYPE::TYPE_NUMBER);
	om_staff.set(-1, -999, 999, "om_staff", DATATYPE::TYPE_NUMBER);
	n_wash_crews.set(-1, -999, 999, "n_wash_crews", DATATYPE::TYPE_NUMBER);
	N_panels.set(-1, -999, 999, "N_panels", DATATYPE::TYPE_NUMBER);
};

parameters::parameters()
{
	/* 
	Initialize members
	*/

	print_messages.set( true, "print_messages", DATATYPE::TYPE_BOOL, false );
	check_max_flux.set( true, "check_max_flux", DATATYPE::TYPE_BOOL, false );
	is_optimize.set( false, "is_optimize", DATATYPE::TYPE_BOOL, false );
	is_dispatch.set( false, "is_dispatch", DATATYPE::TYPE_BOOL, false );
	is_ampl_engine.set( false, "is_ampl_engine", DATATYPE::TYPE_BOOL, false );
	is_stochastic_disp.set( false, "is_stochastic_disp", DATATYPE::TYPE_BOOL, false );

	ampl_data_dir.set( "", "ampl_data_dir", DATATYPE::TYPE_STRING, false );
	solar_resource_file.set( "", "solar_resource_file", DATATYPE::TYPE_STRING, false );

	disp_steps_per_hour.set( 1, "disp_steps_per_hour", DATATYPE::TYPE_INT, false );
	avail_seed.set( 123, "avail_seed", DATATYPE::TYPE_INT, false );
	plant_lifetime.set( 30, "plant_lifetime", DATATYPE::TYPE_INT, false );
	finance_period.set( 25, "finance_period", DATATYPE::TYPE_INT, false );
	ppa_multiplier_model.set( 1, "ppa_multiplier_model", DATATYPE::TYPE_INT, false );

	rec_ref_cost.set( 1.03e+008, "rec_ref_cost", DATATYPE::TYPE_NUMBER, false );
	rec_ref_area.set( 1571., "rec_ref_area", DATATYPE::TYPE_NUMBER, false );
	tes_spec_cost.set( 24., "tes_spec_cost", DATATYPE::TYPE_NUMBER, false );
	tower_fixed_cost.set( 3.e6, "tower_fixed_cost", DATATYPE::TYPE_NUMBER, false );
	tower_exp.set( 0.0113, "tower_exp", DATATYPE::TYPE_NUMBER, false );
	heliostat_spec_cost.set( 145., "heliostat_spec_cost", DATATYPE::TYPE_NUMBER, false );
	site_spec_cost.set( 16., "site_spec_cost", DATATYPE::TYPE_NUMBER, false );
	land_spec_cost.set( 10000, "land_spec_cost", DATATYPE::TYPE_NUMBER, false );
	c_cps0.set( 0., "c_cps0", DATATYPE::TYPE_NUMBER, false );
	c_cps1.set( 1440., "c_cps1", DATATYPE::TYPE_NUMBER, false );
	om_staff_cost.set( 75, "om_staff_cost", DATATYPE::TYPE_NUMBER, false );
	wash_crew_cost.set( 65. + 10. + 25, "wash_crew_cost", DATATYPE::TYPE_NUMBER, false );
	heliostat_refurbish_cost.set( 144. * 25 + 90 * 4., "heliostat_refurbish_cost", DATATYPE::TYPE_NUMBER, false );
	helio_mtf.set( 12000, "helio_mtf", DATATYPE::TYPE_NUMBER, false );
	heliostat_repair_cost.set( 300, "heliostat_repair_cost", DATATYPE::TYPE_NUMBER, false );
	om_staff_max_hours_week.set( 35, "om_staff_max_hours_week", DATATYPE::TYPE_NUMBER, false );
	n_heliostats_sim.set( 1000, "n_heliostats_sim", DATATYPE::TYPE_NUMBER, false );
	wash_units_per_hour.set( 45., "wash_units_per_hour", DATATYPE::TYPE_NUMBER, false );
	wash_crew_max_hours_week.set( 70., "wash_crew_max_hours_week", DATATYPE::TYPE_NUMBER, false );
	degr_per_hour.set( 1.e-7, "degr_per_hour", DATATYPE::TYPE_NUMBER, false );
	degr_accel_per_year.set( 0.125, "degr_accel_per_year", DATATYPE::TYPE_NUMBER, false );
	degr_seed.set( 123, "degr_seed", DATATYPE::TYPE_NUMBER, false );
	soil_per_hour.set( 6.e-4, "soil_per_hour", DATATYPE::TYPE_NUMBER, false );
	adjust_constant.set( 4, "adjust:constant", DATATYPE::TYPE_NUMBER, false );
	helio_reflectance.set( 0.95, "helio_reflectance", DATATYPE::TYPE_NUMBER, false );
	disp_rsu_cost.set( 950., "disp_rsu_cost", DATATYPE::TYPE_NUMBER, false );
	disp_csu_cost.set( 10000., "disp_csu_cost", DATATYPE::TYPE_NUMBER, false );
	disp_pen_delta_w.set( 0.1, "disp_pen_delta_w", DATATYPE::TYPE_NUMBER, false );
	rec_su_delay.set( 0.2, "rec_su_delay", DATATYPE::TYPE_NUMBER, false );
	rec_qf_delay.set( 0.25, "rec_qf_delay", DATATYPE::TYPE_NUMBER, false );
	startup_time.set( 0.5, "startup_time", DATATYPE::TYPE_NUMBER, false );
	startup_frac.set( 0.5, "startup_frac", DATATYPE::TYPE_NUMBER, false );
	v_wind_max.set( 15., "v_wind_max", DATATYPE::TYPE_NUMBER, false );

	std::vector< double > pval = { 0., 7., 200., 12000. };
	c_ces.set( pval, "c_ces", DATATYPE::TYPE_NUMBER, false );

	std::vector< double > pvalts(8760, 1.);
	dispatch_factors_ts.set( pvalts, "dispatch_factors_ts", DATATYPE::TYPE_NUMBER, false );

}

Project::Project()
{
	m_ssc_simdata = 0;
	m_ssc_findata = 0;
	m_ssc_condata = 0;
}

Project::~Project()
{
	if (m_ssc_simdata)
		ssc_data_free(m_ssc_simdata);

	if (m_ssc_findata)
		ssc_data_free(m_ssc_findata);

	if (m_ssc_condata)
		ssc_data_free(m_ssc_condata);

}

void Project::hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars)
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


void Project::update_sscdata_from_current()
{
	/* 
	make sure the ssc data objects are in sync with the variable and parameter project settings
	*/

	//collect variables and parameters into single iterable object
	std::vector< data_base* > varpar;
	for (size_t i = 0; i < m_variables.GetMemberPointer()->size(); i++)
		varpar.push_back(m_variables.GetMemberPointer()->at(i));
	for (size_t i = 0; i < m_parameters.GetMemberPointer()->size(); i++)
		varpar.push_back(m_parameters.GetMemberPointer()->at(i));

	//variables first
	for( std::vector< data_base *>::iterator it = varpar.begin(); it != varpar.end(); it++ )
	{
		switch((*it)->type)
		{
			case DATATYPE::TYPE_BOOL:
			{
				data_unit< bool > *v = static_cast< data_unit< bool >* >(*it);
				ssc_data_set_number(m_ssc_simdata, v->name.c_str(), v->val ? 1. : 0.);
				break;
			}
			case DATATYPE::TYPE_INT:
			{
				data_unit< int > *v = static_cast< data_unit< int >* >( *it );
				ssc_data_set_number(m_ssc_simdata, v->name.c_str(), v->val);
				break;
			}
			case DATATYPE::TYPE_NUMBER:
			{
				data_unit< double > *v = static_cast< data_unit< double >* >( *it );
				ssc_data_set_number(m_ssc_simdata, v->name.c_str(), v->val);
				break;
			}
			case DATATYPE::TYPE_MATRIX:
			{
				data_unit< std::vector< std::vector< double > > > *v = static_cast< data_unit< std::vector< std::vector< double > > >* >( *it );
				int nr = (int)v->val.size();
				int nc = (int)v->val.front().size();

				ssc_number_t *p_vals = new ssc_number_t[nr*nc];
				for(int i=0; i<nr; i++)
					for(int j=0; j<nc; j++)
						p_vals[i*nc + j] = v->val.at(i).at(j);

				ssc_data_set_matrix(m_ssc_simdata, v->name.c_str(), p_vals, nr, nc);
				break;
			}
			case DATATYPE::TYPE_STRING:
			{
				data_unit< std::string > *v = static_cast< data_unit< std::string >* >( *it );
				ssc_data_set_string(m_ssc_simdata, v->name.c_str(), v->val.c_str());
				break;
			}
			case DATATYPE::TYPE_VECTOR:
			{
				data_unit< std::vector< double > > *v = static_cast< data_unit< std::vector< double > >* >( *it );
				int nr = (int)v->val.size();

				ssc_number_t *p_vals = new ssc_number_t[nr];
				for(int i=0; i<nr; i++)
						p_vals[i] = v->val.at(i);

				ssc_data_set_array(m_ssc_simdata, v->name.c_str(), p_vals, nr);
				break;
			}
			default:
			break;
		}
	}


}








void Project::initialize_ssc_project()
{
	//free data structures if already allocated
	if (m_ssc_simdata)
		ssc_data_free(m_ssc_simdata);
	if (m_ssc_findata)
		ssc_data_free(m_ssc_findata);
	if (m_ssc_condata)
		ssc_data_free(m_ssc_condata);

	//create a new data structures
	m_ssc_simdata = ssc_data_create();
	m_ssc_findata = ssc_data_create();
	m_ssc_condata = ssc_data_create();

	//set project initial and default values

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//       MSPT parameters and defaults
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	ssc_data_set_number( m_ssc_simdata, "is_stochastic_disp", 0 );
	ssc_data_set_number( m_ssc_simdata, "q_rec_standby",  9e9 );
	ssc_data_set_number( m_ssc_simdata, "eta_map_aod_format",  0 );
	ssc_data_set_number( m_ssc_simdata, "eta_map_aod_format",  0 );
	ssc_data_set_number( m_ssc_simdata, "ppa_multiplier_model",  0 );
	ssc_data_set_number( m_ssc_simdata, "field_model_type",  2 );
	ssc_data_set_number( m_ssc_simdata, "gross_net_conversion_factor",  0.90 );
	ssc_data_set_number( m_ssc_simdata, "helio_width",  12.2 );
	ssc_data_set_number( m_ssc_simdata, "helio_height",  12.2 );
	ssc_data_set_number( m_ssc_simdata, "helio_optical_error_mrad",  1.523 );
	ssc_data_set_number( m_ssc_simdata, "helio_active_fraction",  0.99 );
	ssc_data_set_number( m_ssc_simdata, "dens_mirror",  0.97 );
	ssc_data_set_number( m_ssc_simdata, "helio_reflectance",  0.9 );
	ssc_data_set_number( m_ssc_simdata, "rec_absorptance",  0.94 );
	ssc_data_set_number( m_ssc_simdata, "rec_hl_perm2",  30 );
	ssc_data_set_number( m_ssc_simdata, "land_max",  9.5 );
	ssc_data_set_number( m_ssc_simdata, "land_min",  0.75 );
	ssc_data_set_number( m_ssc_simdata, "dni_des",  950 );
	ssc_data_set_number( m_ssc_simdata, "p_start",  0.025 );
	ssc_data_set_number( m_ssc_simdata, "p_track",  0.055 );
	ssc_data_set_number( m_ssc_simdata, "hel_stow_deploy",  8 );
	ssc_data_set_number( m_ssc_simdata, "v_wind_max",  15 );
	ssc_data_set_number( m_ssc_simdata, "c_atm_0",  0.006789 );
	ssc_data_set_number( m_ssc_simdata, "c_atm_1",  0.1046 );
	ssc_data_set_number( m_ssc_simdata, "c_atm_2",  -0.017 );
	ssc_data_set_number( m_ssc_simdata, "c_atm_3",  0.002845 );
	ssc_data_set_number( m_ssc_simdata, "n_facet_x",  2 );
	ssc_data_set_number( m_ssc_simdata, "n_facet_y",  8 );
	ssc_data_set_number( m_ssc_simdata, "focus_type",  1 );
	ssc_data_set_number( m_ssc_simdata, "cant_type",  1 );
	ssc_data_set_number( m_ssc_simdata, "n_flux_days",  8 );
	ssc_data_set_number( m_ssc_simdata, "delta_flux_hrs",  2 );
	ssc_data_set_number( m_ssc_simdata, "water_usage_per_wash",  0.7 );
	ssc_data_set_number( m_ssc_simdata, "washing_frequency",  63 );
	ssc_data_set_number( m_ssc_simdata, "check_max_flux",  0 );
	ssc_data_set_number( m_ssc_simdata, "sf_excess",  1 );
	ssc_data_set_number( m_ssc_simdata, "tower_fixed_cost",  3000000 );
	ssc_data_set_number( m_ssc_simdata, "tower_exp",  0.0113 );
	ssc_data_set_number( m_ssc_simdata, "rec_ref_cost",  103000000 );
	ssc_data_set_number( m_ssc_simdata, "rec_ref_area",  1571 );
	ssc_data_set_number( m_ssc_simdata, "rec_cost_exp",  0.7 );
	ssc_data_set_number( m_ssc_simdata, "site_spec_cost",  16 );
	ssc_data_set_number( m_ssc_simdata, "heliostat_spec_cost",  145 );
	ssc_data_set_number( m_ssc_simdata, "plant_spec_cost",  1100 );
	ssc_data_set_number( m_ssc_simdata, "bop_spec_cost",  340 );
	ssc_data_set_number( m_ssc_simdata, "tes_spec_cost",  24 );
	ssc_data_set_number( m_ssc_simdata, "land_spec_cost",  10000 );
	ssc_data_set_number( m_ssc_simdata, "contingency_rate",  7 );
	ssc_data_set_number( m_ssc_simdata, "sales_tax_rate",  5 );
	ssc_data_set_number( m_ssc_simdata, "sales_tax_frac",  80 );
	ssc_data_set_number( m_ssc_simdata, "cost_sf_fixed",  0 );
	ssc_data_set_number( m_ssc_simdata, "fossil_spec_cost",  0 );
	ssc_data_set_number( m_ssc_simdata, "is_optimize",  0 );
	ssc_data_set_number( m_ssc_simdata, "opt_init_step",  0.06 );
	ssc_data_set_number( m_ssc_simdata, "opt_max_iter",  200 );
	ssc_data_set_number( m_ssc_simdata, "opt_conv_tol",  0.001 );
	ssc_data_set_number( m_ssc_simdata, "opt_flux_penalty",  0.25 );
	ssc_data_set_number( m_ssc_simdata, "opt_algorithm",  1 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.cost.epc.per_acre",  0 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.cost.epc.percent",  13 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.cost.epc.per_watt",  0 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.cost.epc.fixed",  0 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.cost.plm.percent",  0 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.cost.plm.per_watt",  0 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.cost.plm.fixed",  0 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.sf.fixed_land_area",  45 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.sf.land_overhead_factor",  1 );
	ssc_data_set_number( m_ssc_simdata, "T_htf_cold_des",  290 );
	ssc_data_set_number( m_ssc_simdata, "T_htf_hot_des",  574 );
	ssc_data_set_number( m_ssc_simdata, "P_ref",  115 );
	ssc_data_set_number( m_ssc_simdata, "design_eff",  0.412 );
	ssc_data_set_number( m_ssc_simdata, "tshours",  10 );
	ssc_data_set_number( m_ssc_simdata, "solarm",  2.4 );
	ssc_data_set_number( m_ssc_simdata, "receiver_type",  0 );
	ssc_data_set_number( m_ssc_simdata, "N_panels",  20 );
	ssc_data_set_number( m_ssc_simdata, "d_tube_out",  40 );
	ssc_data_set_number( m_ssc_simdata, "th_tube",  1.25 );
	ssc_data_set_number( m_ssc_simdata, "mat_tube",  2 );
	ssc_data_set_number( m_ssc_simdata, "rec_htf",  17 );
	ssc_data_set_number( m_ssc_simdata, "Flow_type",  1 );
	ssc_data_set_number( m_ssc_simdata, "epsilon",  0.88 );
	ssc_data_set_number( m_ssc_simdata, "hl_ffact",  1 );
	ssc_data_set_number( m_ssc_simdata, "f_rec_min",  0.25 );
	ssc_data_set_number( m_ssc_simdata, "rec_su_delay",  0.2 );
	ssc_data_set_number( m_ssc_simdata, "rec_qf_delay",  0.25 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.rec.max_oper_frac",  1.2 );
	ssc_data_set_number( m_ssc_simdata, "eta_pump",  0.85 );
	ssc_data_set_number( m_ssc_simdata, "piping_loss",  10200 );
	ssc_data_set_number( m_ssc_simdata, "piping_length_mult",  2.6 );
	ssc_data_set_number( m_ssc_simdata, "piping_length_const",  0 );
	ssc_data_set_number( m_ssc_simdata, "rec_d_spec",  15 );
	ssc_data_set_number( m_ssc_simdata, "csp.pt.tes.init_hot_htf_percent",  30 );
	ssc_data_set_number( m_ssc_simdata, "h_tank",  20 );
	ssc_data_set_number( m_ssc_simdata, "cold_tank_max_heat",  15 );
	ssc_data_set_number( m_ssc_simdata, "u_tank",  0.4 );
	ssc_data_set_number( m_ssc_simdata, "tank_pairs",  1 );
	ssc_data_set_number( m_ssc_simdata, "cold_tank_Thtr",  280 );
	ssc_data_set_number( m_ssc_simdata, "h_tank_min",  1 );
	ssc_data_set_number( m_ssc_simdata, "hot_tank_Thtr",  500 );
	ssc_data_set_number( m_ssc_simdata, "hot_tank_max_heat",  30 );
	ssc_data_set_number( m_ssc_simdata, "tc_fill",  8 );
	ssc_data_set_number( m_ssc_simdata, "tc_void",  0.25 );
	ssc_data_set_number( m_ssc_simdata, "t_dis_out_min",  500 );
	ssc_data_set_number( m_ssc_simdata, "t_ch_out_max",  400 );
	ssc_data_set_number( m_ssc_simdata, "nodes",  100 );
	ssc_data_set_number( m_ssc_simdata, "pc_config",  0 );
	ssc_data_set_number( m_ssc_simdata, "pb_pump_coef",  0.55 );
	ssc_data_set_number( m_ssc_simdata, "startup_time",  0.5 );
	ssc_data_set_number( m_ssc_simdata, "startup_frac",  0.5 );
	ssc_data_set_number( m_ssc_simdata, "cycle_max_frac",  1.05 );
	ssc_data_set_number( m_ssc_simdata, "cycle_cutoff_frac",  0.2 );
	ssc_data_set_number( m_ssc_simdata, "q_sby_frac",  0.2 );
	ssc_data_set_number( m_ssc_simdata, "dT_cw_ref",  10 );
	ssc_data_set_number( m_ssc_simdata, "T_amb_des",  42 );
	ssc_data_set_number( m_ssc_simdata, "P_boil",  100 );
	ssc_data_set_number( m_ssc_simdata, "CT",  2 );
	ssc_data_set_number( m_ssc_simdata, "T_approach",  5 );
	ssc_data_set_number( m_ssc_simdata, "T_ITD_des",  16 );
	ssc_data_set_number( m_ssc_simdata, "P_cond_ratio",  1.0028 );
	ssc_data_set_number( m_ssc_simdata, "pb_bd_frac",  0.02 );
	ssc_data_set_number( m_ssc_simdata, "P_cond_min",  2 );
	ssc_data_set_number( m_ssc_simdata, "n_pl_inc",  8 );
	ssc_data_set_number( m_ssc_simdata, "tech_type",  1 );
	ssc_data_set_number( m_ssc_simdata, "time_start",  0 );
	ssc_data_set_number( m_ssc_simdata, "time_stop",  31536000 );
	ssc_data_set_number( m_ssc_simdata, "pb_fixed_par",  0.0055 );
	ssc_data_set_number( m_ssc_simdata, "aux_par",  0.023 );
	ssc_data_set_number( m_ssc_simdata, "aux_par_f",  1 );
	ssc_data_set_number( m_ssc_simdata, "aux_par_0",  0.483 );
	ssc_data_set_number( m_ssc_simdata, "aux_par_1",  0.571 );
	ssc_data_set_number( m_ssc_simdata, "aux_par_2",  0 );
	ssc_data_set_number( m_ssc_simdata, "bop_par",  0 );
	ssc_data_set_number( m_ssc_simdata, "bop_par_f",  1 );
	ssc_data_set_number( m_ssc_simdata, "bop_par_0",  0 );
	ssc_data_set_number( m_ssc_simdata, "bop_par_1",  0.483 );
	ssc_data_set_number( m_ssc_simdata, "bop_par_2",  0 );
	ssc_data_set_number( m_ssc_simdata, "is_dispatch",  0 );
	ssc_data_set_number( m_ssc_simdata, "disp_horizon",  48 );
	ssc_data_set_number( m_ssc_simdata, "disp_frequency",  24 );
	ssc_data_set_number( m_ssc_simdata, "disp_max_iter",  35000 );
	ssc_data_set_number( m_ssc_simdata, "disp_timeout",  5 );
	ssc_data_set_number( m_ssc_simdata, "disp_mip_gap",  0.001 );
	ssc_data_set_number( m_ssc_simdata, "disp_time_weighting",  0.99 );
	ssc_data_set_number( m_ssc_simdata, "disp_rsu_cost",  950 );
	ssc_data_set_number( m_ssc_simdata, "disp_csu_cost",  10000 );
	ssc_data_set_number( m_ssc_simdata, "disp_pen_delta_w",  0.1 );
	ssc_data_set_number( m_ssc_simdata, "is_wlim_series",  0 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor1",  2.064 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor2",  1.2 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor3",  1 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor4",  1.1 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor5",  0.8 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor6",  0.7 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor7",  1 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor8",  1 );
	ssc_data_set_number( m_ssc_simdata, "dispatch_factor9",  1 );
	ssc_data_set_number( m_ssc_simdata, "is_dispatch_series",  0 );
	ssc_data_set_number( m_ssc_simdata, "rec_height",  21.6 );
	ssc_data_set_number( m_ssc_simdata, "D_rec",  17.65 );
	ssc_data_set_number( m_ssc_simdata, "h_tower",  193.46 );
	ssc_data_set_number( m_ssc_simdata, "land_area_base",  1847.037 );
	ssc_data_set_number( m_ssc_simdata, "const_per_interest_rate1",  4 );
	ssc_data_set_number( m_ssc_simdata, "const_per_interest_rate2",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_interest_rate3",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_interest_rate4",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_interest_rate5",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_months1",  24 );
	ssc_data_set_number( m_ssc_simdata, "const_per_months2",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_months3",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_months4",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_months5",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_percent1",  100 );
	ssc_data_set_number( m_ssc_simdata, "const_per_percent2",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_percent3",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_percent4",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_percent5",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_upfront_rate1",  1 );
	ssc_data_set_number( m_ssc_simdata, "const_per_upfront_rate2",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_upfront_rate3",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_upfront_rate4",  0 );
	ssc_data_set_number( m_ssc_simdata, "const_per_upfront_rate5",  0 );
	ssc_data_set_number( m_ssc_simdata, "adjust:constant",  4 );
	ssc_data_set_number( m_ssc_simdata, "sf_adjust:constant",  0 );
	ssc_data_set_number( m_ssc_simdata, "ud_T_amb_des",  43 );
	ssc_data_set_number( m_ssc_simdata, "ud_f_W_dot_cool_des",  0 );
	ssc_data_set_number( m_ssc_simdata, "ud_m_dot_water_cool_des",  0 );
	ssc_data_set_number( m_ssc_simdata, "ud_T_htf_low",  500 );
	ssc_data_set_number( m_ssc_simdata, "ud_T_htf_high",  580 );
	ssc_data_set_number( m_ssc_simdata, "ud_T_amb_low",  0 );
	ssc_data_set_number( m_ssc_simdata, "ud_T_amb_high",  55 );
	ssc_data_set_matrix(m_ssc_simdata, "fc_dni_scenarios", 0, 0, 0 );
	ssc_data_set_matrix(m_ssc_simdata, "fc_price_scenarios", 0, 0, 0 );
	ssc_data_set_matrix(m_ssc_simdata, "fc_tamb_scenarios", 0, 0, 0 );
	ssc_number_t p_F_wc[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ssc_data_set_array(m_ssc_simdata, "F_wc", p_F_wc, 9 );
	ssc_number_t p_field_fl_props[9] = { 1, 7, 0, 0, 0, 0, 0, 0, 0 };
	ssc_data_set_matrix(m_ssc_simdata, "field_fl_props", p_field_fl_props, 1, 9 );
	ssc_number_t p_f_turb_tou_periods[9] = { 1.05, 1, 1, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_array(m_ssc_simdata, "f_turb_tou_periods", p_f_turb_tou_periods, 9 );
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
	ssc_data_set_matrix(m_ssc_simdata, "weekday_schedule", p_weekday_schedule, 12, 24 );
	ssc_data_set_matrix(m_ssc_simdata, "weekend_schedule", p_weekday_schedule, 12, 24 );
	ssc_number_t p_dispatch_series[1] = { 0 };
	ssc_data_set_array(m_ssc_simdata, "dispatch_series", p_dispatch_series, 1 );
	ssc_data_set_number(m_ssc_simdata, "sf_adjust:constant", 0 );
	ssc_number_t p_sf_adjustperiods[1] = { 0 };
	ssc_data_set_array(m_ssc_simdata, "sf_adjust:periods", p_sf_adjustperiods, 1 );
	ssc_number_t p_sf_adjusthourly[8760];
	for (size_t ii = 0; ii < 8760; ii++)
		p_sf_adjusthourly[ii] = 0.;
	ssc_data_set_array(m_ssc_simdata, "sf_adjust:hourly", p_sf_adjusthourly, 8760 );
	ssc_number_t p_dispatch_factors_ts[8760];
	for (size_t ii = 0; ii < 8760; ii++)
		p_dispatch_factors_ts[ii] = 1.;
	ssc_data_set_array(m_ssc_simdata, "dispatch_factors_ts", p_dispatch_factors_ts, 8760 );
	ssc_number_t p_wlim_series[8760];
	for (size_t ii = 0; ii < 8760; ii++)
		p_wlim_series[ii] = 1.e37;
	ssc_data_set_array(m_ssc_simdata, "wlim_series", p_wlim_series, 8760 );
	ssc_number_t p_helio_positions[1] = { 0 };
	ssc_data_set_array(m_ssc_simdata, "helio_positions", p_helio_positions, 1 );


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//		       Financial parameters and defaults
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------

	ssc_data_set_number(m_ssc_findata, "analysis_period", 25 );
	ssc_number_t p_federal_tax_rate[1] = { 35 };
	ssc_data_set_array(m_ssc_findata, "federal_tax_rate", p_federal_tax_rate, 1 );
	ssc_number_t p_state_tax_rate[1] = { 7 };
	ssc_data_set_array(m_ssc_findata, "state_tax_rate", p_state_tax_rate, 1 );
	ssc_data_set_number(m_ssc_findata, "property_tax_rate", 0 );
	ssc_data_set_number(m_ssc_findata, "prop_tax_cost_assessed_percent", 100 );
	ssc_data_set_number(m_ssc_findata, "prop_tax_assessed_decline", 0 );
	ssc_data_set_number(m_ssc_findata, "real_discount_rate", 5.5 );
	ssc_data_set_number(m_ssc_findata, "inflation_rate", 2.5 );
	ssc_data_set_number(m_ssc_findata, "insurance_rate", 0.5 );
	ssc_data_set_number(m_ssc_findata, "system_capacity", 103500 );
	ssc_number_t p_om_fixed[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "om_fixed", p_om_fixed, 1 );
	ssc_data_set_number(m_ssc_findata, "om_fixed_escal", 0 );
	ssc_number_t p_om_production[1] = { 3.5 };
	ssc_data_set_array(m_ssc_findata, "om_production", p_om_production, 1 );
	ssc_data_set_number(m_ssc_findata, "om_production_escal", 0 );
	ssc_number_t p_om_capacity[1] = { 66 };
	ssc_data_set_array(m_ssc_findata, "om_capacity", p_om_capacity, 1 );
	ssc_data_set_number(m_ssc_findata, "om_capacity_escal", 0 );
	ssc_number_t p_om_fuel_cost[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "om_fuel_cost", p_om_fuel_cost, 1 );
	ssc_data_set_number(m_ssc_findata, "om_fuel_cost_escal", 0 );
	ssc_data_set_number(m_ssc_findata, "itc_fed_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "itc_fed_amount_deprbas_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "itc_fed_amount_deprbas_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "itc_sta_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "itc_sta_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "itc_sta_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "itc_fed_percent", 30 );
	ssc_data_set_number(m_ssc_findata, "itc_fed_percent_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "itc_fed_percent_deprbas_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "itc_fed_percent_deprbas_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "itc_sta_percent", 0 );
	ssc_data_set_number(m_ssc_findata, "itc_sta_percent_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "itc_sta_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "itc_sta_percent_deprbas_sta", 0 );
	ssc_number_t p_ptc_fed_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "ptc_fed_amount", p_ptc_fed_amount, 1 );
	ssc_data_set_number(m_ssc_findata, "ptc_fed_term", 10 );
	ssc_data_set_number(m_ssc_findata, "ptc_fed_escal", 0 );
	ssc_number_t p_ptc_sta_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "ptc_sta_amount", p_ptc_sta_amount, 1 );
	ssc_data_set_number(m_ssc_findata, "ptc_sta_term", 10 );
	ssc_data_set_number(m_ssc_findata, "ptc_sta_escal", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_amount_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_amount_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_amount_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_amount_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_percent", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_percent_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_fed_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_percent", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_percent_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_sta_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_percent", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_percent_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_uti_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_percent", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_percent_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_percent_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_percent_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_percent_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "ibi_oth_percent_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_fed_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_fed_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "cbi_fed_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_fed_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_fed_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_fed_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_sta_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_sta_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "cbi_sta_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_sta_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_sta_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_sta_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_uti_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_uti_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "cbi_uti_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_uti_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_uti_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_uti_deprbas_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_oth_amount", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_oth_maxvalue", 1e38 );
	ssc_data_set_number(m_ssc_findata, "cbi_oth_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_oth_tax_sta", 1 );
	ssc_data_set_number(m_ssc_findata, "cbi_oth_deprbas_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "cbi_oth_deprbas_sta", 0 );
	ssc_number_t p_pbi_fed_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "pbi_fed_amount", p_pbi_fed_amount, 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_fed_term", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_fed_escal", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_fed_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_fed_tax_sta", 1 );
	ssc_number_t p_pbi_sta_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "pbi_sta_amount", p_pbi_sta_amount, 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_sta_term", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_sta_escal", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_sta_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_sta_tax_sta", 1 );
	ssc_number_t p_pbi_uti_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "pbi_uti_amount", p_pbi_uti_amount, 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_uti_term", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_uti_escal", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_uti_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_uti_tax_sta", 1 );
	ssc_number_t p_pbi_oth_amount[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "pbi_oth_amount", p_pbi_oth_amount, 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_oth_term", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_oth_escal", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_oth_tax_fed", 1 );
	ssc_data_set_number(m_ssc_findata, "pbi_oth_tax_sta", 1 );
	ssc_number_t p_degradation[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "degradation", p_degradation, 1 );
	ssc_number_t p_roe_input[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "roe_input", p_roe_input, 1 );
	ssc_data_set_number(m_ssc_findata, "loan_moratorium", 0 );
	ssc_data_set_number(m_ssc_findata, "system_use_recapitalization", 0 );
	ssc_data_set_number(m_ssc_findata, "system_use_lifetime_output", 0 );
	ssc_data_set_number(m_ssc_findata, "total_installed_cost", 704234496 );
	ssc_data_set_number(m_ssc_findata, "reserves_interest", 1.75 );
	ssc_data_set_number(m_ssc_findata, "equip1_reserve_cost", 0 );
	ssc_data_set_number(m_ssc_findata, "equip1_reserve_freq", 12 );
	ssc_data_set_number(m_ssc_findata, "equip2_reserve_cost", 0 );
	ssc_data_set_number(m_ssc_findata, "equip2_reserve_freq", 15 );
	ssc_data_set_number(m_ssc_findata, "equip3_reserve_cost", 0 );
	ssc_data_set_number(m_ssc_findata, "equip3_reserve_freq", 3 );
	ssc_data_set_number(m_ssc_findata, "equip_reserve_depr_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "equip_reserve_depr_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "salvage_percentage", 0 );
	ssc_data_set_number(m_ssc_findata, "ppa_soln_mode", 0 );
	ssc_data_set_number(m_ssc_findata, "ppa_price_input", 0.12999999523162842 );
	ssc_data_set_number(m_ssc_findata, "ppa_escalation", 1 );
	ssc_data_set_number(m_ssc_findata, "construction_financing_cost", 35211724 );
	ssc_data_set_number(m_ssc_findata, "term_tenor", 18 );
	ssc_data_set_number(m_ssc_findata, "term_int_rate", 7 );
	ssc_data_set_number(m_ssc_findata, "dscr", 1.2999999523162842 );
	ssc_data_set_number(m_ssc_findata, "dscr_reserve_months", 6 );
	ssc_data_set_number(m_ssc_findata, "debt_percent", 50 );
	ssc_data_set_number(m_ssc_findata, "debt_option", 1 );
	ssc_data_set_number(m_ssc_findata, "payment_option", 0 );
	ssc_data_set_number(m_ssc_findata, "cost_debt_closing", 450000 );
	ssc_data_set_number(m_ssc_findata, "cost_debt_fee", 2.75 );
	ssc_data_set_number(m_ssc_findata, "months_working_reserve", 6 );
	ssc_data_set_number(m_ssc_findata, "months_receivables_reserve", 0 );
	ssc_data_set_number(m_ssc_findata, "cost_other_financing", 0 );
	ssc_data_set_number(m_ssc_findata, "flip_target_percent", 11 );
	ssc_data_set_number(m_ssc_findata, "flip_target_year", 20 );
	ssc_data_set_number(m_ssc_findata, "depr_alloc_macrs_5_percent", 90 );
	ssc_data_set_number(m_ssc_findata, "depr_alloc_macrs_15_percent", 1.5 );
	ssc_data_set_number(m_ssc_findata, "depr_alloc_sl_5_percent", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_alloc_sl_15_percent", 2.5 );
	ssc_data_set_number(m_ssc_findata, "depr_alloc_sl_20_percent", 3 );
	ssc_data_set_number(m_ssc_findata, "depr_alloc_sl_39_percent", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_alloc_custom_percent", 0 );
	ssc_number_t p_depr_custom_schedule[1] = { 0 };
	ssc_data_set_array(m_ssc_findata, "depr_custom_schedule", p_depr_custom_schedule, 1 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta_macrs_5", 1 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta_macrs_15", 1 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta_sl_5", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta_sl_15", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta_sl_20", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta_sl_39", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_sta_custom", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed_macrs_5", 1 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed_macrs_15", 1 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed_sl_5", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed_sl_15", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed_sl_20", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed_sl_39", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_bonus_fed_custom", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_sta_macrs_5", 1 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_sta_macrs_15", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_sta_sl_5", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_sta_sl_15", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_sta_sl_20", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_sta_sl_39", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_sta_custom", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_fed_macrs_5", 1 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_fed_macrs_15", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_fed_sl_5", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_fed_sl_15", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_fed_sl_20", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_fed_sl_39", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_itc_fed_custom", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_fed_for_ds", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_sta_for_ds", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_uti_for_ds", 0 );
	ssc_data_set_number(m_ssc_findata, "pbi_oth_for_ds", 0 );
	ssc_data_set_number(m_ssc_findata, "depr_stabas_method", 1 );
	ssc_data_set_number(m_ssc_findata, "depr_fedbas_method", 1 );

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//		       Construction financing parameters and defaults
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	ssc_data_set_number(m_ssc_condata, "total_installed_cost", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_interest_rate1", 4. );
	ssc_data_set_number(m_ssc_condata, "const_per_interest_rate2", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_interest_rate3", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_interest_rate4", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_interest_rate5", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_months1", 24 );
	ssc_data_set_number(m_ssc_condata, "const_per_months2", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_months3", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_months4", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_months5", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_percent1", 100. );
	ssc_data_set_number(m_ssc_condata, "const_per_percent2", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_percent3", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_percent4", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_percent5", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_upfront_rate1", 1. );
	ssc_data_set_number(m_ssc_condata, "const_per_upfront_rate2", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_upfront_rate3", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_upfront_rate4", 0. );
	ssc_data_set_number(m_ssc_condata, "const_per_upfront_rate5", 0. );

}