"""
** Objective function model for the O&M / Capital Costs optimization project **

This code provides data structures and algorithms needed to evaluate the 
objective function components for the CSP optimization project. The objective
consists of a number of separate simulations and evaluations. 

class _Data:
    This is the base class for the variable and parameter structures and 
    is not used directly.

class _Parameters:
    This is a collection of values that are static throughout the optimization
    process, and are used to define operations settings, system configuration
    values, and other items of interest that are not treated as variables.

class _Variables:
    Each member in this class describes a degree of freedom in the optimization
    model. Variables can be continuous or integral, and may have upper and 
    lower bounds associated with them. A variable member's value may be changed
    using the following syntax:
        <variable class object>.<variable name>.value = <value to set>
    Variables are "validated" according to whether they adhere to the following
    rules:
        * <value to set> is equal to the specified data type in the initializer
        * <value to set> lies between lower and upper bounds, inclusively

The objective function consists of several subroutines:
===============================================================================================================    
NAME    |  Required Args        | Optional Args         |   Returns
========|=======================|=======================|======================================================
## The entire objective function ##
Z()     | variables             | Return values from:   | [objective function value, >> ($)Sum of all components
        |                       |  D,O,M,S,E,           |  {cost_receiver  >> ($) Receiver cost
        |                       | printout [bool]       |   cost_plant  >> ($) Power cycle cost
        |                       |                       |   cost_tower  >> ($) Tower cost
        |                       |                       |   cost_TES  >> ($) Thermal energy storage cost
        |                       |                       |   cost_land  >> ($) Land cost
        |                       |                       |   heliostat_repair_cost  >> ($) Repair parts cost
        |                       |                       |   heliostat_om_labor  >> ($) Repair labor cost
        |                       |                       |   heliostat_wash_cost >> ($) Mirror washing cost
        |                       |                       |   sales >> ($) Total revenue from electricity sales
        |                       |                       |  }]
--------|-----------------------|-----------------------|------------------------------------------------------        
## Heliostat mechanical/electrical failure availability O&M simulation ##
M()     | variables             | none                  | n_repairs  >> ($) Number of repairs made per year
        | D['design']           |                       | heliostat_repair_cost  >> ($) Repair parts cost
--------|-----------------------|-----------------------|------------------------------------------------------        
## Heliostat optical (soiling and degradation) simulation ##
O()     | variables             | none                  | heliostat_refurbish_cost >> ($) Mirror replacements
        | D['design']           |                       | n_repairs >> (-) number of mirror replacements/year
--------|-----------------------|-----------------------|------------------------------------------------------
## Calculation of explicit (non-simulation) terms ##        
E()     | variables             | none                  | cost_receiver >> ($) Receiver cost                      
        |                       |                       | cost_tower >> ($) Tower cost
        |                       |                       | cost_plant >> ($) Power cycle and BOP cost
        |                       |                       | cost_TES >> ($) Thermal energy storage cost
        |                       |                       | heliostat_wash_cost >> ($) Washing labor cost
        |                       |                       | heliostat_om_labor >> ($) Repair labor cost        
--------|-----------------------|-----------------------|------------------------------------------------------
## Calculation of financial performance ##
F()     | variables             | none                  | lcoe_nom >> (c/kWhe) levelized cost of energy - nominal
        | S                     |                       | lcoe_real >> (c/kWhe) levelized cost of energy - real
        |                       |                       | ppa >> (c/kWhe) power purchase agreement price, first year
        |                       |                       | project_return_aftertax_npv >> ($) net present value
        |                       |                       | project_return_aftertax_irr >> (%) project IRR
        |                       |                       |                                                    
        |                       |                       | 
--------|-----------------------|-----------------------|------------------------------------------------------        
## Solar field layout and characterization (SolarPILOT) ##
D()     | variables             | none                  | design >> {dict containing design information}
        |                       |                       | A_sf  >> (m2) Solar field mirror area
        |                       |                       | A_land  >> (acre) Site land area
        |                       |                       | n_hel  >> (-) Number of heliostats in the layout
        |                       |                       | flux_max_observed >> (kW/m2) Peak receiver flux at design
        |                       |                       | cost_land >> ($) Total land cost
        |                       |                       | cost_heliostats  >> ($) Cost of heliostats & site prep
--------|-----------------------|-----------------------|------------------------------------------------------        
S()     | D['design']           | sf_avail <list|float> | generation >> (MWhe) Hourly generation profile
        | variables             | sf_soil <list|float>  | pricing_mult >> (-) Hourly pricing multiplier profile
        |                       | sf_degr <list|float>  | annual_generation >> (MWhe) Total annual net elec. gen.
        |                       | sample_weeks <int>    | revenue_units >> (MWhe*[-]) Sum of stepwise product of
        |                       |                       |                  pricing_mult and generation
        |                       |                       | sales >> ($) Lifetime electricity sales (real dollars)
        |                       |                       | avg_degradation >> [-] Average lifetime degradation loss
        |                       |                       | avg_soil >> [-] Average lifetime soiling loss
        |                       |                       | avg_avail >> [-] Average steady-state availability loss        
===============================================================================================================

Notes:
    * <variables> is an instance of _Variables()
    * All methods require that <objective>.params items be set
    * Labor costs are indexed to inflation
    * Capital costs include payments on debt interest and financing costs
    * Revenues are escalated by the PPA escalation rate less inflation
    
"""




