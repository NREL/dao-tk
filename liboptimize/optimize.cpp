#include "optimize.h"
#include "optutil.h"
#include <nlopt/nlopt.h>
#include <eigen/Core>
#include <eigen/Dense>
#include <exception>
#include <set>
#include <iostream>
#include <stdio.h>

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


//std::unordered_map<std::string, std::vector<double> > Optimize::main(double (*func)(std::vector<int>&), std::vector<int> _LB, std::vector<int> _UB,
//                std::vector< std::vector< int > > _X, bool data_out, bool m_settings.trust, bool m_settings.convex_flag, int max_delta )
bool optimization::run_integer_optimization()
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

    //require dimensions of matrices to align
    int n, nx;
    n = (int)m_settings.lower_bounds.size();
    if( m_settings.X.front().size() == 0 )
        std::runtime_error("Malformed data in optimization routine. Dimensionality of X is invalid.");
    nx = (int)m_settings.X.size();
    if( (int)m_settings.upper_bounds.size() != n || (int)m_settings.lower_bounds.size() != n )
        std::runtime_error("Dimensionality mismatch in optimization routine input data.");

    //transfer input data into eigen containers
    Vector<int> LB(n), UB(n);
    Matrix<int> X( nx, n);
    
    for(int i=0; i<n; i++)
        LB(i) = m_settings.lower_bounds.at(i);
    for(int i=0; i<n; i++)
        UB(i) = m_settings.upper_bounds.at(i);
    for(int i=0; i<nx; i++)
        for(int j=0; j<n; j++)
            X(i,j) = m_settings.X.at(i).at(j);

    if ( m_settings.convex_flag && !m_settings.trust )
        std::runtime_error("Must have trust=True when convex_flag=True");

    //check for boundedness
    Matrix<int> Xt = X.transpose();
    for(int i=0; i<n; i++)
        if( Xt.at(i).maxCoeff() >= UB(i) || Xt.at(i).minCoeff() <= LB(i) )
            std::runtime_error("Optimization input data outside of specified upper or lower bound range.");

    
    Matrix<double> grid;
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
        Vector< int > x;
        for(int j=0; j<n; j++)
        {
            xstr << X(i,j) << ",";
            x.push_back( X(i,j) );
        }

        int row_in_grid = (int)( std::find(indices_lookup.begin(), indices_lookup.end(), xstr.str() ) - indices_lookup.begin() );
        if( row_in_grid > (int)indices_lookup.size() )
            std::runtime_error("One of the initial points was not in the grid.");

        F(row_in_grid) = m_settings.f_objective(x);
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
    //Eigen::MatrixXi eta_gen(F.size(), not_nans.size() );
    Matrix<int> eta_gen(F.size(), not_nans);
    //for(int i=0; i<F.size(); i++)
        //for(int j=0; j<not_nans.size(); j++)
            //eta_gen(i,j) = not_nans(j);
    
    eta_gen.transposeInPlace();

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
                Vector<double> vals = grid.Subset(points_to_possibly_update).dot(hyperplane);
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

                // Update lower bound eta at these points 
                // flag = eta[points_to_possibly_update] < vals
                // std::vector<bool> points_to_update;
                for(int i=0; i<(int)vals.size(); i++)
                {
                    int point_to_update = points_to_possibly_update(i);
                        
                    if(eta(point_to_update) < vals(i))
                    {
                        // eta([points_to_possibly_update[flag]]) = vals[flag]
                        eta(point_to_update) = vals(i);

                        // Update the set generating this lower bound
                        // eta_gen[points_to_possibly_update[flag]] = comb
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

                // convex: points_within_delta_of_xstar = where(np.logical_and(eta < obj_ub, sp.spatial.distance.cdist([x_star], grid[:,1:], lambda u, v: np.linalg.norm(u-v,np.inf))[0]<=delta))[0]
                // else: points_within_delta_of_xstar = where(np.logical_and(np.isnan(F), sp.spatial.distance.cdist([x_star], grid[:,1:], lambda u, v: np.linalg.norm(u-v,np.inf))[0]<=delta))[0]
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
                        if( xd_norm < delta )
                            points_within_delta_of_xstar.push_back( i );

                        any_nan_F = any_nan_F || F(i)!=F(i);
                    }
                }

                if( !points_within_delta_of_xstar.empty() )
                {
                    // new_ind = points_within_delta_of_xstar[np.argmin(eta[points_within_delta_of_xstar])]
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
                    for(int i=0; i<n; i++)
                        x_star_maybe.push_back(grid(new_ind,i+1));

                    Fnew = m_settings.f_objective(x_star_maybe);

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

                    // if m_settings.convex_flag and np.logical_or(obj_ub - np.min(eta) <= optimality_gap, delta > max(UB-LB))
                    if( m_settings.convex_flag && ( obj_ub - eta.minCoeff() < optimality_gap || delta > (UB-LB).maxCoeff() ))
                        break;
                    // if not m_settings.convex_flag and not any(np.isnan(F))
                    if( !m_settings.convex_flag && !any_nan_F )
                        break;
                    // if not m_settings.convex_flag and delta >= max_delta
                    if( !m_settings.convex_flag && delta >= m_settings.max_delta )
                        break;
                    delta++;
                }
            }
        }
        else
            new_ind = (int)( std::min_element(eta.begin(), eta.end()) - eta.begin() );

        Vector<int> x_eval;
        for(int i=0; i<n; i++)
            x_eval.push_back(grid(new_ind,i+1));

        Fnew = m_settings.f_objective(x_eval);    // Include this value in F in the next iteration (after all combinations with new_ind are formed)
        obj_ub = Fnew < obj_ub ? Fnew : obj_ub;   // Update upper bound on the value of the global optimizer
        eta(new_ind) = Fnew;
        eta_gen(new_ind) = new_ind;

        // Store information about the iteration (do not store if m_settings.trust and no evaluation)
        if ( ( m_settings.trust && eval_performed_flag ) || !m_settings.trust )
        {
            // if( eta_i.empty() )
            // if not len(eta_i):
                // eta_i = eta.copy()
            // else
                // eta_i = np.vstack((eta_i,eta.copy()))
            m_results.eta_i.push_back( eta );

            // obj_ub_i = np.hstack((obj_ub_i,obj_ub))
            m_results.obj_ub_i.push_back(obj_ub);
            // wall_time_i = np.hstack((wall_time_i,time.time()))
            m_results.wall_time_i.push_back( (double)( (std::chrono::system_clock::now() - startcputime).count() ) );
            
            // secants_i = np.hstack((secants_i,count+1))
            m_results.secants_i.push_back(count+1);
            // feas_secants_i = np.hstack((feas_secants_i, feas_secants))
            m_results.feas_secants_i.push_back(feas_secants);
            // eval_order = np.hstack((eval_order, new_ind))
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

        std::cout << obj_ub - eta.minCoeff() << "\t" 
                  << count+1 << "\t"
                  << sum_eta_lt_obj_ub << "\t"
                  << sum_F_defined << "\t";
        for(int i=0; i<n; i++)
            std::cout << grid(new_ind,i+1) << ( i<n-1 ? ", " : "\n" );

        // if (m_settings.convex_flag and obj_ub - np.min(eta) <= optimality_gap) or (not m_settings.convex_flag and not any(np.isnan(F))) or (not m_settings.convex_flag and not(any(points_within_delta_of_xstar)) and delta >= max_delta):
        if
        ( 
            ( m_settings.convex_flag && (obj_ub - eta.minCoeff() <= optimality_gap ) ) ||
            ( !m_settings.convex_flag && !(sum_F_defined > 0) ) ||
            ( ( !m_settings.convex_flag && (int)points_within_delta_of_xstar.size() > 0 ) && ( delta >= m_settings.max_delta ) )
        )
        {
            // F[new_ind] = Fnew
            F(new_ind) = Fnew;
            // print(grid[np.nanargmin(F),1:],sum(~np.isnan(F)),m_settings.trust,func)
            Vector<double> x_at_fmin = grid.at( argmin(F, true) );
            std::vector<double> x_best;

            for(int i=0; i<n; i++)
            {
                std::cout << x_at_fmin(i+1) << (i<n-1 ? ", " : "\t");
                x_best.push_back((double)x_at_fmin(i+1));
            }
            std::cout << sum_F_defined << "\t" << m_settings.trust << "\t" << m_settings.f_objective << "\n";

            std::unordered_map< std::string, std::vector<double> > retdict;
            //if( data_out )
                // return grid[np.nanargmin(F),1:], eta_i, obj_ub_i, wall_time_i, secants_i, feas_secants_i, eval_order
            // return grid[np.nanargmin(F),1:]

            return true;
        }
        
    }
}
//endif 