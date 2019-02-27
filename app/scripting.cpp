#include <set>

#include <lk/env.h>
#include <ssc/sscapi.h>

#include "scripting.h"
#include "project.h"
#include "daotk_app.h"

#include "../liboptimize/optimize.h"

#include "../liboptical/optical_degr.h"
#include "../liboptical/optical_structures.h"
#include "../libsolar/solarfield_avail.h"
#include "../libsolar/solarfield_structures.h"

#include "../libcluster/cluster.h"
#include "../libcluster/metrics.h"
#include "../libcluster/clustersim.h"

#include "../libcycle/component.h"
#include "../libcycle/distributions.h"
#include "../libcycle/failure.h"
#include "../libcycle/lib_util.h"
#include "../libcycle/plant.h"
#include "../libcycle/well512.h"


int set_array(ssc_data_t p_data, const char *name, const char* fn, int len)
{
	char buffer[1024];
	char *record, *line;
	int i = 0;
	ssc_number_t *ary;
	FILE *fp = fopen(fn, "r");
	if (fp == NULL)
	{
		MainWindow::Instance().Log( wxString::Format( "file opening failed " ) );
		return 0;
	}
	ary = (ssc_number_t *)malloc(len * sizeof(ssc_number_t));
	while ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)
	{
		record = strtok(line, ",");
		while ((record != NULL) && (i < len))
		{
			ary[i] = atof(record);
			record = strtok(NULL, ",");
			i++;
		}
	}
	fclose(fp);
	ssc_data_set_array(p_data, name, ary, len);
	free(ary);
	return 1;
}

int set_matrix(ssc_data_t p_data, const char *name, const char* fn, int nr, int nc)
{
	char buffer[1024];
	char *record, *line;
	ssc_number_t *ary;
	int i = 0, len = nr*nc;
	FILE *fp = fopen(fn, "r");
	if (fp == NULL)
	{
		MainWindow::Instance().Log( wxString::Format( "file opening failed " ) );
		return 0;
	}
	ary = (ssc_number_t *)malloc(len * sizeof(ssc_number_t));
	while ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)
	{
		record = strtok(line, ",");
		while ((record != NULL) && (i < len))
		{
			ary[i] = atof(record);
			record = strtok(NULL, ",");
			i++;
		}
	}
	fclose(fp);
	ssc_data_set_matrix(p_data, name, ary, nr, nc);
	free(ary);
	return 1;
}

ssc_bool_t ssc_progress_handler( ssc_module_t , ssc_handler_t , int action, float f0, float f1, const char *s0, const char *, void * )
{
	if (action == SSC_LOG)
	{
		// print log message to console
		wxString msg;
		switch( (int)f0 )
		{
		case SSC_NOTICE: 
            //msg << "Notice: " << s0 << " time " << f1; 
            break;
		case SSC_WARNING: 
            //msg << "Warning: " << s0 << " time " << f1; 
            break;
		case SSC_ERROR: 
            msg << "Error: " << s0 << " time " << f1; 
            break;
		default: 
            msg << "Log notice uninterpretable: " << f0 << " time " << f1; 
            break;
		}
		if(!msg.IsEmpty())
		    MainWindow::Instance().Log(msg);
		return 1;
	}
	else if (action == SSC_UPDATE)
	{
		// print status update to console
		MainWindow::Instance().SetProgress( (int) f0, s0 );
		wxGetApp().Yield(true);
		return !MainWindow::Instance().UpdateIsStopFlagSet();
	}
	else
		return 0;
}

void message_handler(const char *msg)
{
    wxString wmsg(msg);
    if (wmsg.IsEmpty())
        return;
    else
	    MainWindow::Instance().Log(msg);
}

void iterplot_update_handler()
{
    MainWindow::Instance().UpdateIterPlot();
}

bool sim_progress_handler(float progress, const char *msg)
{
	MainWindow::Instance().SetProgress((int)(progress*100.), msg);
	wxGetApp().Yield(true);
	return !MainWindow::Instance().UpdateIsStopFlagSet();
}

void _initialize(lk::invoke_t &cxt)
{
	LK_DOC("initialize", "Reset all variable and parameter values to defaults.", "(void):void");
	MainWindow &mw = MainWindow::Instance();
	mw.GetProject()->Initialize();
	mw.UpdateDataTable();
}

