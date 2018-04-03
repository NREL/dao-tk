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
	flux_max.set(1000., "flux_max", DATATYPE::TYPE_NUMBER, false);
	
	std::vector< double > pval = { 0., 7., 200., 12000. };
	c_ces.set( pval, "c_ces", DATATYPE::TYPE_NUMBER, false );

	std::vector< double > pvalts(8760, 1.);
	dispatch_factors_ts.set( pvalts, "dispatch_factors_ts", DATATYPE::TYPE_NUMBER, false );

}

design_outputs::design_outputs()
{
	/* 
	Set up output members
	*/

	double nan = std::numeric_limits<double>::quiet_NaN();
	number_heliostats.set(-1, "number_heliostats", DATATYPE::TYPE_INT, true);
	area_sf.set(nan, "area_sf", DATATYPE::TYPE_NUMBER, true);
	base_land_area.set(nan, "base_land_area", DATATYPE::TYPE_NUMBER, true);
	land_area.set(nan, "land_area", DATATYPE::TYPE_NUMBER, true);
	h_tower_opt.set(nan, "h_tower_opt", DATATYPE::TYPE_NUMBER, true);
	rec_height_opt.set(nan, "rec_height_opt", DATATYPE::TYPE_NUMBER, true);
	rec_aspect_opt.set(nan, "rec_aspect_opt", DATATYPE::TYPE_NUMBER, true);
	cost_rec_tot.set(nan, "cost_rec_tot", DATATYPE::TYPE_NUMBER, true);
	cost_sf_tot.set(nan, "cost_sf_tot", DATATYPE::TYPE_NUMBER, true);
	cost_sf_real.set(nan, "", DATATYPE::TYPE_NUMBER, true);
	cost_tower_tot.set(nan, "cost_tower_tot", DATATYPE::TYPE_NUMBER, true);
	cost_land_tot.set(nan, "cost_land_tot", DATATYPE::TYPE_NUMBER, true);
	cost_land_real.set(nan, "", DATATYPE::TYPE_NUMBER, true);
	cost_site_tot.set(nan, "cost_site_tot", DATATYPE::TYPE_NUMBER, true);
	flux_max_observed.set(nan, "flux_max_observed", DATATYPE::TYPE_NUMBER, true);

	std::vector< std::vector< double > > empty_mat;
	opteff_table.set(empty_mat, "opteff_table", DATATYPE::TYPE_MATRIX, true);
	flux_table.set(empty_mat, "flux_table", DATATYPE::TYPE_MATRIX, true);
	heliostat_positions.set(empty_mat, "heliostat_positions", DATATYPE::TYPE_MATRIX, true);
}

Project::Project()
{
	m_ssc_data = 0;
	initialize_ssc_project();
}

