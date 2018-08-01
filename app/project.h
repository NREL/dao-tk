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

class data_base : public lk::vardata_t
{
public:
	std::string name;
    unsigned char type;   //type defined in lk::vardata_t {NUMBER, STRING, VECTOR, HASH}

    void assign_vector(float *_vec, int nval)
    {
        this->empty_vector();
        this->vec()->resize(nval);
        for (int i = 0; i < nval; i++)
            this->vec()->at(i).assign(_vec[i]);
    }
};

class variable : public data_base
{
protected:
    void _set_base(double vmin, double vmax, std::string vname, const unsigned char datatype)
    {
		set_limits(vmin, vmax);
        this->name = vname;
		this->type = datatype;
    };

public:
    lk::vardata_t minval;
    lk::vardata_t maxval;
    lk::vardata_t defaultval;

	bool set_limits(double vmin, double vmax)
	{
		if (vmin >= vmax)
			return false;

        minval.assign(vmin);
		maxval.assign(vmax);
		return true;
	};
	
	void set(double _defaultval, double _vmin, double _vmax, std::string _vname, const unsigned char _datatype)
	{
        this->defaultval.assign(_defaultval); 
        this->assign(_defaultval);
        _set_base(_vmin, _vmax, _vname, _datatype);
	};
    
    void set(int _defaultval, double _vmin, double _vmax, std::string _vname, const unsigned char _datatype)
    {
        this->defaultval.assign(_defaultval);
        this->assign(_defaultval);
        _set_base(_vmin, _vmax, _vname, _datatype);
    };

};

struct variables : public lk::varhash_t
{
	variable h_tower;
	variable rec_height;
	variable D_rec;
	variable design_eff;
	variable dni_des;
	variable P_ref;
	variable solarm;
	variable tshours;
	variable degr_replace_limit;
	variable om_staff;
	variable n_wash_crews;
	variable N_panels;

	variables();
};

class parameter : public data_base
{
protected:
    void _set_base(std::string vname, const unsigned int datatype, bool calculated)
    {
        this->name = vname;
        this->type = datatype;
        is_calculated = calculated;
    };

public:
	bool is_calculated;
	//double
	void set(double v, std::string vname, const unsigned int datatype, bool calculated=false)
	{
		this->assign(v);
        _set_base(vname, datatype, calculated);
	};
    //int
    void set(int v, std::string vname, const unsigned int datatype, bool calculated = false)
    {
        this->assign(v);
        _set_base(vname, datatype, calculated);
    };
    //boolean
    void set(bool v, std::string vname, const unsigned int datatype, bool calculated = false)
    {
        this->assign(v);
        _set_base(vname, datatype, calculated);
    };
    //string
    void set(std::string v, std::string vname, const unsigned int datatype, bool calculated = false)
    {
        this->assign(v);
        _set_base(vname, datatype, calculated);
    };
    //vector-double
    void set(std::vector<double> &v, std::string vname, const unsigned int datatype, bool calculated = false)
    {
        this->empty_vector();
        this->vec()->resize(v.size());
        for (size_t i = 0; i < v.size(); i++)
            this->vec()->at(i).assign(v.at(i));

        _set_base(vname, datatype, calculated);
    };
    //matrix-double
    void set(std::vector< std::vector< double > > &v, std::string vname, const unsigned int datatype, bool calculated = false)
    {
        this->empty_vector();
        this->vec()->resize(v.size());
        for (size_t i = 0; i < v.size(); i++)
            for (size_t j = 0; j < v.at(i).size(); j++)
            {
                this->vec()->at(i).empty_vector();
                this->vec()->at(i).resize(v.at(i).size());
                this->vec()->at(i).assign(v.at(i).at(j));
            }
        _set_base(vname, datatype, calculated);
    };

};