void _varinfo(lk::invoke_t &cxt)
{
    LK_DOC2("varinfo", "Interact with variable properties.",
        "Set variable properties. Keys include 'upper_bound', 'lower_bound', 'initializers,' 'is_integer,' and 'is_optimized'. Returns true when "
        "variable with the specified name exists, false if variable does not exist.", "(table:values):boolean",
        "Get table of properties currently assigned to a variable.", "(void):table"
    );

    MainWindow &mw = MainWindow::Instance();
    Project *project = mw.GetProject();

    //collect the item name
    std::string namearg = cxt.arg(0).as_string();
    data_base* dat = project->GetVarPtr(namearg.c_str());
    if (!dat)
    {
        //variable not found
        mw.Log(wxString::Format("The specified variable name (%s) is not "
                                "included in the available variables.",
                                namearg.c_str()
                                )
        );
        cxt.result().assign(0.);
        return;
    }

    if (cxt.arg_count() == 2)
    {

        lk::varhash_t *h = cxt.arg(1).hash();

        variable* v = static_cast<variable*>(dat);
        std::vector<double> inits;

        if (h->find("upper_bound") != h->end())
            v->maxval.assign( h->at("upper_bound")->as_number() );
        if (h->find("lower_bound") != h->end())
            v->minval.assign(h->at("lower_bound")->as_number());
        if (h->find("is_optimized") != h->end())
            v->is_optimized = h->at("is_optimized")->as_boolean();
        if (h->find("is_integer") != h->end())
            v->is_integer= h->at("is_integer")->as_boolean();
        if (h->find("initializers") != h->end())
        {
            v->initializers.empty_vector();
            int n = h->at("initializers")->vec()->size();
            v->initializers.vec()->resize(n);
            for (int i = 0; i < n; i++)
                v->initializers.vec()->at(i).assign(h->at("initializers")->vec()->at(i).as_number());
        }
    }
    else if( cxt.arg_count()==1)
    {
        variable* v = static_cast<variable*>(dat);
        cxt.result().empty_hash();
        cxt.result().hash_item("upper_bound", v->maxval.as_number());
        cxt.result().hash_item("lower_bound", v->minval.as_number());
        cxt.result().hash_item("is_optimized", v->is_optimized);
        cxt.result().hash()->at("initializers")->empty_vector();
        cxt.result().hash()->at("initializers")->vec()->resize(v->initializers.vec()->size());
        for (int i = 0; i < v->initializers.vec()->size(); i++)
            cxt.result().hash()->at("initializers")->vec()->at(i).assign( v->initializers.vec()->at(i).as_number());
    }
    else
    {
        cxt.result().assign(0.);
        return;
    }

}

void _sscvar(lk::invoke_t &cxt)
{
    LK_DOC("set_ssc_par", "Set secondary project settings directly in SSC context (advanced user).", "(string:name, variant:value):none");

    if (cxt.arg_count() != 2)
    {
        MainWindow::Instance().Log("The function set_ssc_par() requires two arguments.");
        return;
    }

    std::string name = cxt.arg(0).as_string();
    lk::vardata_t arg = cxt.arg(1);

    MainWindow::Instance().GetProject()->AddToSSCContext(name, arg);
}

