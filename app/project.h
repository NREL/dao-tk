#ifndef _PROJECT
#define _PROJECT

#include <string>
#include <vector>
#include <sstream>
#include <set>

#include <lk/env.h>
#include <ssc/sscapi.h>
#include "../liboptical/optical_degr.h"
#include "../liboptical/wash_opt.h"
#include "../libsolar/solarfield_avail.h"
#include "../libsolar/solarfield_opt.h"
#include "../libcycle/plant.h"
#include "../libcycle/well512.h"

#include "../libcluster/clustersim.h"
#include "../libclearsky/clearsky.h"

/* 
A class containing the aspects of the current project
*/

#define SIGNIF_FIGURE 5 	//specify the significant digit requirement for data storage

class Project;

extern ssc_bool_t ssc_progress_handler(ssc_module_t, ssc_handler_t, int action, float f0, float f1, const char *s0, const char *, void *);
extern ssc_bool_t ssc_silent_handler(ssc_module_t, ssc_handler_t, int action, float, float, const char *, const char *, void *);
extern bool sim_progress_handler(float progress, const char *msg);
extern void message_handler(const char *msg);
extern void iterplot_update_handler();
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

typedef std::pair< std::string, std::vector<double> > svd_pair;

struct ordered_hash_vector
{
private:
    std::vector< svd_pair > _items;
    std::vector< std::string > _keys;
public:

    std::vector<double>* has_item(std::string key)
    {
        ptrdiff_t ind = std::find(_keys.begin(), _keys.end(), key) - _keys.begin();
        if( ind == (ptrdiff_t)_keys.size() )
        {
            return 0;
        }
        else
        {
            return &_items.at(ind).second;
        }
    };

    std::vector<double>& operator[](const std::string& key)
    {
        std::vector<double>* it = has_item(key);
        if (it)
        {
            return *it;
        }
        else
        {
            _items.push_back(svd_pair(key, std::vector<double>()));
            _keys.push_back(key);
            return _items.back().second;
        }
    };

    void sort(bool reverse = false)
    {
        _keys.clear();

        if(reverse)
            std::sort(_items.begin(), _items.end(), [](const svd_pair &a, const svd_pair &b) ->bool {return a.first > b.first; });
        else
            std::sort(_items.begin(), _items.end(), [](const svd_pair &a, const svd_pair &b) ->bool {return a.first < b.first; });

        for (size_t i = 0; i < _items.size(); i++)
            _keys.push_back(_items.at(i).first);
    };

    size_t item_count()
    {
        return _items.size();
    };

    size_t iteration_count()
    {
        if (_items.size() > 0)
            return _items.front().second.size();
        else
            return 0;
    };

    svd_pair& at_index(int ind)
    {
        return _items.at(ind);
    };

    void clear()
    {
        _keys.clear();
        _items.clear();
    };

    ordered_hash_vector slice(int left, int right)
    {
        /*
        Return a subset of the current ordered_hash_vector between indices [left, right).
        */
        ordered_hash_vector res;
        int ruse = right > (int)item_count() - 1 ? (int)item_count() : right;
        int luse = left < 0 ? 0 : left;
        for (int i = luse; i < ruse; i++)
        {
            svd_pair respr = at_index(i);
            res[respr.first] = respr.second;
        }
        return res;
    }
};

class data_base : public lk::vardata_t
{
protected:
	bool m_is_invalid_allowed; //allow values to contain invalid data during simulation (values will be assigned/updated by the program)
    std::string BaseFormattedDoc(bool is_calculated, const char* limits = 0)
    {
        std::stringstream buf;
        //char buf[2000];

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

        std::string valstr = this->as_string();
        if (valstr.size() > 15)
        {
            valstr.erase(valstr.begin() + 15, valstr.end());
            valstr.append("...");
        }
        

        //sprintf(buf,
        buf << "<h3 name=\"doc_" << this->name << "\">" << this->nice_name << " <font color=\"#C0C0C0\">" << fmt_units << "</font></h3>";
        buf << "<font color=\"#800000\">";
        buf << "<table style=\"background-color:#DDD\">";
        buf << "<tr><td>Handle</td><td><b>" << this->name << "</b></td></tr>";
        buf << "<tr><td>Group</td><td>" << this->group << "</td></tr>";
        buf << "<tr><td>Type</td><td>" << vartype << (is_calculated ? " (calculated)" : "") << "</td></tr>";
        buf << "<tr><td>Default</td><td>" << valstr << "</td></tr>";
        if (limits)
            buf << "<tr><td>Limits</td><td>" << limits << "</td></tr>";
        buf << "</table>";
        buf << "</font>";
        buf << "<p><font size=\"+1\">" << this->doc.description << "</font></p><br>";
        buf << "<table style=\"background-color:#EEE;\"><tr><td><a href=\"sid?" << this->name;
        buf << "\">Insert SET</a></td><td><a href=\"gid?" << this->name << "\">Insert GET</a></td></tr></table><hr>",
        
        this->doc.formatted_doc = buf.str();

        return buf.str();
    };

public:
	std::string name;
	std::string nice_name;
	std::string units;
	std::string group;
	bool is_shown_in_list;
    documentation doc;