Project::~Project()
{
	if (m_ssc_data)
		ssc_data_free(m_ssc_data);
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


void Project::update_sscdata_from_object(datas_base &obj)
{
	sscdata_localdata_map(obj, true);
}

void Project::update_object_from_sscdata(datas_base &obj)
{
	sscdata_localdata_map(obj, false);
}

void Project::sscdata_localdata_map(datas_base &obj, bool set_ssc_from_local)  //set ssc data from obj (set_ssc_from_local=true), or set local from ssc (false)
{
	std::vector< data_base* > *members = obj.GetMemberPointer();

	for (std::vector< data_base *>::iterator it = members->begin(); it != members->end(); it++)
	{
		//if this is a 'get', make sure the data is allocated in the ssc object
		if (!set_ssc_from_local)
			if (ssc_data_query(m_ssc_data, (*it)->name.c_str()) == SSC_INVALID)
				continue;

		//handle the get/set operation by data type
		switch ((*it)->type)
		{
		case DATATYPE::TYPE_BOOL:
		{
			data_unit< bool > *v = static_cast< data_unit< bool >* >(*it);
			if (set_ssc_from_local)
				ssc_data_set_number(m_ssc_data, v->name.c_str(), v->val ? 1. : 0.);
			else
			{
				ssc_number_t tval;
				ssc_data_get_number(m_ssc_data, v->name.c_str(), &tval);
				v->val = tval == 1.;
			}

			break;
		}
		case DATATYPE::TYPE_INT:
		{
			data_unit< int > *v = static_cast< data_unit< int >* >(*it);
			if( set_ssc_from_local )
				ssc_data_set_number(m_ssc_data, v->name.c_str(), v->val);
			else
			{
				ssc_number_t tval;
				ssc_data_get_number(m_ssc_data, v->name.c_str(), &tval);
				v->val = (int)(tval + 1.e-3);
			}

			break;
		}
		case DATATYPE::TYPE_NUMBER:
		{
			data_unit< double > *v = static_cast< data_unit< double >* >(*it);
			if (set_ssc_from_local)
				ssc_data_set_number(m_ssc_data, v->name.c_str(), v->val);
			else
			{
				ssc_number_t tval;
				ssc_data_get_number(m_ssc_data, v->name.c_str(), &tval);
				v->val = (double)tval;
			}
			break;
		}
		case DATATYPE::TYPE_MATRIX:
		{
			data_unit< std::vector< std::vector< double > > > *v = static_cast< data_unit< std::vector< std::vector< double > > >* >(*it);

			if (set_ssc_from_local)
			{
				int nr = (int)v->val.size();
				int nc = (int)v->val.front().size();

				ssc_number_t *p_vals = new ssc_number_t[nr*nc];
				for (int i = 0; i<nr; i++)
					for (int j = 0; j<nc; j++)
						p_vals[i*nc + j] = v->val.at(i).at(j);

				ssc_data_set_matrix(m_ssc_data, v->name.c_str(), p_vals, nr, nc);
			}
			else
			{
				int nr, nc;
				ssc_number_t *p_vals = ssc_data_get_matrix(m_ssc_data, v->name.c_str(), &nr, &nc);
				v->val.resize(nr, std::vector<double>(nc));
				for (int i = 0; i < nr; i++)
					for (int j = 0; j < nc; j++)
						v->val.at(i).at(j) = p_vals[i*nc + j];
			}
			break;
		}
		case DATATYPE::TYPE_STRING:
		{
			data_unit< std::string > *v = static_cast< data_unit< std::string >* >(*it);
			if( set_ssc_from_local )
				ssc_data_set_string(m_ssc_data, v->name.c_str(), v->val.c_str());
			else
				v->val = ssc_data_get_string(m_ssc_data, v->name.c_str());
			
			break;
		}
		case DATATYPE::TYPE_VECTOR:
		{
			data_unit< std::vector< double > > *v = static_cast< data_unit< std::vector< double > >* >(*it);
			
			if (set_ssc_from_local)
			{
				int nr = (int)v->val.size();
				ssc_number_t *p_vals = new ssc_number_t[nr];
				for (int i = 0; i<nr; i++)
					p_vals[i] = v->val.at(i);

				ssc_data_set_array(m_ssc_data, v->name.c_str(), p_vals, nr);
			}
			else
			{
				int nr;
				ssc_number_t *p_vals = ssc_data_get_array(m_ssc_data, v->name.c_str(), &nr);
				v->val.resize(nr);
				for (int i = 0; i < nr; i++)
					v->val.at(i) = p_vals[i];
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
	ssc_number_t nameplate = m_variables.P_ref.val * gross_net_conversion_factor;
	ssc_data_set_number(m_ssc_data, "nameplate", nameplate);
	//
	//D["system_capacity"] = D["nameplate"] * 1000.
	ssc_data_set_number(m_ssc_data, "system_capacity", nameplate*1000.);

	//// q_pb_design(informational, not used as a compute module input for mspt)
	ssc_number_t q_pb_design = m_variables.P_ref.val * m_variables.design_eff.val;
	//D["q_pb_design"] = float(D["P_ref"]) / float(D["design_eff"])
	ssc_data_set_number(m_ssc_data, "q_pb_design", q_pb_design);

	//// Q_rec_des(only used as in input to solarpilot compute module)
	//D["Q_rec_des"] = D["solarm"] * D["q_pb_design"]
	ssc_data_set_number(m_ssc_data, "Q_rec_des", m_variables.solarm.val * q_pb_design);
	//D["q_design"] = D["Q_rec_des"]
	ssc_data_set_number(m_ssc_data, "q_design", m_variables.solarm.val * q_pb_design);
	
	//// tshours_sf(informational, not used as a compute module input)
	//D["tshours_sf"] = D["tshours"] / D["solarm"]

	////receiver aspect ratio(only used as in input to solarpilot compute module)
	//D["rec_aspect"] = float(D["rec_height"]) / float(D["D_rec"]);
	ssc_data_set_number(m_ssc_data, "rec_aspect", m_variables.rec_height.val / m_variables.D_rec.val);

	////always set to MSPT
	//D["tower_technology"] = 0
	ssc_data_set_number(m_ssc_data, "tower_technology", 0.);

	////Flux grid resolution limited by number of panels(only used as in input to solarpilot compute module)
	//D["n_flux_x"] = max(12, D["N_panels"])
	ssc_data_set_number(m_ssc_data, "n_flux_x", m_variables.N_panels.val > 12 ? m_variables.N_panels.val : 12);

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
	//D['csp.pt.par.calc.bop'] = \
	//D['bop_par'] * D['bop_par_f'] * (D['bop_par_0'] + D['bop_par_1'] + \
	//	D['bop_par_2'])*D['P_ref']
	//#Aux parasitic
	//D['csp.pt.par.calc.aux'] = \
	//D['aux_par'] * D['aux_par_f'] * (D['aux_par_0'] + D['aux_par_1'] + \
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
	//D['csp.pt.rec.max_flow_to_rec'] = \
	//(D['csp.pt.rec.max_oper_frac'] * D['Q_rec_des'] * 1e6) \
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

bool Project::run_design()
{

	/*
	run only the design
	
	usr_vars is a dict containing name : value pairs.The name must be a valid
	ssc variable from vartab.py, and corresponding value must match the var
	type given in the same file.
	*/
	
	ssc_module_exec_set_print(m_parameters.print_messages.val); //0 = no, 1 = yes(print progress updates)
	
	//change any defaults
	ssc_data_set_number(m_ssc_data, "calc_fluxmaps", 1.);
	
	//#Check to make sure the weather file exists
	FILE *fp = fopen(m_parameters.solar_resource_file.val.c_str(), "r");
	if (fp == NULL)
	{
		message_handler(wxString::Format("The solar resource file could not be located (Design module). The specified path is:\n%s", m_parameters.solar_resource_file.val.c_str()));
		return false;;
	}
	fclose(fp);

	update_calculated_system_values();
	ssc_data_set_matrix(m_ssc_data, "heliostat_positions_in", (ssc_number_t*)0, 0, 0);
	
	update_sscdata_from_object(m_variables);
	update_sscdata_from_object(m_parameters);

	//Run design to get field layout
	ssc_module_t mod_solarpilot = ssc_module_create("solarpilot");
	ssc_module_exec_with_handler(mod_solarpilot, m_ssc_data, ssc_progress_handler, 0);
	
	//Collect calculated data
	update_object_from_sscdata(m_design_outputs);
	
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
	
	return true;
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
	ssc_data_set_matrix(m_ssc_data, "weekday_schedule", p_weekday_schedule, 12, 24 );
	ssc_data_set_matrix(m_ssc_data, "weekend_schedule", p_weekday_schedule, 12, 24 );
	ssc_number_t p_dispatch_series[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "dispatch_series", p_dispatch_series, 1 );
	ssc_data_set_number(m_ssc_data, "sf_adjust:constant", 0 );
	ssc_number_t p_sf_adjustperiods[1] = { 0 };
	ssc_data_set_array(m_ssc_data, "sf_adjust:periods", p_sf_adjustperiods, 1 );
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

		for (int i = 0; i < m_parameters.plant_lifetime.val; i++)
		{
			cy *= r;
			c += cy;
		}

		return c;
	}
	else if (is_labor)
	{
		return dollars * m_parameters.plant_lifetime.val;
	}
	else
	{
		int analysis_period = m_parameters.finance_period.val;

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
	Run design
	*/
	if( ! run_design() )
		return false;

	//Land cost
	m_design_outputs.cost_land_real.val = calc_real_dollars( m_parameters.land_spec_cost.val * m_design_outputs.land_area.val ); 
	//solar field cost
	m_design_outputs.cost_sf_real.val = calc_real_dollars( (m_parameters.heliostat_spec_cost.val + m_parameters.site_spec_cost.val)*m_design_outputs.area_sf.val );

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

	m_solarfield_availability.m_settings.mf = m_parameters.helio_mtf.val;
	m_solarfield_availability.m_settings.rep_min = 1.;
	m_solarfield_availability.m_settings.rep_max = 100.;
	m_solarfield_availability.m_settings.n_helio = m_design_outputs.number_heliostats.val;
	m_solarfield_availability.m_settings.n_om_staff = m_variables.om_staff.val;
	m_solarfield_availability.m_settings.hr_prod = m_parameters.om_staff_max_hours_week.val;
	m_solarfield_availability.m_settings.n_hr_sim = 8760 * 12;
	m_solarfield_availability.m_settings.seed = m_parameters.avail_seed.val;
	m_solarfield_availability.m_settings.n_helio_sim = 1000;

	//error if trying to simulate with no heliostats
	if (m_design_outputs.number_heliostats.val <= 1)
	{
		message_handler("Error: Empty layout in field availability simulation.");
		return false;
	}

	m_solarfield_availability.simulate(sim_progress_handler);

	//Calculate staff cost and repair cost
	double ann_fact = 8760. / (double)m_solarfield_availability.m_settings.n_hr_sim;

	m_solarfield_availability.m_results.n_repairs *= ann_fact;	//annual repairs
	m_solarfield_availability.m_results.heliostat_repair_cost_y1 = m_solarfield_availability.m_results.n_repairs * m_parameters.heliostat_repair_cost.val;
	
	//lifetime costs
	//treat heliostat repair costs as consuming reserve equipment paid for at the project outset
	m_solarfield_availability.m_results.heliostat_repair_cost = calc_real_dollars(m_solarfield_availability.m_results.heliostat_repair_cost_y1);

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

	m_optical_degradation.m_settings.n_hr_sim = 25 * 8760;
	m_optical_degradation.m_settings.n_wash_crews = m_variables.n_wash_crews.val;
	m_optical_degradation.m_settings.n_helio = m_design_outputs.number_heliostats.val;
	m_optical_degradation.m_settings.degr_loss_per_hr = m_parameters.degr_per_hour.val;
	m_optical_degradation.m_settings.degr_accel_per_year = m_parameters.degr_accel_per_year.val;
	m_optical_degradation.m_settings.replacement_threshold = m_variables.degr_replace_limit.val;
	m_optical_degradation.m_settings.soil_loss_per_hr = m_parameters.soil_per_hour.val;
	m_optical_degradation.m_settings.wash_units_per_hour = m_parameters.wash_units_per_hour.val;
	m_optical_degradation.m_settings.hours_per_week = m_parameters.wash_crew_max_hours_week.val;
	m_optical_degradation.m_settings.hours_per_day = 10.;
	m_optical_degradation.m_settings.seed = m_parameters.degr_seed.val;

	m_optical_degradation.simulate();

	double ann_fact = 8760. / (double)m_optical_degradation.m_settings.n_hr_sim;
	
	m_optical_degradation.m_results.heliostat_refurbish_cost_y1 =
		m_optical_degradation.m_results.n_replacements * m_parameters.heliostat_refurbish_cost.val;
	
	//Annualize
	m_optical_degradation.m_results.n_replacements *= ann_fact;
	
	m_optical_degradation.m_results.heliostat_refurbish_cost = calc_real_dollars( m_optical_degradation.m_results.heliostat_refurbish_cost_y1 ) * ann_fact;

	return true;
}