void _var(lk::invoke_t &cxt)
{
    LK_DOC2("var", "Interact with variables or parameters.",
		"Set a variable value.", "(string:name, variant:value):none",
		"Get a variable value", "(string:name):variant");

	MainWindow &mw = MainWindow::Instance();
	Project *project = mw.GetProject();

	//collect the item name
	std::string namearg = cxt.arg(0).as_string();
	data_base* dat = project->GetVarPtr(namearg.c_str());

	if(! dat)
	{
		//variable not found
		mw.Log( wxString::Format("The specified variable name (%s) is not "
								 "included in the available variables.", 
								 namearg.c_str()
								 ) 
			   );
		return;
	}


	//is this a set or get operation?
    if (cxt.arg_count() == 2) //set
    {
		//validate data type
		if( cxt.arg(1).type() != dat->type() )
		{
			std::string expected_type;
			std::string given_type;

			switch( dat->type() )
			{
				case lk::vardata_t::STRING: expected_type = "string"; break;
				case lk::vardata_t::NUMBER: expected_type = "number"; break;
				case lk::vardata_t::VECTOR: expected_type = "array"; break;
				default: expected_type = "unknown";
			}
			switch( cxt.arg(1).type() )
			{
				case lk::vardata_t::STRING: given_type = "string"; break;
				case lk::vardata_t::NUMBER: given_type = "number"; break;
				case lk::vardata_t::VECTOR: given_type = "array"; break;
				default: given_type = "unknown";
			}

			mw.Log( wxString::Format("The variable %s has an expected data type "
									"of %s, but the provided data type is %s. "
									"Please assign the variable according to the "
									"required type.", namearg.c_str(), 
									expected_type.c_str(), given_type.c_str()
									) 
				);
			return;
		}
		//assign data type
		switch( dat->type() )
		{
			case lk::vardata_t::STRING:
				dat->assign( cxt.arg(1).as_string() );
				break;
			case lk::vardata_t::NUMBER:
				dat->assign( cxt.arg(1).as_number() );
				break;
			case lk::vardata_t::VECTOR:
				dat->vec()->assign( cxt.arg(1).vec()->begin(), cxt.arg(1).vec()->end() );
				break;
			default:
				mw.Log("Specified variable does not contain valid data. Must be of type "
					   "string, double, integer, boolean, or array (vector).");
				return;
		}
	}
	else if (cxt.arg_count() == 1) //get
	{
		switch( dat->type() )
		{
			case lk::vardata_t::STRING:
				cxt.result().assign( dat->as_string() );
				break;
			case lk::vardata_t::NUMBER:
				cxt.result().assign( dat->as_number() );
				break;
			case lk::vardata_t::VECTOR:
				cxt.result().vec()->assign( dat->vec()->begin(), dat->vec()->end() );
				break;
			default:
				mw.Log("Specified variable does not contain valid data. Must be of type "
						"string, double, integer, boolean, or array (vector).");
				return;
		}
	}
	else
	{
		mw.Log( wxString::Format("Invalid argument list: %d arguments provided. "
								 "The var(string, value) function assigns a variable "
								 "value. The var(string) function retrives a variable value.", 
								 (int)cxt.arg_count() 
								 ) 
			   );
	}
	
	mw.UpdateDataTable();

}

