import numpy as np
import scipy as sp
from scipy import linalg # For updating QR factorization
from scipy import spatial # For pairwise distances
import itertools # For generating combinations
import time,sys
#import nlopt

def main(func, LB, UB, X, pflag=False, data_out=False, trust=False, convex_flag=False, max_delta=np.inf):
    """
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
    """
    if not convex_flag:
        assert trust, "Must have trust=True when convex_flag=True"

    assert np.all(np.max(X,axis=0) <= UB) and np.all(np.min(X,axis=0) >= LB), "Points in X are outside of the bounds"

    n = len(LB)
    grid = np.hstack(np.meshgrid(*[np.arange(i,j+1) for i,j in zip(LB,UB)])).swapaxes(0,1).reshape(n,-1).T # Points in the grid
    m = grid.shape[0]
    grid = np.hstack((np.ones((m,1)),grid)) # It is nice to have a this column of ones instead of adding it throughout
    F = np.nan*np.ones(m)  # Function values
    c_mat = np.zeros((n+1,n+1))  # Holds the facets

    if data_out:
        eta_i = []
        obj_ub_i = []
        wall_time_i = []
        secants_i = []
        feas_secants_i = []
        eval_order = []

    # Evaluate func at points in X 
    for x in X:
        row_in_grid = np.argwhere(np.all((grid[:,1:]-x)==0, axis=1))
        assert len(row_in_grid), 'One of the initial points was not in the grid.'
        F[row_in_grid] = func(x)

    if trust:
        x_star = grid[np.nanargmin(F),1:]
        delta = 1

    obj_ub = np.nanmin(F) # Upper bound on optimal objective function value
    eta = -np.inf*np.ones(m) # Lower bound on the objective function value at each point in the grid
    eta[~np.isnan(F)] = F[~np.isnan(F)] # Lowerbound is the function value at already-evaluated points

    eta_gen = np.nan*np.ones((m,n+1)) # To store the set of n+1 points that generate the value eta at each grid point
    eta_gen[~np.isnan(F)] = np.tile(np.where(~np.isnan(F))[0],(n+1,1)).T # The evaluated points are their own generators

    # ruled_out = np.zeros(m,dtype='bool') # Mark if we can exclude a point from future combinations
    optimality_gap = 1e-8 

    # PDist = sp.spatial.distance.squareform(sp.spatial.distance.pdist(grid[:,1:],'euclidean'))

    first_iter = True
    while True:
        # Generate all yet-to-be considered combinations of n+1 points
        if first_iter:
            newcombs = map(list,itertools.combinations(np.where(~np.isnan(F))[0],n+1)) 
            first_iter = False
        else:
            # Only generate newcombs with points that make some hyperplane with
            # value that is better than obj_ub at some point in the grid
            newcombs = map(list,(tup + (new_ind,) for tup in itertools.combinations(np.unique(eta_gen[np.where(np.logical_and(eta < obj_ub,np.isnan(F)))[0] ]).astype('int'),n)))

            # Now that we've used F to generate subsets, we can update the value
            F[new_ind] = Fnew

        # Search over all of these combinations for new cutting planes
        feas_secants = 0

        for count,comb in enumerate(newcombs):
            Q,R = np.linalg.qr(grid[comb].T,mode='complete')

            # Check if combination is poised
            if np.min(np.abs(np.diag(R))) > 1e-8:
                feas_secants += 1

                #### Find n+1 facets (of the form c[1:n]^T x + c[0]<= 0) of the convex hull of this comb  
                # We can update the QR factorization of comb to get the
                # facets quickly by leaving one point out of the QR
                for j in range(n+1): 
                    Q1,_ = sp.linalg.qr_delete(Q,R,n-j,1,'col',check_finite=False)
                    # Check if the sign is right by comparing against the point # being left out
                    if np.dot(Q1[:,-1],grid[comb[n-j]]) > 0:
                        c_mat[j] = -1*Q1[:,-1]
                    else:
                        c_mat[j] = Q1[:,-1]

                points_better_than_obj_ub = np.where(eta < obj_ub)[0]

                # Any grid point outside of exactly n of the n+1 facets is in a cone and should be updated.
                points_to_possibly_update = points_better_than_obj_ub[sum(np.dot(c_mat,grid[points_better_than_obj_ub].T) >= -1e-9) == n ]

                # Find the hyperplane through grid[comb] (via np.linalg.solve) and the value of hyperplane at grid[points_to_possibly_update] (via np.dot) 
                vals = np.dot(grid[points_to_possibly_update], np.linalg.solve(grid[comb], F[comb]))

                # Update lower bound eta at these points 
                flag = eta[points_to_possibly_update] < vals
                eta[points_to_possibly_update[flag]] = vals[flag]

                # Update the set generating this lower bound
                eta_gen[points_to_possibly_update[flag]] = comb

        # Evaluate objective at the point with smallest eta value
        if trust:
            while True:
                if convex_flag:
                    points_within_delta_of_xstar = np.where(np.logical_and(eta < obj_ub, sp.spatial.distance.cdist([x_star], grid[:,1:], lambda u, v: np.linalg.norm(u-v,np.inf))[0]<=delta))[0]
                else:
                    points_within_delta_of_xstar = np.where(np.logical_and(np.isnan(F), sp.spatial.distance.cdist([x_star], grid[:,1:], lambda u, v: np.linalg.norm(u-v,np.inf))[0]<=delta))[0]
                if (points_within_delta_of_xstar.any()):
                    new_ind = points_within_delta_of_xstar[np.argmin(eta[points_within_delta_of_xstar])]
                    eval_performed_flag = True
                    Fnew = func(grid[new_ind,1:])

                    # Update x_star
                    if (Fnew < obj_ub):
                        x_star = grid[new_ind,1:]
                        delta = delta+1
                    else:
                        delta = max(1,delta/2)
                    break
                else:
                    if convex_flag and np.logical_or(obj_ub - np.min(eta) <= optimality_gap, delta > max(UB-LB)):
                        break
                    if not convex_flag and not any(np.isnan(F)):
                        break
                    if not convex_flag and delta >= max_delta:
                        break
                    delta = delta + 1
        else:
            new_ind = np.argmin(eta)

        Fnew = func(grid[new_ind,1:]) # Include this value in F in the next iteration (after all combinations with new_ind are formed)
        obj_ub = min(Fnew,obj_ub)   # Update upper bound on the value of the global optimizer
        eta[new_ind] = Fnew
        eta_gen[new_ind] = new_ind

        # Store information about the iteration (do not store if trust and no evaluation)
        if (data_out and trust and eval_performed_flag) or (data_out and not trust): 
            if not len(eta_i):
                eta_i = eta.copy()
            else:
                eta_i = np.vstack((eta_i,eta.copy()))

            obj_ub_i = np.hstack((obj_ub_i,obj_ub))
            wall_time_i = np.hstack((wall_time_i,time.time()))
            secants_i = np.hstack((secants_i,count+1))
            feas_secants_i = np.hstack((feas_secants_i, feas_secants))
            eval_order = np.hstack((eval_order, new_ind))

        print(obj_ub - np.min(eta), count+1, sum(eta<obj_ub),sum(~np.isnan(F)), grid[new_ind, 1:]);sys.stdout.flush()
        if (convex_flag and obj_ub - np.min(eta) <= optimality_gap) or (not convex_flag and not any(np.isnan(F))) or (not convex_flag and not(any(points_within_delta_of_xstar)) and delta >= max_delta):
            F[new_ind] = Fnew
            print(grid[np.nanargmin(F),1:],sum(~np.isnan(F)),trust,func)
            if data_out:
                return grid[np.nanargmin(F),1:], eta_i, obj_ub_i, wall_time_i, secants_i, feas_secants_i, eval_order
            else:
                return grid[np.nanargmin(F),1:]

