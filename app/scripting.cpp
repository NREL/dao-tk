
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

ssc_bool_t my_handler( ssc_module_t , ssc_handler_t , int action, 
	float f0, float f1, const char *s0, const char *, void * )
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
/*
	else if (action == SSC_EXECUTE)
	{
		// run the executable, pipe the output, and return output to p_mod
		// **TODO**
		default_sync_proc exe( p_handler );
		return exe.spawn( s0, s1 ) == 0;
	}
*/
	else
		return 0;
}



void _test(lk::invoke_t &cxt)
{
	LK_DOC("test", "Test description.", "(void):null");

	// MainWindow::Instance().Log()
	MainWindow &mw = MainWindow::Instance();

	Project P;
	P.initialize_ssc_project();

	
	mw.Log( wxString::Format( "SSC version = %d\n", ssc_version() ) );
	mw.Log( wxString::Format( "SSC build information = %s\n", ssc_build_info() ) );
	ssc_module_exec_set_print(0);
	ssc_data_t data = ssc_data_create();
	if (data == NULL)
	{
		mw.Log( wxString::Format( "error: out of memory." ) );
		return;
	}
	ssc_module_t module;

	ssc_data_set_string( data, "solar_resource_file", "../deploy/samples/USA CA Daggett Barstow-daggett Ap (TMY3).csv" );
	ssc_data_set_number( data, "ppa_multiplier_model", 0 );
	set_array( data, "dispatch_factors_ts", "../deploy/samples/dispatch_factors_ts.csv", 8760);
	ssc_data_set_number( data, "field_model_type", 2 );
	ssc_data_set_number( data, "gross_net_conversion_factor", 0.89999997615814209 );
	ssc_data_set_number( data, "helio_width", 12.199999809265137 );
	ssc_data_set_number( data, "helio_height", 12.199999809265137 );
	ssc_data_set_number( data, "helio_optical_error_mrad", 1.5299999713897705 );
	ssc_data_set_number( data, "helio_active_fraction", 0.99000000953674316 );
	ssc_data_set_number( data, "dens_mirror", 0.97000002861022949 );
	ssc_data_set_number( data, "helio_reflectance", 0.89999997615814209 );
	ssc_data_set_number( data, "rec_absorptance", 0.93999999761581421 );
	ssc_data_set_number( data, "rec_hl_perm2", 30 );
	ssc_data_set_number( data, "land_max", 9.5 );
	ssc_data_set_number( data, "land_min", 0.75 );
	ssc_data_set_number( data, "dni_des", 950 );
	ssc_data_set_number( data, "p_start", 0.02500000037252903 );
	ssc_data_set_number( data, "p_track", 0.054999999701976776 );
	ssc_data_set_number( data, "hel_stow_deploy", 8 );
	ssc_data_set_number( data, "v_wind_max", 15 );
	ssc_data_set_number( data, "c_atm_0", 0.0067889997735619545 );
	ssc_data_set_number( data, "c_atm_1", 0.10459999740123749 );
	ssc_data_set_number( data, "c_atm_2", -0.017000000923871994 );
	ssc_data_set_number( data, "c_atm_3", 0.002845000009983778 );
	ssc_data_set_number( data, "n_facet_x", 2 );
	ssc_data_set_number( data, "n_facet_y", 8 );
	ssc_data_set_number( data, "focus_type", 1 );
	ssc_data_set_number( data, "cant_type", 1 );
	ssc_data_set_number( data, "n_flux_days", 8 );
	ssc_data_set_number( data, "delta_flux_hrs", 2 );
	ssc_data_set_number( data, "water_usage_per_wash", 0.69999998807907104 );
	ssc_data_set_number( data, "washing_frequency", 63 );
	ssc_data_set_number( data, "check_max_flux", 0 );
	ssc_data_set_number( data, "sf_excess", 1 );
	ssc_data_set_number( data, "tower_fixed_cost", 3000000 );
	ssc_data_set_number( data, "tower_exp", 0.011300000362098217 );
	ssc_data_set_number( data, "rec_ref_cost", 103000000 );
	ssc_data_set_number( data, "rec_ref_area", 1571 );
	ssc_data_set_number( data, "rec_cost_exp", 0.69999998807907104 );
	ssc_data_set_number( data, "site_spec_cost", 16 );
	ssc_data_set_number( data, "heliostat_spec_cost", 145 );
	ssc_data_set_number( data, "plant_spec_cost", 1100 );
	ssc_data_set_number( data, "bop_spec_cost", 340 );
	ssc_data_set_number( data, "tes_spec_cost", 24 );
	ssc_data_set_number( data, "land_spec_cost", 10000 );
	ssc_data_set_number( data, "contingency_rate", 7 );
	ssc_data_set_number( data, "sales_tax_rate", 5 );
	ssc_data_set_number( data, "sales_tax_frac", 80 );
	ssc_data_set_number( data, "cost_sf_fixed", 0 );
	ssc_data_set_number( data, "fossil_spec_cost", 0 );
	ssc_data_set_number( data, "flux_max", 1000 );
	ssc_data_set_number( data, "opt_init_step", 0.059999998658895493 );
	ssc_data_set_number( data, "opt_max_iter", 200 );
	ssc_data_set_number( data, "opt_conv_tol", 0.0010000000474974513 );
	ssc_data_set_number( data, "opt_flux_penalty", 0.25 );
	ssc_data_set_number( data, "opt_algorithm", 1 );
	ssc_data_set_number( data, "csp.pt.cost.epc.per_acre", 0 );
	ssc_data_set_number( data, "csp.pt.cost.epc.percent", 13 );
	ssc_data_set_number( data, "csp.pt.cost.epc.per_watt", 0 );
	ssc_data_set_number( data, "csp.pt.cost.epc.fixed", 0 );
	ssc_data_set_number( data, "csp.pt.cost.plm.percent", 0 );
	ssc_data_set_number( data, "csp.pt.cost.plm.per_watt", 0 );
	ssc_data_set_number( data, "csp.pt.cost.plm.fixed", 0 );
	ssc_data_set_number( data, "csp.pt.sf.fixed_land_area", 45 );
	ssc_data_set_number( data, "csp.pt.sf.land_overhead_factor", 1 );
	ssc_data_set_number( data, "T_htf_cold_des", 290 );
	ssc_data_set_number( data, "T_htf_hot_des", 574 );
	ssc_data_set_number( data, "P_ref", 115 );
	ssc_data_set_number( data, "design_eff", 0.41200000047683716 );
	ssc_data_set_number( data, "tshours", 10 );
	ssc_data_set_number( data, "solarm", 2.4000000953674316 );
	ssc_data_set_number( data, "N_panels", 20 );
	ssc_data_set_number( data, "d_tube_out", 40 );
	ssc_data_set_number( data, "th_tube", 1.25 );
	ssc_data_set_number( data, "mat_tube", 2 );
	ssc_data_set_number( data, "rec_htf", 17 );
	ssc_number_t p_field_fl_props[9] ={ 1, 7, 0, 0, 0, 0, 0, 0, 0 };
	ssc_data_set_matrix( data, "field_fl_props", p_field_fl_props, 1, 9 );
	ssc_data_set_number( data, "Flow_type", 1 );
	ssc_data_set_number( data, "epsilon", 0.87999999523162842 );
	ssc_data_set_number( data, "hl_ffact", 1 );
	ssc_data_set_number( data, "f_rec_min", 0.25 );
	ssc_data_set_number( data, "rec_su_delay", 0.20000000298023224 );
	ssc_data_set_number( data, "rec_qf_delay", 0.25 );
	ssc_data_set_number( data, "csp.pt.rec.max_oper_frac", 1.2000000476837158 );
	ssc_data_set_number( data, "eta_pump", 0.85000002384185791 );
	ssc_data_set_number( data, "piping_loss", 10200 );
	ssc_data_set_number( data, "piping_length_mult", 2.5999999046325684 );
	ssc_data_set_number( data, "piping_length_const", 0 );
	ssc_data_set_number( data, "csp.pt.tes.init_hot_htf_percent", 30 );
	ssc_data_set_number( data, "h_tank", 20 );
	ssc_data_set_number( data, "cold_tank_max_heat", 15 );
	ssc_data_set_number( data, "u_tank", 0.40000000596046448 );
	ssc_data_set_number( data, "tank_pairs", 1 );
	ssc_data_set_number( data, "cold_tank_Thtr", 280 );
	ssc_data_set_number( data, "h_tank_min", 1 );
	ssc_data_set_number( data, "hot_tank_Thtr", 500 );
	ssc_data_set_number( data, "hot_tank_max_heat", 30 );
	ssc_data_set_number( data, "pc_config", 0 );
	ssc_data_set_number( data, "pb_pump_coef", 0.55000001192092896 );
	ssc_data_set_number( data, "startup_time", 0.5 );
	ssc_data_set_number( data, "startup_frac", 0.5 );
	ssc_data_set_number( data, "cycle_max_frac", 1.0499999523162842 );
	ssc_data_set_number( data, "cycle_cutoff_frac", 0.20000000298023224 );
	ssc_data_set_number( data, "q_sby_frac", 0.20000000298023224 );
	ssc_data_set_number( data, "dT_cw_ref", 10 );
	ssc_data_set_number( data, "T_amb_des", 42 );
	ssc_data_set_number( data, "P_boil", 100 );
	ssc_data_set_number( data, "CT", 2 );
	ssc_data_set_number( data, "T_approach", 5 );
	ssc_data_set_number( data, "T_ITD_des", 16 );
	ssc_data_set_number( data, "P_cond_ratio", 1.0027999877929688 );
	ssc_data_set_number( data, "pb_bd_frac", 0.019999999552965164 );
	ssc_data_set_number( data, "P_cond_min", 2 );
	ssc_data_set_number( data, "n_pl_inc", 8 );
	ssc_number_t p_F_wc[9] ={ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ssc_data_set_array( data, "F_wc", p_F_wc, 9 );
	ssc_data_set_number( data, "tech_type", 1 );
	ssc_data_set_number( data, "ud_T_amb_des", 43 );
	ssc_data_set_number( data, "ud_f_W_dot_cool_des", 0 );
	ssc_data_set_number( data, "ud_m_dot_water_cool_des", 0 );
	ssc_data_set_number( data, "ud_T_htf_low", 500 );
	ssc_data_set_number( data, "ud_T_htf_high", 580 );
	ssc_data_set_number( data, "ud_T_amb_low", 0 );
	ssc_data_set_number( data, "ud_T_amb_high", 55 );
	ssc_data_set_number( data, "ud_m_dot_htf_low", 0.30000001192092896 );
	ssc_data_set_number( data, "ud_m_dot_htf_high", 1.2000000476837158 );
	ssc_number_t p_ud_T_htf_ind_od[260] ={ 500, 0.14323285222053528, 0.68514901399612427, 0.76894146203994751, 0.22183097898960114, 0.73943662643432617, 0.88732391595840454, 1, 1, 1, 1, 1, 1, 504.21054077148438, 0.14680744707584381, 0.70224791765213013, 0.78813153505325317, 0.22627872228622437, 0.75426238775253296, 0.90511488914489746, 1, 1, 1, 1, 1, 1, 508.42105102539063, 0.15040385723114014, 0.7194511890411377, 0.80743879079818726, 0.2307264655828476, 0.76908820867538452, 0.92290586233139038, 1, 1, 1, 1, 1, 1, 512.631591796875, 0.15402184426784515, 0.73675769567489624, 0.82686179876327515, 0.23517420887947083, 0.78391402959823608, 0.9406968355178833, 1, 1, 1, 1, 1, 1, 516.84210205078125, 0.15766115486621857, 0.75416624546051025, 0.84639936685562134, 0.23962193727493286, 0.79873979091644287, 0.95848774909973145, 1, 1, 1, 1, 1, 1, 521.0526123046875, 0.16132153570652008, 0.77167552709579468, 0.86605000495910645, 0.24406968057155609, 0.81356561183929443, 0.97627872228622437, 1, 1, 1, 1, 1, 1, 525.26318359375, 0.16500274837017059, 0.78928446769714355, 0.88581246137619019, 0.24851742386817932, 0.82839137315750122, 0.99406969547271729, 1, 1, 1, 1, 1, 1, 529.47369384765625, 0.168704554438591, 0.80699187517166138, 0.90568548440933228, 0.25296515226364136, 0.84321719408035278, 1.0118606090545654, 1, 1, 1, 1, 1, 1, 533.6842041015625, 0.17242671549320221, 0.82479673624038696, 0.92566782236099243, 0.25741291046142578, 0.85804301500320435, 1.0296516418457031, 1, 1, 1, 1, 1, 1, 537.89471435546875, 0.1761690080165863, 0.84269779920578003, 0.9457581639289856, 0.26186063885688782, 0.87286877632141113, 1.0474425554275513, 1, 1, 1, 1, 1, 1, 542.10528564453125, 0.17993119359016418, 0.86069411039352417, 0.96595537662506104, 0.26630836725234985, 0.8876945972442627, 1.0652334690093994, 1, 1, 1, 1, 1, 1, 546.3157958984375, 0.18371304869651794, 0.87878447771072388, 0.98625820875167847, 0.27075612545013428, 0.90252035856246948, 1.0830245018005371, 1, 1, 1, 1, 1, 1, 550.52630615234375, 0.18751437962055206, 0.89696800708770752, 1.0066654682159424, 0.27520385384559631, 0.91734617948532104, 1.1008154153823853, 1, 1, 1, 1, 1, 1, 554.73681640625, 0.19133494794368744, 0.91524350643157959, 1.0271761417388916, 0.27965158224105835, 0.93217200040817261, 1.1186063289642334, 1, 1, 1, 1, 1, 1, 558.9473876953125, 0.19517454504966736, 0.93361008167266846, 1.0477888584136963, 0.28409934043884277, 0.94699776172637939, 1.1363973617553711, 1, 1, 1, 1, 1, 1, 563.15789794921875, 0.19903296232223511, 0.95206665992736816, 1.06850266456604, 0.28854706883430481, 0.96182358264923096, 1.1541882753372192, 1, 1, 1, 1, 1, 1, 567.368408203125, 0.20290999114513397, 0.97061234712600708, 1.0893163681030273, 0.29299479722976685, 0.97664934396743774, 1.1719791889190674, 1, 1, 1, 1, 1, 1, 571.57891845703125, 0.20680543780326843, 0.98924607038497925, 1.1102290153503418, 0.29744255542755127, 0.99147516489028931, 1.1897702217102051, 1, 1, 1, 1, 1, 1, 575.78948974609375, 0.21071910858154297, 1.0079669952392578, 1.1312394142150879, 0.30189028382301331, 1.0063009262084961, 1.2075611352920532, 1, 1, 1, 1, 1, 1, 580, 0.21465080976486206, 1.0267740488052368, 1.1523466110229492, 0.30633804202079773, 1.0211267471313477, 1.2253521680831909, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_matrix( data, "ud_T_htf_ind_od", p_ud_T_htf_ind_od, 20, 13 );
	ssc_number_t p_ud_T_amb_ind_od[260] ={ 0, 0.77080875635147095, 1.1106693744659424, 1.1393834352493286, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 2.8947367668151855, 0.7648392915725708, 1.1029571294784546, 1.1315360069274902, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 5.7894735336303711, 0.75890105962753296, 1.0952850580215454, 1.1237294673919678, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 8.6842107772827148, 0.75299358367919922, 1.0876528024673462, 1.1159634590148926, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 11.578947067260742, 0.7471163272857666, 1.0800596475601196, 1.1082371473312378, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 14.473684310913086, 0.74126893281936646, 1.072504997253418, 1.1005500555038452, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 17.36842155456543, 0.73545080423355103, 1.0649881362915039, 1.0929014682769775, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 20.263158798217773, 0.72966164350509644, 1.0575088262557983, 1.0852909088134766, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 23.157894134521484, 0.72390097379684448, 1.0500662326812744, 1.0777179002761841, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 26.052631378173828, 0.71816831827163696, 1.0426598787307739, 1.0701817274093628, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 28.947368621826172, 0.71246337890625, 1.0352892875671387, 1.0626819133758545, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 31.842105865478516, 0.70678573846817017, 1.0279538631439209, 1.055217981338501, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 34.736843109130859, 0.70113492012023926, 1.020653247833252, 1.0477893352508545, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 37.631580352783203, 0.6955106258392334, 1.0133868455886841, 1.0403956174850464, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 40.526317596435547, 0.68991243839263916, 1.0061541795730591, 1.0330361127853394, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 43.421051025390625, 0.68434005975723267, 0.9989548921585083, 1.0257105827331543, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 46.315788269042969, 0.67879307270050049, 0.99178838729858398, 1.0184184312820435, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 49.210525512695313, 0.67327117919921875, 0.98465430736541748, 1.0111593008041382, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 52.105262756347656, 0.6677740216255188, 0.97755211591720581, 1.0039327144622803, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1, 55, 0.66230124235153198, 0.9704815149307251, 0.99673813581466675, 0.73943662643432617, 1, 1.0211267471313477, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_matrix( data, "ud_T_amb_ind_od", p_ud_T_amb_ind_od, 20, 13 );
	ssc_number_t p_ud_m_dot_htf_ind_od[260] ={ 0.30000001192092896, 0.23218943178653717, 0.20905359089374542, 0.20288264751434326, 0.30000001192092896, 0.30000001192092896, 0.30000001192092896, 1, 1, 1, 1, 1, 1, 0.34736841917037964, 0.28093913197517395, 0.25294578075408936, 0.24547921121120453, 0.34736841917037964, 0.34736841917037964, 0.34736841917037964, 1, 1, 1, 1, 1, 1, 0.39473685622215271, 0.33173003792762756, 0.29867574572563171, 0.28985929489135742, 0.39473685622215271, 0.39473685622215271, 0.39473685622215271, 1, 1, 1, 1, 1, 1, 0.44210526347160339, 0.3843865692615509, 0.34608548879623413, 0.33586958050727844, 0.44210526347160339, 0.44210526347160339, 0.44210526347160339, 1, 1, 1, 1, 1, 1, 0.48947367072105408, 0.4387660026550293, 0.39504644274711609, 0.38338527083396912, 0.48947367072105408, 0.48947367072105408, 0.48947367072105408, 1, 1, 1, 1, 1, 1, 0.53684210777282715, 0.49474948644638062, 0.44545161724090576, 0.4323025643825531, 0.53684210777282715, 0.53684210777282715, 0.53684210777282715, 1, 1, 1, 1, 1, 1, 0.58421051502227783, 0.55223637819290161, 0.49721038341522217, 0.48253348469734192, 0.58421051502227783, 0.58421051502227783, 0.58421051502227783, 1, 1, 1, 1, 1, 1, 0.63157892227172852, 0.61114007234573364, 0.55024480819702148, 0.53400242328643799, 0.63157892227172852, 0.63157892227172852, 0.63157892227172852, 1, 1, 1, 1, 1, 1, 0.67894738912582397, 0.67138516902923584, 0.60448694229125977, 0.58664345741271973, 0.67894738912582397, 0.67894738912582397, 0.67894738912582397, 1, 1, 1, 1, 1, 1, 0.72631579637527466, 0.73290544748306274, 0.6598772406578064, 0.64039862155914307, 0.72631579637527466, 0.72631579637527466, 0.72631579637527466, 1, 1, 1, 1, 1, 1, 0.77368420362472534, 0.79564195871353149, 0.71636253595352173, 0.6952165961265564, 0.77368420362472534, 0.77368420362472534, 0.77368420362472534, 1, 1, 1, 1, 1, 1, 0.82105261087417603, 0.85954201221466064, 0.77389544248580933, 0.75105124711990356, 0.82105261087417603, 0.82105261087417603, 0.82105261087417603, 1, 1, 1, 1, 1, 1, 0.86842107772827148, 0.92455822229385376, 0.83243328332901001, 0.80786114931106567, 0.86842107772827148, 0.86842107772827148, 0.86842107772827148, 1, 1, 1, 1, 1, 1, 0.91578948497772217, 0.99064755439758301, 0.89193731546401978, 0.86560869216918945, 0.91578948497772217, 0.91578948497772217, 0.91578948497772217, 1, 1, 1, 1, 1, 1, 0.96315789222717285, 1.0577707290649414, 0.95237225294113159, 0.92425966262817383, 0.96315789222717285, 0.96315789222717285, 0.96315789222717285, 1, 1, 1, 1, 1, 1, 1.0105262994766235, 1.1188032627105713, 1.0073233842849731, 0.9775887131690979, 1.0105262994766235, 1.0105262994766235, 1.0105262994766235, 1, 1, 1, 1, 1, 1, 1.0578947067260742, 1.1541363000869751, 1.0391356945037842, 1.0084620714187622, 1.0578947067260742, 1.0578947067260742, 1.0578947067260742, 1, 1, 1, 1, 1, 1, 1.1052631139755249, 1.1872962713241577, 1.0689915418624878, 1.0374366044998169, 1.1052631139755249, 1.1052631139755249, 1.1052631139755249, 1, 1, 1, 1, 1, 1, 1.1526315212249756, 1.2181394100189209, 1.0967614650726318, 1.0643867254257202, 1.1526315212249756, 1.1526315212249756, 1.1526315212249756, 1, 1, 1, 1, 1, 1, 1.2000000476837158, 1.2465022802352905, 1.1222981214523315, 1.0891696214675903, 1.2000000476837158, 1.2000000476837158, 1.2000000476837158, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_matrix( data, "ud_m_dot_htf_ind_od", p_ud_m_dot_htf_ind_od, 20, 13 );
	ssc_data_set_number( data, "eta_c", 0.88999998569488525 );
	ssc_data_set_number( data, "eta_t", 0.89999997615814209 );
	ssc_data_set_number( data, "recup_eff_max", 0.95999997854232788 );
	ssc_data_set_number( data, "P_high_limit", 25 );
	ssc_data_set_number( data, "deltaT_PHX", 20 );
	ssc_data_set_number( data, "fan_power_perc_net", 1.5 );
	ssc_data_set_number( data, "sco2_T_amb_des", 35 );
	ssc_data_set_number( data, "sco2_T_approach", 10 );
	ssc_data_set_number( data, "is_sco2_preprocess", 0 );
	ssc_data_set_number( data, "sco2ud_T_htf_cold_calc", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "sco2ud_T_htf_low", 0 );
	ssc_data_set_number( data, "sco2ud_T_htf_high", 0 );
	ssc_data_set_number( data, "sco2ud_T_amb_low", 0 );
	ssc_data_set_number( data, "sco2ud_T_amb_high", 0 );
	ssc_data_set_number( data, "sco2ud_m_dot_htf_low", 0 );
	ssc_data_set_number( data, "sco2ud_m_dot_htf_high", 0 );
	ssc_number_t p_sco2ud_T_htf_ind_od[39] ={ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_matrix( data, "sco2ud_T_htf_ind_od", p_sco2ud_T_htf_ind_od, 3, 13 );
	ssc_number_t p_sco2ud_T_amb_ind_od[39] ={ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_matrix( data, "sco2ud_T_amb_ind_od", p_sco2ud_T_amb_ind_od, 3, 13 );
	ssc_number_t p_sco2ud_m_dot_htf_ind_od[39] ={ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_matrix( data, "sco2ud_m_dot_htf_ind_od", p_sco2ud_m_dot_htf_ind_od, 3, 13 );
	ssc_data_set_number( data, "_sco2_P_high_limit", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_P_ref", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_T_amb_des", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_T_approach", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_T_htf_hot_des", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_deltaT_PHX", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_design_eff", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_eta_c", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_eta_t", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "_sco2_recup_eff_max", 9.9999999999999998e+37 );
	ssc_data_set_number( data, "time_start", 0 );
	ssc_data_set_number( data, "time_stop", 31536000 );
	ssc_data_set_number( data, "pb_fixed_par", 0.0054999999701976776 );
	ssc_data_set_number( data, "aux_par", 0.023000000044703484 );
	ssc_data_set_number( data, "aux_par_f", 1 );
	ssc_data_set_number( data, "aux_par_0", 0.4830000102519989 );
	ssc_data_set_number( data, "aux_par_1", 0.57099997997283936 );
	ssc_data_set_number( data, "aux_par_2", 0 );
	ssc_data_set_number( data, "bop_par", 0 );
	ssc_data_set_number( data, "bop_par_f", 1 );
	ssc_data_set_number( data, "bop_par_0", 0 );
	ssc_data_set_number( data, "bop_par_1", 0.4830000102519989 );
	ssc_data_set_number( data, "bop_par_2", 0 );
	ssc_number_t p_f_turb_tou_periods[9] ={ 1.0499999523162842, 1, 1, 1, 1, 1, 1, 1, 1 };
	ssc_data_set_array( data, "f_turb_tou_periods", p_f_turb_tou_periods, 9 );
	ssc_number_t p_weekday_schedule[288] ={ 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5 };
	ssc_data_set_matrix( data, "weekday_schedule", p_weekday_schedule, 12, 24 );
	ssc_number_t p_weekend_schedule[288] ={ 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	ssc_data_set_matrix( data, "weekend_schedule", p_weekend_schedule, 12, 24 );
	ssc_data_set_number( data, "is_dispatch", 0 );
	ssc_data_set_number( data, "disp_horizon", 48 );
	ssc_data_set_number( data, "disp_frequency", 24 );
	ssc_data_set_number( data, "disp_max_iter", 35000 );
	ssc_data_set_number( data, "disp_timeout", 5 );
	ssc_data_set_number( data, "disp_mip_gap", 0.0010000000474974513 );
	ssc_data_set_number( data, "disp_time_weighting", 0.99000000953674316 );
	ssc_data_set_number( data, "disp_rsu_cost", 950 );
	ssc_data_set_number( data, "disp_csu_cost", 10000 );
	ssc_data_set_number( data, "disp_pen_delta_w", 0.10000000149011612 );
	ssc_data_set_number( data, "is_wlim_series", 0 );
	set_array( data, "wlim_series", "../deploy/samples/wlim_series.csv", 8760);
	ssc_number_t p_dispatch_sched_weekday[288] ={ 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5 };
	ssc_data_set_matrix( data, "dispatch_sched_weekday", p_dispatch_sched_weekday, 12, 24 );
	ssc_number_t p_dispatch_sched_weekend[288] ={ 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	ssc_data_set_matrix( data, "dispatch_sched_weekend", p_dispatch_sched_weekend, 12, 24 );
	ssc_data_set_number( data, "dispatch_factor1", 2.0639998912811279 );
	ssc_data_set_number( data, "dispatch_factor2", 1.2000000476837158 );
	ssc_data_set_number( data, "dispatch_factor3", 1 );
	ssc_data_set_number( data, "dispatch_factor4", 1.1000000238418579 );
	ssc_data_set_number( data, "dispatch_factor5", 0.80000001192092896 );
	ssc_data_set_number( data, "dispatch_factor6", 0.69999998807907104 );
	ssc_data_set_number( data, "dispatch_factor7", 1 );
	ssc_data_set_number( data, "dispatch_factor8", 1 );
	ssc_data_set_number( data, "dispatch_factor9", 1 );
	ssc_data_set_number( data, "is_dispatch_series", 0 );
	ssc_number_t p_dispatch_series[1] ={ 0 };
	ssc_data_set_array( data, "dispatch_series", p_dispatch_series, 1 );
	ssc_data_set_number( data, "rec_height", 21.602912902832031 );
	ssc_data_set_number( data, "D_rec", 17.650043487548828 );
	ssc_data_set_number( data, "h_tower", 193.45753479003906 );
	set_matrix( data, "helio_positions", "../deploy/samples/helio_positions.csv", 8790, 2);
	ssc_data_set_number( data, "land_area_base", 1847.037353515625 );
	ssc_data_set_number( data, "const_per_interest_rate1", 4 );
	ssc_data_set_number( data, "const_per_interest_rate2", 0 );
	ssc_data_set_number( data, "const_per_interest_rate3", 0 );
	ssc_data_set_number( data, "const_per_interest_rate4", 0 );
	ssc_data_set_number( data, "const_per_interest_rate5", 0 );
	ssc_data_set_number( data, "const_per_months1", 24 );
	ssc_data_set_number( data, "const_per_months2", 0 );
	ssc_data_set_number( data, "const_per_months3", 0 );
	ssc_data_set_number( data, "const_per_months4", 0 );
	ssc_data_set_number( data, "const_per_months5", 0 );
	ssc_data_set_number( data, "const_per_percent1", 100 );
	ssc_data_set_number( data, "const_per_percent2", 0 );
	ssc_data_set_number( data, "const_per_percent3", 0 );
	ssc_data_set_number( data, "const_per_percent4", 0 );
	ssc_data_set_number( data, "const_per_percent5", 0 );
	ssc_data_set_number( data, "const_per_upfront_rate1", 1 );
	ssc_data_set_number( data, "const_per_upfront_rate2", 0 );
	ssc_data_set_number( data, "const_per_upfront_rate3", 0 );
	ssc_data_set_number( data, "const_per_upfront_rate4", 0 );
	ssc_data_set_number( data, "const_per_upfront_rate5", 0 );
	ssc_data_set_number( data, "adjust:constant", 4 );
	ssc_data_set_number( data, "sf_adjust:constant", 0 );
	module = ssc_module_create("tcsmolten_salt"); 
	if (NULL == module)
	{
		mw.Log( wxString::Format( "error: could not create 'tcsmolten_salt' module." ) ); 
		ssc_data_free(data); 
		return; 
	}
	if (ssc_module_exec_with_handler(module, data, my_handler, 0) == 0)
	{
		mw.Log( wxString::Format( "error during simulation." ) ); 
		ssc_module_free(module); 
		ssc_data_free(data); 
		return; 
	}
	ssc_module_free(module);
	ssc_data_set_number( data, "analysis_period", 25 );
	ssc_number_t p_federal_tax_rate[1] ={ 35 };
	ssc_data_set_array( data, "federal_tax_rate", p_federal_tax_rate, 1 );
	ssc_number_t p_state_tax_rate[1] ={ 7 };
	ssc_data_set_array( data, "state_tax_rate", p_state_tax_rate, 1 );
	ssc_data_set_number( data, "property_tax_rate", 0 );
	ssc_data_set_number( data, "prop_tax_cost_assessed_percent", 100 );
	ssc_data_set_number( data, "prop_tax_assessed_decline", 0 );
	ssc_data_set_number( data, "real_discount_rate", 5.5 );
	ssc_data_set_number( data, "inflation_rate", 2.5 );
	ssc_data_set_number( data, "insurance_rate", 0.5 );
	ssc_data_set_number( data, "system_capacity", 103500 );
	ssc_number_t p_om_fixed[1] ={ 0 };
	ssc_data_set_array( data, "om_fixed", p_om_fixed, 1 );
	ssc_data_set_number( data, "om_fixed_escal", 0 );
	ssc_number_t p_om_production[1] ={ 3.5 };
	ssc_data_set_array( data, "om_production", p_om_production, 1 );
	ssc_data_set_number( data, "om_production_escal", 0 );
	ssc_number_t p_om_capacity[1] ={ 66 };
	ssc_data_set_array( data, "om_capacity", p_om_capacity, 1 );
	ssc_data_set_number( data, "om_capacity_escal", 0 );
	ssc_number_t p_om_fuel_cost[1] ={ 0 };
	ssc_data_set_array( data, "om_fuel_cost", p_om_fuel_cost, 1 );
	ssc_data_set_number( data, "om_fuel_cost_escal", 0 );
	ssc_data_set_number( data, "itc_fed_amount", 0 );
	ssc_data_set_number( data, "itc_fed_amount_deprbas_fed", 1 );
	ssc_data_set_number( data, "itc_fed_amount_deprbas_sta", 1 );
	ssc_data_set_number( data, "itc_sta_amount", 0 );
	ssc_data_set_number( data, "itc_sta_amount_deprbas_fed", 0 );
	ssc_data_set_number( data, "itc_sta_amount_deprbas_sta", 0 );
	ssc_data_set_number( data, "itc_fed_percent", 30 );
	ssc_data_set_number( data, "itc_fed_percent_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "itc_fed_percent_deprbas_fed", 1 );
	ssc_data_set_number( data, "itc_fed_percent_deprbas_sta", 1 );
	ssc_data_set_number( data, "itc_sta_percent", 0 );
	ssc_data_set_number( data, "itc_sta_percent_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "itc_sta_percent_deprbas_fed", 0 );
	ssc_data_set_number( data, "itc_sta_percent_deprbas_sta", 0 );
	ssc_number_t p_ptc_fed_amount[1] ={ 0 };
	ssc_data_set_array( data, "ptc_fed_amount", p_ptc_fed_amount, 1 );
	ssc_data_set_number( data, "ptc_fed_term", 10 );
	ssc_data_set_number( data, "ptc_fed_escal", 0 );
	ssc_number_t p_ptc_sta_amount[1] ={ 0 };
	ssc_data_set_array( data, "ptc_sta_amount", p_ptc_sta_amount, 1 );
	ssc_data_set_number( data, "ptc_sta_term", 10 );
	ssc_data_set_number( data, "ptc_sta_escal", 0 );
	ssc_data_set_number( data, "ibi_fed_amount", 0 );
	ssc_data_set_number( data, "ibi_fed_amount_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_fed_amount_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_fed_amount_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_fed_amount_deprbas_sta", 0 );
	ssc_data_set_number( data, "ibi_sta_amount", 0 );
	ssc_data_set_number( data, "ibi_sta_amount_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_sta_amount_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_sta_amount_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_sta_amount_deprbas_sta", 0 );
	ssc_data_set_number( data, "ibi_uti_amount", 0 );
	ssc_data_set_number( data, "ibi_uti_amount_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_uti_amount_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_uti_amount_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_uti_amount_deprbas_sta", 0 );
	ssc_data_set_number( data, "ibi_oth_amount", 0 );
	ssc_data_set_number( data, "ibi_oth_amount_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_oth_amount_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_oth_amount_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_oth_amount_deprbas_sta", 0 );
	ssc_data_set_number( data, "ibi_fed_percent", 0 );
	ssc_data_set_number( data, "ibi_fed_percent_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "ibi_fed_percent_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_fed_percent_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_fed_percent_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_fed_percent_deprbas_sta", 0 );
	ssc_data_set_number( data, "ibi_sta_percent", 0 );
	ssc_data_set_number( data, "ibi_sta_percent_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "ibi_sta_percent_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_sta_percent_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_sta_percent_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_sta_percent_deprbas_sta", 0 );
	ssc_data_set_number( data, "ibi_uti_percent", 0 );
	ssc_data_set_number( data, "ibi_uti_percent_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "ibi_uti_percent_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_uti_percent_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_uti_percent_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_uti_percent_deprbas_sta", 0 );
	ssc_data_set_number( data, "ibi_oth_percent", 0 );
	ssc_data_set_number( data, "ibi_oth_percent_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "ibi_oth_percent_tax_fed", 1 );
	ssc_data_set_number( data, "ibi_oth_percent_tax_sta", 1 );
	ssc_data_set_number( data, "ibi_oth_percent_deprbas_fed", 0 );
	ssc_data_set_number( data, "ibi_oth_percent_deprbas_sta", 0 );
	ssc_data_set_number( data, "cbi_fed_amount", 0 );
	ssc_data_set_number( data, "cbi_fed_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "cbi_fed_tax_fed", 1 );
	ssc_data_set_number( data, "cbi_fed_tax_sta", 1 );
	ssc_data_set_number( data, "cbi_fed_deprbas_fed", 0 );
	ssc_data_set_number( data, "cbi_fed_deprbas_sta", 0 );
	ssc_data_set_number( data, "cbi_sta_amount", 0 );
	ssc_data_set_number( data, "cbi_sta_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "cbi_sta_tax_fed", 1 );
	ssc_data_set_number( data, "cbi_sta_tax_sta", 1 );
	ssc_data_set_number( data, "cbi_sta_deprbas_fed", 0 );
	ssc_data_set_number( data, "cbi_sta_deprbas_sta", 0 );
	ssc_data_set_number( data, "cbi_uti_amount", 0 );
	ssc_data_set_number( data, "cbi_uti_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "cbi_uti_tax_fed", 1 );
	ssc_data_set_number( data, "cbi_uti_tax_sta", 1 );
	ssc_data_set_number( data, "cbi_uti_deprbas_fed", 0 );
	ssc_data_set_number( data, "cbi_uti_deprbas_sta", 0 );
	ssc_data_set_number( data, "cbi_oth_amount", 0 );
	ssc_data_set_number( data, "cbi_oth_maxvalue", 9.9999996802856925e+37 );
	ssc_data_set_number( data, "cbi_oth_tax_fed", 1 );
	ssc_data_set_number( data, "cbi_oth_tax_sta", 1 );
	ssc_data_set_number( data, "cbi_oth_deprbas_fed", 0 );
	ssc_data_set_number( data, "cbi_oth_deprbas_sta", 0 );
	ssc_number_t p_pbi_fed_amount[1] ={ 0 };
	ssc_data_set_array( data, "pbi_fed_amount", p_pbi_fed_amount, 1 );
	ssc_data_set_number( data, "pbi_fed_term", 0 );
	ssc_data_set_number( data, "pbi_fed_escal", 0 );
	ssc_data_set_number( data, "pbi_fed_tax_fed", 1 );
	ssc_data_set_number( data, "pbi_fed_tax_sta", 1 );
	ssc_number_t p_pbi_sta_amount[1] ={ 0 };
	ssc_data_set_array( data, "pbi_sta_amount", p_pbi_sta_amount, 1 );
	ssc_data_set_number( data, "pbi_sta_term", 0 );
	ssc_data_set_number( data, "pbi_sta_escal", 0 );
	ssc_data_set_number( data, "pbi_sta_tax_fed", 1 );
	ssc_data_set_number( data, "pbi_sta_tax_sta", 1 );
	ssc_number_t p_pbi_uti_amount[1] ={ 0 };
	ssc_data_set_array( data, "pbi_uti_amount", p_pbi_uti_amount, 1 );
	ssc_data_set_number( data, "pbi_uti_term", 0 );
	ssc_data_set_number( data, "pbi_uti_escal", 0 );
	ssc_data_set_number( data, "pbi_uti_tax_fed", 1 );
	ssc_data_set_number( data, "pbi_uti_tax_sta", 1 );
	ssc_number_t p_pbi_oth_amount[1] ={ 0 };
	ssc_data_set_array( data, "pbi_oth_amount", p_pbi_oth_amount, 1 );
	ssc_data_set_number( data, "pbi_oth_term", 0 );
	ssc_data_set_number( data, "pbi_oth_escal", 0 );
	ssc_data_set_number( data, "pbi_oth_tax_fed", 1 );
	ssc_data_set_number( data, "pbi_oth_tax_sta", 1 );
	ssc_number_t p_degradation[1] ={ 0 };
	ssc_data_set_array( data, "degradation", p_degradation, 1 );
	ssc_number_t p_roe_input[1] ={ 0 };
	ssc_data_set_array( data, "roe_input", p_roe_input, 1 );
	ssc_data_set_number( data, "loan_moratorium", 0 );
	ssc_data_set_number( data, "system_use_recapitalization", 0 );
	ssc_data_set_number( data, "system_use_lifetime_output", 0 );
	ssc_data_set_number( data, "total_installed_cost", 704234496 );
	ssc_data_set_number( data, "reserves_interest", 1.75 );
	ssc_data_set_number( data, "equip1_reserve_cost", 0 );
	ssc_data_set_number( data, "equip1_reserve_freq", 12 );
	ssc_data_set_number( data, "equip2_reserve_cost", 0 );
	ssc_data_set_number( data, "equip2_reserve_freq", 15 );
	ssc_data_set_number( data, "equip3_reserve_cost", 0 );
	ssc_data_set_number( data, "equip3_reserve_freq", 3 );
	ssc_data_set_number( data, "equip_reserve_depr_sta", 0 );
	ssc_data_set_number( data, "equip_reserve_depr_fed", 0 );
	ssc_data_set_number( data, "salvage_percentage", 0 );
	ssc_data_set_number( data, "ppa_soln_mode", 0 );
	ssc_data_set_number( data, "ppa_price_input", 0.12999999523162842 );
	ssc_data_set_number( data, "ppa_escalation", 1 );
	ssc_data_set_number( data, "construction_financing_cost", 35211724 );
	ssc_data_set_number( data, "term_tenor", 18 );
	ssc_data_set_number( data, "term_int_rate", 7 );
	ssc_data_set_number( data, "dscr", 1.2999999523162842 );
	ssc_data_set_number( data, "dscr_reserve_months", 6 );
	ssc_data_set_number( data, "debt_percent", 50 );
	ssc_data_set_number( data, "debt_option", 1 );
	ssc_data_set_number( data, "payment_option", 0 );
	ssc_data_set_number( data, "cost_debt_closing", 450000 );
	ssc_data_set_number( data, "cost_debt_fee", 2.75 );
	ssc_data_set_number( data, "months_working_reserve", 6 );
	ssc_data_set_number( data, "months_receivables_reserve", 0 );
	ssc_data_set_number( data, "cost_other_financing", 0 );
	ssc_data_set_number( data, "flip_target_percent", 11 );
	ssc_data_set_number( data, "flip_target_year", 20 );
	ssc_data_set_number( data, "depr_alloc_macrs_5_percent", 90 );
	ssc_data_set_number( data, "depr_alloc_macrs_15_percent", 1.5 );
	ssc_data_set_number( data, "depr_alloc_sl_5_percent", 0 );
	ssc_data_set_number( data, "depr_alloc_sl_15_percent", 2.5 );
	ssc_data_set_number( data, "depr_alloc_sl_20_percent", 3 );
	ssc_data_set_number( data, "depr_alloc_sl_39_percent", 0 );
	ssc_data_set_number( data, "depr_alloc_custom_percent", 0 );
	ssc_number_t p_depr_custom_schedule[1] ={ 0 };
	ssc_data_set_array( data, "depr_custom_schedule", p_depr_custom_schedule, 1 );
	ssc_data_set_number( data, "depr_bonus_sta", 0 );
	ssc_data_set_number( data, "depr_bonus_sta_macrs_5", 1 );
	ssc_data_set_number( data, "depr_bonus_sta_macrs_15", 1 );
	ssc_data_set_number( data, "depr_bonus_sta_sl_5", 0 );
	ssc_data_set_number( data, "depr_bonus_sta_sl_15", 0 );
	ssc_data_set_number( data, "depr_bonus_sta_sl_20", 0 );
	ssc_data_set_number( data, "depr_bonus_sta_sl_39", 0 );
	ssc_data_set_number( data, "depr_bonus_sta_custom", 0 );
	ssc_data_set_number( data, "depr_bonus_fed", 0 );
	ssc_data_set_number( data, "depr_bonus_fed_macrs_5", 1 );
	ssc_data_set_number( data, "depr_bonus_fed_macrs_15", 1 );
	ssc_data_set_number( data, "depr_bonus_fed_sl_5", 0 );
	ssc_data_set_number( data, "depr_bonus_fed_sl_15", 0 );
	ssc_data_set_number( data, "depr_bonus_fed_sl_20", 0 );
	ssc_data_set_number( data, "depr_bonus_fed_sl_39", 0 );
	ssc_data_set_number( data, "depr_bonus_fed_custom", 0 );
	ssc_data_set_number( data, "depr_itc_sta_macrs_5", 1 );
	ssc_data_set_number( data, "depr_itc_sta_macrs_15", 0 );
	ssc_data_set_number( data, "depr_itc_sta_sl_5", 0 );
	ssc_data_set_number( data, "depr_itc_sta_sl_15", 0 );
	ssc_data_set_number( data, "depr_itc_sta_sl_20", 0 );
	ssc_data_set_number( data, "depr_itc_sta_sl_39", 0 );
	ssc_data_set_number( data, "depr_itc_sta_custom", 0 );
	ssc_data_set_number( data, "depr_itc_fed_macrs_5", 1 );
	ssc_data_set_number( data, "depr_itc_fed_macrs_15", 0 );
	ssc_data_set_number( data, "depr_itc_fed_sl_5", 0 );
	ssc_data_set_number( data, "depr_itc_fed_sl_15", 0 );
	ssc_data_set_number( data, "depr_itc_fed_sl_20", 0 );
	ssc_data_set_number( data, "depr_itc_fed_sl_39", 0 );
	ssc_data_set_number( data, "depr_itc_fed_custom", 0 );
	ssc_data_set_number( data, "pbi_fed_for_ds", 0 );
	ssc_data_set_number( data, "pbi_sta_for_ds", 0 );
	ssc_data_set_number( data, "pbi_uti_for_ds", 0 );
	ssc_data_set_number( data, "pbi_oth_for_ds", 0 );
	ssc_data_set_number( data, "depr_stabas_method", 1 );
	ssc_data_set_number( data, "depr_fedbas_method", 1 );
	module = ssc_module_create("singleowner"); 
	if (NULL == module)
	{
		mw.Log( wxString::Format( "error: could not create 'singleowner' module." ) ); 
		ssc_data_free(data); 
		return; 
	}
	if (ssc_module_exec(module, data) == 0)
	{
		mw.Log( wxString::Format( "error during simulation." ) ); 
		ssc_module_free(module); 
		ssc_data_free(data); 
		return; 
	}
	ssc_module_free(module);
	ssc_number_t annual_energy;
	ssc_data_get_number(data, "annual_energy", &annual_energy);
	mw.Log( wxString::Format( "%s = %.17g\n", "Annual energy (year 1)", (double)annual_energy ) );
	ssc_number_t capacity_factor;
	ssc_data_get_number(data, "capacity_factor", &capacity_factor);
	mw.Log( wxString::Format( "%s = %.17g\n", "Capacity factor (year 1)", (double)capacity_factor ) );
	ssc_number_t annual_total_water_use;
	ssc_data_get_number(data, "annual_total_water_use", &annual_total_water_use);
	mw.Log( wxString::Format( "%s = %.17g\n", "Annual Water Usage", (double)annual_total_water_use ) );
	ssc_number_t ppa;
	ssc_data_get_number(data, "ppa", &ppa);
	mw.Log( wxString::Format( "%s = %.17g\n", "PPA price (year 1)", (double)ppa ) );
	ssc_number_t lppa_nom;
	ssc_data_get_number(data, "lppa_nom", &lppa_nom);
	mw.Log( wxString::Format( "%s = %.17g\n", "Levelized PPA price (nominal)", (double)lppa_nom ) );
	ssc_number_t lppa_real;
	ssc_data_get_number(data, "lppa_real", &lppa_real);
	mw.Log( wxString::Format( "%s = %.17g\n", "Levelized PPA price (real)", (double)lppa_real ) );
	ssc_number_t lcoe_nom;
	ssc_data_get_number(data, "lcoe_nom", &lcoe_nom);
	mw.Log( wxString::Format( "%s = %.17g\n", "Levelized COE (nominal)", (double)lcoe_nom ) );
	ssc_number_t lcoe_real;
	ssc_data_get_number(data, "lcoe_real", &lcoe_real);
	mw.Log( wxString::Format( "%s = %.17g\n", "Levelized COE (real)", (double)lcoe_real ) );
	ssc_number_t project_return_aftertax_npv;
	ssc_data_get_number(data, "project_return_aftertax_npv", &project_return_aftertax_npv);
	mw.Log( wxString::Format( "%s = %.17g\n", "Net present value", (double)project_return_aftertax_npv ) );
	ssc_number_t flip_actual_irr;
	ssc_data_get_number(data, "flip_actual_irr", &flip_actual_irr);
	mw.Log( wxString::Format( "%s = %.17g\n", "Internal rate of return (IRR)", (double)flip_actual_irr ) );
	ssc_number_t flip_actual_year;
	ssc_data_get_number(data, "flip_actual_year", &flip_actual_year);
	mw.Log( wxString::Format( "%s = %.17g\n", "Year IRR is achieved", (double)flip_actual_year ) );
	ssc_number_t project_return_aftertax_irr;
	ssc_data_get_number(data, "project_return_aftertax_irr", &project_return_aftertax_irr);
	mw.Log( wxString::Format( "%s = %.17g\n", "IRR at end of project", (double)project_return_aftertax_irr ) );
	ssc_number_t cost_installed;
	ssc_data_get_number(data, "cost_installed", &cost_installed);
	mw.Log( wxString::Format( "%s = %.17g\n", "Net capital cost", (double)cost_installed ) );
	ssc_number_t size_of_equity;
	ssc_data_get_number(data, "size_of_equity", &size_of_equity);
	mw.Log( wxString::Format( "%s = %.17g\n", "Equity", (double)size_of_equity ) );
	ssc_number_t size_of_debt;
	ssc_data_get_number(data, "size_of_debt", &size_of_debt);
	mw.Log( wxString::Format( "%s = %.17g\n", "Size of debt", (double)size_of_debt ) );
	ssc_data_free(data);
	return;

}


void _power_cycle(lk::invoke_t &cxt)
{
	LK_DOC("power_cycle_avail", "Simulate a power cycle failure series.", "(void):null");

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