void _test(lk::invoke_t &cxt)
{
	LK_DOC("test", "Test description.", "(void):null");

	// MainWindow::Instance().Log()
	MainWindow &mw = MainWindow::Instance();

	Project *P = mw.GetProject();
	
	P->m_variables.h_tower.assign( 193. );
	P->m_variables.rec_height.assign( 21. );
	P->m_variables.D_rec.assign( 17. );
	P->m_variables.design_eff.assign( .41 );
	P->m_variables.dni_des.assign( 950. );
	P->m_variables.P_ref.assign( 115. );
	P->m_variables.solarm.assign( 2.4 );
	P->m_variables.tshours.assign( 10. );
	P->m_variables.degr_replace_limit.assign( .7 );
	P->m_variables.om_staff.assign( 5 );
	//P->m_variables.n_wash_crews.assign( 3 );
    P->m_parameters.heliostat_repair_cost.assign(0.);
	P->m_variables.N_panel_pairs.assign( 8 );
    P->m_parameters.degr_per_hour.assign(0.);
    P->m_parameters.degr_accel_per_year.assign(0.);

    //P->m_parameters.solar_resource_file.assign( "/home/mike/workspace/dao-tk/deploy/samples/USA CA Daggett Barstow-daggett Ap (TMY3).csv" );
    //P->m_parameters.solar_resource_file.assign( "C:/Users/mwagner/Documents/NREL/projects/dao-tk/deploy/samples/USA CA Daggett Barstow-daggett Ap (TMY3).csv" );
	P->m_parameters.solar_resource_file.assign( "C:/Users/AZOLAN/Documents/GitHub/daotk_dev/dao-tk/deploy/samples/USA CA Daggett Barstow-daggett Ap (TMY3).csv" );

	/*
	P->m_parameters.sim_length.assign( 720 );
	P->m_parameters.cycle_power.empty_vector();
	P->m_parameters.thermal_power.empty_vector();
	P->m_parameters.standby.empty_vector();
	P->m_parameters.ambient_temperature.empty_vector();
	for (int i = 0; i < 30; i++) //simulate 30 days of dispatch
	{
		for (int j = 0; j < 8; j++)
		{
			P->m_parameters.cycle_power.vec_append(0);
			P->m_parameters.thermal_power.vec_append(0);
			P->m_parameters.standby.vec_append(0);
			P->m_parameters.ambient_temperature.vec_append(0);
		}
		for (int j = 0; j < 8; j++)
		{
			P->m_parameters.cycle_power.vec_append(1e5);
			P->m_parameters.thermal_power.vec_append(3e5);
			P->m_parameters.standby.vec_append(0);
			P->m_parameters.ambient_temperature.vec_append(10);
		}
		for (int j = 0; j < 8; j++)
		{
			P->m_parameters.cycle_power.vec_append(0);
			P->m_parameters.thermal_power.vec_append(0);
			P->m_parameters.standby.vec_append(0);
			P->m_parameters.ambient_temperature.vec_append(15);
		}
	}
	*/
    if (!P->D()) return;
    if (!P->O()) return;
    if (!P->M()) return;
    if (!P->E()) return;
    if (!P->S()) return;
    if (!P->F()) return;
    //if (!P->C()) return;
	
	mw.Log(wxString::Format("Total field area: %.2f", P->m_design_outputs.area_sf.as_number()));
	mw.Log(wxString::Format("Number of heliostats: %d", (int)P->m_design_outputs.number_heliostats.as_integer()));
	mw.Log(wxString::Format("Number of repairs: %d", (int)P->m_solarfield_outputs.n_repairs.as_integer()));
	mw.Log(wxString::Format("Number of mirror replacements: %d", (int)P->m_optical_outputs.n_replacements.as_integer()));
	mw.Log(wxString::Format("Heliostat replacement cost: %.2f", P->m_optical_outputs.heliostat_refurbish_cost.as_number()));
	mw.Log(wxString::Format("Heliostat replacement cost_y1: %.2f", P->m_optical_outputs.heliostat_refurbish_cost_y1.as_number()));
	mw.Log(wxString::Format("Average soiling: %.2f", P->m_optical_outputs.avg_soil.as_number()));
	mw.Log(wxString::Format("Average degradation: %.2f", P->m_optical_outputs.avg_degr.as_number()));
	mw.Log(wxString::Format("Average cycle repair labor costs: %.2f", P->m_cycle_outputs.cycle_labor_cost.as_number()));
	mw.Log(wxString::Format("Number of failed components: %d", P->m_cycle_outputs.num_failures.as_integer()));
	mw.Log(wxString::Format("Number of wash crews: %d", P->m_optical_outputs.n_wash_crews.as_integer()));
	mw.Log(wxString::Format("Total sales: %.2f", P->m_financial_outputs.ppa.as_number()));
	mw.Log(wxString::Format("Total Cash flow: %.2f", P->m_objective_outputs.cash_flow.as_number()));
	mw.Log(wxString::Format("Real LCOE: %.2f", P->m_financial_outputs.lcoe_real.as_number()));
	mw.Log(wxString::Format("Nominal LCOE: %.2f", P->m_financial_outputs.lcoe_nom.as_number()));
	mw.Log(wxString::Format("PPA Price: %.2f", P->m_financial_outputs.ppa.as_number()));

	mw.SetProgress(0.);
	mw.UpdateDataTable();

	return;

}

void _generate_solarfield(lk::invoke_t &cxt)
{
	LK_DOC("generate_solarfield", "Creates a new solar field layout and geometry from current project settings.", "([table:options]):table");

	MainWindow::Instance().GetProject()->D();
	MainWindow::Instance().SetProgress(0.);
	
}

