#ifndef _PROJECT
#define _PROJECT

#include <string>
#include <vector>

#include <lk/env.h>
#include <ssc/sscapi.h>
#include "../liboptical/optical_degr.h"
#include "../libsolar/solarfield_avail.h"
#include "../libcycle/plant.h"
#include "../libcycle/well512.h"

#include "../libcluster/clustersim.h"
#include "../libclearsky/clearsky.h"

/* 
A class containing the aspects of the current project
*/

#define SIGNIF_FIGURE 5 	//specify the significant digit requirement for data storage

extern ssc_bool_t ssc_progress_handler(ssc_module_t, ssc_handler_t, int action, float f0, float f1, const char *s0, const char *, void *);
extern bool sim_progress_handler(float progress, const char *msg);
extern void message_handler(const char *msg);
extern int double_scale(double val, int *scale);
extern double double_unscale(int val, int power);

struct documentation
{
    std::string formatted_doc;
    std::string description;
    std::string units;

    documentation()
    {
        formatted_doc.clear();
        description.clear();
        units.clear();
    };

    void set(const char* _units, const char* _description)
    {
        this->description = _description;
        this->units = _units;
    };
};

class data_base : public lk::vardata_t
{
protected:
	bool m_is_invalid_allowed; //allow values to contain invalid data during simulation (values will be assigned/updated by the program)
    std::string BaseFormattedDoc(bool is_calculated, const char* limits = 0)
    {
        char buf[2000];

        std::string vartype;
        switch (this->type())
        {
        case lk::vardata_t::NUMBER:
            vartype = "Number";
            break;
        case lk::vardata_t::STRING:
            vartype = "String";
            break;
        case lk::vardata_t::VECTOR:
            vartype = "Array";
            break;
        case lk::vardata_t::HASH:
            vartype = "Table";
            break;
        default:
            break;
        }

        std::string fmt_units;
        if (this->doc.units.empty() || this->doc.units == "-")
            fmt_units = "";
        else
            fmt_units = "[" + this->doc.units + "]";

        sprintf(buf,
            "<h3>%s <font color=\"#C0C0C0\">%s</font></h3>"
            "<font color=\"#800000\">"
            "<table style=\"background-color:#DDD\">"
            "<tr><td>Handle</td><td><b>%s</b></td></tr>"
            "<tr><td>Group</td><td>%s</td></tr>"
            "<tr><td>Type</td><td>%s%s</td></tr>"
            "</table>"
            "</font>"
            "<p><font size=\"+1\">%s</font></p><br><a href=\"id?%s\">Add</a><hr>",
            this->nice_name.c_str(),
            fmt_units.c_str(),
            this->name.c_str(), 
            this->group.c_str(),
            vartype.c_str(),
            is_calculated ? " (calculated)" : "",
            this->doc.description.c_str(),
            this->name.c_str()
            );
        
        this->doc.formatted_doc = buf;

        return std::string(buf);
    };

public:
	std::string name;
	std::string nice_name;
	std::string units;
	std::string group;
	bool is_shown_in_list;
    documentation doc;

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

    virtual void CreateDoc() {};
};

