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


std::vector<double> Optimize::main(double (*func)(std::vector<int>&), std::vector<int> _LB, std::vector<int> _UB,
                std::vector< std::vector< int > > _X, bool data_out, bool trust, bool convex_flag, int max_delta )
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
    trust:       [Bool] if a trust region should be used
    convex_flag: [Bool] Is the problem is not known to be convex?
    max_delta:   [Int] For nonconvex problem, how large of an infinity-norm neighborhood around the best point should be evaluated before terminating 

    Returns: 
    --------
    x_opt: A point satisfying (obj_ub - model_lower_bound(x_opt)) <= optimality_gap
    */   
    
    if( convex_flag )
        if( ! trust )
            std::runtime_error("Must have trust=True when convex_flag=True");

    //require dimensions of matrices to align
    int n, nx;
    // n = len(LB)
    n = (int)_LB.size();
    if( _X.front().size() == 0 )
        std::runtime_error("Malformed data in optimization routine. Dimensionality of X is invalid.");
    nx = (int)_X.size();
    if( _UB.size() != n || _LB.size() != n )
        std::runtime_error("Dimensionality mismatch in optimization routine input data.");

    //transfer input data into eigen containers
    Vector<int> LB(n), UB(n);
    Matrix<int> X( nx, n);
    
    for(int i=0; i<n; i++)
        LB(i) = _LB.at(i);
    for(int i=0; i<n; i++)
        UB(i) = _UB.at(i);
    for(int i=0; i<nx; i++)
        for(int j=0; j<n; j++)
            X(i,j) = _X.at(i).at(j);

    if ( convex_flag && !trust )
        std::runtime_error("Must have trust=True when convex_flag=True");

    //check for boundedness
    // assert np.all(np.max(X,axis=0) <= UB) and np.all(np.min(X,axis=0) >= LB), "Points in X are outside of the bounds"
    // Eigen::MatrixXi Xt = X.transpose();
    Matrix<int> Xt = X.transpose();
    for(int i=0; i<n; i++)
        if( Xt.at(i).maxCoeff() >= UB(i) || Xt.at(i).minCoeff() <= LB(i) )
            std::runtime_error("Optimization input data outside of specified upper or lower bound range.");

    
    Matrix<int> grid;
    // m = grid.shape[0]
    int m=1;
    for(int i=0; i<n; i++)
        m *= (UB(i)+1-LB(i));
    grid.Resize(m, n+1);

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
        // grid = np.hstack((np.ones((m,1)),grid)) # It is nice to have a this column of ones instead of adding it throughout
        grid(mi,0) = 1;
    
        std::stringstream myind;
        for(int ni=0; ni<n; ni++)
        {
            // grid = np.hstack(np.meshgrid(*[np.arange(i,j+1) for i,j in zip(LB,UB)])).swapaxes(0,1).reshape(n,-1).T # Points in the grid
            int v = ranges.at(ni)( indices.at(ni) );
            grid(mi,ni+1) = v;
            myind << v << ",";
        }
        increment( limits, indices );
        indices_lookup.push_back( myind.str() );
    }


    // F = np.nan*np.ones(m)  # Function values
    Vector<double> F = Vector<double>::Ones(m)*std::numeric_limits<double>::quiet_NaN();
    
    // c_mat = np.zeros((n+1,n+1))  # Holds the facets
    Matrix<double> c_mat;
    c_mat.Zeros(n + 1, n + 1);
    
    // if data_out:
    Matrix<double> eta_i, obj_ub_i, wall_time_i, secants_i, feas_secants_i, eval_order;

    // # Evaluate func at points in X 
    for(int i=0; i<nx; i++)
    {
        std::stringstream xstr;
        Vector< int > x;
        for(int j=0; j<n; j++)
        {
            xstr << X(i,j) << ",";
            x.push_back( X(i,j) );
        }

        // row_in_grid = np.argwhere(np.all((grid[:,1:]-x)==0, axis=1))
        int row_in_grid = (int)( std::find(indices_lookup.begin(), indices_lookup.end(), xstr.str() ) - indices_lookup.begin() );
        // assert len(row_in_grid), 
        if( row_in_grid > indices_lookup.size() )
            std::runtime_error("One of the initial points was not in the grid.");
        

        F(row_in_grid) = func(x);
    }

    Vector<int> x_star(n);
    double delta;
    if(trust)
    {
        int i_fmin = argmin(F, true);

        for(int i=0; i<n; i++)
            x_star(i) = grid(i_fmin,i+1);
        delta = 1;
    }

    double obj_ub = nanmin(F); // Upper bound on optimal objective function value

    // eta[~np.isnan(F)] = F[~np.isnan(F)] # Lowerbound is the function value at already-evaluated points
    Vector<int> not_nans;
    nanfilter(F, &not_nans);

    Vector<double> eta = Vector<double>::Ones(m) * std::numeric_limits<double>::quiet_NaN();
    assign_where(eta, F, &assign_filter_nan);

    // eta_gen = np.nan*np.ones((m,n+1)) # To store the set of n+1 points that generate the value eta at each grid point
    // eta_gen[~np.isnan(F)] = np.tile(np.where(~np.isnan(F))[0],(n+1,1)).T # The evaluated points are their own generators
    Eigen::MatrixXi eta_gen(F.size(), not_nans.size() );
    for(int i=0; i<F.size(); i++)
        for(int j=0; j<not_nans.size(); j++)
            eta_gen(i,j) = not_nans(j);
    
    eta_gen.transposeInPlace();

    //# ruled_out = np.zeros(m,dtype='bool') # Mark if we can exclude a point from future combinations
    double optimality_gap = 1e-8;
    //# PDist = sp.spatial.distance.squareform(sp.spatial.distance.pdist(grid[:,1:],'euclidean'))


    bool first_iter = true;
    int new_ind = -1;   //index of best objective function value
    double Fnew = std::numeric_limits<double>::quiet_NaN();
    
    while(true)
    {
        // # Generate all yet-to-be considered combinations of n+1 points
        Matrix<int> newcombs;

        if (first_iter)
        {
            // newcombs = map(list,itertools.combinations(np.where(~np.isnan(F))[0],n+1)) 
            combinations(not_nans, n+1, newcombs);
            first_iter = false;
        }
        else
        {
            // # Only generate newcombs with points that make some hyperplane with
            // # value that is better than obj_ub at some point in the grid
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

            // # Now that we've used F to generate subsets, we can update the value
            F(new_ind) = Fnew;
        }

        // Search over all of these combinations for new cutting planes
        int feas_secants = 0;

        // for count,comb in enumerate(newcombs):
        for(int count=0; count<newcombs.rows(); count++)
        {
            //int comb_size = newcombs.cols();
            Matrix<double> grid_comb(n+1, n+1);
            Vector<int> comb;
                        
            for(int j=0; j<n+1; j++) //for each row index in newcombs(count)...
                for(int k=0; k<n+1; k++) //for each value in the row corresponding to newcombs(count,j)
                {
                    grid_comb(j,k) = (double)grid( newcombs(count,j), k );
                    comb.push_back(newcombs(count,j));
                }
            grid_comb.transposeInPlace();

            // Q,R = np.linalg.qr(grid[comb].T,mode='complete')
            Eigen::MatrixXd grid_comb_e = grid_comb.AsEigenMatrixType();
            Eigen::HouseholderQR<Eigen::MatrixXd> qr = grid_comb_e.householderQr();
            
            Eigen::MatrixXd R_e = qr.matrixQR();
            Eigen::MatrixXd Q_e = qr.householderQ();
            Matrix<double> R = R_e;
            Matrix<double> Q = Q_e;

            // Check if combination is poised
            // if np.min(np.abs(np.diag(R))) > 1e-8:
            if( R_e.diagonal().cwiseAbs().minCoeff() > 1.e-8 )
            {
                feas_secants += 1;

                //////// Find n+1 facets (of the form c[1:n]^T x + c[0]<= 0) of the convex hull of this comb  
                // We can update the QR factorization of comb to get the
                // facets quickly by leaving one point out of the QR
                // for j in range(n+1): 
                for(int j=0; j<n+1; j++)
                {
                    // Q1,_ = sp.linalg.qr_delete(Q,R,n-j,1,'col',check_finite=False)
                    Matrix<double> grid_temp = grid_comb;
                    grid_temp.erase(grid_temp.begin()+j,grid_temp.begin()+j+1);

                    // Eigen::MatrixXd grid_temp(n+1,n);
                    // int kk=0;
                    // for(int k=0; k<n+1; k++) //over all columns
                    // {
                    //     if( k != n-j )
                    //     {
                    //         for(int jj=0; jj<n+1; jj++) //over all rows
                    //             grid_temp(jj,kk) = grid_comb(jj,k);
                    //         //only increment the column number for grid_temp if we aren't deleting
                    //         kk++;
                    //     }
                    // }
                    //redo the factorization
                    // grid_comb.resize(n+1,n);
                    // grid_comb = grid_temp;
                    Eigen::MatrixXd grid_temp_e = grid_temp.AsEigenMatrixType();
                    Eigen::HouseholderQR<Eigen::MatrixXd> qr1 = grid_temp_e.householderQr();
                    // Eigen::MatrixXd Q1 = qr1.householderQ();
                    Matrix<double> Q1( qr1.householderQ() );
                    
                    // Check if the sign is right by comparing against the point # being left out
                    // if np.dot(Q1[:,-1],grid[comb[n-j]]) > 0:
                    for(int k=0; k<n+1; k++)
                        c_mat(j,k) = Q1(n-1,k);

                    if( Q1.at(n-1).dot(grid_comb.at(n-j)) > 0.)
                        c_mat.at(j)*=-1.;
                    grid_comb = grid_temp;
                }

                // points_better_than_obj_ub = np.where(eta < obj_ub)[0]
                Vector<int> points_better_than_obj_ub = filter_where(eta, obj_ub, &filter_where_lt);

                // Any grid point outside of exactly n of the n+1 facets is in a cone and should be updated.
                // points_to_possibly_update = points_better_than_obj_ub[sum(np.dot(c_mat,grid[points_better_than_obj_ub].T) >= -1e-9) == n ]

                //collect the points that are better
                Matrix<double> points_better_than_obj_ub_gridvals( (int)points_better_than_obj_ub.size(), n+1);
                for(int i=0; i<points_better_than_obj_ub.size(); i++)
                    for(int j=0; j<n+1; j++)
                        points_better_than_obj_ub_gridvals(i,j) = grid(points_better_than_obj_ub.at(i),j);
                // Eigen::MatrixXd points_better_than_obj_ub_dp(n+1,points_better_than_obj_ub.size);
                Eigen::MatrixXd points_better_than_obj_ub_dp = c_mat.dot( points_better_than_obj_ub_gridvals.transpose() ).AsEigenMatrixType();
                
                Vector<int> points_to_possibly_update;
                
                for(int i=0; i<points_better_than_obj_ub_dp.cols(); i++)
                {
                    int ok_ct=0;
                    for(int j=0; j<n+1; j++)
                        if( points_better_than_obj_ub_dp.col(i)(j) >= -1.e-9 )
                            ok_ct++;
                    if(ok_ct == n)
                        points_to_possibly_update.push_back(points_better_than_obj_ub.at(i));
                }
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                // Find the hyperplane through grid[comb] (via np.linalg.solve) and the value of hyperplane at grid[points_to_possibly_update] (via np.dot) 
                // vals = np.dot(grid[points_to_possibly_update], np.linalg.solve(grid[comb], F[comb]))
                Eigen::MatrixXd F_comb(n+1,1);
                for(int i=0; i<n+1; i++)
                    F_comb(i,0) = F(comb.at(i));
                
                Eigen::MatrixXd hyperplane = grid_comb.AsEigenMatrixType().bdcSvd().solve(F_comb.transpose());
                
                // Eigen::MatrixXd 
                Matrix<double> vals( points_better_than_obj_ub_gridvals.AsEigenMatrixType() * hyperplane );

                // Update lower bound eta at these points 
                // flag = eta[points_to_possibly_update] < vals
                // std::vector<bool> points_to_update;
                for(int i=0; i<vals.rows(); i++)
                {
                    bool flagval = eta(points_to_possibly_update(i)) < vals(i,0);
                    // flag.push_back( flagval );

                    if(flagval)
                    {
                        int point_to_update = points_to_possibly_update(i);
                        
                        // eta([points_to_possibly_update[flag]]) = vals[flag]
                        eta(point_to_update) = vals(point_to_update,0);

                        // Update the set generating this lower bound
                        // eta_gen[points_to_possibly_update[flag]] = comb
                        for(int j=0; j<comb.size(); j++)
                            eta_gen(point_to_update,j) = comb(j);
                    }
                }
            }
        }

        bool eval_performed_flag = false;

//#ifdef __junk
        // Evaluate objective at the point with smallest eta value
        if( trust )
        {

            while(true)
            {
                Vector<int> points_within_delta_of_xstar;

                if( convex_flag )
                    points_within_delta_of_xstar = where(np.logical_and(eta < obj_ub, sp.spatial.distance.cdist([x_star], grid[:,1:], lambda u, v: np.linalg.norm(u-v,np.inf))[0]<=delta))[0]
                else
                    points_within_delta_of_xstar = where(np.logical_and(np.isnan(F), sp.spatial.distance.cdist([x_star], grid[:,1:], lambda u, v: np.linalg.norm(u-v,np.inf))[0]<=delta))[0]

                if( !points_within_delta_of_xstar.empty() )
                {
                    // new_ind = points_within_delta_of_xstar[np.argmin(eta[points_within_delta_of_xstar])]
                    int new_ind = 0;
                    double eta_min_iter = 9.e36;
                    for(int i=0; i<points_within_delta_of_xstar.size(); i++)
                    {
                        int point_i = points_within_delta_of_xstar.at(i);
                        if( eta(point_i) < eta_min_iter )
                        {
                            eta_min_iter = eta(point_i);
                            new_ind = point_i;
                        }
                    }
                    
                    eval_performed_flag = true;

                    std::vector<int> x_star_maybe;
                    for(int i=0; i<n; i++)
                        x_star_maybe.push_back(grid(new_ind,i+1));

                    Fnew = func(x_star_maybe);

                    // Update x_star
                    if (Fnew < obj_ub)
                    {
                        x_star = x_star_maybe;
                        delta = delta+1;
                    }
                    else
                        delta = std::max(1,delta/2);

                    break;
                }
                else
                {

                    if convex_flag and np.logical_or(obj_ub - np.min(eta) <= optimality_gap, delta > max(UB-LB))
                        break;
                    if not convex_flag and not any(np.isnan(F))
                        break;
                    if not convex_flag and delta >= max_delta
                        break;
                    delta = delta + 1;
                }
            }
        }
        else
            new_ind = std::min_element(eta.begin(), eta.end()) - eta.begin();

        std::vector<int> x_eval;
        for(int i=0; i<n; i++)
            x_eval.push_back(grid(new_ind,i+1));

        Fnew = func(x_eval);    // Include this value in F in the next iteration (after all combinations with new_ind are formed)
        obj_ub = std::min(Fnew,obj_ub);   // Update upper bound on the value of the global optimizer
        eta(new_ind) = Fnew;
        eta_gen(new_ind) = new_ind;

        // Store information about the iteration (do not store if trust and no evaluation)
        if ( (data_out and trust && eval_performed_flag) || (data_out && !trust) )
        {
            if( eta_i.empty() )
            // if not len(eta_i):
                // eta_i = eta.copy()
                eta_i = eta;
            else

                eta_i = np.vstack((eta_i,eta.copy()))

            obj_ub_i = np.hstack((obj_ub_i,obj_ub))
            wall_time_i = np.hstack((wall_time_i,time.time()))
            secants_i = np.hstack((secants_i,count+1))
            feas_secants_i = np.hstack((feas_secants_i, feas_secants))
            eval_order = np.hstack((eval_order, new_ind))
        }

        print(obj_ub - np.min(eta), count+1, sum(eta<obj_ub),sum(~np.isnan(F)), grid[new_ind, 1:]);sys.stdout.flush()
        if (convex_flag and obj_ub - np.min(eta) <= optimality_gap) or (not convex_flag and not any(np.isnan(F))) or (not convex_flag and not(any(points_within_delta_of_xstar)) and delta >= max_delta):
            F[new_ind] = Fnew
            print(grid[np.nanargmin(F),1:],sum(~np.isnan(F)),trust,func)
            if data_out:
                return grid[np.nanargmin(F),1:], eta_i, obj_ub_i, wall_time_i, secants_i, feas_secants_i, eval_order
            else:
                return grid[np.nanargmin(F),1:]
        
#endif 
    }
}