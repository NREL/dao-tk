#include "optimize.h"
#include "optutil.h"
#include <nlopt/nlopt.hpp>
#include <eigen/Core>
#include <eigen/Dense>
#include <exception>
#include <set>
#include <iostream>
#include <stdio.h>
#include <algorithm>

//------------------------------------------
template <typename T>
static inline bool assign_filter_nan(T c, void*)
{
   return c == c;
}

template <typename T>
static inline bool filter_where_lt(T c, T d)
{
    return c < d;
};
//------------------------------------------

//optimization::optimization() {};

optimization::optimization(Project* P) { m_project_ptr = P; }

void optimization::set_project(Project* P) { m_project_ptr = P; }

Project* optimization::get_project() { return m_project_ptr; }


double continuous_objective_eval(unsigned n, const double *x, double *, void *data)
{
    /* 
    For a given vector of continuous variables 'x', check which have been modified and call
    the necessary methods to update the objective function
    */

    optimization* O = static_cast<optimization* const>(data);

    //figure out which variables were changed and as a result, which components of the objective function need updating
    std::set<std::string> triggered_methods;
    int i = 0; 
    int ncheck = 0;
    std::stringstream message; 
    message << "Optimization evaluation point:\n";
    for (std::vector<optimization_variable>::iterator vit = O->m_settings.variables.begin(); vit != O->m_settings.variables.end(); vit++)
    {
        optimization_variable &v = *vit;

        if (!v.is_optimized)
            continue;

        //collect all triggered objective methods
        if (v.value_changed())
            for (size_t j = 0; j < v.triggers.size(); j++)
                triggered_methods.insert(v.triggers.at(j));

        message << v.nice_name << "[" << v.units << "]\t" << v.as_number() << "\n";
        

        //keep track of variable iteration history
        if (v.is_integer)
        {
            v.iteration_history.push_back(v.iteration_history.back());
        }
        else
        {
            double newx = x[i++];
            v.iteration_history.push_back(newx);
            v.assign(newx);
            ncheck++;
        }
    }

    if (ncheck != n)
        throw std::runtime_error("Error in continuous objective function evaluation. Variable count has changed. See user support for help.");

    Project* P = O->get_project();

    //run all of the methods in order
    std::vector<std::string> allmethods = P->GetAllMethodNames();
    for (std::vector<std::string>::iterator mit = allmethods.begin(); mit != allmethods.end(); mit++)
    {
        if (triggered_methods.find(*mit) != triggered_methods.end())
            P->CallMethodByName(*mit);
    }

    double ppa = P->m_financial_outputs.ppa.as_number();

    message << "Results\n-------------------------------------------------\n";
    message << "Objective function value (PPA) [c/kWh]\t" << ppa << "\n";
    message << "Solar field area [m2]\t" << P->m_design_outputs.area_sf.as_number() << "\n";
    message << "Average soiling eff. [%]\t" << P->m_optical_outputs.avg_soil.as_number() << "\n";
    message << "Average mirror degradation [%]\t" << P->m_optical_outputs.avg_degr.as_number() << "\n";
    message << "Annual generation [MWhe]\t" << P->m_simulation_outputs.annual_generation.as_number() << "\n";
    message << "Annual cycle starts\t" << P->m_simulation_outputs.annual_cycle_starts.as_number() << "\n";
    message << "Annual receiver starts\t" << P->m_simulation_outputs.annual_rec_starts.as_number() << "\n";
    message << "Annual revenue units\t" << P->m_simulation_outputs.annual_revenue_units.as_number() << "\n";
    message << "Average field availability [%]\t" << P->m_solarfield_outputs.avg_avail.as_number() << "\n";
    message << "Heliostat repair events /yr\t" << P->m_solarfield_outputs.n_repairs.as_number() << "\n";
    

    message_handler(message.str().c_str());

    return ppa;
};