    ordered_hash_vector hash_vector;

	void assign_vector(float *_vec, int nval)
    {
        if (!_vec)
            return;
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
		std::stringstream ss;
		ss << "[" << units << "] " << nice_name;
		return ss.str();
	};

	bool IsInvalidAllowed()
	{
		return m_is_invalid_allowed;
	};

    virtual void CreateDoc() {};
};


struct ObjectiveMethodPtr
{
protected:
    bool (Project::*_m)();
public:
    ObjectiveMethodPtr() {};
    ObjectiveMethodPtr(bool (Project::*m)())
    {
        _m = m;
    };
    bool operator < (const ObjectiveMethodPtr &rhs) const
    {
        return (void*)&_m < rhs.MethodPointer();
    };
    bool operator > (const ObjectiveMethodPtr &rhs) const
    {
        return (void*)&_m > rhs.MethodPointer();
    };
    void operator = ( bool (Project::*rhs)() ) 
    {
        _m = rhs;
    };
    bool operator == (const ObjectiveMethodPtr &rhs) const 
    {
        return (void*)&_m == rhs.MethodPointer();
    };
    bool operator != (const ObjectiveMethodPtr &rhs) const
    {
        return (void*)&_m != rhs.MethodPointer();
    };
    void* MethodPointer() const
    {
        return (void*)&_m;
    };
    bool Run(Project* parent)
    {
        return (*parent.*_m)();
    };
};

typedef unordered_map<std::string, ObjectiveMethodPtr > ObjectiveMethodSet;

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
    lk::vardata_t initializers;
    std::vector< std::string > triggers;
    bool is_optimized;
    bool is_integer;

	bool set_limits(double vmin, double vmax)
	{
		if (vmin >= vmax)
			return false;

        minval.assign(vmin);
		maxval.assign(vmax);
		return true;
	};
	
	void set(double _defaultval, double _vmin, double _vmax, std::string _vname, const char *_nice_name=0, const char *_units=0, const char *_group=0, 
            bool _is_optimized = false, bool _is_integer = false)
	{
        this->defaultval.assign(_defaultval); 
        this->assign(_defaultval);
        this->is_optimized = _is_optimized;
        this->is_integer = _is_integer;
        _set_base(_vmin, _vmax, _vname, _nice_name, _units, _group);
	};
    
    void set(int _defaultval, double _vmin, double _vmax, std::string _vname, const char *_nice_name=0, const char *_units=0, const char *_group=0, 
            bool _is_optimized = false, bool _is_integer = false)
    {
        this->defaultval.assign(_defaultval);
        this->assign(_defaultval);
        this->is_optimized = _is_optimized;
        this->is_integer = _is_integer;
        _set_base(_vmin, _vmax, _vname, _nice_name, _units, _group);
    };

    void CreateDoc()
    {
        char lims[100];
        sprintf(lims, "[%f,%f]", this->minval.as_number(), this->maxval.as_number());
        BaseFormattedDoc(false, lims);
    };

};

class hash_base : public lk::varhash_t
{
public:
    virtual void initialize() = 0;
};

struct variables : public hash_base
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
	//variable om_staff;
	//variable n_wash_crews;
	variable N_panel_pairs;

	variables();
    void initialize();
};