void _power_cycle(lk::invoke_t &cxt)
{
	LK_DOC("power_cycle", "Simulate the power cycle capacity over time, "
		"after accounting for maintenance and failures. "
		"Table keys include: cycle_power, ambient_temperature, standby, "
		"read_periods, sim_length, eps, output, num_scenarios, "
		"cycle_hourly_labor_cost, stop_cycle_at_first_failure, "
		"stop_cycle_at_first_repair, maintenance_interval, maintenance_duration, "
		"downtime_threshold,steplength, hours_to_maintenance, power_output, "
		"thermal_output, current_standby, capacity, thermal_capacity, "
		"temp_threshold, time_online, time_in_standby, downtime, "
		"shutdown_capacity, no_restart_capacity, shutdown_efficiency, "
		"no_restart_efficiency, num_condenser_trains, fans_per_train, "
		"radiators_per_train, num_salt_steam_trains, num_fwh, num_salt_pumps, "
		"num_salt_pumps_required, num_water_pumps, num_water_pumps_required, "
		"num_boiler_pumps, num_boiler_pumps_required, num_turbines,"
		" condenser_eff_cold, condenser_eff_hot", "(table:cycle_inputs):table");

	MainWindow &mw = MainWindow::Instance();

	PowerCycle cycle = PowerCycle();

	lk::varhash_t *h = cxt.arg(0).hash();

	// Plant attributes

	double maintenance_interval = 1000000.;
	if (h->find("maintenance_interval") != h->end())
		maintenance_interval = h->at("maintenance_interval")->as_number();

	double maintenance_duration = 168.;
	if (h->find("maintenance_duration") != h->end())
		maintenance_duration = h->at("maintenance_duration")->as_number();

	double downtime_threshold = 24.;
	if (h->find("downtime_threshold") != h->end())
		downtime_threshold = h->at("downtime_threshold")->as_number();

	double hours_to_maintenance = 1000000.;
	if (h->find("hours_to_maintenance") != h->end())
		hours_to_maintenance = h->at("hours_to_maintenance")->as_number();

	double power_output = 0.;
	if (h->find("power_output") != h->end())
		power_output = h->at("power_output")->as_number();

	double thermal_output = 0.;
	if (h->find("thermal_output") != h->end())
		thermal_output = h->at("thermal_output")->as_number();

	bool current_standby = false;
	if (h->find("current_standby") != h->end())
		current_standby = h->at("current_standby")->as_boolean();

	double capacity = 500000.;
	if (h->find("capacity") != h->end())
		capacity = h->at("capacity")->as_number();

	double thermal_capacity = 1500000.;
	if (h->find("thermal_capacity") != h->end())
		thermal_capacity = h->at("thermal_capacity")->as_number();

	double temp_threshold = 20.;
	if (h->find("temp_threshold") != h->end())
		temp_threshold = h->at("temp_threshold")->as_number();

	double time_online = 0.;
	if (h->find("time_online") != h->end())
		time_online = h->at("time_online")->as_number();

	double time_in_standby = 0.;
	if (h->find("time_in_standby") != h->end())
		time_in_standby = h->at("time_in_standby")->as_number();

	double downtime = 0.;
	if (h->find("downtime") != h->end())
		downtime = h->at("downtime")->as_number();

	double shutdown_capacity = 0.45;
	if (h->find("shutdown_capacity") != h->end())
		shutdown_capacity = h->at("shutdown_capacity")->as_number();

	double no_restart_capacity = 0.9;
	if (h->find("no_restart_capacity") != h->end())
		no_restart_capacity = h->at("no_restart_capacity")->as_number();
	
	double shutdown_efficiency = 0.45;
	if (h->find("shutdown_efficiency") != h->end())
		shutdown_efficiency = h->at("shutdown_efficiency")->as_number();

	double no_restart_efficiency = 0.9;
	if (h->find("no_restart_efficiency") != h->end())
		no_restart_efficiency = h->at("no_restart_efficiency")->as_number();

	cycle.SetPlantAttributes(
		maintenance_interval, 
		maintenance_duration,
		downtime_threshold, 
		hours_to_maintenance, 
		power_output,
		thermal_output,
		current_standby, 
		capacity, 
		thermal_capacity,
		temp_threshold, 
		time_online, 
		time_in_standby, 
		downtime, 
		shutdown_capacity, 
		no_restart_capacity,
		shutdown_efficiency,
		no_restart_efficiency
		);

	// Power cycle components

	int num_condenser_trains = 2;
	if (h->find("num_condenser_trains") != h->end())
		num_condenser_trains = h->at("num_condenser_trains")->as_integer();

	int fans_per_train = 30;
	if (h->find("fans_per_train") != h->end())
		fans_per_train = h->at("fans_per_train")->as_integer();

	int radiators_per_train = 2;
	if (h->find("radiators_per_train") != h->end())
		radiators_per_train = h->at("radiators_per_train")->as_integer();

	int num_salt_steam_trains = 2;
	if (h->find("num_salt_steam_trains") != h->end())
		num_salt_steam_trains = h->at("num_salt_steam_trains")->as_integer();

	int num_fwh = 6;
	if (h->find("num_fwh") != h->end())
		num_fwh = h->at("num_fwh")->as_integer();

	int num_salt_pumps = 3;
	if (h->find("num_salt_pumps") != h->end())
		num_salt_pumps = h->at("num_salt_pumps")->as_integer();

	int num_salt_pumps_required = 2;
	if (h->find("num_salt_pumps_required") != h->end())
		num_salt_pumps_required = h->at("num_salt_pumps_required")->as_integer();

	int num_water_pumps = 2;
	if (h->find("num_water_pumps") != h->end())
		num_water_pumps = h->at("num_water_pumps")->as_integer();

	int num_water_pumps_required = 1;
	if (h->find("num_water_pumps_required") != h->end())
		num_water_pumps_required = h->at("num_water_pumps_required")->as_integer();

	int num_boiler_pumps = 2;
	if (h->find("num_boiler_pumps") != h->end())
		num_boiler_pumps = h->at("num_boiler_pumps")->as_integer();

	int num_boiler_pumps_required = 1;
	if (h->find("num_boiler_pumps_required") != h->end())
		num_boiler_pumps_required = h->at("num_boiler_pumps_required")->as_integer();

	int num_turbines = 1;
	if (h->find("num_turbines") != h->end())
		num_turbines = h->at("num_turbines")->as_integer();

	std::vector < double > condenser_eff_cold = { 0.,1.,1. };
	if (h->find("condenser_eff_cold") != h->end())
	{
		std::vector<lk::vardata_t> *c_eff_cold = h->at("condenser_eff_cold")->vec();
		lk::vardata_t q;
		condenser_eff_cold.clear();
		for (size_t i = 0; i < (num_condenser_trains + 1); i++)
		{
			q = c_eff_cold->at(i);
			condenser_eff_cold.push_back(q.as_number());
		}
	}

	std::vector < double > condenser_eff_hot = { 0.,1.,1. };
	if (h->find("condenser_eff_hot") != h->end())
	{
		std::vector<lk::vardata_t> *c_eff_cold = h->at("condenser_eff_hot")->vec();
		lk::vardata_t q;
		condenser_eff_hot.clear();
		for (size_t i = 0; i < (num_condenser_trains + 1); i++)
		{
			q = c_eff_cold->at(i);
			condenser_eff_hot.push_back(q.as_number());
		}
	}
		
	cycle.GeneratePlantComponents(
		num_condenser_trains,
		fans_per_train,
		radiators_per_train,
		num_salt_steam_trains,
		num_fwh,
		num_salt_pumps,
		num_salt_pumps_required,
		num_water_pumps,
		num_water_pumps_required,
		num_boiler_pumps_required,
		num_boiler_pumps_required,
		num_turbines,
		condenser_eff_cold,
		condenser_eff_hot
	);

	// Simulation parameters

	int read_periods = 0;
	if (h->find("read_periods") != h->end())
		read_periods = h->at("read_periods")->as_integer();

	int sim_length = 48;
	if (h->find("sim_length") != h->end())
		sim_length = h->at("sim_length")->as_integer();

	double steplength = 1.;
	if (h->find("steplength") != h->end())
		steplength = h->at("steplength")->as_number();

	double eps = 0;
	if (h->find("eps") != h->end())
		eps = h->at("eps")->as_number();

	bool output = false;
	if (h->find("output") != h->end())
		output = h->at("output")->as_boolean();

	int num_scenarios = 1;
	if (h->find("num_scenarios") != h->end())
		num_scenarios = h->at("num_scenarios")->as_integer();

	double cycle_hourly_labor_cost = 50.;
	if (h->find("cycle_hourly_labor_cost") != h->end())
		eps = h->at("cycle_hourly_labor_cost")->as_number();

	bool stop_at_first_failure = false;
	if (h->find("stop_cycle_at_first_failure") != h->end())
		stop_at_first_failure = h->at("stop_cycle_at_first_failure")->as_boolean();

	bool stop_at_first_repair = false;
	if (h->find("stop_cycle_at_first_repair") != h->end())
		stop_at_first_repair = h->at("stop_cycle_at_first_repair")->as_boolean();

	cycle.SetSimulationParameters(
		read_periods,
		sim_length,
		steplength,
		eps,
		output,
		num_scenarios,
		cycle_hourly_labor_cost,
		stop_at_first_failure,
		stop_at_first_repair
		);

	std::unordered_map<std::string, std::vector<double> > dispatch;
	dispatch["cycle_power"] = { 0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0 };
	dispatch["thermal_power"] = { 0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0 };
	dispatch["ambient_temperature"] = { 0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0 };
	dispatch["standby"] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	/*
	// Dispatch parameters
	// For now, we will assume that the dispatch parameters
	// will be required inputs, i.e., there are no defaults.  These
	// should come from a prior run of SAM.
	std::vector<lk::vardata_t> *cycle_power = h->at("cycle_power")->vec();
	std::vector<lk::vardata_t> *ambient_temperature = h->at("ambient_temperature")->vec();
	std::vector<lk::vardata_t> *standby_by_hour = h->at("standby")->vec();
	lk::vardata_t q;
	for (int i = 0; i < sim_length; i++)
	{
		q = cycle_power->at(start_period + i);
		dispatch.at("cycle_power").push_back(q.as_number());
		q = ambient_temperature->at(start_period + i);
		dispatch.at("ambient_temperature").push_back(q.as_number());
		q = standby_by_hour->at(start_period + i);
		dispatch.at("standby").push_back(q.as_number());
	}
	*/
	cycle.SetDispatch(dispatch, true);

	WELLFiveTwelve gen1(0);
	WELLFiveTwelve gen2(0);
	WELLFiveTwelve gen3(0);
	cycle.AssignGenerators(&gen1, &gen2, &gen3);
	cycle.Initialize();
	cycle.Simulate(false);

	mw.SetProgress(0.);

	mw.Log(wxString::Format("Average cycle repair labor costs: %.2f",cycle.m_results.avg_labor_cost));
	mw.Log(wxString::Format("Total number of failures: %i", cycle.m_results.failure_event_labels.size()));

	return;
}