#from pysdk import run_design, run_simulation
import pysdk
from pysdk import PySDKError
from math import pi, exp

import dni_clustering

#base data structure
class _Data:
    __typename = "Data"
    __namemap = {}
    
    def __repr__(self):
        """
        Provide a better representation of the class and member variable values
        """
        dat = []
        for attr in self.__get_members():
            a = getattr(self,attr)
            if hasattr(a, "value"):
                aa = a.value
            else:
                aa = a
            dat.append(":".join([attr, str(aa)]))
        return "<type " + self.__typename + "| " + (", ".join(dat)) + ">"
    
    def __str__(self):
        return self.__repr__()
        
    def __get_members(self):
        return [attr for attr in dir(self) if not callable(getattr(self,attr)) and not attr.startswith("_")]
    
    def add_map_name(self, member, pseudonym):
        self.__namemap[member] = pseudonym
        
    def as_dict(self):
        d = {}
        for m in self.__get_members():
            a = getattr(self,m)
            #check to see if m has been mapped to another name
            if m in self.__namemap:
                mm = self.__namemap[m]
            else:
                mm = m
                
            if hasattr(a, "value"):
                d[mm] = a.value
            else:
                d[mm] = a
        return d    

#Class with parameters
class _Parameters(_Data):
    def __init__(self):
        self.__typename = "Parameters"
        #
        # -- simulation control --
        self.print_messages = True
        self.check_max_flux = True
        self.is_optimize = False
        self.is_dispatch = False
        self.is_ampl_engine = False
        self.ampl_data_dir = None
        self.solar_resource_file = None
        self.disp_steps_per_hour = 1 
        self.is_stochastic_disp = False

        # -- capital costs --
        self.rec_ref_cost = 1.03e+008  #$ Receiver reference cost
        self.rec_ref_area = 1571.  #m^2 Receiver reference area
        self.tes_spec_cost = 24.  #$/kWh-t
        self.tower_fixed_cost = 3.e6  #$ 
        self.tower_exp = 0.0113  #1/m
        self.heliostat_spec_cost = 145.  #$/m2
        self.site_spec_cost = 16.  #$/m2 - site improvement cost per m2 of heliostat area
        self.land_spec_cost = 10000   #$/acre
        self.c_cps0 = 0.  #cycle power cost scaling - order 2        
        self.c_cps1 = 1440.  #cycle power cost scaling - order 1
        # -- O&M costs --
        self.om_staff_cost = 75 #$/hr #Placeholder
        self.wash_crew_cost = 65. + 10. + 25  #$/hr, placeholder; labor + equip + water
        self.heliostat_refurbish_cost = 144.*25 + 90*4.  #guess -- area*glass cost + labor*hours
        # -- availability model -- 
        self.helio_mtf = 12000 #[hr] mean time to failure (mechanical) for heliostats
        self.heliostat_repair_cost = 300  #$/repair  placeholder
        self.om_staff_max_hours_week = 35  #hr/week Max working hours per week
        self.avail_seed = 123
        self.n_heliostats_sim = 1000
        # -- soiling model --
        self.wash_units_per_hour = 45.  #heliostats/hr/crew
        self.wash_crew_max_hours_week = 70.  #hours/week max for heliostat washing crew
        self.degr_per_hour = 1.e-7      #guess
        self.degr_accel_per_year = 0.125  #12.5% increase in degr_per_hour per year, combined about 15% in 25 years
        self.degr_seed = 123
        self.soil_per_hour = 6.e-4 #guess, 3.e-4=5% per week
        # -- simulation parameters --
        self.adjust_constant = 4 #%
        self.add_map_name("adjust_constant","adjust:constant")
        self.c_ces = [0, 7., 200., 12000.]   #cycle efficiency cost scaling polynomial coefs
        self.helio_reflectance = 0.95  #pristine mirror reflectivity
        # -- financial parameters --
        self.plant_lifetime = 30 #years
        self.finance_period = 25 #years
        # -- dispatch optimization parameters
        self.disp_rsu_cost = 950.
        self.disp_csu_cost = 10000.
        self.disp_pen_delta_w = 0.1
        # -- startup energy and time requirements
        self.rec_su_delay = 0.2 
        self.rec_qf_delay = 0.25  #>> equivalent to MWt-hrs by rec_qf_delay * q_design
        self.startup_time = 0.5 
        self.startup_frac = 0.5 
        
        # -- ppa multipliers --
        self.ppa_multiplier_model = 1
        self.dispatch_factors_ts = [float(v) for v in open('dispatch_factors_ts.csv','r').readlines()]
        
        # -- wind stow speed --
        self.v_wind_max = 15.
        
        #these names should correspond to the input listing under pysdk field_design if they 
        #are part of the design or simulation models
        
    


#helper variable class
class _Variable:
    def __init__(self, minval, maxval, datatype = float): # is_integer=False):
        self.minval = minval  #lower range limit
        self.maxval = maxval  #upper range limit
        self.value = None
        self.datatype = datatype
        
    def __str__(self):
        return str(self.value)
    def __repr__(self):
        return self.__str__()
    
    def __validate(self):
        ok = self.value >= self.minval
        ok = ok and self.value <= self.maxval
        ok = ok and isinstance(self.value, self.datatype)
        return ok
        