def nlopt_for_continuous(x_int):
    # For a given set of integer variables x, this objective function will call
    # nlopt to optimize the continuous variables y return a value that depends on both.

    def nlopt_obj_fun(x_cont, grad, x_int):

        fval = sum((x_cont-x_int)**2)

        return fval

    # n_cont = len(x_int) # Dimension of continuous variables (can be different from length of x_int)

    # opt = nlopt.opt(nlopt.LN_BOBYQA, n_cont)

    # # Bounds on continuous variables
    # lb = 2*np.ones(n) 
    # ub = 10*np.ones(n)
    # opt.set_lower_bounds(lb)
    # opt.set_upper_bounds(ub)

    # x0 = 5*np.ones(n) # Starting point for continuous variables

    # opt.set_initial_step(1)

    # opt.set_maxeval(100) # Max number of objective evaluations for a given set of integer values
    # opt.set_min_objective(lambda x_cont, grad: nlopt_obj_fun(x_cont, grad, x_int))

    # opt.set_ftol_rel(1e-2)
    # opt.set_xtol_rel(1e-2)

    # # You may want to use x_opt as a starting point for the next evaluation
    # x_opt = opt.optimize(x0)

    # minf = opt.last_optimum_value()

    return sum([x**2 for x in x_int]) #minf

if __name__ == "__main__":
    np.set_printoptions(precision=16,linewidth=300,suppress=True)
    n = 2 # Dimension of integer variables

    LB = -6*np.ones(n) # Lower bound on integer variables
    UB =  2*np.ones(n) # Upper bound on integer variables

    x0 = np.zeros((1,n)) 
    X = np.vstack((x0+ np.eye(n),x0-np.eye(n),x0)) # Initial integer values to evaluate

    trust = True
    # convex_flag = True
    convex_flag = False; max_delta = 1

    x_opt, eta_i, obj_ub_i, wall_time_i, secants_i, feas_secants_i, eval_order = main(nlopt_for_continuous, LB, UB, X, data_out=True, trust=trust, convex_flag=convex_flag, max_delta=max_delta)

    np.savez('func=%s'%func.__name__ + '_n=%d'%n + '_bound=4' + '_trust=%d'%trust,
            x_opt = x_opt, 
            eta_i         =eta_i,
            obj_ub_i      =obj_ub_i,
            wall_time_i   =wall_time_i,
            secants_i     =secants_i,
            feas_secants_i=feas_secants_i,
            eval_order    =eval_order,
            )