void _simulate_optical(lk::invoke_t &cxt)
{
	LK_DOC("simulate_optical", "Simulate the baseline optical reflectance over time, including soiling and degradation. "
		"Table keys include: n_helio, n_wash_crews, wash_units_per_hour, hours_per_day, hours_per_week, replacement_threshold, "
		"soil_loss_per_hr, degr_loss_per_hr, degr_accel_per_year, n_hr_sim, rng_seed."
		, "(table:inputs):table");
	


	std::string error_msg;
	MainWindow &mw = MainWindow::Instance();
    Project* P = mw.GetProject();
	if( ! mw.GetProject()->Validate(Project::CALLING_SIM::HELIO_OPTIC, &error_msg) )
	{
		mw.Log( error_msg );
		return;
	}

    if (cxt.arg_count() > 0)
    {
	    lk::varhash_t *h = cxt.arg(0).hash();

	    if( h->find("n_helio") != h->end() )
            P->m_design_outputs.number_heliostats.assign( h->at("n_helio")->as_integer() );

	    if ( h->find("n_wash_crews") != h->end() )
            P->m_optical_outputs.n_wash_crews.assign( h->at("n_wash_crews")->as_integer() );

	    if ( h->find("wash_units_per_hour") != h->end() )
            P->m_parameters.wash_rate.assign( h->at("wash_units_per_hour")->as_number() );
	
	    if ( h->find("hours_per_day") != h->end() )
            P->m_parameters.wash_crew_max_hours_day.assign( h->at("hours_per_day")->as_number() );

	    if ( h->find("hours_per_week") != h->end() )
            P->m_parameters.wash_crew_max_hours_week.assign( h->at("hours_per_week")->as_number() );

	    if (h->find("replacement_threshold") != h->end())
            P->m_variables.degr_replace_limit.assign( h->at("replacement_threshold")->as_number() );

	    if (h->find("soil_loss_per_hr") != h->end())
            P->m_parameters.soil_per_hour.assign( h->at("soil_loss_per_hr")->as_number() );

	    if (h->find("degr_loss_per_hr") != h->end())
            P->m_parameters.degr_per_hour.assign( h->at("degr_loss_per_hr")->as_number() );

	    if (h->find("degr_accel_per_year") != h->end())
            P->m_parameters.degr_accel_per_year.assign( h->at("degr_accel_per_year")->as_number() );

	    if (h->find("n_hr_sim") != h->end())
            P->m_parameters.plant_lifetime.assign( h->at("n_hr_sim")->as_number() );

	    if (h->find("rng_seed") != h->end())
            P->m_parameters.degr_seed.assign( h->at("rng_seed")->as_integer() );
    }
    
    P->O();

	mw.SetProgress(0.);
	return;

}

