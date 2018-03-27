#ifndef _PROJECT
#define _PROJECT

#include <string>
#include <vector>

/* 
A class containing the aspects of the current project
*/

//declared classes
class solarfield_availability;
class optical_degradation;


template<typename T> class variable
{
public:
	T val;
	T minval;
	T maxval;
	std::string name;

	std::string as_string()
	{
		return  std::to_string(val);
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
		name = vname;
	};

	void set(const T &v, const T &vmin, const T &vmax, std::string &vname)
	{
		val = v;
		set_limits(vmin, vmax);
		set_name(vname);
	};
};

struct variables
{
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
};

template<typename T> class parameter
{
	T val;
	std::string name;
};

struct parameters
{

	parameter< std::vector<double> > dispatch_factors_ts;
};

//main class
class Project
{
	solarfield_availability *m_solarfield_availability;
	optical_degradation *m_optical_degradation;


public:
	Project();
	~Project();

	variables m_variables;
	parameters m_parameters;

};





#endif // !_PROJECT