double optimization::run_continuous_subproblem()
{
    /* 
    Optimize the continuous variable problem given the current fixed integer values
    specified in 'x_int'.


    */

    int nvmax = (int)m_settings.variables.size();
    int n = 0;
    double* x = new double[nvmax];
    double* ub = new double[nvmax];
    double* lb = new double[nvmax];

    //get initial state for all continuous variables x
    for (std::vector<optimization_variable>::iterator vit = m_settings.variables.begin(); vit != m_settings.variables.end(); vit++)
    {
        if (vit->is_optimized && !vit->is_integer)
        {
            lb[n] = (*vit).minval.as_number();
            ub[n] = (*vit).maxval.as_number();
            x[n++] = (*vit).as_number();
        }
    }
    
    double minf = std::numeric_limits<double>::infinity(); //initialize minimum obj function return value
    
    if (n < 1)
        goto CLEAN_AND_RETURN;

    //set up the NLOpt optimization problem
    nlopt_opt opt = nlopt_create(nlopt_algorithm::NLOPT_LN_BOBYQA, n);
    nlopt_set_lower_bounds(opt, lb);
    nlopt_set_upper_bounds(opt, ub);
    nlopt_set_min_objective(opt, continuous_objective_eval, (void*)this);

    nlopt_set_ftol_rel(opt, .01);
    nlopt_set_xtol_rel(opt, .001);

    int res = nlopt_optimize(opt, x, &minf);

    if (res < 0)
    {
        std::string message;
        switch (res)
        {
        case NLOPT_INVALID_ARGS: // = -2,
            message = "Invalid arguments in continuous optimization subproblem. Ensure lower and upper "
                "bounds are correct and initial value(s) are within the specified bounds.";
            break;
        case NLOPT_OUT_OF_MEMORY: // = -3,
            message = "Memory error! DAO-Tk ran out of memory while executing the continuous optimization subproblem.";
            break;
        case NLOPT_FORCED_STOP: // = -5,
        case NLOPT_ROUNDOFF_LIMITED: // = -4,
        case NLOPT_FAILURE: // = -1, /* generic failure code */
        default:
            message = "An error occurred while executing the continuous optimization subproblem.";
        }
        message_handler((message + "\n").c_str());

        goto CLEAN_AND_RETURN;
    }
    else
    {
        std::string message;

        switch (res)
        {
        case NLOPT_SUCCESS:  // = 1, /* generic success code */
            message = "Continuous optimization reached optimal solution.";
            break;
        case NLOPT_STOPVAL_REACHED: // = 2,
            message = "Continuous optimization reach the specified stopping value: optimization complete";
            break;
        case NLOPT_FTOL_REACHED: // = 3,
            message = "Continuous optimization converged below specified objective function tolerance: optimization complete";
            break;
        case NLOPT_XTOL_REACHED: // = 4,
            message = "Continuous optimization converged below specified variable change tolerance: optimization complete";
            break;
        case NLOPT_MAXEVAL_REACHED: // = 5,
            message = "Continuous optimization terminated at the maximum number of function evaluations. Result may be significantly suboptimal.";
            break;
        case NLOPT_MAXTIME_REACHED: // = 6
            message = "Continuous optimization solution timed out. Result may be significantly suboptimal.";
            break;
        }
        
        message.append("\n>> Final PPA: " + std::to_string(minf) + "\n>> Variable values:\n");

        int i = 0;
        for (std::vector<optimization_variable>::iterator vit = m_settings.variables.begin(); vit != m_settings.variables.end(); vit++)
        {
            if (vit->is_optimized && !vit->is_integer)
            {
                message.append(">> " + vit->nice_name + " [" + vit->units + "]\t" + std::to_string(x[i]) + "\n");
                vit->assign(x[i++]);
            }
        }

        
        message_handler((message + "\n").c_str());

    }

CLEAN_AND_RETURN:
    delete[] x, lb, ub;

    return minf;
}

