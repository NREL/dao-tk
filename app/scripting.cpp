
#include <lk/env.h>
#include <ssc/sscapi.h>

#include "scripting.h"
#include "project.h"
#include "daotk_app.h"

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

void _var(lk::invoke_t &cxt)
{
	LK_DOC2("var", "Sets or gets a variable value.",
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
		if( cxt.arg(1).type() != dat->type )
		{
			std::string expected_type;
			std::string given_type;

			switch( dat->type )
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
		switch( dat->type )
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
		switch( dat->type )
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

	P->m_parameters.solar_resource_file.assign( "/home/mike/workspace/dao-tk/deploy/samples/USA CA Daggett Barstow-daggett Ap (TMY3).csv" );

	P->D();
	P->M();
	P->O();


	mw.Log(wxString::Format("Total field area: %.2f", P->m_design_outputs.area_sf.as_number()));
	mw.Log(wxString::Format("Number of repairs: %d", (int)P->m_solarfield_outputs.n_repairs.as_integer()));
	mw.Log(wxString::Format("Number of mirror replacements: %d", (int)P->m_optical_outputs.n_replacements.as_integer()));
	mw.Log(wxString::Format("Average soiling: %.2f", P->m_optical_outputs.avg_soil.as_number()));
	mw.Log(wxString::Format("Average degradation: %.2f", P->m_optical_outputs.avg_degr.as_number()));

	mw.SetProgress(0.);
	mw.UpdateDataTable();

	return;

}

void _generate_solarfield(lk::invoke_t &cxt)
{
	LK_DOC("generate_solarfield", "Creates a new solar field layout and geometry from current project settings.", "([table:options]):table");

	MainWindow::Instance().GetProject()->D();
	
}

void _power_cycle(lk::invoke_t &cxt)
{
	LK_DOC("power_cycle_avail", "Simulate the power cycle capacity over time, after accounting for maintenance and failures. "
		"Table keys include: cycle_power, ambient_temperature, standby, "
		"read_periods, num_periods, eps, output, num_scenarios, "
		"maintenance_interval, maintenance_duration, downtime_threshold, "
		"steplength, hours_to_maintenance, power_output, current_standby, "
		"capacity, temp_threshold, time_online, time_in_standby, downtime, "
		"shutdown_capacity, no_restart_capacity, num_condenser_trains, "
		"fans_per_train, radiators_per_train, num_salt_steam_traoins, num_fwh, "
		"num_salt_pumps, num_water_pumps, num_turbines, condenser_eff_cold, "
		"condenser_eff_hot", "(table:cycle_inputs):table");

	PowerCycle cycle;

	lk::varhash_t *h = cxt.arg(0).hash();

	// Plant attributes

	double maintenance_interval = 5000.;
	if (h->find("maintenance_interval") != h->end())
		maintenance_interval = h->at("maintenance_interval")->as_number();

	double maintenance_duration = 24.;
	if (h->find("maintenance_duration") != h->end())
		maintenance_duration = h->at("maintenance_duration")->as_number();

	double downtime_threshold = 24.;
	if (h->find("downtime_threshold") != h->end())
		downtime_threshold = h->at("downtime_threshold")->as_number();

	double steplength = 1.;
	if (h->find("steplength") != h->end())
		steplength = h->at("steplength")->as_number();

	double hours_to_maintenance = 5000.;
	if (h->find("hours_to_maintenance") != h->end())
		hours_to_maintenance = h->at("hours_to_maintenance")->as_number();

	double power_output = 0.;
	if (h->find("power_output") != h->end())
		power_output = h->at("power_output")->as_number();

	bool standby = false;
	if (h->find("current_standby") != h->end())
		standby = h->at("current_standby")->as_boolean();

	double capacity = 500000.;
	if (h->find("capacity") != h->end())
		capacity = h->at("capacity")->as_number();

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

	cycle.SetPlantAttributes(maintenance_interval, maintenance_duration,
		downtime_threshold, steplength, hours_to_maintenance, power_output,
		standby, capacity, temp_threshold, time_online, time_in_standby, downtime,
		shutdown_capacity, no_restart_capacity);

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

	int num_salt_pumps = 2;
	if (h->find("num_salt_pumps") != h->end())
		num_salt_pumps = h->at("num_salt_pumps")->as_integer();

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
		num_water_pumps,
		num_turbines,
		condenser_eff_cold,
		condenser_eff_hot
	);

	// Simulation parameters

	int read_periods = 0;
	if (h->find("read_periods") != h->end())
		read_periods = h->at("read_periods")->as_integer();

	int num_periods = 0;
	if (h->find("num_periods") != h->end())
		num_periods = h->at("num_periods")->as_integer();

	double eps = 0;
	if (h->find("eps") != h->end())
		eps = h->at("eps")->as_number();

	bool output = false;
	if (h->find("output") != h->end())
		output = h->at("output")->as_boolean();

	int num_scenarios = 0;
	if (h->find("num_scenarios") != h->end())
		num_scenarios = h->at("num_scenarios")->as_integer();

	cycle.SetSimulationParameters(read_periods, num_periods,
		eps, output, num_scenarios);

	// Dispatch parameters
	// For now, we will assume that the dispatch parameters
	// will be required inputs, i.e., no defaults.  These
	// should come from a prior run of SAM. 

	std::unordered_map<std::string, std::vector<double> > dispatch;
	dispatch["cycle_power"] = {};
	dispatch["ambient_temperature"] = {};
	dispatch["standby"] = {};
	std::vector<lk::vardata_t> *cycle_power = h->at("cycle_power")->vec();
	std::vector<lk::vardata_t> *ambient_temperature = h->at("ambient_temperature")->vec();
	std::vector<lk::vardata_t> *standby_by_hour = h->at("standby")->vec();
	lk::vardata_t q;
	for (int i = 0; i < num_periods; i++)
	{
		q = cycle_power->at(i);
		dispatch.at("cycle_power").push_back(q.as_number());
		q = ambient_temperature->at(i);
		dispatch.at("ambient_temperature").push_back(q.as_number());
		q = standby_by_hour->at(i);
		dispatch.at("standby").push_back(q.as_number());
	}
	
	cycle.SetDispatch(dispatch, true);
	WELLFiveTwelve gen(0);
	cycle.AssignGenerator(&gen);
	cycle.Simulate(true);

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

	return;

}

void _simulate_solarfield(lk::invoke_t &cxt)
{
	LK_DOC("simulate_solarfield", "Simulate the solar field availability over time due to heliostat failures. "
		"Table keys include: "
		"mean_time_to_failure, n_helio_actual, n_helio_simulated, n_om_staff, n_hr_sim, rng_seed, repair_min_downtime, "
		"rep_max_downtime, staff_productive_hr_week."
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

	SA.m_settings.mf = 12000;
	if (H->find("mean_time_to_failure") != H->end())
		SA.m_settings.mf = H->at("mean_time_to_failure")->as_integer();

	SA.m_settings.n_helio = 9264;
	if (H->find("n_helio_actual") != H->end())
		SA.m_settings.n_helio = H->at("n_helio_actual")->as_integer();

	SA.m_settings.n_helio_sim = 1000;
	if (H->find("n_helio_simulated") != H->end())
		SA.m_settings.n_helio_sim = H->at("n_helio_simulated")->as_integer();

	SA.m_settings.n_om_staff = 5;
	if (H->find("n_om_staff") != H->end())
		SA.m_settings.n_om_staff = H->at("n_om_staff")->as_integer();

	SA.m_settings.n_hr_sim = 105120;
	if (H->find("n_hr_sim") != H->end())
		SA.m_settings.n_hr_sim = H->at("n_hr_sim")->as_integer();

	SA.m_settings.seed = 123;
	if (H->find("rng_seed") != H->end())
		SA.m_settings.seed = H->at("rng_seed")->as_integer();

	SA.m_settings.rep_min = 1; //[hr]
	if (H->find("repair_min_downtime") != H->end())
		SA.m_settings.rep_min = H->at("repair_min_downtime")->as_number();

	SA.m_settings.rep_max = 100; //[hr]
	if (H->find("rep_max_downtime") != H->end())
		SA.m_settings.rep_max = H->at("rep_max_downtime")->as_number();

	SA.m_settings.hr_prod = 35;
	if (H->find("staff_productive_hr_week") != H->end())
		SA.m_settings.hr_prod = H->at("staff_productive_hr_week")->as_number();
	
	SA.simulate();

	return;

}

void _simulate_performance(lk::invoke_t &cxt)
{

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

	//--- User inputs for clustering
	P->m_cluster_parameters.initialize();

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
			P->m_cluster_parameters.alg = KMEANS;
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
	
	mw.UpdateDataTable();

	return;

}