#class with variable values
class Variables(_Data):
    def __init__(self):
        self.__typename = "Variables"

        #---- variables 
        # (lower bound,   upper bound,   data type [default=float])
        self.h_tower = _Variable(100., 250.)
        self.rec_height = _Variable(12., 30.)
        self.D_rec = _Variable(12., 30.)
        self.design_eff = _Variable(0.35, 0.45)  #[-] Cycle design point conversion efficiency
        self.dni_des = _Variable(700., 1100.)  #equivalent to x^rec,des
        self.N_panels = _Variable(12, 30, int)
        self.P_ref = _Variable(65., 150.)  #MWe  Cycle design point power
        self.solarm = _Variable(0.9, 5.)
        self.tshours = _Variable(2., 16.)
        self.om_staff = _Variable(1, 40, int)
        self.n_wash_crews = _Variable(0,100,int)  #number of washing crews
        self.degr_replace_limit = _Variable(0,.999)  #mirror degradation replacement threshold
        
    def validate(self):
        
        all_ok = True
        for v in self.as_dict().keys():
            a = getattr(self,v)
            if not a._Variable__validate():
                all_ok = False and all_ok
                print "VALIDATION ERROR: " + v + " invalid"
        return all_ok
#------------------------------
               

class objective:
    #------- internally used methods -------------------
    def __init__(self):
        self.params = None