bool optimization::run_optimization()
{
    /*
    Performs in implementation of our cutting plane approach for mixed-integer
    optimization of convex, derivative-free functions over a bound-constrained
    domain.

    Arguments:
    -----------
    func:        The function being optimized
    LB:          [n x 1 numpy array] Lower bounds on parameters
    UB:          [n x 1 numpy array] Upper bounds on parameters
    X:           [m x n numpy array] Initial points to sample
    data_out:    [Bool] if storing per-iteration data
    m_settings.trust:       [Bool] if a m_settings.trust region should be used
    m_settings.convex_flag: [Bool] Is the problem is not known to be convex?
    max_delta:   [Int] For nonconvex problem, how large of an infinity-norm neighborhood around the best point should be evaluated before terminating 

    Returns: 
    --------
    x_opt: A point satisfying (obj_ub - model_lower_bound(x_opt)) <= optimality_gap
    */   

    std::chrono::time_point<std::chrono::system_clock> startcputime = std::chrono::system_clock::now();

    
    if( m_settings.convex_flag )
        if( ! m_settings.trust )
            std::runtime_error("Must have trust=True when convex_flag=True");

    //count number of integer variables
    int n = 0;
    for (size_t i=0; i<m_settings.variables.size(); i++)
        if (m_settings.variables.at(i).is_integer)
            n++;  //count the number of integer variables

    if (n < 1)
    {
        //only run continuous subproblem

        //update the variable guess value to be the first in the initializer list
        for (size_t i = 0; i < m_settings.variables.size(); i++)
            m_settings.variables.at(i).assign(m_settings.variables.at(i).initializers.front());

        return run_continuous_subproblem();    // Include this value in F in the next iteration (after all combinations with new_ind are formed)
    }

    //require dimensions of matrices to align
    int nx = (int)m_settings.n_initials;

    //sort the variables so all integer variables appear first in the list
    std::sort(m_settings.variables.begin(), m_settings.variables.end(), 
        [](const optimization_variable &a, const optimization_variable &b) -> bool 
    { 
        return (int)a.is_integer > (int)b.is_integer; 
    });

    //transfer input data into eigen containers
    Vector<int> LB(n), UB(n);
    Matrix<int> X( nx, n);
    {
        int i = 0;
        for (std::vector<optimization_variable>::iterator vi = m_settings.variables.begin(); vi != m_settings.variables.end(); vi++)
        {
               
            if (!(*vi).is_integer)
                break;
            
            optimization_variable &v = (*vi);

            LB(i) = v.minval.as_integer();
            UB(i) = vi->maxval.as_integer();
            if (vi->initializers.size() != n)
            {
                std::runtime_error( (std::stringstream()
                    << "Malformed data in optimization routine. Dimensionality of the initializer array for variable '" << vi->name << "' is incorrect. "
                    << "Expecting " << n << " values but received " << vi->initializers.size() << " instead.").str()
                );
            }
            for (int j = 0; j < vi->initializers.size(); j++)
                X(j, i) = (int)vi->initializers.at(j);
        }
    }

    if ( m_settings.convex_flag && !m_settings.trust )
        std::runtime_error("Must have trust=True when convex_flag=True");

    //check for boundedness
    Matrix<int> Xt = X.transpose();
    for(int i=0; i<n; i++)
        if( Xt.at(i).maxCoeff() >= UB(i) || Xt.at(i).minCoeff() <= LB(i) )
            std::runtime_error("Optimization input data outside of specified upper or lower bound range.");

    
    Matrix<int> grid;
    int m=1;
    for(int i=0; i<n; i++)
        m *= (UB(i)+1-LB(i));
    grid.resize(m, n+1);

    Matrix<int> ranges;
    for(int i=0; i<n; i++)
        ranges.push_back( range(LB(i), UB(i)+1) );

    Vector< int > limits(n);
    for(int i=0; i<n; i++)
        limits.at(i) = (int)ranges.at(i).size();

    Vector< int > indices(n);
    Vector< std::string > indices_lookup;  //save string versions of the indices for quick location later

    for(int mi=0; mi<m; mi++)
    {
        // It is nice to have a this column of ones instead of adding it throughout
        grid(mi,0) = 1;
    
        std::stringstream myind;
        for(int ni=0; ni<n; ni++)
        {
            //Points in the grid
            int v = ranges.at(ni)( indices.at(ni) );
            grid(mi,ni+1) = v;
            myind << v << ",";
        }
        increment( limits, indices );
        indices_lookup.push_back( myind.str() );
    }


    // Function values
    Vector<double> F = Ones<double>(m)*std::numeric_limits<double>::quiet_NaN();
    
    // Holds the facets
    Matrix<double> c_mat;
    c_mat = Zeros<double>(n + 1, n + 1);
    
    // Evaluate func at points in X 
    for(int i=0; i<nx; i++)
    {
        std::stringstream xstr;
        //Vector< int > x;
        for(int j=0; j<n; j++)
        {
            xstr << X(i,j) << ",";
            //x.push_back( X(i,j) );
            m_settings.variables.at(j).assign( X(i, j) );
        }

        int row_in_grid = (int)( std::find(indices_lookup.begin(), indices_lookup.end(), xstr.str() ) - indices_lookup.begin() );
        if( row_in_grid > (int)indices_lookup.size() )
            std::runtime_error("One of the initial points was not in the grid.");

        F(row_in_grid) = run_continuous_subproblem();
    }

    Vector<int> x_star(n);

    double delta = std::numeric_limits<double>::quiet_NaN();
    if(m_settings.trust)
    {
        int i_fmin = argmin(F, true);

        for(int i=0; i<n; i++)
            x_star(i) = grid(i_fmin,i+1);
        delta = 1;
    }

    double obj_ub = nanmin(F); // Upper bound on optimal objective function value

    // Lowerbound is the function value at already-evaluated points
    Vector<int> not_nans;
    nanfilter(F, &not_nans);

    Vector<double> eta = Ones<double>(m) * (-std::numeric_limits<double>::infinity());
    assign_where(eta, F, &assign_filter_nan);

    // To store the set of n+1 points that generate the value eta at each grid point.. The evaluated points are their own generators
    Matrix<int> eta_gen((int)F.size(), n+1);
    for(int i=0; i<not_nans.size(); i++)
        for(int j=0; j<n+1; j++)
            eta_gen( not_nans.at(i), j) = not_nans.at(i);
    
    // Mark if we can exclude a point from future combinations
    double optimality_gap = 1e-8;

    bool first_iter = true;
    int new_ind = -1;   //index of best objective function value
    double Fnew = std::numeric_limits<double>::quiet_NaN();
    
    Vector<int> points_within_delta_of_xstar;

    while(true)
    {
        // Generate all yet-to-be considered combinations of n+1 points
        Matrix<int> newcombs;

        if (first_iter)
        {
            // newcombs = map(list,itertools.combinations(np.where(~np.isnan(F))[0],n+1)) 
            combinations(not_nans, n+1, newcombs);
            first_iter = false;
        }
        else
        {
            // Only generate newcombs with points that make some hyperplane with
            // value that is better than obj_ub at some point in the grid
            // newcombs = map(list,(tup + (new_ind,) for tup in itertools.combinations(np.unique(eta_gen[np.where(np.logical_and(eta < obj_ub,np.isnan(F)))[0] ]).astype('int'),n)))
            /* 
            1. Get unique indices from within all eta_gen[i] where:
                a. eta[i] is less than obj_up
                b. F[i] is nan
            2. Generate all index combinations from resulting vector
            3. Add the new index to each combination
            */
            std::set<int> match_set;
            for( int i=0; i<m; i++)
                if( eta(i) < obj_ub && F(i) != F(i) )
                    for( int j=0; j<eta_gen.cols(); j++ )
                        match_set.insert( eta_gen(i,j) );
            
            Vector<int> all_index_matches( (int)match_set.size() );

            {
                int i=0;
                for( std::set<int>::iterator match = match_set.begin(); match != match_set.end(); match ++)
                    all_index_matches(i++) = *match;
            }
            
            combinations(all_index_matches, n, newcombs, n+1);

            for(int i=0; i<newcombs.rows(); i++)
                newcombs(i,n) = new_ind;

            // Now that we've used F to generate subsets, we can update the value
            F(new_ind) = Fnew;
        }

        // Search over all of these combinations for new cutting planes
        int feas_secants = 0;

        // for count,comb in enumerate(newcombs):
        int count; //record value later
        for( count=0; count<newcombs.rows(); count++)
        {
            Matrix<double> grid_comb(n+1, n+1);
            Vector<int> comb;
                        
            for (int j = 0; j < n + 1; j++) //for each row index in newcombs(count)...
            {
                for(int k=0; k<n+1; k++) //for each value in the row corresponding to newcombs(count,j)
                    grid_comb(j,k) = (double)grid( newcombs(count,j), k );
                comb.push_back(newcombs(count,j));
            }
            
            Eigen::MatrixXd grid_comb_e = grid_comb.transpose().AsEigenMatrixType();
            Eigen::HouseholderQR<Eigen::MatrixXd> qr = grid_comb_e.householderQr();
            
            // Check if combination is poised
            if( qr.matrixQR().diagonal().cwiseAbs().minCoeff() > 1.e-8 )
            {
                feas_secants += 1;

                /* 
                 Find n+1 facets (of the form c[1:n]^T x + c[0]<= 0) of the convex hull of this comb  
                 We can update the QR factorization of comb to get the
                 facets quickly by leaving one point out of the QR
                 for j in range(n+1): 
                */
                for(int j=0; j<n+1; j++)
                {
                    //first, delete the appropriate row (n-j) of the untransposed matrix
                    Matrix<double> grid_temp;
                    for (Matrix<double>::iterator v = grid_comb.begin(); v != grid_comb.end(); v++)
                        if (v != (grid_comb.begin() + (n - j)))
                            grid_temp.push_back(*v);

                    //redo the factorization
                    Matrix<double> Q1( grid_temp.transpose().AsEigenMatrixType().householderQr().householderQ() );
                    
                    // being left out
                    if (Q1.col(n).dot(grid_comb.at(n - j)) > 0.)
                        c_mat(j) = Q1.col(n)*-1.; //last column in Q1
                    else
                        c_mat(j) = Q1.col(n);

                }

                Vector<int> points_better_than_obj_ub = filter_where(eta, obj_ub, &filter_where_lt);

                /* 
                Any grid point outside of exactly n of the n+1 facets is in a cone and should be updated.
                Collect the points that are better
                */
                Matrix<double> points_better_than_obj_ub_gridvals( (int)points_better_than_obj_ub.size(), n+1);
                for(int i=0; i<(int)points_better_than_obj_ub.size(); i++)
                    for(int j=0; j<n+1; j++)
                        points_better_than_obj_ub_gridvals(i,j) = grid(points_better_than_obj_ub.at(i),j);
                Matrix<double> points_better_than_obj_ub_dp = c_mat.dot( points_better_than_obj_ub_gridvals.transpose() );
                
                Vector<int> points_to_possibly_update;
                // Matrix<double> points_to_possibly_update_gridvals;
                
                for(int i=0; i<points_better_than_obj_ub_dp.cols(); i++)
                {
                    int ok_ct=0;
                    for(int j=0; j<n+1; j++)
                        if( points_better_than_obj_ub_dp(j,i) >= -1.e-9 )
                            ok_ct++;
                    if(ok_ct == n)
                    {
                        points_to_possibly_update.push_back(points_better_than_obj_ub.at(i));
                        // points_to_possibly_update_gridvals.push_back(grid.at(points_better_than_obj_ub.at(i)));;
                    }
                }
                // Find the hyperplane through grid[comb] (via np.linalg.solve) and the value of hyperplane at grid[points_to_possibly_update] (via np.dot) 
                // vals = np.dot(grid[points_to_possibly_update], np.linalg.solve(grid[comb], F[comb]))
                Vector<double> F_comb(n+1);
                for (int i = 0; i < n + 1; i++)
                    F_comb(i) = F(comb(i));

                Vector<double> hyperplane;
                hyperplane.Set( grid_comb.AsEigenMatrixType().householderQr().solve(F_comb.AsEigenVectorType()) );
                Vector<int> vals = grid.Subset(points_to_possibly_update).dot(hyperplane);

                // Update lower bound eta at these points 
                for(int i=0; i<(int)vals.size(); i++)
                {
                    int point_to_update = points_to_possibly_update(i);
                        
                    if(eta(point_to_update) < vals(i))
                    {
                        eta(point_to_update) = vals(i);

                        // Update the set generating this lower bound
                        for(int j=0; j<(int)comb.size(); j++)
                            eta_gen(point_to_update,j) = comb(j);
                    }
                }
            }
        }

        bool eval_performed_flag = false;

        // Evaluate objective at the point with smallest eta value
        if( m_settings.trust )
        {

            while(true)
            {
                points_within_delta_of_xstar.clear();

                bool any_nan_F=false;   //keep track of whether there are any NAN's in F

                for(int i=0; i<grid.rows(); i++)
                {
                    if( (m_settings.convex_flag && eta(i) < obj_ub) || (!m_settings.convex_flag && F(i)!=F(i)) )
                    {
                        //calculate the spatial distance between x_star and other grid points
                        Vector<int> x(n);
                        for(int j=0; j<n; j++)
                            x(j) = grid(i,j+1);
                        double xd_norm = (x_star - x).AsEigenVectorType().lpNorm<Eigen::Infinity>();
                        if( xd_norm <= delta )
                            points_within_delta_of_xstar.push_back( i );

                        any_nan_F = any_nan_F || F(i)!=F(i);
                    }
                }

                if( !points_within_delta_of_xstar.empty() )
                {
                    double eta_min_iter = 9.e36;
                    for(int i=0; i<(int)points_within_delta_of_xstar.size(); i++)
                    {
                        int point_i = points_within_delta_of_xstar.at(i);
                        if( eta(point_i) < eta_min_iter )
                        {
                            eta_min_iter = eta(point_i);
                            new_ind = point_i;
                        }
                    }
                    
                    eval_performed_flag = true;

                    Vector<int> x_star_maybe;
                    for (int i = 0; i < n; i++)
                    {
                        int vv = grid(new_ind, i + 1);
                        m_settings.variables.at(i).assign( vv );
                        x_star_maybe.push_back(vv);
                    }

                    Fnew = run_continuous_subproblem();

                    // Update x_star
                    if (Fnew < obj_ub)
                    {
                        x_star = x_star_maybe;
                        delta = delta+1;
                    }
                    else
                        delta = delta/2. < 1. ? 1. : delta/2.;

                    break;
                }
                else
                {

                    if( m_settings.convex_flag && ( obj_ub - eta.minCoeff() < optimality_gap || delta > (UB-LB).maxCoeff() ))
                        break;
                    if( !m_settings.convex_flag && !any_nan_F )
                        break;
                    if( !m_settings.convex_flag && delta >= m_settings.max_delta )
                        break;
                    delta++;
                }
            }
        }
        else
            new_ind = (int)( std::min_element(eta.begin(), eta.end()) - eta.begin() );

        for (int i = 0; i < n; i++)
            m_settings.variables.at(i).assign( grid(new_ind, i + 1) );

        Fnew = run_continuous_subproblem();    // Include this value in F in the next iteration (after all combinations with new_ind are formed)
        obj_ub = Fnew < obj_ub ? Fnew : obj_ub;   // Update upper bound on the value of the global optimizer
        eta(new_ind) = Fnew;
        eta_gen(new_ind) = new_ind;

        // Store information about the iteration (do not store if m_settings.trust and no evaluation)
        if ( ( m_settings.trust && eval_performed_flag ) || !m_settings.trust )
        {
            //m_results.eta_i.push_back( eta );

            m_results.obj_ub_i.push_back(obj_ub);
            
            m_results.wall_time_i.push_back( (double)( (std::chrono::system_clock::now() - startcputime).count() ) );
            
            m_results.secants_i.push_back(count+1);
            
            m_results.feas_secants_i.push_back(feas_secants);
            
            m_results.eval_order.push_back(new_ind);
        }

        // print(obj_ub - np.min(eta), count+1, sum(eta<obj_ub),sum(~np.isnan(F)), grid[new_ind, 1:]);sys.stdout.flush()
        int sum_eta_lt_obj_ub=0;
        for(int i=0; i<(int)eta.size(); i++)
            if( eta(i) < obj_ub )
                sum_eta_lt_obj_ub ++;
        int sum_F_defined=0;
        for(int i=0; i<(int)F.size(); i++)
            if(! std::isnan(F(i)) )
                sum_F_defined++;

        //std::cout << obj_ub - eta.minCoeff() << "\t" 
        //          << count+1 << "\t"
        //          << sum_eta_lt_obj_ub << "\t"
        //          << sum_F_defined << "\t";
        //for(int i=0; i<n; i++)
        //    std::cout << grid(new_ind,i+1) << ( i<n-1 ? ", " : "\n" );

        if
        ( 
            ( m_settings.convex_flag && (obj_ub - eta.minCoeff() <= optimality_gap ) ) ||
            ( !m_settings.convex_flag && !(sum_F_defined > 0) ) ||
            ( ( !m_settings.convex_flag && (int)points_within_delta_of_xstar.size() > 0 ) && ( delta >= m_settings.max_delta ) )
        )
        {
            F(new_ind) = Fnew;
            // print(grid[np.nanargmin(F),1:],sum(~np.isnan(F)),m_settings.trust,func)
            Vector<int> x_at_fmin = grid.at( argmin(F, true) );
            
            //m_results.x_star.clear();
            for (int i = 0; i < n; i++)
                m_settings.variables.at(i).assign( x_at_fmin(i + 1) );
            //std::cout << sum_F_defined << "\t" << m_settings.trust << "\t" << m_settings.f_objective << "\n";

            return true;
        }
        
    }
}
