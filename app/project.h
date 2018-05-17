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

extern ssc_bool_t ssc_progress_handler(ssc_module_t, ssc_handler_t, int action, float f0, float f1, const char *s0, const char *, void *);
extern bool sim_progress_handler(float progress, const char *msg);
extern void message_handler(const char *msg);

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
		this->set_name(vname);
		this->type = datatype;
	};
};

struct datas_base 
{
protected:
	std::unordered_map< std::string, data_base* > _member_map;

	void _construct_member_map()
	{
		_member_map.clear();

		std::vector<data_base*> *members = this->GetMemberPointer();
		for (std::vector<data_base*>::iterator it = members->begin(); it != members->end(); it++)
			_member_map[(*it)->name] = (*it);
	};

public:
	virtual std::vector<data_base*>* GetMemberPointer()=0;
	data_base *find(const char *name)
	{
		std::unordered_map<std::string, data_base*>::iterator itfind = this->_member_map.find(name);
		
		if (itfind != this->_member_map.end())
			return itfind->second;
		else
			return 0;
	};
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
		this->set_name(vname);
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
		&disp_pen_delta_w, &rec_su_delay, &rec_qf_delay, &startup_time, &startup_frac, &v_wind_max, &c_ces, 
		&dispatch_factors_ts, &flux_max
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
	parameter< double > flux_max;

	parameter< std::vector< double > > c_ces;
	parameter< std::vector< double > > dispatch_factors_ts;
	//-----------------------------------------------------------------------

	parameters();

	std::vector<data_base*> *GetMemberPointer() { return &_members; }

};

struct design_outputs : public datas_base
{
private:
	std::vector<data_base*> _members = {
		&opteff_table, &flux_table, &heliostat_positions, &number_heliostats, &area_sf, &base_land_area, &land_area, 
		&h_tower_opt, &rec_height_opt, &rec_aspect_opt, &cost_rec_tot, &cost_sf_tot, &cost_tower_tot, &cost_land_tot, 
		&cost_site_tot, &flux_max_observed, &cost_sf_real, &cost_land_real
	};

public:

	//-----------------------------------------------------------------------
	parameter< int > number_heliostats;
	parameter< double > area_sf;
	parameter< double > base_land_area;
	parameter< double > land_area;
	parameter< double > h_tower_opt;
	parameter< double > rec_height_opt;
	parameter< double > rec_aspect_opt;
	parameter< double > cost_rec_tot;
	parameter< double > cost_sf_tot;
	parameter< double > cost_sf_real;
	parameter< double > cost_tower_tot;
	parameter< double > cost_land_tot;
	parameter< double > cost_land_real;
	parameter< double > cost_site_tot;
	parameter< double > flux_max_observed;
	parameter< std::vector< std::vector< double > > > opteff_table;
	parameter< std::vector< std::vector< double > > > flux_table;
	parameter< std::vector< std::vector< double > > > heliostat_positions;
	//-----------------------------------------------------------------------

	design_outputs();

	std::vector<data_base*> *GetMemberPointer() { return &_members; }

};

struct solarfield_outputs : public datas_base
{
private:
	std::vector<data_base*> _members = {&n_repairs, &staff_utilization, &heliostat_repair_cost_y1, &heliostat_repair_cost, &avail_schedule};

public:
	parameter< double > n_repairs;
	parameter< double > staff_utilization;
	parameter< double> heliostat_repair_cost_y1;
	parameter< double> heliostat_repair_cost;
	parameter< std::vector< double > > avail_schedule;

	solarfield_outputs();

	std::vector<data_base*> *GetMemberPointer() { return &_members; }
};

struct optical_outputs : public datas_base
{
private:
	std::vector<data_base*> _members = { &n_replacements, &heliostat_refurbish_cost, &heliostat_refurbish_cost_y1, &avg_soil, 
		&avg_degr, &soil_schedule, &degr_schedule, &repl_schedule, &repl_total };

public:
	parameter< double > n_replacements;
	parameter< double > heliostat_refurbish_cost;
	parameter< double > heliostat_refurbish_cost_y1;
	parameter< double > avg_soil;
	parameter< double > avg_degr;

	parameter< std::vector< double > > soil_schedule;
	parameter< std::vector< double > > degr_schedule;
	parameter< std::vector< double > > repl_schedule;
	parameter< std::vector< double > > repl_total;

	optical_outputs();

	std::vector<data_base*> *GetMemberPointer() { return &_members; }

};

struct cycle_outputs : public datas_base
{
private:
	std::vector<data_base*> _members = {};

public:

	cycle_outputs() {};

	std::vector<data_base*> *GetMemberPointer() { return &_members; }
};

//main class
class Project
{
	ssc_data_t m_ssc_data;
	
	unordered_map<std::string, data_base*> _merged_data;

	void lk_hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars);
	void lk_var_to_data(lk::vardata_t &var, data_base &data);
	void initialize_ssc_project();
	void update_sscdata_from_object(datas_base &obj);
	void update_object_from_sscdata(datas_base &obj);
	void sscdata_localdata(data_base *obj, bool set_ssc_from_local);       //set ssc data from obj (set_ssc_from_local=true), or set local from ssc (false)
	void sscdata_localdata_map(datas_base &objs, bool set_ssc_from_local);  //set ssc data from list (set_ssc_from_local=true), or set local from ssc (false)
	void update_calculated_system_values();
	void update_calculated_values_post_layout();
	double calc_real_dollars(const double &dollars, bool is_revenue=false, bool is_labor=false);
	bool run_design();

public:
	variables m_variables;
	parameters m_parameters;
	design_outputs m_design_outputs;
	solarfield_outputs m_solarfield_outputs;
	optical_outputs m_optical_outputs;
	cycle_outputs m_cycle_outputs;

	Project();
	~Project();

	//data access
	void SetData(data_base *obj);
	void GetData(data_base *obj);

	//objective function methods
	bool D();
	bool M();
	bool O();
	bool S(float *sf_avail = 0, float *sf_soil = 0, float *sf_degr = 0);
	int E();
	int F();
	int Z();

	data_base *GetVarPtr(const char *name);

	// def setup_clusters(self, Nclusters, Ndays = 2, Nprev = 1, Nnext = 1, user_weights = None, user_divisions = None):
	// def M(self, variables, design):
	// def O(self, variables, design):
	// def S(self, design, variables, sf_avail=None, sf_soil=None, sf_degr=None, sample_weeks=None, Nclusters = None, cluster_inputs = None, pv_production = None):
	// def E(self, variables):
	// def F(self, variables, S, om_cost): #total_installed_cost, generation, pricing_mult):
	// def Z(self, variables, **kwargs):

};





#endif // !_PROJECT
