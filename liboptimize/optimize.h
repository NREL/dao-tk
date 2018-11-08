#ifndef __LIBOPT_H_
#define __LIBOPT_H_

#include <vector>
#include <string>
#include <limits>
#include <unordered_map>

struct optimization_settings
{
    double (*f_objective)(std::vector<int>&);
    std::vector<int> lower_bounds;
    std::vector<int> upper_bounds;
    std::vector< std::vector< int > > X_sample;

    bool trust;
    bool convex_flag;
    double max_delta;

    optimization_settings()
    {
        trust = false;
        convex_flag = false;
        max_delta = std::numeric_limits<double>::infinity();
    };
};

struct optimization_results
{
    std::vector< std::vector< double > > eta_i;
    std::vector<double> obj_ub_i, secants_i, feas_secants_i, eval_order, wall_time_i;
    std::vector<double> x_star;     //optimal solution location
};

class optimization
{
public:
    optimization_settings m_settings;
    optimization_results m_results;

    bool run_optimization();
    bool run_continuous_optimization(std::vector<int> &x_int);
                
}; // optimize




#endif