class parameter : public data_base
{
protected:
    void _set_base(std::string vname, bool calculated, const char *_nice_name, const char *_units, const char *_group)
    {
        this->name = vname;
		if(_nice_name)
            this->nice_name = std::string(_nice_name);
        else
            this->nice_name = "";
		if(_units)
            this->units = std::string(_units);
        else
            this->units = "";
		if(_group)
            this->group = std::string(_group);
        else
            this->group = "";
        
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
        this->assign(v.c_str());
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

    void set(ordered_hash_vector& hv, std::string vname, bool calculated = false, const char *_nice_name = 0, const char *_units = 0, const char *_group = 0)
    {
        this->hash_vector.clear();
        this->empty_hash();     //create an empty hash to trigger the type definition as "table"

        for (size_t i = 0; i < hv.item_count(); i++)
        {
            svd_pair* p = &hv.at_index((int)i);
            this->hash_vector[p->first] = p->second;
        }
        _set_base(vname, calculated, _nice_name, _units, _group);
    }

    void CreateDoc()
    {
        BaseFormattedDoc(is_calculated, 0);
    };

};

struct parameters : public hash_base
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
	parameter is_use_clusters;
	parameter is_run_continuous;
	parameter is_cycle_avail;
	parameter is_reoptimize_at_repairs;
	parameter is_reoptimize_at_failures;
	parameter is_use_target_heuristic;
	parameter is_uniform_helio_assign;

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
	parameter num_water_pumps_required;
	parameter num_boiler_pumps;
	parameter num_boiler_pumps_required;
	parameter num_turbines;
	parameter num_cycle_scenarios;
	parameter n_clusters;
	parameter cluster_ndays;
	parameter cluster_nprev;
	parameter cycle_nyears;
	parameter wash_vehicle_life;
    parameter n_sim_threads;
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
	parameter wash_rate;
	parameter wash_crew_max_hours_day;
	parameter wash_crew_max_hours_week;
	parameter wash_crew_vehicle_cost;
	parameter price_per_kwh;
	parameter TES_powercycle_eff;
	parameter wash_crew_seasonal_multiple;
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
	parameter hours_to_maintenance;
	parameter temp_threshold;
	parameter shutdown_capacity;
	parameter no_restart_capacity;
	parameter shutdown_efficiency;
	parameter no_restart_efficiency;
	parameter cycle_hourly_labor_cost;
	parameter avail_model_timestep;
    parameter forecast_gamma;
	parameter convergence_tol_obj;
	parameter convergence_tol_step;
    //vector-doubles
	parameter c_ces;
	parameter dispatch_factors_ts;
    parameter wlim_series;
	parameter user_sf_avail;
	parameter condenser_eff_cold;
	parameter condenser_eff_hot;
	
	parameter helio_comp_weibull_shape;
	parameter helio_comp_weibull_scale;
	parameter helio_comp_mtr;
	parameter helio_comp_repair_cost;
	parameter clustering_feature_weights;
	parameter clustering_feature_divisions;

	//-----------------------------------------------------------------------

	parameters();
    void initialize();
};

struct design_outputs : public hash_base
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
	parameter annual_helio_energy;
	//-----------------------------------------------------------------------

	design_outputs();
    void initialize();

};

struct solarfield_outputs : public hash_base
{
	parameter n_om_staff;
	parameter n_repairs;
	parameter staff_utilization;
	parameter heliostat_repair_cost_y1;
	parameter heliostat_repair_cost;
	parameter avail_schedule;

	parameter avg_avail;
	parameter n_repairs_per_component;

	solarfield_outputs();
    void initialize();

};

struct optical_outputs : public hash_base
{
	parameter n_wash_vehicles;
	parameter wash_crew_schedule;
	parameter n_replacements;
	parameter heliostat_wash_labor_cost;
	parameter heliostat_wash_labor_cost_y1;
	parameter heliostat_refurbish_cost;
	parameter heliostat_refurbish_cost_y1;
	parameter avg_soil;
	parameter avg_degr;

	parameter soil_schedule;
	parameter degr_schedule;
	parameter repl_schedule;
	parameter repl_total;

	optical_outputs();
    void initialize();

};

struct cycle_outputs : public hash_base
{
	parameter cycle_efficiency;
	parameter cycle_capacity;
	parameter cycle_labor_cost;
	parameter num_failures;
    parameter cycle_efficiency_ave;
    parameter cycle_capacity_ave;
    
	cycle_outputs();
    void initialize();
};

struct simulation_outputs : public hash_base
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

	parameter gross_gen;
	parameter q_pb;
	parameter tamb;
	parameter is_standby;

	simulation_outputs();
    void initialize();
};

struct explicit_outputs : public hash_base
{

	parameter cost_receiver_real;
	parameter cost_tower_real;
	parameter cost_plant_real;
	parameter cost_tes_real;

	parameter heliostat_om_labor_y1;
	parameter heliostat_om_labor_real;
	parameter heliostat_wash_cost_y1;
	parameter heliostat_wash_cost_real;
	parameter heliostat_wash_capital_cost;

	explicit_outputs();
    void initialize();
};

struct financial_outputs : public hash_base 
{
	parameter lcoe_nom;
	parameter lcoe_real;
	parameter ppa;
	parameter project_return_aftertax_npv;
	parameter project_return_aftertax_irr;
	parameter total_installed_cost;

	financial_outputs();
    void initialize();
};

struct objective_outputs : public hash_base
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
	parameter heliostat_wash_capital_cost;
	parameter om_cost_real;

	parameter cycle_repair_cost_real;

	parameter sales;
	parameter cash_flow;
	parameter ppa;
	parameter lcoe_nom;
	parameter lcoe_real;

	objective_outputs();
    void initialize();
};

