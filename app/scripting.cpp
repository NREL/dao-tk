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
		case SSC_NOTICE: msg << "Notice: " << s0 << " time " << f1; break;
		case SSC_WARNING: msg << "Warning: " << s0 << " time " << f1; break;
		case SSC_ERROR: msg << "Error: " << s0 << " time " << f1; break;
		default: msg << "Log notice uninterpretable: " << f0 << " time " << f1; break;
		}
		
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
	MainWindow::Instance().Log(msg);
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
	
	P->m_variables.h_tower.assign( 100. );
	P->m_variables.rec_height.assign( 15. );
	P->m_variables.D_rec.assign( 12. );
	P->m_variables.design_eff.assign( .41 );
	P->m_variables.dni_des.assign( 950. );
	P->m_variables.P_ref.assign( 25. );
	P->m_variables.solarm.assign( 2.4 );
	P->m_variables.tshours.assign( 10. );
	P->m_variables.degr_replace_limit.assign( .7 );
	P->m_variables.om_staff.assign( 5 );
	P->m_variables.n_wash_crews.assign( 3 );
	P->m_variables.N_panels.assign( 16 );

	//P->m_parameters.solar_resource_file.assign( "/home/mike/workspace/dao-tk/deploy/samples/USA CA Daggett Barstow-daggett Ap (TMY3).csv" );
	P->m_parameters.solar_resource_file.assign( "C:/Users/AZOLAN/Documents/GitHub/daotk_dev/dao-tk/deploy/samples/USA CA Daggett Barstow-daggett Ap (TMY3).csv" );

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
	P->D();
	P->M();
	P->O();
	P->C();
	
	mw.Log(wxString::Format("Total field area: %.2f", P->m_design_outputs.area_sf.as_number()));
	mw.Log(wxString::Format("Number of repairs: %d", (int)P->m_solarfield_outputs.n_repairs.as_integer()));
	mw.Log(wxString::Format("Number of mirror replacements: %d", (int)P->m_optical_outputs.n_replacements.as_integer()));
	mw.Log(wxString::Format("Average soiling: %.2f", P->m_optical_outputs.avg_soil.as_number()));
	mw.Log(wxString::Format("Average degradation: %.2f", P->m_optical_outputs.avg_degr.as_number()));
	mw.Log(wxString::Format("Average cycle repair labor costs: %.2f", P->m_cycle_outputs.cycle_labor_cost.as_number()));
	mw.Log(wxString::Format("Number of failed components: %d", P->m_cycle_outputs.num_failures.as_integer()));


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
		"num_salt_pumps_required, num_water_pumps, num_turbines,"
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

	int num_salt_pumps = 4;
	if (h->find("num_salt_pumps") != h->end())
		num_salt_pumps = h->at("num_salt_pumps")->as_integer();

	int num_salt_pumps_required = 3;
	if (h->find("num_salt_pumps_required") != h->end())
		num_salt_pumps_required = h->at("num_salt_pumps_required")->as_integer();

	int num_water_pumps = 2;
	if (h->find("num_water_pumps") != h->end())
		num_water_pumps = h->at("num_water_pumps")->as_integer();

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

	//mw.Log(wxString::Format(""));
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
	
	optical_degradation OD;
	
	lk::varhash_t *h = cxt.arg(0).hash();

	std::string error_msg;
	MainWindow &mw = MainWindow::Instance();
	if( ! mw.GetProject()->Validate(Project::CALLING_SIM::HELIO_OPTIC, &error_msg) )
	{
		mw.Log( error_msg );
		return;
	}

	OD.m_settings.n_helio = 8000;
	if( h->find("n_helio") != h->end() )
		OD.m_settings.n_helio = h->at("n_helio")->as_integer();

	OD.m_settings.n_wash_crews = 3;
	if ( h->find("n_wash_crews") != h->end() )
		OD.m_settings.n_wash_crews = h->at("n_wash_crews")->as_integer();

	OD.m_settings.wash_units_per_hour = 45;
	if ( h->find("wash_units_per_hour") != h->end() )
		OD.m_settings.wash_units_per_hour = h->at("wash_units_per_hour")->as_number();
	
	OD.m_settings.hours_per_day = 10.;
	if ( h->find("hours_per_day") != h->end() )
		OD.m_settings.hours_per_day = h->at("hours_per_day")->as_number();

	OD.m_settings.hours_per_week = 70;
	if ( h->find("hours_per_week") != h->end() )
		OD.m_settings.hours_per_week = h->at("hours_per_week")->as_number();

	OD.m_settings.replacement_threshold = 0.771;
	if (h->find("replacement_threshold") != h->end())
		OD.m_settings.replacement_threshold = h->at("replacement_threshold")->as_number();

	OD.m_settings.soil_loss_per_hr = 1.e-4;
	if (h->find("soil_loss_per_hr") != h->end())
		OD.m_settings.soil_loss_per_hr = h->at("soil_loss_per_hr")->as_number();

	OD.m_settings.degr_loss_per_hr = 1.e-7;
	if (h->find("degr_loss_per_hr") != h->end())
		OD.m_settings.degr_loss_per_hr = h->at("degr_loss_per_hr")->as_number();

	OD.m_settings.degr_accel_per_year = 0.125;
	if (h->find("degr_accel_per_year") != h->end())
		OD.m_settings.degr_accel_per_year = h->at("degr_accel_per_year")->as_number();

	OD.m_settings.n_hr_sim = 219000;
	if (h->find("n_hr_sim") != h->end())
		OD.m_settings.n_hr_sim = h->at("n_hr_sim")->as_number();

	OD.m_settings.seed = 123;
	if (h->find("rng_seed") != h->end())
		OD.m_settings.seed = h->at("rng_seed")->as_integer();

	OD.simulate();

	mw.SetProgress(0.);
	return;

}

