#ifndef _PROJECT
#define _PROJECT

#include <string>
#include <vector>

#include <lk/env.h>
#include <ssc/sscapi.h>
#include "../liboptical/optical_degr.h"
#include "../libsolar/solarfield_avail.h"

#include "../libcluster/clustersim.h"
#include "../libclearsky/clearsky.h"

/* 
A class containing the aspects of the current project
*/

extern ssc_bool_t ssc_progress_handler(ssc_module_t, ssc_handler_t, int action, float f0, float f1, const char *s0, const char *, void *);
extern bool sim_progress_handler(float progress, const char *msg);
extern void message_handler(const char *msg);

class data_base : public lk::vardata_t
{
protected:
	bool m_is_invalid_allowed; //allow values to contain invalid data during simulation (values will be assigned/updated by the program)
public:
	std::string name;
    unsigned char type;   //type defined in lk::vardata_t {NUMBER, STRING, VECTOR, HASH}
	std::string nice_name;
	std::string units;
	std::string group;
	bool is_shown_in_list;

    void assign_vector(float *_vec, int nval)
    {
        this->empty_vector();
        this->vec()->resize(nval);
        for (int i = 0; i < nval; i++)
            this->vec()->at(i).assign(_vec[i]);
    };

    void assign_vector(std::vector<double> &vec)
    {
        this->empty_vector();
        this->vec()->resize(vec.size());
        for (size_t i = 0; i < vec.size(); i++)
            this->vec()->at(i).assign(vec.at(i));
    };
	std::string GetDisplayName()
	{
		return wxString::Format("[%s] %s", units, nice_name).ToStdString();
	};

	bool IsInvalidAllowed()
	{
		return m_is_invalid_allowed;
	};
};