struct optimization_outputs : public hash_base
{
    parameter eta_i; //Matrix<double> 
    //Vector<double> 
    parameter obj_ub_i;
    parameter secants_i;
    parameter feas_secants_i;
    parameter eval_order;
    //Vector<long long> 
    parameter wall_time_i;
    parameter iteration_history;
    parameter best_point;

    optimization_outputs();
    void initialize();
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
    bool is_stop_flag;

	ssc_data_t m_ssc_data;
	
	lk::varhash_t _merged_data;

    ObjectiveMethodSet _all_method_pointers;
    std::vector<std::string> _all_method_names;


	struct plant_state
	{
		double tes_charge;
		double tes_thot;
		double tes_tcold;
		double capacity_avail;
		double efficiency_avail;
		double is_rec_on;
		double is_pc_on;
		double is_pc_standby;

		plant_state()
		{
			tes_charge = tes_thot = tes_tcold = std::numeric_limits<double>::quiet_NaN();
			capacity_avail = efficiency_avail = 1.;
			is_rec_on = is_pc_on = is_pc_standby = false;
		}
	};

	struct cycle_ssc_integration_inputs
	{
		double start_time;  //[hr]
		double end_time;    //[hr]
		double horizon;		//[hr]
		bool use_stored_state;  
		bool use_existing_ssc_soln;
		unordered_map < std::string, std::vector<double>> initial_ssc_soln;
		plant_state initial_state;

		cycle_ssc_integration_inputs()
		{
			start_time = 0.;
			end_time = 8760.;
			horizon = 168.;
			use_stored_state = use_existing_ssc_soln = false;
		}
	};

    void add_documentation();
	void lk_hash_to_ssc(ssc_data_t &cxt, lk::varhash_t &vars);
    void ssc_to_lk_hash(ssc_data_t &cxt, lk::varhash_t &vars);
	void initialize_ssc_project();
	void update_calculated_system_values();
	void update_calculated_values_post_layout();
	double calc_real_dollars(const double &dollars, bool is_revenue=false, bool is_labor=false, bool one_time_exp=false, int num_years=0);
	
	bool simulate_clusters(std::unordered_map<std::string, std::vector<double>> &ssc_soln);
	std::unordered_map<std::string, std::vector<double>> ssc_data_to_map(const ssc_data_t & ssc_data, std::vector<std::string> keys);
	bool accumulate_annual_results(const std::vector<double> &soln, double &sum, double &summult_price, double &starts, double &ramp, double &ramp_index);
	bool save_simulation_outputs(std::unordered_map < std::string, std::vector<double>> &ssc_soln);
	void calc_avg_annual_schedule(double original_ts, double new_ts, const parameter &full_sch, std::vector<double> &output_sch);
	
	void initialize_cycle_model(PowerCycle &pc);
	
	void save_cycle_outputs(std::vector<double>&capacity, std::vector<double>&efficiency, double n_failures, double labor_cost);

	bool integrate_cycle_and_simulation(PowerCycle &pc, const cycle_ssc_integration_inputs &inputs,
										plant_state &final_state, std::unordered_map<std::string, std::vector<double>> &soln,
										std::vector<double> &capacity, std::vector<double> &efficiency, int &n_failures, double &labor_cost);

	bool integrate_cycle_and_clusters(const std::unordered_map<std::string, std::vector<double>> &initial_soln, int first_index, int last_index, std::unordered_map<std::string, std::vector<double>> &final_soln);

    
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
    optimization_outputs m_optimization_outputs;

	s_metric_outputs metric_outputs;
	s_cluster_outputs cluster_outputs;

	Project();
	~Project();

	struct CALLING_SIM{ enum E {DESIGN=1, HELIO_AVAIL, HELIO_OPTIC, CYCLE_AVAIL, SIMULATION, EXPLICIT, FINANCE, OBJECTIVE, NULLSIM=0}; };
	bool Validate(CALLING_SIM::E simtype=CALLING_SIM::E::NULLSIM, std::string *error_msg=0);
	void Initialize();

	//objective function methods
	bool D();       //Solar field layout and design
	bool M();       //Heliostat mechanical availability
	bool C();       //Cycle availability 
	bool O();       //Optical degradation and soiling
	bool S();       //Production simulation
	bool E();       //Explicit cost calculations
	bool F();       //Financial model calculations
	bool Z();       //Rolled-up objective function

	bool setup_clusters();

	data_base *GetVarPtr(const char *name);
	lk::varhash_t *GetMergedData();
    std::vector< void* > GetDataObjects();
    bool CallMethodByName(const std::string &method);
    std::vector<std::string> GetAllMethodNames();
    void SetStopFlag(bool is_cancel);
    bool IsStopFlag();
    void PrintCurrentResults();
    void ClearStoredData();
    void AddToSSCContext(std::string varname, lk::vardata_t& dat);
};



#endif // !_PROJECT
