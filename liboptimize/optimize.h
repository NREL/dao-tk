#ifndef __LIBOPT_H_
#define __LIBOPT_H_

#include <vector>
#include <string>
#include <limits>

namespace optimize
{
    std::vector<double> main( void (*f)(std::vector<double>), std::vector<double> LB, std::vector<double> UB,
                std::vector< std::vector< double > > X, 
                bool data_out = false, bool trust = false, bool convex_flag = false, 
                int max_delta = std::numeric_limits<double>::infinity() );

    
} // optimize

#endif