void _simulate_solarfield(lk::invoke_t &cxt)
{

	LK_DOC("simulate_solarfield", "Simulates solar field availability from current project settings.", "([table:options]):table");
	
	MainWindow &mw = MainWindow::Instance();
	mw.GetProject()->M();
	mw.UpdateDataTable();

	/*
	LK_DOC("simulate_solarfield", "Simulate the solar field availability over time due to heliostat failures. "
		"Table keys include: "
		"weibull_shape_param, weibull_scale_param, rep_mean_downtime, rep_min_downtime, rep_max_downtime, rep_good_as_new,"
		"n_helio_simulated, n_om_staff, n_year_sim, rng_seed, staff_productive_hr_week, model_time_step, repair_order."
		, "(table:inputs):table");

	solarfield_availability SA;

	lk::varhash_t *H = cxt.arg(0).hash();
	
	
	std::string error_msg;
	MainWindow &mw = MainWindow::Instance();

	if( ! mw.GetProject()->Validate(Project::CALLING_SIM::HELIO_AVAIL, &error_msg) )
	{
		mw.Log( error_msg );
		return;
	}

	SA.m_settings.n_helio = mw.GetProject()->m_design_outputs.number_heliostats.as_integer();
	
	std::string weatherfile = mw.GetProject()->m_parameters.solar_resource_file.as_string();
	s_location loc(weatherfile);
	SA.m_settings.location = loc;

	SA.m_settings.n_helio_sim = 8000;
	if (H->find("n_helio_simulated") != H->end())
		SA.m_settings.n_helio_sim = H->at("n_helio_simulated")->as_integer();

	SA.m_settings.n_years = 30;
	if (H->find("n_year_sim") != H->end())
		SA.m_settings.n_years = H->at("n_year_sim")->as_integer();

	double om_staff = 5;
	if (H->find("n_om_staff") != H->end())
		om_staff = H->at("n_om_staff")->as_integer();
	SA.m_settings.n_om_staff.assign(SA.m_settings.n_years, om_staff);

	SA.m_settings.seed = 123;
	if (H->find("rng_seed") != H->end())
		SA.m_settings.seed = H->at("rng_seed")->as_integer();


	SA.m_settings.max_hours_per_week = 35;
	if (H->find("staff_productive_hr_week") != H->end())
		SA.m_settings.max_hours_per_week = H->at("staff_productive_hr_week")->as_number();

	SA.m_settings.step = 4.;
	if (H->find("model_time_step") != H->end())
		SA.m_settings.step = H->at("model_time_step")->as_number();


	//-- Heliostat component inputs
	double beta = 1.0;
	if (H->find("weibull_shape_param") != H->end())
		beta = H->at("weibull_shape_param")->as_integer();

	double eta = 12000; //[hr]
	if (H->find("weibull_scale_param") != H->end())
		eta = H->at("weibull_scale_param")->as_integer();

	double rep_mean = 2.;
	if (H->find("rep_mean_downtime") != H->end())
		rep_mean = H->at("rep_mean_downtime")->as_number();

	double rep_min = 1; //[hr]
	if (H->find("rep_min_downtime") != H->end())
		rep_min = H->at("rep_min_downtime")->as_number();

	double rep_max = 100; //[hr]
	if (H->find("rep_max_downtime") != H->end())
		rep_max = H->at("rep_max_downtime")->as_number();

	bool good_as_new = true; 
	if (H->find("rep_good_as_new") != H->end())
		good_as_new = H->at("rep_good_as_new")->as_boolean();

	helio_component_inputs component(beta, eta, rep_mean, rep_min, rep_max, good_as_new, 0.0);
	SA.m_settings.helio_components.push_back(component);

	//-- Repair ordering
	SA.m_settings.repair_order = MEAN_REPAIR_TIME;

	if (H->find("repair_order") != H->end())
	{
		std::string ro = H->at("repair_order")->as_string();

		if (ro == "failure_order")
			SA.m_settings.repair_order = FAILURE_ORDER;
		else if (ro == "performance")
			SA.m_settings.repair_order = PERFORMANCE;
		else if (ro == "repair_time")
			SA.m_settings.repair_order = REPAIR_TIME;
		else if (ro == "mean_repair_time")
			SA.m_settings.repair_order = MEAN_REPAIR_TIME;
		else if (ro == "random")
			SA.m_settings.repair_order = RANDOM;
		else
			mw.Log("Specified repair order not recognized. Valid inputs are 'failure_order', 'performance', 'repair_time', 'mean_repair_time', 'random'");
	}


	SA.m_settings.helio_performance.assign(SA.m_settings.n_helio, 1.0);
	SA.simulate();

	mw.SetProgress(0.);
	*/

	return;

}