void _simulate_solarfield(lk::invoke_t &cxt)
{

	LK_DOC("simulate_solarfield", "Simulates solar field availability from current project settings.", "([table:options]):table");
	
	MainWindow &mw = MainWindow::Instance();
	mw.GetProject()->M();
	mw.UpdateDataTable();
    mw.SetProgress(0.);

	return;

}

void _simulate_performance(lk::invoke_t &cxt)
{

	LK_DOC("simulate_performance", "Simulates annual performance from current project settings.", "([table:options]):table");

	MainWindow &mw = MainWindow::Instance();
	Project* P = mw.GetProject();
	P->S();
	mw.UpdateDataTable();
    mw.SetProgress(0.);
	return;

}

void _simulate_financial(lk::invoke_t &cxt)
{

	LK_DOC("simulate_financial", "Simulates financial performance from current project settings.", "([table:options]):table");

	MainWindow &mw = MainWindow::Instance();
	Project* P = mw.GetProject();
	P->E();
	P->F();
	mw.UpdateDataTable();

	return;
}

void _simulate_objective(lk::invoke_t &cxt)
{
	LK_DOC("simulate_objective", "Simulates full objective function from current project settings.", "([table:options]):table");
	MainWindow &mw = MainWindow::Instance();
    Project* P = mw.GetProject();
	P->Z();
    P->PrintCurrentResults();
	mw.UpdateDataTable();
    mw.SetProgress(0.);
    return;
}

