#ifndef _PROJECT
#define _PROJECT

#include <string>
#include <vector>

#include <lk/env.h>
#include <ssc/sscapi.h>
#include "../liboptical/optical_degr.h"
#include "../libsolar/solarfield_avail.h"


/* 
A class containing the aspects of the current project
*/

enum DATATYPE { TYPE_INT, TYPE_NUMBER, TYPE_BOOL, TYPE_STRING, TYPE_VECTOR, TYPE_MATRIX };

class data_base
{
public:
	std::string name;
	unsigned int type;
	
	virtual void set_name(const std::string &vname)
	{
		name = vname;
	};
};

template<typename T> class data_unit : public data_base
{
public:
	T val;
};

template<typename T> class variable : public data_unit<T>
{
public:
	T minval;
	T maxval;

	std::string as_string()
	{
		return  std::to_string(this->val);
	};
	
	bool set_limits(const T& vmin, const T& vmax)
	{
		if (vmin >= vmax)
			return false;

		minval = vmin;
		maxval = vmax;

		return true;
	};
	
	void set(const T &v, const T &vmin, const T &vmax, std::string vname, const unsigned int datatype)
	{
		this->val = v;
		set_limits(vmin, vmax);
		set_name(vname);
		this->type = datatype;
	};
};

struct datas_base 
{
	virtual std::vector<data_base*>* GetMemberPointer()=0;
};

struct variables : public datas_base
{
private:
	//list all members here
	std::vector<data_base*> _members = { 
		&h_tower, &rec_height, &D_rec, &design_eff, 
		&dni_des, &P_ref, &solarm, &tshours, 
		&degr_replace_limit, &om_staff, &n_wash_crews, &N_panels};

public:
	variable<double> h_tower;
	variable<double> rec_height;
	variable<double> D_rec;
	variable<double> design_eff;
	variable<double> dni_des;
	variable<double> P_ref;
	variable<double> solarm;
	variable<double> tshours;
	variable<double> degr_replace_limit;
	variable<int> om_staff;
	variable<int> n_wash_crews;
	variable<int> N_panels;

	variables();

	std::vector<data_base*> *GetMemberPointer(){return &_members;}
};

template<typename T> class parameter : public data_unit<T>
{
public:
	bool is_calculated;
	
	void set(const T &v, std::string vname, const unsigned int datatype, bool calculated=false)
	{
		this->val = v;
		set_name(vname);
		this->type = datatype;
		is_calculated = calculated;
	};
};

struct parameters : public datas_base
{
private:
	std::vector<data_base*> _members = { 
		&print_messages, &check_max_flux, &is_optimize, &is_dispatch, &is_ampl_engine, &is_stochastic_disp, 
		&ampl_data_dir, &solar_resource_file, &disp_steps_per_hour, &avail_seed, &plant_lifetime, 
		&ppa_multiplier_model, &rec_ref_cost, &rec_ref_area, &tes_spec_cost, &tower_fixed_cost, &tower_exp, 
		&heliostat_spec_cost, &site_spec_cost, &land_spec_cost, &c_cps0, &c_cps1, &om_staff_cost, &wash_crew_cost, 
		&heliostat_refurbish_cost, &helio_mtf, &heliostat_repair_cost, &om_staff_max_hours_week, &n_heliostats_sim, 
		&wash_units_per_hour, &wash_crew_max_hours_week, &degr_per_hour, &degr_accel_per_year, &degr_seed, 
		&soil_per_hour, &adjust_constant, &helio_reflectance, &finance_period, &disp_rsu_cost, &disp_csu_cost, 
		&disp_pen_delta_w, &rec_su_delay, &rec_qf_delay, &startup_time, &startup_frac, &v_wind_max, &c_ces, &dispatch_factors_ts
	};

public:

	//-----------------------------------------------------------------------
	parameter< bool > print_messages;
	parameter< bool > check_max_flux;
	parameter< bool > is_optimize;
	parameter< bool > is_dispatch;
	parameter< bool > is_ampl_engine;
	parameter< bool > is_stochastic_disp;

	parameter< std::string> ampl_data_dir;
	parameter< std::string> solar_resource_file;

	parameter< int > disp_steps_per_hour;
	parameter< int > avail_seed;
	parameter< int > plant_lifetime;
	parameter< int > finance_period;
	parameter< int > ppa_multiplier_model;

	parameter< double > rec_ref_cost;
	parameter< double > rec_ref_area;
	parameter< double > tes_spec_cost;
	parameter< double > tower_fixed_cost;
	parameter< double > tower_exp;
	parameter< double > heliostat_spec_cost;
	parameter< double > site_spec_cost;
	parameter< double > land_spec_cost;
	parameter< double > c_cps0;
	parameter< double > c_cps1;
	parameter< double > om_staff_cost;
	parameter< double > wash_crew_cost;
	parameter< double > heliostat_refurbish_cost;
	parameter< double > helio_mtf;
	parameter< double > heliostat_repair_cost;
	parameter< double > om_staff_max_hours_week;
	parameter< double > n_heliostats_sim;
	parameter< double > wash_units_per_hour;
	parameter< double > wash_crew_max_hours_week;
	parameter< double > degr_per_hour;
	parameter< double > degr_accel_per_year;
	parameter< double > degr_seed;
	parameter< double > soil_per_hour;
	parameter< double > adjust_constant;
	parameter< double > helio_reflectance;
	parameter< double > disp_rsu_cost;
	parameter< double > disp_csu_cost;
	parameter< double > disp_pen_delta_w;
	parameter< double > rec_su_delay;
	parameter< double > rec_qf_delay;
	parameter< double > startup_time;
	parameter< double > startup_frac;
	parameter< double > v_wind_max;

	parameter< std::vector< double > > c_ces;
	parameter< std::vector< double > > dispatch_factors_ts;
	//-----------------------------------------------------------------------

	parameters();

	std::vector<data_base*> *GetMemberPointer() { return &_members; }

};

//main class
class Project
{
	ssc_data_t m_ssc_simdata;
	ssc_data_t m_ssc_findata;
	ssc_data_t m_ssc_condata;
	

	solarfield_availability m_solarfield_availability;
	optical_degradation m_optical_degradation;


public:
	variables m_variables;
	parameters m_parameters;

	Project();
	~Project();

	void hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars);
	void initialize_ssc_project();
	void update_sscdata_from_current();

	//objective function methods
	int D(ssc_data_t &cxt, lk::varhash_t &vars);
	int M(ssc_data_t &cxt, lk::varhash_t &vars);
	int O(ssc_data_t &cxt, lk::varhash_t &vars);
	int S(ssc_data_t &cxt, lk::varhash_t &vars);
	int E(ssc_data_t &cxt, lk::varhash_t &vars);
	int F(ssc_data_t &cxt, lk::varhash_t &vars);
	int Z(ssc_data_t &cxt, lk::varhash_t &vars);

	

	// double calc_real_dollars(double value, bool is_revenue=false, bool is_labor=false);
	// def setup_clusters(self, Nclusters, Ndays = 2, Nprev = 1, Nnext = 1, user_weights = None, user_divisions = None):
	// def M(self, variables, design):
	// def O(self, variables, design):
	// def S(self, design, variables, sf_avail=None, sf_soil=None, sf_degr=None, sample_weeks=None, Nclusters = None, cluster_inputs = None, pv_production = None):
	// def E(self, variables):
	// def F(self, variables, S, om_cost): #total_installed_cost, generation, pricing_mult):
	// def Z(self, variables, **kwargs):

};





#endif // !_PROJECT
