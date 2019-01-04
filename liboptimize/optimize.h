#ifndef __LIBOPT_H_
#define __LIBOPT_H_

#include <vector>
#include <string>
#include <limits>
#include <unordered_map>
#include "project.h"

class optimization_variable : public variable
{
    lk::vardata_t* m_parent;
public:
    std::vector< double > initializers;
    std::vector< double > iteration_history;

    //optimization_variable() {};

    optimization_variable(variable &v)
    {
        m_parent = &v;
        is_integer = v.is_integer;
        is_optimized = v.is_optimized;
        minval.assign(v.minval.as_number());
        maxval.assign(v.maxval.as_number());
        this->assign(v.as_number());
        this->name = v.name;
        this->nice_name = v.nice_name;
        this->units = v.units;
        iteration_history.clear();
        iteration_history.push_back(as_number());
        triggers = v.triggers;
        initializers.clear();
        if (v.initializers.vec())
        {
            for (size_t i = 0; i < v.initializers.vec()->size(); i++)
                initializers.push_back(v.initializers.vec()->at(i).as_number());
        }
    };

    /*optimization_variable(bool is_integer, std::string _name, double _lower, double _upper, std::vector<double> _inits, double _value = 0.)
    {
        this->is_integer = is_integer;
        this->name = _name;
        this->minval.assign(_lower);
        this->maxval.assign(_upper);
        initializers = _inits;
        this->assign(_value);
        
        iteration_history.clear();
        iteration_history.push_back(_value);
    };*/

    bool value_changed(double testval = std::numeric_limits<double>::quiet_NaN() )
    {
        if (std::isnan(testval))
            return this->as_number() == iteration_history.back();
        else
            return this->as_number() == testval;
    };

    void optimization_variable::assign(double d)
    {
        m_parent->assign(d);
        lk::vardata_t::assign(d);
        iteration_history.push_back(d);
    };
};

struct optimization_settings
{
    std::vector< optimization_variable > variables;

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

    std::vector<std::string> get_all_variable_names()
    {
        std::vector< std::string > varnames;
        for (size_t i = 0; i < variables.size(); i++)
            varnames.push_back(variables.at(i).name);

        return varnames;
    };

    std::vector< optimization_variable* > integer_variables()
    {
        std::vector< optimization_variable* > res;
        for (size_t i = 0; i < variables.size(); i++)
            if (variables.at(i).is_integer)
                res.push_back(&variables.at(i));
        return res;
    };

    std::vector< optimization_variable* > continuous_variables()
    {
        std::vector< optimization_variable* > res;
        for (size_t i = 0; i < variables.size(); i++)
            if (!variables.at(i).is_integer)
                res.push_back(&variables.at(i));
        return res;
    };
};

class optimization
{
    Project *m_project_ptr;
    int m_current_iteration;
    long long m_time_elapsed_ms;
    long long m_time_init_ms;
public:
    //optimization();
    optimization(Project* p);

    optimization_settings m_settings;

    Project* get_project();
    void set_project(Project *p);

    bool run_optimization();
    double run_continuous_subproblem();

    int get_and_up_iteration();
    long long get_time_init_ms();
                
}; // optimize




#endif