class variable : public data_base
{
protected:
    void _set_base(double vmin, double vmax, std::string vname, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
		set_limits(vmin, vmax);
        this->name = vname;
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
	
	void set(double _defaultval, double _vmin, double _vmax, std::string _vname, const char *_nice_name=0, const char *_units=0, const char *_group=0)
	{
        this->defaultval.assign(_defaultval); 
        this->assign(_defaultval);
        _set_base(_vmin, _vmax, _vname, _nice_name, _units, _group);
	};
    
    void set(int _defaultval, double _vmin, double _vmax, std::string _vname, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->defaultval.assign(_defaultval);
        this->assign(_defaultval);
        _set_base(_vmin, _vmax, _vname, _nice_name, _units, _group);
    };

    void CreateDoc()
    {
        char lims[100];
        sprintf(lims, "[%f,%f]", this->minval.as_number(), this->maxval.as_number());
        BaseFormattedDoc(false, lims);
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
    void _set_base(std::string vname, bool calculated, const char *_nice_name, const char *_units, const char *_group)
    {
        this->name = vname;
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
	void set(double v, std::string vname, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
	{
		this->assign(v);
        _set_base(vname, calculated, _nice_name, _units, _group) ;
	};
    //int
    void set(int v, std::string vname, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->assign(v);
        _set_base(vname, calculated, _nice_name, _units, _group) ;
    };
    //boolean
    void set(bool v, std::string vname, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->assign(v);
        _set_base(vname, calculated, _nice_name, _units, _group) ;
    };
    //string
    void set(std::string v, std::string vname, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->assign(v);
        _set_base(vname, calculated, _nice_name, _units, _group) ;
    };
    //vector-double
    void set(std::vector<double> &v, std::string vname, bool calculated, const char *_nice_name=0, const char *_units=0, const char *_group=0)
    {
        this->empty_vector();
        this->vec()->resize(v.size());
        for (size_t i = 0; i < v.size(); i++)
            this->vec()->at(i).assign(v.at(i));

        _set_base(vname, calculated, _nice_name, _units, _group);
    };
    //matrix-double
    void set(std::vector< std::vector< double > > &v, std::string vname, bool calculated = false, const char *_nice_name=0, const char *_units=0, const char *_group=0)
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
        _set_base(vname, calculated, _nice_name, _units, _group);
    };

    void CreateDoc()
    {
        BaseFormattedDoc(is_calculated, 0);
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
	parameter current_standby;
	parameter stop_cycle_at_first_failure;
	parameter stop_cycle_at_first_repair;
	parameter is_use_clusters;
	parameter is_run_continuous;
	parameter is_hard_partitions;
	parameter is_cycle_ssc_integration;
	parameter is_reoptimize_at_repairs;
	parameter is_reoptimize_at_failures;
	parameter is_use_target_heuristic;

    //strings
	parameter ampl_data_dir;
	parameter solar_resource_file;
	parameter helio_repair_priority;
	parameter cluster_algorithm;
    //ints
	parameter disp_steps_per_hour;
	parameter avail_seed;
	parameter plant_lifetime;
	parameter finance_period;
	parameter ppa_multiplier_model;
	parameter num_condenser_trains;
	parameter fans_per_train;
	parameter radiators_per_train;
	parameter num_salt_steam_trains;
	parameter num_fwh;
	parameter num_salt_pumps;
	parameter num_salt_pumps_required;
	parameter num_water_pumps;
	parameter num_turbines;
	parameter read_periods;
	parameter sim_length;
	parameter num_scenarios;
	parameter n_clusters;
	parameter cluster_ndays;
	parameter cluster_nprev;
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
	parameter maintenance_interval;
	parameter maintenance_duration;
	parameter downtime_threshold;
	parameter steplength;
	parameter hours_to_maintenance;
	parameter power_output;
	parameter capacity;
	parameter temp_threshold;
	parameter time_online;
	parameter time_in_standby;
	parameter downtime;
	parameter shutdown_capacity;
	parameter no_restart_capacity;
	parameter shutdown_efficiency;
	parameter no_restart_efficiency;
	parameter cycle_hourly_labor_cost;
	parameter avail_model_timestep;
    //vector-doubles
	parameter c_ces;
	parameter dispatch_factors_ts;
	parameter user_sf_avail;
	parameter condenser_eff_cold;
	parameter condenser_eff_hot;
	parameter cycle_power;
	parameter ambient_temperature;
	parameter standby;

	parameter helio_comp_weibull_shape;
	parameter helio_comp_weibull_scale;
	parameter helio_comp_mtr;
	parameter helio_comp_repair_cost;
	parameter clustering_feature_weights;
	parameter clustering_feature_divisions;

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
	parameter cycle_efficiency;
	parameter cycle_capacity;
	parameter cycle_labor_cost;
	parameter turbine_efficiency;
	parameter turbine_capacity;
	parameter expected_time_to_next_cycle_failure;
	parameter expected_starts_to_next_cycle_failure;
	parameter period_of_last_failure;
	parameter num_failures;

	cycle_outputs();
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
	parameter annual_revenue_units;
	parameter annual_rec_starts;
	parameter annual_cycle_starts;
	parameter annual_cycle_ramp;
	parameter cycle_ramp_index;
	parameter annual_rec_starts_disp;
	parameter annual_cycle_starts_disp;
	parameter annual_cycle_ramp_disp;
	parameter cycle_ramp_index_disp;

	parameter cycle_capacity_avail;
	parameter cycle_efficiency_avail;

	simulation_outputs();
};

struct explicit_outputs : public lk::varhash_t
{

	parameter cost_receiver_real;
	parameter cost_tower_real;
	parameter cost_plant_real;
	parameter cost_tes_real;

	parameter heliostat_om_labor_y1;
	parameter heliostat_om_labor_real;
	parameter heliostat_wash_cost_y1;
	parameter heliostat_wash_cost_real;

	explicit_outputs();
};

struct financial_outputs : public lk::varhash_t 
{
	parameter lcoe_nom;
	parameter lcoe_real;
	parameter ppa;
	parameter project_return_aftertax_npv;
	parameter project_return_aftertax_irr;
	parameter total_installed_cost;

	financial_outputs();
};

struct objective_outputs : public lk::varhash_t
{
	parameter cost_receiver_real;
	parameter cost_tower_real;
	parameter cost_plant_real;
	parameter cost_tes_real;
	parameter cost_land_real;
	parameter cost_sf_real;
	parameter cap_cost_real;

	parameter rec_start_cost_real;
	parameter cycle_start_cost_real;
	parameter cycle_ramp_cost_real;
	parameter heliostat_repair_cost_real;
	parameter heliostat_om_labor_real;
	parameter heliostat_wash_cost_real;
	parameter heliostat_refurbish_cost_real;
	parameter om_cost_real;

	parameter cycle_repair_cost_real;

	parameter sales;
	parameter cash_flow;
	parameter ppa;
	parameter lcoe_nom;
	parameter lcoe_real;

	objective_outputs();
};

//main class
class Project
{
	bool is_design_valid;
	bool is_sf_avail_valid;
	bool is_sf_optical_valid;
	bool is_cycle_avail_valid;
	bool is_simulation_valid;
	bool is_explicit_valid;
	bool is_financial_valid;

	ssc_data_t m_ssc_data;
	
	lk::varhash_t _merged_data;

    void add_documentation();
	void lk_hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars);
    void ssc_to_lk_hash(ssc_data_t &cxt, lk::varhash_t &vars);
	void initialize_ssc_project();
	void update_calculated_system_values();
	void update_calculated_values_post_layout();
	double calc_real_dollars(const double &dollars, bool is_revenue=false, bool is_labor=false);
	
	
	bool simulate_system(PowerCycle &pc);
	void calc_avg_annual_schedule(double original_ts, double new_ts, const parameter &full_sch, std::vector<double> &output_sch);
	
	bool integrate_cycle_and_simulation(PowerCycle &pc, double start_time, double end_time, double horizon, bool start_stored_pc_state,
									std::unordered_map<std::string, std::vector<double>>&initial_ssc_soln,
									std::unordered_map<std::string, std::vector<double>> &soln, std::vector<double> &capacity, std::vector<double> &efficiency);

	double estimate_capacity_factor(double sm, double tes);
	


public:

	variables m_variables;
	parameters m_parameters;
	design_outputs m_design_outputs;
	solarfield_outputs m_solarfield_outputs;
	optical_outputs m_optical_outputs;
	cycle_outputs m_cycle_outputs;
	simulation_outputs m_simulation_outputs;
	explicit_outputs m_explicit_outputs;
	financial_outputs m_financial_outputs;
	objective_outputs m_objective_outputs;

	s_metric_outputs metric_outputs;
	s_cluster_outputs cluster_outputs;

	Project();
	~Project();

	struct CALLING_SIM{ enum E {DESIGN=1, HELIO_AVAIL, HELIO_OPTIC, CYCLE_AVAIL, SIMULATION, EXPLICIT, FINANCE, OBJECTIVE, NULLSIM=0}; };
	bool Validate(CALLING_SIM::E simtype=CALLING_SIM::E::NULLSIM, std::string *error_msg=0);
	void Initialize();

	//objective function methods
	bool D();
	bool M();
	bool C();
	bool O();
	bool S();
	bool E();
	bool F();
	bool Z();

	bool setup_clusters();


	data_base *GetVarPtr(const char *name);
	lk::varhash_t *GetMergedData();
    std::vector< void* > GetDataObjects();

	// def setup_clusters(self, Nclusters, Ndays = 2, Nprev = 1, Nnext = 1, user_weights = None, user_divisions = None):
	// def M(self, variables, design):
	// def O(self, variables, design):
	// def S(self, design, variables, sf_avail=None, sf_soil=None, sf_degr=None, sample_weeks=None, Nclusters = None, cluster_inputs = None, pv_production = None):
	// def E(self, variables):
	// def F(self, variables, S, om_cost): #total_installed_cost, generation, pricing_mult):
	// def Z(self, variables, **kwargs):

};



#endif // !_PROJECT