struct parameters : public lk::varhash_t
{
	//-----------------------------------------------------------------------
	//bools
    parameter print_messages;
	parameter check_max_flux;
	parameter is_optimize;
	parameter is_dispatch;
	parameter is_ampl_engine;
	parameter is_stochastic_disp;
    //strings
	parameter ampl_data_dir;
	parameter solar_resource_file;
    //ints
	parameter disp_steps_per_hour;
	parameter avail_seed;
	parameter plant_lifetime;
	parameter finance_period;
	parameter ppa_multiplier_model;
    //doubles
	parameter rec_ref_cost;
	parameter rec_ref_area;
	parameter tes_spec_cost;
	parameter tower_fixed_cost;
	parameter tower_exp;
	parameter heliostat_spec_cost;
	parameter site_spec_cost;
	parameter land_spec_cost;
	parameter c_cps0;
	parameter c_cps1;
	parameter om_staff_cost;
	parameter wash_crew_cost;
	parameter heliostat_refurbish_cost;
	parameter helio_mtf;
	parameter heliostat_repair_cost;
	parameter om_staff_max_hours_week;
	parameter n_heliostats_sim;
	parameter wash_units_per_hour;
	parameter wash_crew_max_hours_week;
	parameter degr_per_hour;
	parameter degr_accel_per_year;
	parameter degr_seed;
	parameter soil_per_hour;
	parameter adjust_constant;
	parameter helio_reflectance;
	parameter disp_rsu_cost;
	parameter disp_csu_cost;
	parameter disp_pen_delta_w;
	parameter rec_su_delay;
	parameter rec_qf_delay;
	parameter startup_time;
	parameter startup_frac;
	parameter v_wind_max;
	parameter flux_max;
    //vector-doubles
	parameter c_ces;
	parameter dispatch_factors_ts;
	//-----------------------------------------------------------------------

	parameters();

};

struct design_outputs : public lk::varhash_t
{
	//-----------------------------------------------------------------------
	parameter number_heliostats;
	parameter area_sf;
	parameter base_land_area;
	parameter land_area;
	parameter h_tower_opt;
	parameter rec_height_opt;
	parameter rec_aspect_opt;
	parameter cost_rec_tot;
	parameter cost_sf_tot;
	parameter cost_sf_real;
	parameter cost_tower_tot;
	parameter cost_land_tot;
	parameter cost_land_real;
	parameter cost_site_tot;
	parameter flux_max_observed;
	parameter opteff_table;
	parameter flux_table;
	parameter heliostat_positions;
	//-----------------------------------------------------------------------

	design_outputs();

};

struct solarfield_outputs : public lk::varhash_t
{
	parameter n_repairs;
	parameter staff_utilization;
	parameter heliostat_repair_cost_y1;
	parameter heliostat_repair_cost;
	parameter avail_schedule;

	solarfield_outputs();

};

struct optical_outputs : public lk::varhash_t
{
	parameter n_replacements;
	parameter heliostat_refurbish_cost;
	parameter heliostat_refurbish_cost_y1;
	parameter avg_soil;
	parameter avg_degr;

	parameter soil_schedule;
	parameter degr_schedule;
	parameter repl_schedule;
	parameter repl_total;

	optical_outputs();

};

struct cycle_outputs : public lk::varhash_t
{
	cycle_outputs() {};
};

//main class
class Project
{
	ssc_data_t m_ssc_data;
	
	lk::varhash_t _merged_data;

	void lk_hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars);
    void ssc_to_lk_hash(ssc_data_t &cxt, lk::varhash_t &vars);
    //void lk_var_to_data(lk::vardata_t &var, data_base &data);
	void initialize_ssc_project();
	//void update_sscdata_from_object(datas_base &obj);
	//void update_object_from_sscdata(datas_base &obj);
	//void sscdata_localdata(data_base *obj, bool set_ssc_from_local);       //set ssc data from obj (set_ssc_from_local=true), or set local from ssc (false)
	//void sscdata_localdata_map(datas_base &objs, bool set_ssc_from_local);  //set ssc data from list (set_ssc_from_local=true), or set local from ssc (false)
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
	//void SetData(data_base *obj);
	//void GetData(data_base *obj);

	//objective function methods
	bool D();
	bool M();
	bool O();
	bool S(float *sf_avail = 0, float *sf_soil = 0, float *sf_degr = 0);
	int E();
	int F();
	int Z();

	data_base *GetVarPtr(const char *name);
	lk::varhash_t *GetMergedData();

	// def setup_clusters(self, Nclusters, Ndays = 2, Nprev = 1, Nnext = 1, user_weights = None, user_divisions = None):
	// def M(self, variables, design):
	// def O(self, variables, design):
	// def S(self, design, variables, sf_avail=None, sf_soil=None, sf_degr=None, sample_weeks=None, Nclusters = None, cluster_inputs = None, pv_production = None):
	// def E(self, variables):
	// def F(self, variables, S, om_cost): #total_installed_cost, generation, pricing_mult):
	// def Z(self, variables, **kwargs):

};





#endif // !_PROJECT
