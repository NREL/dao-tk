
#include <lk/env.h>
#include <ssc/sscapi.h>

#include "scripting.h"
#include "project.h"
#include "daotk_app.h"

#include "../liboptical/optical_degr.h"
#include "../liboptical/optical_structures.h"
#include "../libsolar/solarfield_avail.h"
#include "../libsolar/solarfield_structures.h"



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

void _var(lk::invoke_t &cxt)
{
	LK_DOC2("var", "Sets or gets a variable value.",
		"Set a variable value.", "(string:name, variant:value):none",
		"Get a variable value", "(string:name):variant");

	Project *project = MainWindow::Instance().GetProject();

	//collect the item name
	std::string namearg = cxt.arg(0).as_string();
	data_base* dat = project->GetVarPtr(namearg.c_str());

	//is this a set or get operation?
    if (cxt.arg_count() > 1)
    {
		
	}

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


void _power_cycle(lk::invoke_t &cxt)
{
	LK_DOC("power_cycle_avail", "Simulate a power cycle failure series.", "(vector:dispatch, int:nsim):table");

	//do something
	
}

void _simulate_optical(lk::invoke_t &cxt)
{
	LK_DOC("simulate_optical", "Simulate the baseline optical reflectance over time, including soiling and degradation. "
		"Table keys include: n_helio, n_wash_crews, wash_units_per_hour, hours_per_day, hours_per_week, replacement_threshold, "
		"soil_loss_per_hr, degr_loss_per_hr, degr_accel_per_year, n_hr_sim, rng_seed."
		, "(table:inputs):table");
	
	optical_degradation OD;
	
	lk::varhash_t *h = cxt.arg(0).hash();

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