void _simulate_performance(lk::invoke_t &cxt)
{

	LK_DOC("simulate_performance", "Simulates annual performance from current project settings.", "([table:options]):table");

	MainWindow &mw = MainWindow::Instance();
	Project* P = mw.GetProject();

	std::string error_msg;
	if (!P->Validate(Project::CALLING_SIM::SIMULATION, &error_msg))
	{
		mw.Log(error_msg);
		return;
	}
	P->S();
	mw.UpdateDataTable();

	/*
	LK_DOC("simulate_performance", "Test creation/simulation of clusters."
		"Table keys for clustering include: "
		"is_clustering_used, n_cluster, n_sim_days, n_prev, weather_file, price_file, "
		"sfavail_file, algorithm, hard_partitions, is_run_continuous, "
		"is_run_full"
		, "(table:cluster_options):table");


	lk::varhash_t *H = cxt.arg(0).hash();

	MainWindow &mw = MainWindow::Instance();

	Project* P = mw.GetProject();

	std::string error_msg;
	if( ! P->Validate(Project::CALLING_SIM::SIMULATION, &error_msg) )
	{
		mw.Log( error_msg );
		return;
	}

	// P->m_variables.h_tower.assign( 180. );
	// P->m_variables.rec_height.assign( 17. );
	// P->m_variables.D_rec.assign( 15. );
	// P->m_variables.design_eff.assign( .41 );
	// P->m_variables.dni_des.assign( 950. );
	// P->m_variables.P_ref.assign( 100. );
	// P->m_variables.solarm.assign( 2.1 );
	// P->m_variables.tshours.assign( 10. );
	// P->m_variables.degr_replace_limit.assign( .7 );
	// P->m_variables.om_staff.assign( 5 );
	// P->m_variables.n_wash_crews.assign( 3 );
	// P->m_variables.N_panels.assign( 16 );

	// P->m_parameters.is_dispatch.assign( 1. );
	// P->m_parameters.solar_resource_file.assign( "/home/mike/workspace/dao-tk/deploy/samples/clustering/2015_weather.csv" );	
	// std::string price_file = "/home/mike/workspace/dao-tk/deploy/samples/clustering/2015_price.csv";


	if (H->find("weather_file") != H->end())
		P->m_parameters.solar_resource_file.assign( H->at("weather_file")->as_string() );

	std::string price_file = "";
	if (H->find("price_file") != H->end())
		price_file = H->at("price_file")->as_string();

	std::string  sfavail_file = "";
	if (H->find("sfavail_file") != H->end() )
		sfavail_file = H->at("sfavail_file")->as_string();

	if (H->find("is_run_full") != H->end())
		P->m_cluster_parameters.is_run_full = H->at("is_run_full")->as_boolean();
	else 
		P->m_cluster_parameters.is_run_full = true;

	if( P->m_cluster_parameters.is_run_full )
	{
		if (H->find("n_cluster") != H->end())
			P->m_cluster_parameters.ncluster = H->at("n_cluster")->as_integer();
		else
			P->m_cluster_parameters.ncluster = 40;

		if (H->find("n_sim_days") != H->end())
			P->m_cluster_parameters.nsim = H->at("n_sim_days")->as_integer();
		else
			P->m_cluster_parameters.nsim = 2;

		if (H->find("n_prev") != H->end())
			P->m_cluster_parameters.nprev = H->at("n_prev")->as_integer();
		else
			P->m_cluster_parameters.nprev = 1;

		if (H->find("hard_partitions") != H->end())
			P->m_cluster_parameters.hard_partitions = H->at("hard_partitions")->as_boolean();

		if (H->find("is_run_continuous") != H->end())
			P->m_cluster_parameters.is_run_continuous = H->at("is_run_continuous")->as_boolean();

		if (H->find("algorithm") != H->end())
		{
			std::string algorithm = H->at("algorithm")->as_string();

			if (algorithm == "affinity propagation")
				P->m_cluster_parameters.alg = AFFINITY_PROPAGATION;
			else if (algorithm == "kmeans")
				P->m_cluster_parameters.alg = KMEANS;
			else if (algorithm == "random")
				P->m_cluster_parameters.alg = RANDOM_SELECTION;
			else
				mw.Log("Specified clustering algorithm not recognized");
		}
		else
			P->m_cluster_parameters.alg = AFFINITY_PROPAGATION;
	}

	//--- Price array from price file
	std::vector<double> price_data;
	if( ! price_file.empty() )
	{
		FILE *fp = fopen(price_file.c_str(), "r");
		if (fp == NULL)
		{
			mw.Log(wxString::Format("Failed to open price file"));
			return;
		}
		char *line, *record;
		char buffer[1024];
		while ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)
		{
			record = strtok(line, ",");
			price_data.push_back(atof(record));
		}
		fclose(fp);
	}
	else
	{
		//use default vector of 1.
		mw.Log("No price file specified. Using default unity vector.");
		price_data.resize(8760, 1.);
	}
	
	P->m_parameters.dispatch_factors_ts.assign_vector( price_data );


	//--- solar field availability 
	std::vector<double> sfavail;
	if( ! sfavail_file.empty() )
	{
		char *line, *record;
		char buffer[1024];
		FILE *fp = fopen(sfavail_file.c_str(), "r");
		while ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)
		{
			record = strtok(line, ",");
			sfavail.push_back(atof(record));
		}
		fclose(fp);
	}
	else
	{
		sfavail.resize(8760, 1.);
	}
	P->m_parameters.user_sf_avail.assign_vector( sfavail );
	

	//--- Run simulation
	P->S();
	
	mw.SetProgress(0.);
	mw.UpdateDataTable();
	*/
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

	mw.UpdateDataTable();

	/*
	mw.Log(wxString::Format("Mirror replacements per year: %0.1f ", P->m_optical_outputs.n_replacements.as_number()));
	mw.Log(wxString::Format("Heliostat repairs per year: %0.1f ", P->m_solarfield_outputs.n_repairs.as_number()));
	mw.Log(wxString::Format("Average degradation loss: %0.3f ", P->m_optical_outputs.avg_degr.as_number()));
	mw.Log(wxString::Format("Average soiling loss: %0.3f ", P->m_optical_outputs.avg_soil.as_number()));
	mw.Log(wxString::Format("Average availability loss: %0.3f ", P->m_solarfield_outputs.avg_avail.as_number()));	

	mw.Log(wxString::Format("Average generation (GWhe): %0.2f ", P->m_simulation_outputs.annual_generation.as_number()));
	mw.Log(wxString::Format("Receiver starts: %d ", P->m_simulation_outputs.annual_rec_starts.as_integer()));
	mw.Log(wxString::Format("Cycle starts: %d ", P->m_simulation_outputs.annual_cycle_starts.as_integer()));
	mw.Log(wxString::Format("Total cycle ramping (GW): %0.2f ", P->m_simulation_outputs.annual_cycle_ramp.as_number()));

	mw.Log(wxString::Format("Heliostat cost ($): %0.0f ", P->m_objective_outputs.cost_sf_real.as_number()));
	mw.Log(wxString::Format("Land cost ($): %0.0f ", P->m_objective_outputs.cost_land_real.as_number()));
	mw.Log(wxString::Format("Tower cost ($): %0.0f ", P->m_objective_outputs.cost_tower_real.as_number()));
	mw.Log(wxString::Format("Receiver cost ($): %0.0f ", P->m_objective_outputs.cost_receiver_real.as_number()));
	mw.Log(wxString::Format("TES cost ($): %0.0f ", P->m_objective_outputs.cost_tes_real.as_number()));
	mw.Log(wxString::Format("Plant cost ($): %0.0f ", P->m_objective_outputs.cost_plant_real.as_number()));

	mw.Log(wxString::Format("Cycle ramp cost ($): %0.0f ", P->m_objective_outputs.cycle_ramp_cost_real.as_number()));
	mw.Log(wxString::Format("Cycle start cost ($): %0.0f ", P->m_objective_outputs.cycle_start_cost_real.as_number()));
	mw.Log(wxString::Format("Receiver start cost ($): %0.0f ", P->m_objective_outputs.rec_start_cost_real.as_number()));

	mw.Log(wxString::Format("Heliostat O&M labor cost ($): %0.0f ", P->m_objective_outputs.heliostat_om_labor_real.as_number()));
	mw.Log(wxString::Format("Heliostat repair cost ($): %0.0f ", P->m_objective_outputs.heliostat_repair_cost_real.as_number()));
	mw.Log(wxString::Format("Heliostat wash labor cost ($): %0.0f ", P->m_objective_outputs.heliostat_wash_cost_real.as_number()));
	mw.Log(wxString::Format("Heliostat refurbish cost ($): %0.0f ", P->m_objective_outputs.heliostat_refurbish_cost_real.as_number()));

	mw.Log(wxString::Format("Total Capital costs ($): %0.0f ", P->m_objective_outputs.cap_cost_real.as_number()));
	mw.Log(wxString::Format("Total O&M costs ($): %0.0f ", P->m_objective_outputs.om_cost_real.as_number()));
	mw.Log(wxString::Format("Sales ($): %0.0f ", P->m_objective_outputs.sales.as_number()));

	mw.Log(wxString::Format("Total cash flow ($): %0.0f ", P->m_objective_outputs.cash_flow.as_number()));
	mw.Log(wxString::Format("PPA price (c/kWhe): %0.3f ", P->m_objective_outputs.ppa.as_number()));
	*/

	return;
}