class variable : public data_base
{
protected:
    void _set_base(double vmin, double vmax, std::string vname, const unsigned char datatype, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
		set_limits(vmin, vmax);
        this->name = vname;
		this->type = datatype;
		this->nice_name = _nice_name; 
		this->units = _units;
		this->group = _group;
		this->is_shown_in_list = true;
		this->m_is_invalid_allowed = false;
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
	
	void set(double _defaultval, double _vmin, double _vmax, std::string _vname, const unsigned char _datatype, const char *_nice_name=0, const char *_units=0, const char *_group=0)
	{
        this->defaultval.assign(_defaultval); 
        this->assign(_defaultval);
        _set_base(_vmin, _vmax, _vname, _datatype, _nice_name, _units, _group);
	};
    
    void set(int _defaultval, double _vmin, double _vmax, std::string _vname, const unsigned char _datatype, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->defaultval.assign(_defaultval);
        this->assign(_defaultval);
        _set_base(_vmin, _vmax, _vname, _datatype, _nice_name, _units, _group);
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
    void _set_base(std::string vname, const unsigned int datatype, bool calculated, const char *_nice_name, const char *_units, const char *_group)
    {
        this->name = vname;
        this->type = datatype;
		this->nice_name = _nice_name != '\0' ? _nice_name : "";
		this->units = _units != '\0' ? _units : "";
		this->group = _group != '\0' ? _group : "";
		this->is_shown_in_list = true;
        is_calculated = calculated;
		this->m_is_invalid_allowed = is_calculated ? true : false;
    };

public:
	bool is_calculated;
	//double
	void set(double v, std::string vname, const unsigned int datatype, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
	{
		this->assign(v);
        _set_base(vname, datatype, calculated, _nice_name, _units, _group) ;
	};
    //int
    void set(int v, std::string vname, const unsigned int datatype, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->assign(v);
        _set_base(vname, datatype, calculated, _nice_name, _units, _group) ;
    };
    //boolean
    void set(bool v, std::string vname, const unsigned int datatype, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->assign(v);
        _set_base(vname, datatype, calculated, _nice_name, _units, _group) ;
    };
    //string
    void set(std::string v, std::string vname, const unsigned int datatype, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->assign(v);
        _set_base(vname, datatype, calculated, _nice_name, _units, _group) ;
    };
    //vector-double
    void set(std::vector<double> &v, std::string vname, const unsigned int datatype, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->empty_vector();
        this->vec()->resize(v.size());
        for (size_t i = 0; i < v.size(); i++)
            this->vec()->at(i).assign(v.at(i));

        _set_base(vname, datatype, calculated, _nice_name, _units, _group);
    };
    //matrix-double
    void set(std::vector< std::vector< double > > &v, std::string vname, const unsigned int datatype, bool calculated = false, const char *_nice_name=0, const char *_units=0, const char *_group=0)
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
        _set_base(vname, datatype, calculated, _nice_name, _units, _group);
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
	parameter helio_repair_priority;
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
	parameter avail_model_timestep;
    //vector-doubles
	parameter c_ces;
	parameter dispatch_factors_ts;
	parameter user_sf_avail;
	parameter helio_comp_weibull_shape;
	parameter helio_comp_weibull_scale;
	parameter helio_comp_mtr;
	parameter helio_comp_repair_cost;
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

	parameter avg_avail;
	parameter n_repairs_per_component;

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

struct simulation_outputs : public lk::varhash_t
{
	parameter generation_arr;
	parameter solar_field_power_arr;
	parameter tes_charge_state;
	parameter dni_arr;
	parameter price_arr;
	parameter dni_templates;
	parameter price_templates;
	parameter annual_generation;
	parameter annual_revenue;

	simulation_outputs();
};

struct explicit_outputs : public lk::varhash_t
{

	parameter cost_receiver;
	parameter cost_tower;
	parameter cost_plant;
	parameter cost_tes;

	parameter heliostat_om_labor_y1;
	parameter heliostat_om_labor;
	parameter heliostat_wash_cost_y1;
	parameter heliostat_wash_cost;

	explicit_outputs();
};


struct project_cluster_inputs
{
	int ncluster;
	int nsim;
	int nprev;
	int alg;
	bool hard_partitions;
	bool is_run_continuous;
	bool is_run_full;

	project_cluster_inputs()
	{
		initialize();
	};
	
	void initialize()
	{
		ncluster = nsim = nprev = alg = -1;
		hard_partitions = true;
		is_run_continuous = false;
		is_run_full = false;
	};

};

//main class
class Project
{
	bool is_design_valid;
	bool is_sf_avail_valid;
	bool is_sf_optical_valid;
	bool is_simulation_valid;
	bool is_explicit_valid;

	ssc_data_t m_ssc_data;
	
	lk::varhash_t _merged_data;

	void lk_hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars);
    void ssc_to_lk_hash(ssc_data_t &cxt, lk::varhash_t &vars);
	void initialize_ssc_project();
	void update_calculated_system_values();
	void update_calculated_values_post_layout();
	double calc_real_dollars(const double &dollars, bool is_revenue=false, bool is_labor=false);
	
	void setup_clusters(const project_cluster_inputs &user_inputs, const std::vector<double> &sfavail, s_metric_outputs &metric_results, s_cluster_outputs &cluster_results);
	bool simulate_system(const project_cluster_inputs &user_inputs, const std::vector<double> &sfavail);

public:

	variables m_variables;
	parameters m_parameters;
	project_cluster_inputs m_cluster_parameters;
	design_outputs m_design_outputs;
	solarfield_outputs m_solarfield_outputs;
	optical_outputs m_optical_outputs;
	cycle_outputs m_cycle_outputs;
	simulation_outputs m_simulation_outputs;
	explicit_outputs m_explicit_outputs;

	Project();
	~Project();

	struct CALLING_SIM{ enum E {DESIGN=1, HELIO_AVAIL, HELIO_OPTIC, SIMULATION, EXPLICIT, FINANCE, OBJECTIVE, NULLSIM=0}; };
	bool Validate(CALLING_SIM::E simtype=CALLING_SIM::E::NULLSIM, std::string *error_msg=0);
	void Initialize();

	//objective function methods
	bool D();
	bool M();
	bool O();
	bool S();
	bool E();
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