#        self.design = None  #store the current design
        self.setup()

    def setup(self):
        self.params = _Parameters()
    
    def calc_real_dollars(self, dollar_value, is_revenue=False, is_labor=False):
        """
        Calculate lifetime dollar amount expressed in today's dollar value.
        Account for interest on debt and closing costs if not revenue or labor.
        The effective interest rate subtracts inflation.
        Assume labor is indexed to inflation.
        """
        
        inflation_rate = pysdk.F["inflation_rate"]/100.
        
        if is_revenue:
            ppa_escalation = pysdk.F["ppa_escalation"]/100.

            r = 1. + ppa_escalation - inflation_rate
            
            c = 0.
            cy = dollar_value
            
            for i in range(self.params.plant_lifetime):
                cy *= r
                c += cy
            
            return c
            
        if is_labor:
            return dollar_value*self.params.plant_lifetime
            
        else: #cost
            analysis_period = self.params.finance_period 

            pv = dollar_value * pysdk.F["debt_percent"]/100.  #present value of debt
            dp = pv * pysdk.F["cost_debt_fee"]/100. #debt financing cost
    
            r = (pysdk.F["term_int_rate"]/100. - inflation_rate) #annual interest rate
            n = analysis_period
            
            c = r*pv/(1.-(1.+r)**(-n)) * n
    
            tot = c + (dollar_value-pv) + dp
                
            return tot
    #--------------------------------------------------------------------------
    
    # Clustering inputs that have no dependence on independent variables
    def setup_clusters(self, Nclusters, Ndays = 2, Nprev = 1, Nnext = 1, user_weights = None, user_divisions = None):

        algorithm = 'affinity-propagation'
        hard_partitions = True
        afp_enforce_Ncluster = True
        
        vd = {}
        vd.update(self.params.as_dict())
        solar_resource = vd['solar_resource_file']
        ppamult = vd['dispatch_factors_ts']
        stowlim = vd['v_wind_max']

        # Calculate classification metrics
        ret =  dni_clustering.calc_metrics(weatherfile = solar_resource, Ndays = Ndays, ppa = ppamult, stowlimit = stowlim, user_weights = user_weights, user_divisions = user_divisions)
        data = ret['data']
        data_first = ret['firstday']
        data_last = ret['lastday']
        
        # Create clusters
        cluster = dni_clustering.cluster()
        cluster.algorithm = algorithm
        cluster.Ncluster = Nclusters
        cluster.sim_hard_partitions = hard_partitions
        cluster.afp_enforce_Ncluster = afp_enforce_Ncluster
        clusters = dni_clustering.create_clusters(data, cluster)     
        sim_start_days = (1 + clusters['exemplars']*Ndays).tolist()

        # Adjust weighting for first and last days
        ret = dni_clustering.adjust_weighting_firstlast(data, data_first, data_last, clusters, Ndays)
        clusters = ret[0]
        firstpt_cluster = ret[1]
        lastpt_cluster = ret[2]
        
        # Calculate cluster-average PPA multipliers and solar field adjustment factors
        avg_ppamult = dni_clustering.compute_cluster_avg_from_timeseries(ppamult, clusters['partition_matrix'], Ndays = Ndays, Nprev = Nprev, Nnext = Nnext, adjust_wt = True, k1 = firstpt_cluster, k2 = lastpt_cluster)
        
        cluster_inputs = {}
        cluster_inputs['exemplars']= clusters['exemplars']
        cluster_inputs['weights'] = clusters['weights_adjusted']
        cluster_inputs['day_start'] = sim_start_days
        cluster_inputs['partition_matrix'] = clusters['partition_matrix']
        cluster_inputs['first_pt_cluster'] = firstpt_cluster
        cluster_inputs['last_pt_cluster'] = lastpt_cluster
        cluster_inputs['avg_ppamult'] = avg_ppamult 

        return cluster_inputs
    
    
    #------ subroutines for the objective function ----------------------------
    def D(self, variables):
        """
        The design (SolarPILOT) problem alone
        """
        print "Running field design D()"
        args = variables.as_dict()
        args.update(self.params.as_dict())
        
        design = pysdk.run_design(args)
        
        #Land cost
        cland = self.params.land_spec_cost * design['land_area']
        #solar field cost
        csf = (self.params.heliostat_spec_cost + self.params.site_spec_cost)* design['area_sf']
        
        return {
                "A_sf":design['area_sf'], 
                "A_land":design['land_area'],
                "n_hel":design['number_heliostats'],
                "flux_max_observed":design['flux_max_observed'],
                "cost_land":self.calc_real_dollars(cland),
                "cost_heliostats":self.calc_real_dollars(csf),
                "design":design,
                }



    def M(self, variables, design):
        """
        The heliostat field availability problem
        
        Returns a dict with keys:
            avail_steady            Steady-state availability
            n_repairs               Total number of repairs made per year
            staff_utilization       Fractional staff utilization factor (1. = all staff always working)
            heliostat_repair_cost   Total cost of heliostat repairs ($ lifetime)
            heliostat_repair_cost_y1 Total "" in first year ($/year)
        """
        print "Running availability simulation M()"
        args = {
                    'helio_mtf':3000, 
                    'helio_min_fail_time':1, 
                    'helio_max_fail_time':100,
                    'n_hel':int(design['number_heliostats']), 
                    'om_staff':26, 
                    'om_staff_max_hours_week':35, 
                    'n_avail_sim_hours':8760*12,
                    'avail_seed':123,
                    'n_heliostats_sim':1000,
                }
        vd = variables.as_dict()
        vd.update(self.params.as_dict())
        #overwrite any parameters in args that are provided from V
        for key in args.keys():
            try:
                args[key] = vd[key]
            except KeyError:
                pass
        
        a = pysdk.run_availability(args)
        
        #Calculate staff cost and repair cost
        ann_fact = 8760./float(args['n_avail_sim_hours'])

        a['n_repairs'] *= ann_fact #annual repairs
        a['heliostat_repair_cost_y1'] = a['n_repairs'] * self.params.heliostat_repair_cost 
        
        #lifetime costs
        a['heliostat_repair_cost'] = self.calc_real_dollars(a['heliostat_repair_cost_y1'])
            #treat heliostat repair costs as consuming reserve equipment paid for at the project outset
        
        return a

    def O(self, variables, design):
        """
        The heliostat field soiling and degradation problem
        
        Returns a dict with keys:
            soil_steady             Steady-state availability
            n_repairs               Total number of repairs made
            heliostat_refurbish_cost  Cost to refurbish heliostats ($ lifetime)
            heliostat_refurbish_cost_y1  Cost "" in year 1 ($/year)
        """
        print "Running soiling/degradation simulation O()"

        args = {
                    'n_hours_sim':25*8760,
                    'n_wash_crews':3,
                    'n_heliostat':int(design['number_heliostats']),
                    'degr_per_hour':7.e-7,
                    'degr_accel_per_year':0.,
                    'degr_replace_limit':0.8,                    
                    'soil_per_hour':5.e-4,
                    'wash_units_per_hour':45,
                    'wash_crew_max_hours_week':70.,
                    'wash_crew_max_hours_day':10.,
                    'degr_seed':123,
                }
                
        vd = variables.as_dict()
        vd.update(self.params.as_dict())
        #overwrite any parameters in args that are provided from V
        for key in args.keys():
            try:
                args[key] = vd[key]
            except KeyError:
                pass

        a = pysdk.run_degradation(args)

        ann_fact = 8760. / args['n_hours_sim']

        a['heliostat_refurbish_cost_y1'] = a['n_repairs'] * self.params.heliostat_refurbish_cost


        #Annualize
        a["n_repairs"] *= ann_fact
        a['heliostat_refurbish_cost'] = self.calc_real_dollars(a['heliostat_refurbish_cost_y1']) * ann_fact

        return a


    def S(self, design, variables, sf_avail=None, sf_soil=None, sf_degr=None, sample_weeks=None, Nclusters = None, cluster_inputs = None, pv_production = None):
        """
        The production simulation (SAM)
        """
        print "Running production simulation S()"
        vd = variables.as_dict()
        vd.update(self.params.as_dict())

        #Figure out how many records are in the weather file
        nrec = open(vd['solar_resource_file'], 'r').read().count('\n')-3
        wf_steps_per_hour = nrec/8760

        #disp_freq = self.params.disp_steps_per_hour

        sf_adjust_tot = [1. for i in range(nrec)]
        #Degradation
        if sf_degr:
            #for now, just take the average
            if isinstance(sf_degr, list):
                degr_mult = sum(sf_degr)/float(len(sf_degr))
            elif isinstance(sf_degr, float):
                degr_mult = sf_degr
        else:
            degr_mult = 1.
        for i in range(nrec):
            sf_adjust_tot[i] *= degr_mult

        #soiling
        avg_soil = 1.
        if sf_soil:

            if isinstance(sf_soil, list):
                for i in range(nrec):
                    sf_adjust_tot[i] *= sf_soil[int(float(i)/wf_steps_per_hour+0.0001)]
                avg_soil = sum(sf_soil)/float(len(sf_soil))
            elif isinstance(sf_soil, float):
                avg_soil = sf_soil
                for i in range(nrec):
                    sf_adjust_tot[i] *= sf_soil

        #availability
        avg_avail = 1.
        if sf_avail:
            if isinstance(sf_avail, list):
                avg_avail = sum(sf_avail)/float(len(sf_avail))
                for i in range(nrec):
                    sf_adjust_tot[i] *= sf_avail[int(float(i)/wf_steps_per_hour+0.0001)]
            elif isinstance(sf_avail, float):
                avg_avail = sf_avail
                for i in range(nrec):
                    sf_adjust_tot[i] *= sf_avail

        vd['sf_adjust:hourly'] = [100.*(1.-v) for v in sf_adjust_tot]  #convert from availability to percent down
        self._sfadjust = vd['sf_adjust:hourly']

        if not design:
            raise PySDKError("The simulation S() was called without a valid design: Terminating.")

        # Run simulation from evenly spaced sampled weeks
        if sample_weeks:
            inc = 52./float(sample_weeks+1)
            wt = 52./float(sample_weeks)
            sall = []
            for iwk in range(sample_weeks):
                tstart = int( iwk*inc*7 )*86400  #86400=seconds per day
                tend = min([8760*3600, tstart + 9*24*3600])  #9 days to eliminate end effects
                nhr = int((tend - tstart)/3600.)-48

                vd['time_start'] = tstart
                vd['time_stop'] = tend
                vd['vacuum_arrays'] = True

                sall.append( pysdk.run_simulation(vd, design) )
            #accumulate the results
            #s = {'generation':[], 'pricing_mult':[]}
            s = sall[0] #adopt the first instance, then update
            for key in ['generation','pricing_mult']:
                s[key] = []
                for sr in sall:
                    s[key] += sr[key][24:24+nhr]

            #recalculate annual generation
            s['annual_generation'] = sum(s['generation'])*wt
            #recalculate pricing units
            s['revenue_units'] = sum( [s['generation'][i]*s['pricing_mult'][i]*wt for i in range(len(s['generation'])) ] )

        # Run simulation based on clustered weather and pricing profiles   
        elif Nclusters is not None or cluster_inputs is not None:
            Ndays = 2
            Nprev = 1
            Nnext = int(vd['is_dispatch'])            # Use 1 subsequent day if dispatch optimization is enabled
            run_continuous = False

            inputs = {}
            inputs['run_continuous_with_dispatch_days'] = run_continuous 
            inputs['nprev'] = Nprev
            for key in ['nday', 'day_start', 'group_weight', 'avg_ppamult', 'avg_sfadjust']:
                inputs[key] = None

            if cluster_inputs is None:  # Re-calculate cluster inputs
                cluster_inputs = self.setup_clusters(Nclusters, Ndays, Nprev, Nnext)
            for key in cluster_inputs.keys():
                inputs[key] = cluster_inputs[key]

            # Combine consecutive exemplars into a single simulation
            days = inputs['day_start']
            base_weight = inputs['weights']
            avg_sfadjust = dni_clustering.compute_cluster_avg_from_timeseries(sf_adjust_tot, inputs['partition_matrix'], Ndays = Ndays, Nprev = Nprev, Nnext = Nnext,adjust_wt = True, k1 = inputs['first_pt_cluster'], k2 = inputs['last_pt_cluster'])
            avg_ppamult = inputs['avg_ppamult']
            combined = dni_clustering.combine_consecutive_exemplars(days, base_weight, avg_ppamult, avg_sfadjust, Ndays, Nprev = Nprev, Nnext = Nnext)  # Combine simulations of consecutive exemplars
            inputs['day_start'] = combined['start_days']
            inputs['nday'] = combined['Nsim_days'] 
            inputs['avg_ppamult'] = combined['avg_ppa']
            inputs['group_weight'] = combined['weights']
            inputs['avg_sfadjust'] = combined['avg_sfadj']
            Ng = len(inputs['day_start'])       # Number of simulations 

            # Run simulations
            steps_per_day = wf_steps_per_hour*24
            s = {}
            hourly_results_keys = ['generation', 'pricing_mult', 'sf_thermal_gen', 'disp_qsfprod_expected', 'disp_wpb_expected']
            for key in hourly_results_keys:
                s[key] = [0.0 for i in range(nrec)]     # Array of yearly generation


            # Run full annual simulation with dispatch enabled only on selected days
            if inputs['run_continuous_with_dispatch_days']:  
                select_dispatch_series = [0 for v in range(365)]
                for g in range(Ng):    # Number of simulation groupings  
                    d1 = inputs['day_start'][g] - Nprev     # First day to be included in simulation group g 
                    for j in range(inputs['nday'][g] + Nprev ):
                        if (d1+j)>=0 and (d1+j)<365:
                            select_dispatch_series[d1 + j] = 1           
                vd['select_dispatch_series'] = select_dispatch_series

                # Set up array of cluster-average solar field availability
                sf_adjust_hourly = list(sf_adjust_tot)    # Hourly adjustment factors from previous calculation  
                if inputs['avg_sfadjust'] is not None:
                    sf_adjust_hourly = dni_clustering.create_annual_array_with_cluster_average_values(sf_adjust_hourly, inputs['avg_ppamult'], inputs['day_start'], inputs['nday'], Nprev = inputs['nprev'], Nnext = Nnext, overwrite_surrounding_days = True)
                    vd['sf_adjust:hourly'] = [100.*(1.-v) for v in sf_adjust_hourly]

                sgroup = pysdk.run_simulation(vd, design)      # Run continuous simulation

            for g in range(Ng):     

                # Run simulation only for group g
                if not inputs['run_continuous_with_dispatch_days']:   # Run simulation for group g
                    d1 = inputs['day_start'][g] - Nprev               # First day to be included in simulation group g 
                    Nprev_sim = inputs['day_start'][g] - max(0,d1)    # Number of previous days actually allowed  in the simulation
                    Ndays_tot = inputs['nday'][g] + Nprev + Nnext     # Number of days to be simulated
                    tstart = int(max(0, d1*24.*3600.))   
                    tend = int(min(8760.*3600., (d1+Ndays_tot)*24.*3600.)) 
                    vd['time_start'] = tstart
                    vd['time_stop'] = tend
                    vd['vacuum_arrays'] = True 
                    vd['time_steps_per_hour'] = wf_steps_per_hour

                    # Update solar field hourly adjustment factors to reflect cluster-average values
                    if inputs['avg_sfadjust'] is not None:
                        sf_adjust_hourly_new = list(sf_adjust_tot)
                        Nsim = inputs['nday'][g] + Nprev + Nnext    
                        for h in range(Nsim*steps_per_day):
                            time_pt = (inputs['day_start'][g] - Nprev)*steps_per_day+ h 
                            if h>=0 and h<8760*wf_steps_per_hour:
                                sf_adjust_hourly_new[time_pt] = inputs['avg_sfadjust'][g][h]
                        vd['sf_adjust:hourly'] = [100.*(1.-v) for v in sf_adjust_hourly_new]
                    else:
                        vd['sf_adjust:hourly'] = [100.*(1.-v) for v in sf_adjust_tot]

                    sgroup = pysdk.run_simulation(vd, design) # Run simulation for selected set of days

                # Collect simulation results in full annual array 
                for d in range(inputs['nday'][g]):                  # Days counted in simulation grouping
                    day_of_year = inputs['day_start'][g] + d          
                    if inputs['run_continuous_with_dispatch_days']:    # Simulation output contains full annual array
                        i = day_of_year * steps_per_day     
                    else:                                              # Simulation output contains only simulated subset of days
                        i = (Nprev_sim+d)*steps_per_day    

                    for key in hourly_results_keys:
                        for h in range(steps_per_day):    # Hours in day d
                            s[key][day_of_year*steps_per_day + h] = sgroup[key][i+h]


            clusters = {'exemplars':inputs['exemplars'], 'partition_matrix':inputs['partition_matrix']}
            
            for key in hourly_results_keys:
                s[key]  = dni_clustering.compute_annual_array_from_clusters(s[key], clusters, Ndays, adjust_wt = True, k1 = inputs['first_pt_cluster'], k2 = inputs['last_pt_cluster'])
                
            s['pricing_mult'] = self.params.dispatch_factors_ts
            s['total_installed_cost'] = sgroup['total_installed_cost']
            s['system_capacity'] = sgroup['system_capacity']
            
            s['annual_generation'] = 0.0
            s['revenue_units'] = 0.0
            for h in range(len(s['generation'])):
                s['annual_generation'] += s['generation'][h] / float(wf_steps_per_hour)
                s['revenue_units'] += s['generation'][h] * s['pricing_mult'][h] / float(wf_steps_per_hour)

        # Run full simulation
        else:       
            s = pysdk.run_simulation(vd, design)

        #lifetime revenue
        rev = s['revenue_units']
        s['sales'] = self.calc_real_dollars(rev * pysdk.F['ppa_price_input'], is_revenue=True)

        #Calculating number of receiver starts
        s['rec_starts'] = 0
        for i in range(1, len(s['sf_thermal_gen'])):
            if s['sf_thermal_gen'][i] > 0. and s['sf_thermal_gen'][i-1] <= 0.:
                s['rec_starts'] += 1

        #Calculating number of cycle starts and cycle ramping
        s['cycle_starts'] = 0
        s['cycle_ramp'] = 0.0
        for i in range(1, len(s['generation'])):
            if s['generation'][i] > 0. and  s['generation'][i-1] <= 0.:
                s['cycle_starts'] += 1
            if s['generation'][i] > s['generation'][i-1] and s['generation'][i] > 0.:
                s['cycle_ramp'] += (s['generation'][i] - s['generation'][i-1]) # 'generation' has units of kWe
        s['cycle_ramp_index'] = s['cycle_ramp']/(max(s['generation'])*365.)


        # Dispatched results for comparison
        #--initialize
        s['disp_rec_starts'] = 0
        s['disp_cycle_starts'] = 0
        s['disp_cycle_ramp'] = 0.0
        s['disp_cycle_ramp_index'] = 0.0
        
        if self.params.is_dispatch:

            #Calculating number of receiver starts DISPATCHED
            gen = s['disp_qsfprod_expected']
            for i in range(1, len(gen)):
                if gen[i] > 0. and gen[i-1] <= 0.:
                    s['disp_rec_starts'] += 1
    
            #Calculating number of cycle starts and cycle ramping DISPATCHED
            gen = s['disp_wpb_expected']
            for i in range(1, len(gen)):
                if gen[i] > 0. and  gen[i-1] <= 0.:
                    s['disp_cycle_starts'] += 1
                if gen[i] > gen[i-1] and gen[i] > 0.:
                    s['disp_cycle_ramp'] += (gen[i] - gen[i-1])
            s['disp_cycle_ramp_index'] = s['disp_cycle_ramp']/(max(gen)*365.)
            s['disp_cycle_ramp'] *= 1000. #convert MW to kW

    
        # Receiver and cycle start up cost
        s['rec_start_cost_y1'] = self.params.disp_rsu_cost*s['rec_starts']
        s['cycle_start_cost_y1'] = self.params.disp_csu_cost*s['cycle_starts']
        s['cycle_ramp_cost_y1'] = self.params.disp_pen_delta_w*s['cycle_ramp']

        # Lifetime cost
        s['rec_start_cost'] = self.calc_real_dollars(s['rec_start_cost_y1'])
        s['cycle_start_cost'] = self.calc_real_dollars(s['cycle_start_cost_y1'])
        s['cycle_ramp_cost'] = self.calc_real_dollars(s['cycle_ramp_cost_y1'])

        if rev < 0:
            s['sales'] = float('nan')  #return not a number if simulation is invalid
        s['avg_degradation'] = degr_mult
        s['avg_soil'] = avg_soil
        s['avg_avail'] = avg_avail
        return s

    def E(self, variables):
        """
        The explicit terms
        """
        print "Running explicit terms E()"
        #receiver cost
        arec = variables.D_rec.value*pi*variables.rec_height.value
        erec = self.params.rec_ref_cost*(arec/self.params.rec_ref_area)**0.7
        #Tower cost
        etower = self.params.tower_fixed_cost * exp(self.params.tower_exp * variables.h_tower.value)

        #Plant Cost
        del_eff = variables.design_eff.value - 0.412 
        effscale = 0.
        for i,c in enumerate(self.params.c_ces):
            effscale += c*del_eff**i

        eplant = (self.params.c_cps0 + self.params.c_cps1 * variables.P_ref.value * 1000.) * (1. + effscale)

        #TES cost
        eTES = self.params.tes_spec_cost * variables.P_ref.value * 1000. \
                    / variables.design_eff.value \
                    * variables.tshours.value

        #OM labor costs
        heliostat_om_labor_y1 = self.params.om_staff_cost * variables.om_staff.value * self.params.om_staff_max_hours_week*52. #annual
        heliostat_om_labor = self.calc_real_dollars(heliostat_om_labor_y1, is_labor = True)  #lifetime

        #Washing labor costs
        heliostat_wash_cost_y1 = variables.n_wash_crews.value * self.params.wash_crew_cost * self.params.wash_crew_max_hours_week*52.  #annual
        heliostat_wash_cost = self.calc_real_dollars(heliostat_wash_cost_y1, is_labor=True)  #lifetime

        d = {'cost_receiver':erec, 
                'cost_tower':etower,
                'cost_plant':eplant,
                'cost_TES':eTES,
             }
        for k,v in d.items():
            d[k] = self.calc_real_dollars(v)
        d.update({
                'heliostat_om_labor_y1':heliostat_om_labor_y1,
                'heliostat_om_labor':heliostat_om_labor,
                'heliostat_wash_cost_y1':heliostat_wash_cost_y1,
                'heliostat_wash_cost':heliostat_wash_cost,
                })
        return d

    def dE(self, variables):
        """
        return derivatives of E
        """

        de = {}  #dictionary containing derivatives by variable key

        return de


    def F(self, variables, S, om_cost): #total_installed_cost, generation, pricing_mult):
        """
        Financial model run


        """
        print "Running financial model F()"
        vd = variables.as_dict()
        vd.update(self.params.as_dict())

        
        f = pysdk.run_financial(vd, S['total_installed_cost'], S['generation'], S['pricing_mult'], om_cost, S['system_capacity'])
        return f


    def Z(self, variables, **kwargs):
        """
        The whole objective function
        """

