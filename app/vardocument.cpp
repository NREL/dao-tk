#include "project.h"
#include <vector>
#include <string>

void Project::add_documentation()
{

    m_variables.h_tower.doc.set("m",
        "Height of the vertical centerline of the receiver above the average heliostat drive height");
    m_variables.rec_height.doc.set("m",
        "Height of the receiver flux absorbing surface");
    m_variables.D_rec.doc.set("m",
        "Diameter of the receiver flux absorbing surface");
    m_variables.design_eff.doc.set("-",
        "Design-point thermodynamic conversion efficiency of "
        "the power cycle at nominal ambient temperature and thermal load conditions");
    m_variables.dni_des.doc.set("W/m<sup>2</sup>",
        "The direct normal irradiance (DNI) at design-point conditions. "
        "This specifies the irradiance that corresponds with the design-point thermal production "
        "from the solar field.<br><it>Increased design-point DNI will result in a smaller number "
        "of heliostats in the solar field.</it>");
    m_variables.P_ref.doc.set("MW<sub>e</sub>",
        "Design-point gross electrical power output from the power block. Gross power "
        "accounts for feedwater pumps and parasitics within the power block excluding condenser fan operation.");
    m_variables.solarm.doc.set("-",
        "The solar multiple represents the ratio of the thermal power delivered by the solar field at "
        "design conditions to the thermal power delivered to the power block at design conditions."
        "<br><sl>The larger the solar multiple, the more energy will be aviailable for thermal storage.</sl>");
    m_variables.tshours.doc.set("hr",
        "The number of hours that the power block can operate at full load while drawing thermal power "
        "exclusively from the thermal storage system.");
    m_variables.degr_replace_limit.doc.set("-",
        "The lower limit for individual heliostat reflectivity after long-term degradation "
        "upon which the mirrors are replaced. The replacement occurs at the first time that "
        "the mirror wash crew encounters the heliostat in question after its reflectivity is "
        "below the limit. Upon replacement, a cost equal to the parameter 'heliostat_refurbish_cost' "
        "is incurred.");
    m_variables.om_staff.doc.set("-",
        "The number of staff (continuous) available for addressing operations and maintenance events, "
        "including:<br><ul><li>Heliostat failures</li><li>Cycle maintenance events</li></ul>");
    //m_variables.n_wash_crews.doc.set("-",
    //    "Number of crews available to wash heliostats. Each crew consists of a washing device "
    //    "and sufficient staff to operate it. Crews operate independently from one another.");
    m_variables.N_panel_pairs.doc.set("-",
        "Number of flux absorption panels on the receiver. A panel consists of a bank of tubes "
        "in parallel flow that share a single distribution header and collection header. The receiver "
        "\"cylinder\" is comprised of the assembly of panels.");

    m_parameters.print_messages.doc.set("-",
        "Flag indicating whether additional information will be printed during simulation.");
    m_parameters.check_max_flux.doc.set("-",
        "Flag indicating whether the observed maximum flux should be calculated using the "
        "detailed flux performance model after a field design has been created. This action "
        "is required when executing system optimization runs.");
    m_parameters.is_optimize.doc.set("-",
        "Flag indicating whether the built-in local optimizer should be used to identify optimal "
        "values of receiver diameter, receiver height, tower height, and heliostat positions. "
        "The optimizer uses the <a href=\"https://nlopt.readthedocs.io/en/latest/NLopt_Algorithms/#cobyla-constrained-optimization-by-linear-approximations\">COBYLA algorithm</a> "
        "from the <a href=\"https://nlopt.readthedocs.io/en/latest/\">NLopt library</a>, which "
        "identifies locally optimal variable values subject to a constraint on <a href=#doc_flux_max>"
        "maximum allowable receiver flux</a>.");
    m_parameters.is_dispatch.doc.set("-",
        "Flag indicating whether the dispatch optimiation algorithm will be used to determine the plant "
        "operations schedule. If <b>false</b>, the <sl>heuristic</sl> dispatch method is used, wherein "
        "energy is discharged from thermal storage to the power block when the energy content of the tank "
        "remains sufficient to operate the power block above at least minimum load.");
    m_parameters.is_ampl_engine.doc.set("-",
        "<font color=\"red\">(Expert mode)</font> Flag indicating whether the AMPL dispatch optimization "
        "engine should be used to determine the operating schedule. <sl>Note that an AMPL+CPLEX license "
        "is required for this option.</sl> Requires that <a href=\"#doc_is_dispatch\">dispatch optimization"
        "</a> is enabled.");
    m_parameters.is_stochastic_disp.doc.set("-",
        "Flag indicating whether the dispatch optimization algorithm should consider weather and/or price "
        "forecast uncertainty in identifying the optimal operations schedule. Requires that <a href="
        "\"#doc_is_dispatch\">dispatch optimization</a> is enabled.");
    m_parameters.ampl_data_dir.doc.set("-",
        "Local path to the directory containing the AMPL run and data output files. Requires that <a href=\""
        "#doc_is_dispatch\">dispatch optimization</a> and the <a href=\"#doc_is_ampl_engine\">AMPL engine</a> are enabled.");

    //m_parameters.solar_resource_file.doc.set("","");
    //m_parameters.disp_steps_per_hour.doc.set("","");
    //m_parameters.avail_seed.doc.set("","");
    //m_parameters.plant_lifetime.doc.set("","");
    //m_parameters.finance_period.doc.set("","");
    //m_parameters.ppa_multiplier_model.doc.set("","");
    //m_parameters.rec_ref_cost.doc.set("","");
    //m_parameters.rec_ref_area.doc.set("","");
    //m_parameters.tes_spec_cost.doc.set("","");
    //m_parameters.tower_fixed_cost.doc.set("","");
    //m_parameters.tower_exp.doc.set("","");
    //m_parameters.heliostat_spec_cost.doc.set("","");
    //m_parameters.site_spec_cost.doc.set("","");
    //m_parameters.land_spec_cost.doc.set("","");
    //m_parameters.c_cps0.doc.set("","");
    //m_parameters.c_cps1.doc.set("","");
    //m_parameters.om_staff_cost.doc.set("","");
    //m_parameters.wash_crew_cost.doc.set("","");
    //m_parameters.heliostat_refurbish_cost.doc.set("","");
    //m_parameters.helio_mtf.doc.set("","");
    //m_parameters.heliostat_repair_cost.doc.set("","");
    //m_parameters.om_staff_max_hours_week.doc.set("","");
    //m_parameters.n_heliostats_sim.doc.set("","");
    //m_parameters.wash_units_per_hour.doc.set("","");
    //m_parameters.wash_crew_max_hours_week.doc.set("","");
    //m_parameters.degr_per_hour.doc.set("","");
    //m_parameters.degr_accel_per_year.doc.set("","");
    //m_parameters.degr_seed.doc.set("","");
    //m_parameters.soil_per_hour.doc.set("","");
    //m_parameters.adjust_constant.doc.set("","");
    //m_parameters.helio_reflectance.doc.set("","");
    //m_parameters.disp_rsu_cost.doc.set("","");
    //m_parameters.disp_csu_cost.doc.set("","");
    //m_parameters.disp_pen_delta_w.doc.set("","");
    //m_parameters.rec_su_delay.doc.set("","");
    //m_parameters.rec_qf_delay.doc.set("","");
    //m_parameters.startup_time.doc.set("","");
    //m_parameters.startup_frac.doc.set("","");
    //m_parameters.v_wind_max.doc.set("","");
    //m_parameters.flux_max.doc.set("","");
    //m_parameters.c_ces.doc.set("","");
    //m_parameters.dispatch_factors_ts.doc.set("","");
    //m_parameters.user_sf_avail.doc.set("","");

    //m_design_outputs.number_heliostats.doc.set("","");
    //m_design_outputs.area_sf.doc.set("","");
    //m_design_outputs.base_land_area.doc.set("","");
    //m_design_outputs.land_area.doc.set("","");
    //m_design_outputs.h_tower_opt.doc.set("","");
    //m_design_outputs.rec_height_opt.doc.set("","");
    //m_design_outputs.rec_aspect_opt.doc.set("","");
    //m_design_outputs.cost_rec_tot.doc.set("","");
    //m_design_outputs.cost_sf_tot.doc.set("","");
    //m_design_outputs.cost_sf_real.doc.set("","");
    //m_design_outputs.cost_tower_tot.doc.set("","");
    //m_design_outputs.cost_land_tot.doc.set("","");
    //m_design_outputs.cost_land_real.doc.set("","");
    //m_design_outputs.cost_site_tot.doc.set("","");
    //m_design_outputs.flux_max_observed.doc.set("","");
    //m_design_outputs.opteff_table.doc.set("","");
    //m_design_outputs.flux_table.doc.set("","");
    //m_design_outputs.heliostat_positions.doc.set("","");
	m_design_outputs.annual_helio_energy.doc.set("", "");

    //m_solarfield_outputs.n_repairs.doc.set("","");
    //m_solarfield_outputs.staff_utilization.doc.set("","");
    //m_solarfield_outputs.heliostat_repair_cost_y1.doc.set("","");
    //m_solarfield_outputs.heliostat_repair_cost.doc.set("","");
    //m_solarfield_outputs.avail_schedule.doc.set("","");

    //m_optical_outputs.n_replacements.doc.set("","");
    //m_optical_outputs.heliostat_refurbish_cost.doc.set("","");
    //m_optical_outputs.heliostat_refurbish_cost_y1.doc.set("","");
    //m_optical_outputs.avg_soil.doc.set("","");
    //m_optical_outputs.avg_degr.doc.set("","");

    //m_optical_outputs.soil_schedule.doc.set("","");
    //m_optical_outputs.degr_schedule.doc.set("","");
    //m_optical_outputs.repl_schedule.doc.set("","");
    //m_optical_outputs.repl_total.doc.set("","");

    //m_simulation_outputs.generation_arr.doc.set("","");
    //m_simulation_outputs.solar_field_power_arr.doc.set("","");
    //m_simulation_outputs.tes_charge_state.doc.set("","");
    //m_simulation_outputs.dni_arr.doc.set("","");
    //m_simulation_outputs.price_arr.doc.set("","");
    //m_simulation_outputs.dni_templates.doc.set("","");
    //m_simulation_outputs.price_templates.doc.set("","");
    //m_simulation_outputs.annual_generation.doc.set("","");
    //m_simulation_outputs.annual_revenue_units.doc.set("","");
    m_parameters.solar_resource_file.doc.set("solar_resource_file", "_solar_resource_file");
    m_parameters.disp_steps_per_hour.doc.set("disp_steps_per_hour", "_disp_steps_per_hour");
    m_parameters.avail_seed.doc.set("avail_seed", "_avail_seed");
    m_parameters.plant_lifetime.doc.set("plant_lifetime", "_plant_lifetime");
    m_parameters.finance_period.doc.set("finance_period", "_finance_period");
    m_parameters.ppa_multiplier_model.doc.set("ppa_multiplier_model", "_ppa_multiplier_model");
    m_parameters.rec_ref_cost.doc.set("rec_ref_cost", "_rec_ref_cost");
    m_parameters.rec_ref_area.doc.set("rec_ref_area", "_rec_ref_area");
    m_parameters.tes_spec_cost.doc.set("tes_spec_cost", "_tes_spec_cost");
    m_parameters.tower_fixed_cost.doc.set("tower_fixed_cost", "_tower_fixed_cost");
    m_parameters.tower_exp.doc.set("tower_exp", "_tower_exp");
    m_parameters.heliostat_spec_cost.doc.set("heliostat_spec_cost", "_heliostat_spec_cost");
    m_parameters.site_spec_cost.doc.set("site_spec_cost", "_site_spec_cost");
    m_parameters.land_spec_cost.doc.set("land_spec_cost", "_land_spec_cost");
    m_parameters.c_cps0.doc.set("c_cps0", "_c_cps0");
    m_parameters.c_cps1.doc.set("c_cps1", "_c_cps1");
    m_parameters.om_staff_cost.doc.set("om_staff_cost", "_om_staff_cost");
    m_parameters.wash_crew_cost.doc.set("wash_crew_cost", "_wash_crew_cost");
    m_parameters.heliostat_refurbish_cost.doc.set("heliostat_refurbish_cost", "_heliostat_refurbish_cost");
    m_parameters.helio_mtf.doc.set("helio_mtf", "_helio_mtf");
    m_parameters.heliostat_repair_cost.doc.set("heliostat_repair_cost", "_heliostat_repair_cost");
    m_parameters.om_staff_max_hours_week.doc.set("om_staff_max_hours_week", "_om_staff_max_hours_week");
    m_parameters.n_heliostats_sim.doc.set("n_heliostats_sim", "_n_heliostats_sim");
    m_parameters.wash_rate.doc.set("wash_rate", "wash_rate");
    m_parameters.n_sim_threads.doc.set("", "");
    m_parameters.wash_crew_max_hours_week.doc.set("wash_crew_max_hours_week", "_wash_crew_max_hours_week");
    m_parameters.degr_per_hour.doc.set("degr_per_hour", "_degr_per_hour");
    m_parameters.degr_accel_per_year.doc.set("degr_accel_per_year", "_degr_accel_per_year");
    m_parameters.degr_seed.doc.set("degr_seed", "_degr_seed");
    m_parameters.soil_per_hour.doc.set("soil_per_hour", "_soil_per_hour");
    m_parameters.adjust_constant.doc.set("adjust_constant", "_adjust_constant");
    m_parameters.helio_reflectance.doc.set("helio_reflectance", "_helio_reflectance");
    m_parameters.disp_rsu_cost.doc.set("disp_rsu_cost", "_disp_rsu_cost");
    m_parameters.disp_csu_cost.doc.set("disp_csu_cost", "_disp_csu_cost");
    m_parameters.disp_pen_delta_w.doc.set("disp_pen_delta_w", "_disp_pen_delta_w");
    m_parameters.rec_su_delay.doc.set("rec_su_delay", "_rec_su_delay");
    m_parameters.rec_qf_delay.doc.set("rec_qf_delay", "_rec_qf_delay");
    m_parameters.startup_time.doc.set("startup_time", "_startup_time");
    m_parameters.startup_frac.doc.set("startup_frac", "_startup_frac");
    m_parameters.v_wind_max.doc.set("v_wind_max", "_v_wind_max");
    m_parameters.flux_max.doc.set("flux_max", "_flux_max");
    m_parameters.c_ces.doc.set("c_ces", "_c_ces");
    m_parameters.dispatch_factors_ts.doc.set("dispatch_factors_ts", "_dispatch_factors_ts");
    m_parameters.user_sf_avail.doc.set("user_sf_avail", "_user_sf_avail");

    m_design_outputs.number_heliostats.doc.set("number_heliostats", "_number_heliostats");
    m_design_outputs.area_sf.doc.set("area_sf", "_area_sf");
    m_design_outputs.base_land_area.doc.set("base_land_area", "_base_land_area");
    m_design_outputs.land_area.doc.set("land_area", "_land_area");
    m_design_outputs.h_tower_opt.doc.set("h_tower_opt", "_h_tower_opt");
    m_design_outputs.rec_height_opt.doc.set("rec_height_opt", "_rec_height_opt");
    m_design_outputs.rec_aspect_opt.doc.set("rec_aspect_opt", "_rec_aspect_opt");
    m_design_outputs.cost_rec_tot.doc.set("cost_rec_tot", "_cost_rec_tot");
    m_design_outputs.cost_sf_tot.doc.set("cost_sf_tot", "_cost_sf_tot");
    m_design_outputs.cost_sf_real.doc.set("cost_sf_real", "_cost_sf_real");
    m_design_outputs.cost_tower_tot.doc.set("cost_tower_tot", "_cost_tower_tot");
    m_design_outputs.cost_land_tot.doc.set("cost_land_tot", "_cost_land_tot");
    m_design_outputs.cost_land_real.doc.set("cost_land_real", "_cost_land_real");
    m_design_outputs.cost_site_tot.doc.set("cost_site_tot", "_cost_site_tot");
    m_design_outputs.flux_max_observed.doc.set("flux_max_observed", "_flux_max_observed");
    m_design_outputs.opteff_table.doc.set("opteff_table", "_opteff_table");
    m_design_outputs.flux_table.doc.set("flux_table", "_flux_table");
    m_design_outputs.heliostat_positions.doc.set("heliostat_positions", "_heliostat_positions");

    m_solarfield_outputs.n_repairs.doc.set("n_repairs", "_n_repairs");
    m_solarfield_outputs.staff_utilization.doc.set("staff_utilization", "_staff_utilization");
    m_solarfield_outputs.heliostat_repair_cost_y1.doc.set("heliostat_repair_cost_y1", "_heliostat_repair_cost_y1");
    m_solarfield_outputs.heliostat_repair_cost.doc.set("heliostat_repair_cost", "_heliostat_repair_cost");
    m_solarfield_outputs.avail_schedule.doc.set("avail_schedule", "_avail_schedule");

    m_optical_outputs.n_replacements.doc.set("n_replacements", "_n_replacements");
    m_optical_outputs.heliostat_refurbish_cost.doc.set("heliostat_refurbish_cost", "_heliostat_refurbish_cost");
    m_optical_outputs.heliostat_refurbish_cost_y1.doc.set("heliostat_refurbish_cost_y1", "_heliostat_refurbish_cost_y1");
    m_optical_outputs.avg_soil.doc.set("avg_soil", "_avg_soil");
    m_optical_outputs.avg_degr.doc.set("avg_degr", "_avg_degr");

    m_optical_outputs.soil_schedule.doc.set("soil_schedule", "_soil_schedule");
    m_optical_outputs.degr_schedule.doc.set("degr_schedule", "_degr_schedule");
    m_optical_outputs.repl_schedule.doc.set("repl_schedule", "_repl_schedule");
    m_optical_outputs.repl_total.doc.set("repl_total", "_repl_total");

    m_simulation_outputs.generation_arr.doc.set("generation_arr", "_generation_arr");
    m_simulation_outputs.solar_field_power_arr.doc.set("solar_field_power_arr", "_solar_field_power_arr");
    m_simulation_outputs.tes_charge_state.doc.set("tes_charge_state", "_tes_charge_state");
    m_simulation_outputs.dni_arr.doc.set("dni_arr", "_dni_arr");
    m_simulation_outputs.price_arr.doc.set("price_arr", "_price_arr");
    m_simulation_outputs.dni_templates.doc.set("dni_templates", "_dni_templates");
    m_simulation_outputs.price_templates.doc.set("price_templates", "_price_templates");
    m_simulation_outputs.annual_generation.doc.set("annual_generation", "_annual_generation");
    m_simulation_outputs.annual_revenue_units.doc.set("annual_revenue_units", "annual_revenue_units");

    for (lk::varhash_t::iterator vit = _merged_data.begin(); vit != _merged_data.end(); vit++)
        static_cast<data_base*>(vit->second)->CreateDoc();
}