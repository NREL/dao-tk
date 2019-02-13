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

    m_parameters.solar_resource_file.doc.set("", "Path specifying the weather file location.");
    m_parameters.disp_steps_per_hour.doc.set("-", 
        "Number of time steps per hour to be used in the dispatch optimization model. "
        "This value may differ from the time step in the weather file data.");
    m_parameters.avail_seed.doc.set("-", "Seed for the solar field availability model random number generator.");
    m_parameters.plant_lifetime.doc.set("yr", "Expected operational lifetime of the plant. The expected lifetime has total revenue, degradation, and failure implications.");
    m_parameters.finance_period.doc.set("yr", "Term during which loans are repayed in full.");
    m_parameters.ppa_multiplier_model.doc.set("-", 
        "Flag indicating whether the PPA price multiplier schedule is determined "
        "from the monthly-hourly TOD table (=0) or from a time series with values at each time step (=1).");
    m_parameters.rec_ref_cost.doc.set("$", "Cost of the receiver in the reference case, corresponding to the receiver reference area 'rec_ref_area'.");
    m_parameters.rec_ref_area.doc.set("m2", "Receiver surface area in the reference case, corresponding to the receiver reference cost 'rec_ref_cost'.");
    m_parameters.tes_spec_cost.doc.set("$/kWht", "Cost of thermal storage per unit capacity.");
    m_parameters.tower_fixed_cost.doc.set("$", "Cost multiplier for the tower. The fixed cost is scaled by an exponential term that is dependent on tower height.");
    m_parameters.tower_exp.doc.set("-", "Coefficient multiplier of tower height that relates tower cost to height.");
    m_parameters.heliostat_spec_cost.doc.set("$/m2", "Heliostat specific cost per unit aperture area.");
    m_parameters.site_spec_cost.doc.set("$/m2", "Site preparation cost per unit of total solar field aperture area.");
    m_parameters.land_spec_cost.doc.set("$/acre", "Cost of land per acre occupied.");
    m_parameters.c_cps0.doc.set("c_cps0", "_c_cps0");
    m_parameters.c_cps1.doc.set("c_cps1", "_c_cps1");
    m_parameters.om_staff_cost.doc.set("$/person-hr", "Cost of O&M staff per hour worked per person.");
    m_parameters.wash_crew_cost.doc.set("$/person-hr", "Cost of washing crew labor and supplies per hour worked per crew.");
    m_parameters.heliostat_refurbish_cost.doc.set("$", "Cost to replace a mirror due to degradation below the reflectivity replacement threshold.");
    m_parameters.helio_mtf.doc.set("hr", "Mean time to failure for a heliostat.");
    m_parameters.heliostat_repair_cost.doc.set("$", "Cost per failue to repair a heliostat that has failed.");
    m_parameters.om_staff_max_hours_week.doc.set("hr", 
        "Maximum number of hours worked for each O&M staff member per week. "
        "Worked hours are considered to be productive.");
    m_parameters.n_heliostats_sim.doc.set("-", 
        "Number of heliostats to be simulated in the degradation, soiling, and availabiliy models. The number is the <sl>actual</sl> "
        "count in the simulation, and the results of the simulation are scaled according the number of heliostats included in the "
        "plant. Fewer heliostats for simulation corresponds to more significant random effects during stochastic evaluation, while "
        "more heliostats reduces uncertainty effects but incurs more computational expense.");
    m_parameters.wash_rate.doc.set("1/hr", "Number of heliostats washed per hour per wash crew.");
    m_parameters.n_sim_threads.doc.set("-", "Maximum number of CPU threads to utilize in simulating plant performance. "
        "Multithreading is only available if ");
    m_parameters.is_run_continuous.doc.set("", 
        "Simulate plant performance using a single evaluation in the SSC engine wherein all simulated days "
        "are evaluated within a single call to the SSC engine. If <b>false</b>, performance simulation days "
        "are evaluated using separate calls to the SSC engine. <br> <sl> Tip: If running with a single thread/CPU, "
        "it is typically faster to run continuously. If running with multi-threading, the runs must not be "
        "continuous so as to allow the software to evaluate simulation days on multiple threads.</sl>"
    );
    m_parameters.wash_crew_max_hours_week.doc.set("hr", "Maximum number of hours that a wash crew can work per week.");
    m_parameters.degr_per_hour.doc.set("1/hr", "Expected mirror reflectivity degradation (fractional) per hour of service.");
    m_parameters.degr_accel_per_year.doc.set("1/(hr-yr)", "Rate at which mirror degradation accelerates (or decelerates) per year.");
    m_parameters.degr_seed.doc.set("-", "Seed for the mirror degradation random number generator.");
    m_parameters.soil_per_hour.doc.set("1/hr", "Expected mirror soiling rate (fractional) per hour of service.");
    m_parameters.adjust_constant.doc.set("%", "Miscellaneous fixed power loss from the plant.");
    m_parameters.helio_reflectance.doc.set("-", "Heliostat material reflectivity in a <b>new</b> condition.");
    m_parameters.disp_rsu_cost.doc.set("$/start", 
        "Expected cost to start the receiver in long-term maintenance. "
        "This value is used to optimize plant performance and is not included as a cost in the financial calculations.");
    m_parameters.disp_csu_cost.doc.set("$/start", 
        "Expected cost to start the power cycle in long-term maintenance. "
        "This value is used to optimize plant performance and is not included as a cost in the financial calculations.");
    m_parameters.disp_pen_delta_w.doc.set("$/kWe-change", 
        "Expected cost of ramping power cycle output up or down in expected long-term maintenance. "
        "This value is used to optimize plant performance and is not included as a cost in the financial calculations.");
    m_parameters.rec_su_delay.doc.set("hr", "Required minimum receiver startup time.");
    m_parameters.rec_qf_delay.doc.set("MWht/MWt-cap-hr", 
        "Required minimum receiver energy input to achieve start up. "
        "The value is expressed as a fraction of energy (rated thermal capacity producing over a one hour period) that is consumed by "
        "the receiver before producing useful power.");
    m_parameters.startup_time.doc.set("hr", "Required minimum power cycle startup time.");
    m_parameters.startup_frac.doc.set("MWht/MWt-cap-hr", "Required minimum power cycle energy input to achieve start up. "
        "The value is expressed as a fraction of energy (rated capacity over a one hour period) that is consumed by "
        "the cycle before producing useful power.");
    m_parameters.v_wind_max.doc.set("m/s", "Maximum wind speed before the heliostat field goes into stow mode and ceases power production.");
    m_parameters.flux_max.doc.set("kW/m2", "Maximum allowable flux density incident at any location on the receiver under design conditions.");
    m_parameters.c_ces.doc.set("c_ces", "_c_ces");
    m_parameters.dispatch_factors_ts.doc.set("-", 
        "Time series vector of length equal to the number of time steps in the simulation horizon that "
        "specifies the temporally dependent PPA price multiplier."
    );
    m_parameters.user_sf_avail.doc.set("-", 
        "Time series vector of length equal to the number of time steps in the simulation horizon that "
        "specifies the solar field power output loss as a function of time."
    );
    m_parameters.forecast_gamma.doc.set("-", 
        "Weighting factor for hedging thermal storage inventory against forecast uncertainty. "
        "The dispatch optimization objective function is formulated such that:<br>"
        "max Z = (1-gamma) * (Revenue days 1 and 2) + gamma * (TES charge end day 1)"
    );
    m_parameters.wlim_series.doc.set("kWe",
        "Time series representing the maximum allowable net power output from the power cycle."
    );

    //calculated
    m_design_outputs.number_heliostats.doc.set("-", "Calculated number of heliostats in the solar field layout.");
    m_design_outputs.area_sf.doc.set("m2", "Total heliostat aperture area of the solar field (i.e., the summation of "
        "aperture area over all heliostats in the field.)");
    m_design_outputs.base_land_area.doc.set("acre", 
        "The land area directly occupied by the solar field and power block. "
        "The base land area is calculated as the convex hull of all heliostat positions."
    );
    m_design_outputs.land_area.doc.set("acre", "Total land area occupied, including base land area and supplemental land area.");
    m_design_outputs.h_tower_opt.doc.set("m", "Optimized tower height.");
    m_design_outputs.rec_height_opt.doc.set("m", "Optimized reciever height.");
    m_design_outputs.rec_aspect_opt.doc.set("-", "Optimized receiver aspect ratio (height/diameter).");
    m_design_outputs.cost_rec_tot.doc.set("$", "Total receiver subsystem cost.");
    m_design_outputs.cost_sf_tot.doc.set("$", "Total solar field (heliostats) cost.");
    m_design_outputs.cost_sf_real.doc.set("$", "Total solar field cost - discounted to today's dollars.");
    m_design_outputs.cost_tower_tot.doc.set("$", "Total tower cost.");
    m_design_outputs.cost_land_tot.doc.set("$", "Total land cost");
    m_design_outputs.cost_land_real.doc.set("$", "Total land cost - discounted to today's dollars.");
    m_design_outputs.cost_site_tot.doc.set("$", "Site preparation cost.");
    m_design_outputs.flux_max_observed.doc.set("kW/m2", "Peak observed flux incident on the receiver at design.");
    m_design_outputs.opteff_table.doc.set("-", "Table providing total optical efficiency of the heliostat field "
        "over a set of representative sun positions. The table entries are rows including sets of [azimuth, zenith, efficiency] "
        "in which azimuth is defined as 0deg to the south.");
    m_design_outputs.flux_table.doc.set("-", "Table providing the normalized flux profile on the receiver over "
        "a set of representative sun positions.");
    m_design_outputs.heliostat_positions.doc.set("heliostat_positions", "_heliostat_positions");

    m_solarfield_outputs.n_repairs.doc.set("-", "Number of repairs made to heliostats in the heliostat availability model.");
    m_solarfield_outputs.staff_utilization.doc.set("-", "Fractional usage (hours worked over maximum hours available) of "
        "heliostat field O&M staff.");
    m_solarfield_outputs.heliostat_repair_cost_y1.doc.set("$/yr", "Annualized cost of all heliostat repairs made discounted to year 1.");
    m_solarfield_outputs.heliostat_repair_cost.doc.set("$", "Lifetime cost of all heliostat repairs made.");
    m_solarfield_outputs.avail_schedule.doc.set("-", "Calculated solar field availability time series based on modeled failure and repair events.");

    m_optical_outputs.n_replacements.doc.set("1/yr", "Number of replacements made due to heliostat optical degradation.");
    m_optical_outputs.heliostat_refurbish_cost.doc.set("$", "Total cost incurred for mirror replacement on a heliostat due to optical degradation.");
    m_optical_outputs.heliostat_refurbish_cost_y1.doc.set("$", "Cost of replacing degraded mirrors discounted to year 1.");
    m_optical_outputs.avg_soil.doc.set("-", "Average steady-state soiling efficiency.");
    m_optical_outputs.avg_degr.doc.set("-", "Average lifetime degradation efficiency.");

    m_optical_outputs.soil_schedule.doc.set("-", "Time series vector of solar field average soiling efficiency.");
    m_optical_outputs.degr_schedule.doc.set("-", "Time series vector of solar field average optical degradation.");
    m_optical_outputs.repl_schedule.doc.set("-", "Time series vector indicating the number of mirror replacements at each time step.");
    m_optical_outputs.repl_total.doc.set("-", "Total number of lifetime modeled mirror replacements.");

    m_optimization_outputs.best_point.doc.set("-", "Best point and response values identified during optimization iterations.");
    m_optimization_outputs.iteration_history.doc.set("-", "All points and response values evaluated during optimization iterations.");

    m_simulation_outputs.generation_arr.doc.set("MWe", "Time series vector of net power generation from the power cycle.");
    m_simulation_outputs.solar_field_power_arr.doc.set("MWt", "Time series vector of solar field thermal power output.");
    m_simulation_outputs.tes_charge_state.doc.set("MWht", "Time series vector of energy stored in thermal storage");
    m_simulation_outputs.dni_arr.doc.set("W/m2", "Time series vector of direct normal irradiance (DNI) from the weather file.");
    m_simulation_outputs.price_arr.doc.set("-", "Time series vector of PPA price multipliers used in the simulation.");
    m_simulation_outputs.dni_templates.doc.set("W/m2", "Listing of DNI clustering templates that were used in the simulation (if applicable).");
    m_simulation_outputs.price_templates.doc.set("-", "Listing of pricing clustering templates that were used in the simulation (if applicable).");
    m_simulation_outputs.annual_generation.doc.set("GWhe", "Total simulated electrical energy production per year.");
    m_simulation_outputs.annual_revenue_units.doc.set("-", "The total normalized revenue achieved by the evaluated system over the course of 1 year.");

    m_parameters.maintenance_interval.doc.set("h","Runtime duration between maintenance events");
    m_parameters.maintenance_duration.doc.set("h","Duration of maintenance events");
    m_parameters.downtime_threshold.doc.set("h","Downtime threshold for warm start");
    m_parameters.hours_to_maintenance.doc.set("h","Runtime duration before next maintenance event");
    m_parameters.temp_threshold.doc.set("C","Ambient temperature threshold for condensers");
    m_parameters.shutdown_capacity.doc.set("-","Threshold capacity to shut plant down");
    m_parameters.no_restart_capacity.doc.set("-","Threshold capacity for maintenance on shutdown");
    m_parameters.shutdown_efficiency.doc.set("-","Threshold efficiency to shut plant down");
    m_parameters.no_restart_efficiency.doc.set("-","Threshold efficiency for maintenance on shutdown");
    m_parameters.cycle_hourly_labor_cost.doc.set("h","Hourly cost for repair of cycle components");

    for (lk::varhash_t::iterator vit = _merged_data.begin(); vit != _merged_data.end(); vit++)
        static_cast<data_base*>(vit->second)->CreateDoc();
}