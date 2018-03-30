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

enum DATATYPE { TYPE_INT, TYPE_NUMBER, TYPE_STRING, TYPE_VECTOR, TYPE_MATRIX };

class data_base
{
public:
	std::string name;
	unsigned int type;
};

template<typename T> class data_unit : public data_base
{
public:
	T val;
};

template<typename T> class variable : public data_unit<T>
{
public:
	//T val;
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
	
	void set_name(const std::string &vname)
	{
		this->name = vname;
	};

	void set(const T &v, const T &vmin, const T &vmax, std::string vname, const unsigned int datatype)
	{
		this->val = v;
		set_limits(vmin, vmax);
		set_name(vname);
		this->type = datatype;
	};
};

struct variables_base 
{
	virtual std::vector<data_base*>* GetMemberPointer()=0;
};

struct variables : public variables_base
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

template<typename T> class parameter
{
	
	T val;
	std::string ssc_name;
	unsigned int type;
	bool is_calculated;
};

struct parameters
{

	parameter< std::vector<double> > dispatch_factors_ts;
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