void _setup_clusters(lk::invoke_t &cxt)
{
	LK_DOC("setup_clusters", "Set up clusters based on current project settings.", "([table:options]):table");
	MainWindow &mw = MainWindow::Instance();
	Project* P = mw.GetProject();
	P->setup_clusters();

}

void _simulate_cycle(lk::invoke_t &cxt)
{
	LK_DOC("simulate_cycle", "Simulates cycle availablity.", "([table:options]):table");
	MainWindow &mw = MainWindow::Instance();
	Project* P = mw.GetProject();
	P->C();
	mw.UpdateDataTable();
    mw.SetProgress(0.);
}


void _optimize(lk::invoke_t &cxt)
{
    LK_DOC("optimize_system", 
    "Run outer-loop optimization. Specify the variables and parameters using the 'var_info()' call. "
    "Several optimization settings may also be specified, including 'convex_flag,' 'max_delta,' and 'trust.' ",
    "([table:settings]):void");

	MainWindow &mw = MainWindow::Instance();
    Project *P = mw.GetProject();
    optimization Opt(P);

    //defaults
    Opt.m_settings.convex_flag = false;
    Opt.m_settings.max_delta = 1;
    Opt.m_settings.trust = false;
    //override if needed
    if (cxt.arg_count() > 0)
    {
        lk::varhash_t *h = cxt.arg(0).hash();

        if (h->find("convex_flag") != h->end())
            Opt.m_settings.convex_flag = h->at("convex_flag")->as_boolean();
        if (h->find("max_delta") != h->end())
            Opt.m_settings.max_delta = h->at("max_delta")->as_number();
        if (h->find("trust") != h->end())
            Opt.m_settings.trust = h->at("trust")->as_boolean();
    }

    //collect all of the variables to be optimized
    Opt.m_settings.variables.clear();

    for (variables::iterator vh = P->m_variables.begin(); vh != P->m_variables.end(); vh++)
    {
        variable *v = static_cast<variable*>(vh->second);
    
        if (v->is_optimized)
        {
            std::vector<double> inits;

            for (int i = 0; i < (int)v->initializers.vec()->size(); i++)
                inits.push_back( v->initializers.vec()->at(i).as_number() );

            Opt.m_settings.variables.push_back(optimization_variable(*v));
        }

    }


    Opt.run_optimization();

    mw.UpdateDataTable();
    mw.SetProgress(0.);
}