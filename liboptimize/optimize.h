#ifndef __LIBOPT_H_
#define __LIBOPT_H_

#include <vector>
#include <string>
#include <limits>
#include <unordered_map>

namespace Optimize
{
    std::unordered_map<std::string, std::vector<double> > main( double (*f)(std::vector<int>&), 
                std::vector<int> LB, std::vector<int> UB,
                std::vector< std::vector< int > > X, 
                bool data_out = false, bool trust = false, bool convex_flag = false, 
                int max_delta = std::numeric_limits<double>::infinity() );

        
} // optimize




#endif