void _setup_clusters(lk::invoke_t &cxt)
{
	LK_DOC("setup_clusters", "Set up clusters based on current project settings.", "([table:options]):table");
	MainWindow &mw = MainWindow::Instance();
	Project* P = mw.GetProject();
	P->setup_clusters();

}

void _optimize(lk::invoke_t &cxt)
{
    LK_DOC("optimize_system", 
    "Run outer-loop optimization. Specify the variables to optimize in the options table, "
    "optionally, along with upper bounds, lower bounds, and initial value(s) to sample. "
    "Note that the sample values will be tested for each variable in the order that they are specified, "
    "and the length of the sample list must be the same for all variables. "
    "For example:\n\n"
    "my_opt_vars = {\n\t \"n_wash_crews\" = {\"upper_bound\" = 15, \"lower_bound\" = 1, \"guess\" = [5,10]},\n\t"
    "\"h_tower\" = { \"upper_bound\" = 250, \"lower_bound\" = 50, \"guess\"=[175,155]}, \n\t...\t};\rresult = optimize_system(options=my_opt_vars);\n\n"
    "Several optimization settings may also be specified, including 'convex_flag,' 'max_delta,' and 'trust.' ",
    "([table:options, table:settings]):table");

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


    ////npanel, nom, nwash
    //Opt.m_settings.lower_bounds = std::vector<int>{ -6,-6 };
    //Opt.m_settings.upper_bounds = std::vector<int>{ 2, 2 };
    //Opt.m_settings.X_sample = std::vector< std::vector<int> >
    //{
    //    std::vector<int>{1,0},
    //    std::vector<int>{0,1},
    //    std::vector<int>{-1,0},
    //    std::vector<int>{0,-1},
    //    std::vector<int>{0,0}
    //};
    //
    Opt.run_optimization();

    optimization_outputs* oo = &mw.GetProject()->m_optimization_outputs;

    int n, m;

    /*oo->eta_i.empty_vector();
    n = (int)Opt.m_results.eta_i.size();
    m = (int)Opt.m_results.eta_i.front().size();
    for (int i = 0; i < n; i++)
    {
        lk::vardata_t row;
        row.empty_vector();
        for (int j = 0; j < m; j++)
            row.vec_append(Opt.m_results.eta_i.at(i).at(j));
        oo->eta_i.vec()->push_back(row);
    }*/

}