#        if not self.design:
#            raise PySDKError("Attempting to run objective function without a valid design.")

        vd = variables.as_dict()
        vd.update(self.params.as_dict())

        #user can provide keyword arguments to use existing S or P results
        if "S" in kwargs and not "D" in kwargs:
            raise PySDKError("When providing simulation results 'S' to the objective function, you must also "+
                             "provide design results 'D'.")
        #Design 
        if "D" in kwargs:
            D = kwargs["D"]
        else:
            #run the design module
            D = self.D(variables)  #updates self.design
        #Heliostat availability and OM
        if "M" in kwargs:
            M = kwargs["M"]
        else:
            M = self.M(variables, D['design'])
        #Heliostat soiling and degradation
        if "O" in kwargs:
            O = kwargs["O"]
        else:
            O = self.O(variables, D['design'])
        #Performance simulation - revenue model
        if "S" in kwargs:
            S = kwargs["S"]
        else:
            if "cluster_inputs" in kwargs:
                cluster_inputs = kwargs["cluster_inputs"]
                S = self.S(D['design'], variables, M['avail_sched'], O['soiling_sched'], O['degradation_sched'], cluster_inputs = cluster_inputs)
            elif "Nclusters" in kwargs:
                Nclusters = kwargs["Nclusters"]
                S = self.S(D['design'], variables, M['avail_sched'], O['soiling_sched'], O['degradation_sched'], Nclusters = Nclusters)
            else:
                S = self.S(D['design'], variables, M['avail_sched'], O['soiling_sched'], O['degradation_sched'])
        #Explicit (cost) terms
        if "E" in kwargs:
            E = kwargs["E"]
        else:
            E = self.E(variables)


        #----- the objective function -----
        z = {}       
        z['cost_receiver'] = E['cost_receiver'] 
        z['cost_tower'] = E['cost_tower'] 
        z['cost_plant'] = E['cost_plant'] 
        z['cost_TES'] = E['cost_TES'] 
        z['cost_land'] = D['cost_land'] 
        z['cost_heliostats'] = D['cost_heliostats'] 
        cap_cost = sum( [v for v in z.itervalues()] )

        z['rec_start_cost'] = S['rec_start_cost']
        z['cycle_start_cost'] = S['cycle_start_cost']
        z['cycle_ramp_cost'] = S['cycle_ramp_cost']
        z['heliostat_repair_cost'] = M['heliostat_repair_cost']    #structural, mechanical, electrical repair costs
        z['heliostat_om_labor'] = E['heliostat_om_labor']   #OM labor on heliostat field
        z['heliostat_wash_cost'] = E['heliostat_wash_cost'] #costs due to labor and materials for mirror washing
        z['heliostat_refurbish_cost'] = O['heliostat_refurbish_cost']  #Mirror replacement cost
        om_cost_lifetime = sum( [v for v in z.itervalues()] ) - cap_cost


        om_cost = (
            M['heliostat_repair_cost_y1'] +
            E['heliostat_om_labor_y1'] +
            E['heliostat_wash_cost_y1'] +
            O['heliostat_refurbish_cost_y1'] +
            S['rec_start_cost_y1'] +
            S['cycle_start_cost_y1'] +
            S['cycle_ramp_cost_y1']
          )

        z['sales'] = -S['sales']   #electricity sales

        zobj = sum( [v for v in z.itervalues()] )

        #--- financial model
        f = self.F(variables, S, om_cost)
        z['ppa'] = f['ppa']
        z['lcoe_nom'] = f['lcoe_nom']
        z['lcoe_real'] = f['lcoe_real']
        #------

        if "printout" in kwargs:
            if kwargs["printout"]:
                fmtf = "{:30s}{:^5s}{: 11.1f}"
                fmtd = "{:30s}{:^5s}{: 11d}"
                print "="*20 + " PERF TABLE " + "="*20
                print fmtf.format("Mirror replacements per year","1/yr",O['n_repairs'])
                print fmtf.format("Heliostat repairs per year","1/yr",M['n_repairs'])
                print fmtf.format("Average degradation loss", "%", S['avg_degradation']*100.)
                print fmtf.format("Average soiling loss", "%", S['avg_soil']*100.)
                print fmtf.format("Average availability loss", "%", S['avg_avail']*100.)
                print fmtf.format("Annual generation", "GWhe", S['annual_generation']/1000.)
                print fmtd.format("Receiver Starts", "", S['rec_starts'])
                if self.params.is_dispatch:
                    print fmtd.format("   Dispatched", "", S['disp_rec_starts'])

                print fmtd.format("Cycle Starts", "", S['cycle_starts'])
                if self.params.is_dispatch:
                    print fmtd.format("   Dispatched", "", S['disp_cycle_starts'])

                print fmtf.format("Total Cycle Ramping", "GW", S['cycle_ramp']/1000.) 
                if self.params.is_dispatch:
                    print fmtf.format("   Dispatched", "GW", S['disp_cycle_ramp']/1000.)

                print fmtf.format("Cycle Ramp Index", "%", S['cycle_ramp_index']*100.) 
                if self.params.is_dispatch:
                    print fmtf.format("   Dispatched", "%", S['disp_cycle_ramp_index']*100.)

#                print fmtv.format("Cycle Max" + str(max(s['generation'])) + "\t Dispatched: \t" + str(max(gen))
                
                hdr = "="*20 + " COST TABLE " + "="*20
                print hdr

                zl = [[key,val] for key,val in z.items()]
                zl.sort(key=lambda x: x[1], reverse=True)

                fmt = "{:30s}${: 16,.0f}"
                for k in zl:
                    if k[0] in ['ppa', 'lcoe_real','lcoe_nom']:
                        continue
                    print fmt.format(k[0],-k[1])
                print "-"*len(hdr)
                print fmt.format("Subtot. capital costs", -cap_cost)
                print fmt.format("Subtot. O&M costs", -om_cost_lifetime)
                print "-"*len(hdr)
                print fmt.format("Total cash flow",-zobj)
                print "{:30s}c/kWhe{: 11.3f}".format("PPA price", z['ppa'])

        return zobj, z

