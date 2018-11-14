#ifndef __LIBOPT_H_
#define __LIBOPT_H_

#include <vector>
#include <string>
#include <limits>
#include <unordered_map>

template <typename T> //can be one of {int, double}
struct optimization_variable
{
    optimization_variable() {};
    optimization_variable(std::string _name, T _lower, T _upper, std::vector<T> _inits, T _value = (T)0.)
        : name(_name), lower_bound(_lower), upper_bound(_upper), initializers(_inits), value(_value) {};
    std::string name;
    T lower_bound;
    T upper_bound;
    std::vector< T > initializers;
    std::vector< T > iteration_history;
    T value;
};

struct optimization_settings
{
    double (*f_objective)(std::vector<int>&);

    std::vector< optimization_variable<int> > variables_int_t;
    std::vector< optimization_variable<double> > variables_dbl_t;

    bool trust;
    bool convex_flag;
    double max_delta;
    int n_initials;

    optimization_settings()
    {
        n_initials = 1;
        trust = false;
        convex_flag = false;
        max_delta = std::numeric_limits<double>::infinity();
    };
};

struct optimization_results
{
    std::vector<double> 
        obj_ub_i,   //optimization upper bound at iteration 'i'
        secants_i,  //secant values at iteration 'i'
        feas_secants_i,     //feasible secants at iteration 'i'
        eval_order,         //evaluation order at iteration 'i'
        wall_time_i;        //wall time elapsed at iteration 'i'
};

class optimization
{
public:
    optimization_settings m_settings;
    optimization_results m_results;

    bool run_optimization();
    bool run_continuous_subproblem();
                
}; // optimize




#endif