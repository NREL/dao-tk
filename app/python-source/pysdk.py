"""
Mike Wagner

2016.5.11

SDK script for molten salt power tower + single owner modules

Runs on Python 2.7.10, 32 bit

******************************************************************************

TO RUN:

See the __main__ code at the end of the file. Set values there and run the 
script. 


"""

import os
import time
import copy

import sscapi as api

from vartab import vartab as V  #power tower simulation parameters
from vartab import fintab as F  #financial model simulation parameters
from vartab import contab as C  #construction financing parameters

from ctypes import *
import sys, struct
c_number = c_float # must be c_double or c_float depending on how defined in sscapi.h

#import scipy.optimize as sop
from math import exp, sqrt

#basepath = 'C:/users/mwagner/Documents/NREL/OM Optimization/cspopt/software/sdk/languages/python/'
import sdk_paths
basepath = sdk_paths.path


costouts = ['csp.pt.cost.site_improvements','csp.pt.cost.heliostats',
    'csp.pt.cost.tower','csp.pt.cost.receiver','csp.pt.cost.storage',
    'csp.pt.cost.power_block','csp.pt.cost.bop','csp.pt.cost.fossil',
    'ui_direct_subtotal','csp.pt.cost.contingency','total_direct_cost',
    'csp.pt.cost.epc.total','csp.pt.cost.plm.total','csp.pt.cost.sales_tax.total',
    'total_indirect_cost','total_installed_cost'] 

#custom exception class for errors with messages
class PySDKError(Exception):
    pass

def set_ssc_data_from_dict(ssc_api, ssc_data, Dict):
    """
    A method to take a dictionary (such as V, F, or C) and call the correct
    method from the API to set the value in the data object.
    """
    for key in Dict.keys():
       try: 
           if type(Dict[key]) in [type(1),type(1.)]:
               ssc_api.data_set_number(ssc_data, key, Dict[key])
           elif type(Dict[key]) == type(True):
               ssc_api.data_set_number(ssc_data, key, 1 if Dict[key] else 0)
           elif type(Dict[key]) == type(""):
               ssc_api.data_set_string(ssc_data, key, Dict[key])
           elif type(Dict[key]) == type([]):
               if len(Dict[key]) > 0:
                   if type(Dict[key][0]) == type([]):
                       ssc_api.data_set_matrix(ssc_data, key, Dict[key])
                   else:
                       ssc_api.data_set_array(ssc_data, key, Dict[key])
               else:
#                   print ("Did not assign empty array " + key)
                   pass
           else:
               print ("Could not assign variable " + key )
               raise KeyError
       except:
           print "Error assigning variable " + key + ": bad data type"

def htf_cp(T_C, *args):
    """
    provide T in degr Celsius
    kJ/kg-k specific heat 
    """
    try:
        htftype = args[0]
    except IndexError:
        htftype = 17
    
    if htftype == 10:
        #Salt_465_LiF_115_NaF_42KF
        return 2.010;
    elif htftype == 17:
        #Salt_60_NaNO3_40_KNO3
        T_K = T_C + 273.15
        return -1E-10*T_K*T_K*T_K + 2E-07*T_K*T_K + 5E-06*T_K + 1.4387



def update_calculated_system_values(ssc_api, ssc_data, Dict):
    
    """
    Inputs that you can specify in Dict:
    -------------------------------------------------------------
    D_rec
    design_eff
    dni_des
    gross_net_conversion_factor
    N_panels
    P_ref
    solarm
    tshours
    helio_optical_error_mrad
    
    Values that are set by this algorithm:
    -------------------------------------------------------------
    dni_des_calc
    n_flux_x (if less than 12)
    nameplate
    q_design
    q_pb_design
    Q_rec_des
    rec_aspect
    field_model_type
    system_capacity
    tower_technology
    tshours_sf
    helio_optical_error

    """    
    
    D = Dict
    
    #net power (only used as an input to system costs compute module )
    D['nameplate'] = D['P_ref']*D['gross_net_conversion_factor']  #MWe
    D['system_capacity'] = D['nameplate'] * 1000.
    	
    # q_pb_design(informational, not used as a compute module input for mspt)
    D['q_pb_design'] = float(D['P_ref'])/float(D['design_eff'])
    
    # Q_rec_des (only used as in input to solarpilot compute module)
    D['Q_rec_des'] = D['solarm']*D['q_pb_design'] 
    D['q_design'] = D['Q_rec_des'] 
    
    # tshours_sf  (informational, not used as a compute module input)
    D['tshours_sf'] = D['tshours']/D['solarm']

    #receiver aspect ratio (only used as in input to solarpilot compute module)
#    D['rec_aspect'] = 1
#    if (D['D_rec'] != 0):
    D['rec_aspect'] = float(D['rec_height']) / float(D['D_rec']);
    
    #always set to MSPT
    D['tower_technology'] = 0
    
    #Flux grid resolution limited by number of panels  (only used as in input to solarpilot compute module)
    D['n_flux_x'] = max(12, D['N_panels'])
    
    D['field_model_type'] = 2  # 0=design field and tower/receiver geometry 1=design field 2=user field, calculate performance 3=user performance maps vs solar position
   
    D['helio_optical_error'] = D['helio_optical_error_mrad']/1000.  # only used as in input to solarpilot compute module
    return


def update_calculated_values_post_layout(ssc_api, ssc_data, Dict):
    """
    Do the calculation for all constrained values. These calculations mimic
    those in the SAM interface for calculated values. 
    
    Inputs that you can specify in Dict:
    --------------------------------------------------------------------------
    aux_par
    aux_par_0
    aux_par_1
    aux_par_2
    aux_par_f
    bop_par
    bop_par_0
    bop_par_1
    bop_par_2
    bop_par_f
    csp.pt.rec.max_oper_frac
    csp.pt.sf.fixed_land_area
    csp.pt.sf.land_overhead_factor
    D_rec
    dens_mirror
    design_eff
    rec_height
    h_tank
    helio_height
    helio_positions
    helio_width
    csp.pt.rec.htf_t_avg
    HTF_dens
    land_area_base
    P_ref
    piping_length_const
    piping_length_mult
    piping_loss
    rec_htf
    T_htf_cold_des
    T_htf_hot_des
    h_tower
    tshours

    Values that are set by update_calculated_system_values()
    Q_rec_des

    Values that are set by this algorithm:
    --------------------------------------------------------------------------
    A_sf
    csp.pt.cost.bop
    csp.pt.cost.contingency
    csp.pt.cost.epc.total
    csp.pt.cost.fossil
    csp.pt.cost.heliostats
    csp.pt.cost.installed_per_capacity
    csp.pt.cost.plm.total
    csp.pt.cost.power_block
    csp.pt.cost.receiver
    csp.pt.cost.receiver.area
    csp.pt.cost.sales_tax.total
    csp.pt.cost.site_improvements
    csp.pt.cost.storage
    csp.pt.cost.storage_mwht
    csp.pt.cost.total_land_area
    csp.pt.cost.tower
    csp.pt.par.calc.aux
    csp.pt.par.calc.bop
    csp.pt.rec.htf_c_avg
    csp.pt.rec.htf_t_avg
    csp.pt.rec.max_flow_to_rec
    csp.pt.sf.heliostat_area
    csp.pt.sf.total_land_area
    csp.pt.sf.total_reflective_area
    csp.pt.sf.tower_height
    d_tank
    dens
    eta_pb_des
    field_control
    helio_area_tot
    H_rec
    m_dot_htf_max
    n_hel
    piping_length
    piping_loss_tot
    q_dot_loss
    q_tes_des
    T_HTF_cold
    T_HTF_hot
    tes_hrs
    TES_HTF_code
    TES_HTF_props
    total_direct_cost
    total_indirect_cost
    total_installed_cost 
    ui_direct_subtotal  
    V_wind_10
    vol_one_temp_avail
    vol_one_temp_total
    W_dot_pb_des   
    
    """ 
    D = Dict  #ptr
    

    #------------solar field (only 'n_hel' and 'A_sf' used as inputs for mspt or system costs compute module)
    D['n_hel'] =  len(D['helio_positions'])
    
    D['csp.pt.sf.heliostat_area'] =  D['helio_height']*D['helio_width']*D['dens_mirror']
    
    D['csp.pt.sf.total_reflective_area'] =  D['n_hel']*D['csp.pt.sf.heliostat_area']
        
    D['csp.pt.sf.total_land_area'] =  D['csp.pt.sf.fixed_land_area'] + D['land_area_base']*D['csp.pt.sf.land_overhead_factor']
    
    D['A_sf'] =  D['helio_width'] * D['helio_height'] * D['dens_mirror'] * D['n_hel']
    D['helio_area_tot'] =  D['A_sf']
    
    D['field_control'] =  1
    
    D['V_wind_10'] =  0
   
    #------------parasitics (informational, not used as a compute module input)
    #parasitic BOP
    D['csp.pt.par.calc.bop'] = \
        D['bop_par']*D['bop_par_f']*( D['bop_par_0'] + D['bop_par_1'] + \
        D['bop_par_2'] )*D['P_ref']
    #Aux parasitic
    D['csp.pt.par.calc.aux'] = \
        D['aux_par']*D['aux_par_f']*( D['aux_par_0'] + D['aux_par_1'] + \
        D['aux_par_2'] )*D['P_ref']


    #------------receiver max mass flow rate (informational, not used as a compute module input)
    #Receiver average temperature 
    D['csp.pt.rec.htf_t_avg'] = (D['T_htf_cold_des']+D['T_htf_hot_des'])/2.
    
    #htf specific heat
    D['csp.pt.rec.htf_c_avg'] = htf_cp(D['csp.pt.rec.htf_t_avg'])
    
    #maximum flow rate to the receiver
    D['csp.pt.rec.max_flow_to_rec'] = \
        (D['csp.pt.rec.max_oper_frac']*D['Q_rec_des']*1e6) \
        / (D['csp.pt.rec.htf_c_avg']*1e3*(D['T_htf_hot_des']-D['T_htf_cold_des']))
        
    #max flow rate in kg/hr
    D['m_dot_htf_max'] = D['csp.pt.rec.max_flow_to_rec'] * 60 * 60
    
    
    #------------piping length and piping loss (informational, not used as a compute module input for mspt)
    #Calculate the thermal piping length
    D['piping_length'] = D['h_tower']*D['piping_length_mult']+D['piping_length_const']
    
    #total piping length
    D['piping_loss_tot'] = D['piping_length'] * D['piping_loss'] / 1000.
    
    #------------TES (informational, not used as a compute module input)
    #update data object with D items
    D['W_dot_pb_des'] = D['P_ref']       #[MWe]
    D['eta_pb_des'] = D['design_eff']       #[-]
    D['tes_hrs'] = D['tshours']       #[hrs]
    D['T_HTF_hot'] = D['T_htf_hot_des']       #[C]
    D['T_HTF_cold'] = D['T_htf_cold_des']       #[C]
    D['TES_HTF_code'] = D['rec_htf']       #[-]
    D['TES_HTF_props'] = [[]]
    
    set_ssc_data_from_dict(ssc_api, ssc_data, D)
    #use the built in calculations for sizing TES
    tescalcs = ssc_api.module_create("ui_tes_calcs")
    ret = ssc_api.module_exec(tescalcs, ssc_data)
    #---Collect calculated values
    #TES thermal capacity at design
    D['q_tes_des'] = ssc_api.data_get_number(ssc_data, 'q_tes_des')
    #Available single temp storage volume
    D['vol_one_temp_avail'] = ssc_api.data_get_number(ssc_data, 'vol_one_temp_avail')
    #Total single temp storage volume
    D['vol_one_temp_total'] = ssc_api.data_get_number(ssc_data, 'vol_one_temp_total')
    #Single tank diameter
    D['d_tank'] = ssc_api.data_get_number(ssc_data, 'd_tank')
    #Estimated tank heat loss to env.
    D['q_dot_loss'] = ssc_api.data_get_number(ssc_data, 'q_dot_loss')
    D['dens'] = ssc_api.data_get_number(ssc_data, 'HTF_dens')
    ssc_api.module_free(tescalcs)
    
    #------------capital costs (informational, not used as a compute module input for mspt)
    # Receiver Area
    D['csp.pt.cost.receiver.area'] = D['rec_height']*D['D_rec']*3.1415926 
    	
    # Storage Capacity
    D['csp.pt.cost.storage_mwht'] = D['P_ref'] / D['design_eff'] * D['tshours']
    	
    # Total land area
    D['csp.pt.cost.total_land_area'] = D['csp.pt.sf.total_land_area']
    	
    D['H_rec'] = D['rec_height']
    D['csp.pt.cost.power_block_per_kwe'] = D['plant_spec_cost'] 
    D['csp.pt.cost.plm.per_acre'] = D['land_spec_cost'] 
    D['csp.pt.cost.fixed_sf'] = D['cost_sf_fixed'] 

    set_ssc_data_from_dict(ssc_api, ssc_data, D)
    #init cost
    cost = ssc_api.module_create("cb_mspt_system_costs")
    #do cost calcs
    ret = ssc_api.module_exec(cost, ssc_data)
    if ret == 0:
        print "Cost model failed"
    
    #collect calculated values 
    D['csp.pt.cost.site_improvements'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.site_improvements')
    D['csp.pt.cost.heliostats'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.heliostats')    
    D['csp.pt.cost.tower'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.tower')        
    D['csp.pt.cost.receiver'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.receiver')	         
    D['csp.pt.cost.storage'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.storage')             
    D['csp.pt.cost.power_block'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.power_block')	         
    D['csp.pt.cost.bop'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.bop')           
    D['csp.pt.cost.fossil'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.fossil')	             
    D['ui_direct_subtotal'] = ssc_api.data_get_number(ssc_data, 'ui_direct_subtotal')             
    D['csp.pt.cost.contingency'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.contingency')	         
    D['total_direct_cost'] = ssc_api.data_get_number(ssc_data, 'total_direct_cost')       
    D['csp.pt.cost.epc.total'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.epc.total')	         
    D['csp.pt.cost.plm.total'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.plm.total')         
    D['csp.pt.cost.sales_tax.total'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.sales_tax.total')	     
    D['total_indirect_cost'] = ssc_api.data_get_number(ssc_data, 'total_indirect_cost')       
    D['total_installed_cost'] = ssc_api.data_get_number(ssc_data, 'total_installed_cost')	         
    D['csp.pt.cost.installed_per_capacity'] = ssc_api.data_get_number(ssc_data, 'csp.pt.cost.installed_per_capacity')

    ssc_api.module_free(cost)

    return True


def field_design(ssc_api, ssc_data, Dict):
    """
    The following values need to be specified as inputs for this algorithm:
    ========================================================================
    bop_spec_cost     [$/kWe] BOS specific cost
    c_atm_0     [] Attenuation coefficient 0
    c_atm_1     [] Attenuation coefficient 1
    c_atm_2     [] Attenuation coefficient 2
    c_atm_3     [] Attenuation coefficient 3
    cant_type     [] Heliostat cant method
    contingency_rate     [%] Contingency for cost overrun
    cost_sf_fixed     [$] Soalr field fixed cost
    D_rec       [m] Receiver diameter
    delta_flux_hrs     [] Hourly frequency in flux map lookup
    dens_mirror     [frac] Ratio of reflective area to profile
    dni_des     [W/m2] Design-point DNI
    flux_max     [] Maximum allowable flux
    focus_type     [] Heliostat focus method
    rec_height     [m] Receiver height
    h_tower     [m] Tower height
    helio_active_fraction     [frac] Active fraction of reflective area
    helio_height     [m] Heliostat height
    helio_optical_error     [rad] Optical error
    helio_reflectance     [frac] Mirror reflectance
    helio_width     [m] Heliostat width
    heliostat_spec_cost     [$/m2] Heliostat field cost
    is_optimize     [] Do SolarPILOT optimization
    land_max     [] Max heliostat-dist-to-tower-height ratio
    land_min     [] Min heliostat-dist-to-tower-height ratio
    land_spec_cost     [$/acre] Total land area cost
    n_facet_x     [] Number of heliostat facets - X
    n_facet_y     [] Number of heliostat facets - Y
    n_flux_days     [] No. days in flux map lookup
    opt_algorithm     [] Optimization algorithm
    opt_conv_tol     [] Optimization convergence tol
    opt_flux_penalty     [] Optimization flux overage penalty
    opt_init_step     [] Optimization initial step size
    opt_max_iter     [] Max. number iteration steps
    plant_spec_cost     [$/kWe] Power cycle specific cost
    q_design     [MW] Receiver thermal design power
    rec_absorptance     [frac] Absorptance
    rec_aspect     [frac] Receiver aspect ratio (H/W)
    rec_cost_exp     [] Receiver cost scaling exponent
    rec_hl_perm2     [kW/m2] Receiver design heat loss
    rec_ref_area     [] Receiver reference area for cost scale
    rec_ref_cost     [$] Receiver reference cost
    sales_tax_frac     [%] Percent of cost to which sales tax applies
    sales_tax_rate     [%] Sales tax rate
    site_spec_cost     [$/m2] Site improvement cost
    solar_resource_file     [] Solar weather data file
    tes_spec_cost     [$/kWht] Thermal energy storage cost
    tower_exp     [] Tower cost scaling exponent
    tower_fixed_cost     [$] Tower fixed cost

    
        
    The following values are set by this algorithm:
    ========================================================================
    A_sf     [m^2] Total reflective heliostat area
    area_sf     [m^2] Total reflective heliostat area
    base_land_area     [acre] Land area occupied by heliostats
    calc_fluxmaps     [] Include fluxmap calculations
    cost_land_tot     [$] Total land cost
    cost_rec_tot     [$] Total receiver cost
    cost_sf_tot     [$] Total heliostat field cost
    cost_site_tot     [$] Total site cost
    cost_tower_tot     [$] Total tower cost
    flux_table     [frac] Flux intensity table (flux(X) x (flux(y) x position)
    h_tower_opt     [m] Optimized tower height
    heliostat_positions     [m] Heliostat positions (x,y)
    land_area     [acre] Total land area
    number_heliostats     [] Number of heliostats
    opteff_table     [] Optical efficiency (azi, zen, eff x nsim)
    rec_aspect_opt     [-] Optimized receiver aspect ratio
    rec_height_opt     [m] Optimized receiver height

    
    """
    D = Dict

    #Check to make sure the weather file exists
    if not os.path.exists(D['solar_resource_file']):
        raise Exception("\nThe specified Weather file could not be found. Aborting design.\n")

    D['flux_max'] = 1000.
    if 'check_max_flux' not in D:
        D['check_max_flux'] = True
    
    
    update_calculated_system_values(ssc_api, ssc_data, Dict)
    
    Dict['heliostat_positions_in'] = [[]]
    set_ssc_data_from_dict(ssc_api, ssc_data, Dict)
    sp = ssc_api.module_create("solarpilot")
    ssc_api.module_exec(sp, ssc_data)
        
    #Collect calculated data
    D['opteff_table'] = ssc_api.data_get_matrix(ssc_data, "opteff_table")
    D['flux_table'] = ssc_api.data_get_matrix(ssc_data, "flux_table")
    D['heliostat_positions'] = ssc_api.data_get_matrix(ssc_data, "heliostat_positions")
    D['number_heliostats'] = ssc_api.data_get_number(ssc_data, "number_heliostats")
    D['area_sf'] = ssc_api.data_get_number(ssc_data, "area_sf")
    D['base_land_area'] = ssc_api.data_get_number(ssc_data, "base_land_area")
    D['land_area'] = ssc_api.data_get_number(ssc_data, "land_area")
    D['h_tower_opt'] = ssc_api.data_get_number(ssc_data, "h_tower_opt")
    D['rec_height_opt'] = ssc_api.data_get_number(ssc_data, "rec_height_opt")
    D['rec_aspect_opt'] = ssc_api.data_get_number(ssc_data, "rec_aspect_opt")
    D['cost_rec_tot'] = ssc_api.data_get_number(ssc_data, "cost_rec_tot")
    D['cost_sf_tot'] = ssc_api.data_get_number(ssc_data, "cost_sf_tot")
    D['cost_tower_tot'] = ssc_api.data_get_number(ssc_data, "cost_tower_tot")
    D['cost_land_tot'] = ssc_api.data_get_number(ssc_data, "cost_land_tot")
    D['cost_site_tot'] = ssc_api.data_get_number(ssc_data, "cost_site_tot")
    D['flux_max_observed'] = ssc_api.data_get_number(ssc_data, "flux_max_observed")
    
    D['calc_fluxmaps'] = 0

    #update values
    D['helio_positions'] = D['heliostat_positions']    
    D['A_sf'] = D['area_sf']
    D['land_area_base'] = D['base_land_area']
    
    if D['print_messages']:
        print "Finished layout with " + str(D['number_heliostats']) + " heliostats in layout"
    
    
    update_calculated_values_post_layout(ssc_api, ssc_data, Dict)
    D['field_model_type'] = 2  # use this layout

    set_ssc_data_from_dict(ssc_api, ssc_data, Dict)
    
    ssc_api.module_free(sp)
    return


def run_design(usr_vars):
    
    """
    run only the design
    
    usr_vars is a dict containing name:value pairs. The name must be a valid
    ssc variable from vartab.py, and corresponding value must match the var
    type given in the same file.     
    """    
    Vt = copy.deepcopy(V)

    ssc = api.PySSC()
    dat = ssc.data_create()
    try:
        prm = usr_vars['print_messages']
    except KeyError:
        prm = True
    ssc.module_exec_set_print( 1 if prm else 0 )  #0 = no, 1 = yes (print progress updates)

    
    #*************************************************************************
    #   change any defaults 
    #*************************************************************************
    Vt.update(usr_vars)
    Vt['calc_fluxmaps'] = True

    #Run design to get field layout    
    field_design(ssc, dat, Vt)    
    
    #check whether all items required in the design have been provided
    dkeys = ['opteff_table', 'flux_table', 'heliostat_positions', 'number_heliostats',
             'area_sf', 'base_land_area', 'land_area', 'cost_rec_tot', 'cost_sf_tot',
             'cost_tower_tot', 'cost_land_tot', 'cost_site_tot', 'flux_max_observed']
    R = {}
    for k in dkeys:
        R[k] = Vt[k]
    return R
    
    
def run_simulation(usr_vars, design):
    
    """
    Run just the simulation without updating the design
    
    Returns total electricity sales, om costs
    
    usr_vars is a dict containing name:value pairs. The name must be a valid
    ssc variable from vartab.py, and corresponding value must match the var
    type given in the same file.     
    """    
    
    ssc = api.PySSC()
    dat = ssc.data_create()
    try:
        prm = usr_vars['print_messages']
    except KeyError:
        prm = True
    ssc.module_exec_set_print( 1 if prm else 0 )  #0 = no, 1 = yes (print progress updates)

    #------------ configure and run the performance simulation ----------------    
    
    Vt = copy.deepcopy(V)
    
    #*************************************************************************
    #   change any defaults 
    #*************************************************************************
    Vt.update(usr_vars)
    
    #*************************************************************************
    if Vt['is_ampl_engine']:
        os.chdir(Vt['ampl_data_dir'])
    
    update_calculated_system_values(ssc, dat, Vt)
    
#    design['calc_fluxmaps'] = 1
    design['flux_max'] = 1000.
    Vt.update(design)

    #update values
    Vt['helio_positions'] = design['heliostat_positions']    
    Vt['n_hel'] = design['number_heliostats']
    Vt['A_sf_in'] = Vt['helio_width'] * Vt['helio_height'] * Vt['dens_mirror'] * design['number_heliostats']
    Vt['land_area_base'] = design['base_land_area']
    Vt['eta_map'] = [[p[0]+180]+p[1:] for p in Vt['opteff_table']]
    Vt['flux_positions'] = [p[0:2] for p in Vt['eta_map']]
    Vt['flux_maps'] = Vt['flux_table']
    Vt['field_model_type'] = 3 # Use previously defined flux maps
    

#    Vt['vacuum_arrays'] = True
#    Vt['time_start'] = 24*100*3600
#    Vt['time_stop'] = 24*136*3600

    set_ssc_data_from_dict(ssc, dat, Vt)
    #----------------------------------------
    
    #instantiate molten salt power tower module
    mspt = ssc.module_create("tcsmolten_salt")	
    #run
    if ssc.module_exec(mspt, dat) == 0:
        idx = 1
        msg = ssc.module_log(mspt, 0)
        while (msg != None):
            print ('\t: ' + msg)
            msg = ssc.module_log(mspt, idx)
            idx = idx + 1
        raise PySDKError( 'tcsmolten_salt simulation error')
    else:
        if Vt['print_messages']:
            ann = ssc.data_get_number(dat, "annual_energy")
            print( '\nSimulation ok, annual energy {:,.3f} (MWhe)'.format(ann/1.e3) )
        
    if Vt['print_messages']:
        curfmt = '{:<30} ${:>15,.0f}'
        
        for cn in costouts:
            cd= ssc.data_get_number(dat, cn)
            print (curfmt.format(cn,cd))
    total_installed_cost = ssc.data_get_number(dat, "total_installed_cost")
    system_capacity = Vt['system_capacity']

    #calculate total sales
    gen = ssc.data_get_array(dat, "gen")
    price = ssc.data_get_array(dat, "pricing_mult")
    sf_gen = ssc.data_get_array(dat, "Q_thermal")
    e_ch_tes = ssc.data_get_array(dat, "e_ch_tes")
    
    disp_actuals = {}
    for k in [
            "disp_solve_state",		#Dispatch solver state
            "disp_solve_iter",		#Dispatch iterations count
            "disp_objective",		#Dispatch objective function value
            "disp_obj_relax",		#Dispatch objective function - relaxed max
            "disp_qsf_expected",		#Dispatch expected solar field available energy
            "disp_qsfprod_expected",		#Dispatch expected solar field generation
            "disp_qsfsu_expected",		#Dispatch expected solar field startup enegy
            "disp_tes_expected",		#Dispatch expected TES charge level
            "disp_pceff_expected",		#Dispatch expected power cycle efficiency adj.
            "disp_thermeff_expected",		#Dispatch expected SF thermal efficiency adj.
            "disp_qpbsu_expected",		#Dispatch expected power cycle startup energy
            "disp_wpb_expected",		#Dispatch expected power generation
            "disp_rev_expected",		#Dispatch expected revenue factor
            "disp_presolve_nconstr",		#Dispatch number of constraints in problem
            "disp_presolve_nvar",		#Dispatch number of variables in problem
            "disp_solve_time",		#Dispatch solver time
            ]:
        disp_actuals[k] = ssc.data_get_array(dat, k)
    
# Print simulation results for debuging
    print_sim_results = True
    if print_sim_results:
	sim_results = {}
    	for k in [
		"q_sf_inc",			# Field incident thermal power
		"eta_field",			# Field optical efficiency
		"q_dot_rec_inc",		# Rec. incident thermal power
		"eta_therm",			# Rec. thermal efficiency
		"Q_thermal",			# Rec. thermal power to HTF less piping loss
		"q_piping_losses",		# Rec. header/tower piping losses
		"q_thermal_loss",		# Rec. convection and emission losses
		"eta",				# PC efficiency: gross
		"q_pb",				# PC input energy
		"P_cycle",			# PC electrical power output: gross
		"pparasi",			# Parasitic power heliostat drives
		"P_tower_pump",			# Parasitic power receiver/tower HTF pump
		"htf_pump_power",		# Parasitic power TES and Cycle HTF pump
		"P_cooling_tower_tot",		# Parasitic power condenser operation
		"P_fixed",			# Parasitic power fixed load
		"P_plant_balance_tot",		# Parasitic power generation-dependent load
		"P_out_net"			# Total electric power to grid
		]:
	    sim_results[k] = ssc.data_get_array(dat,k)

	fout = open("CSP_SAM_results.csv","w")
	for key in sim_results.keys():
	    if key == sim_results.keys()[-1]:
	        fout.write(key + "\n")
	    else:
	        fout.write(key + ",")

	for i in range(len(sim_results[key])):
	    for key in sim_results.keys():
	        if key == sim_results.keys()[-1]:
		    fout.write(str(sim_results[key][i]) + "\n")
	        else:
		    fout.write(str(sim_results[key][i]) + ",")

    revunits = 0.
    for i in range(len(gen)):
        revunits += gen[i]*price[i]

    ssc.module_free(mspt)
    ssc.data_free(dat)
    
    rdict = {
            'revenue_units':revunits,   #annual
            'annual_generation':sum(gen),  #annual
            'generation':gen, #hourly array
            'sf_thermal_gen':sf_gen,  #hourly array
            'pricing_mult':price, #hourly array
            'e_ch_tes':e_ch_tes, #hourly array
            'total_installed_cost':total_installed_cost,
            'system_capacity': system_capacity
            }
    
    rdict.update(disp_actuals)
    
    return rdict


def run_financial(usr_vars, total_installed_cost, gen, price_mult, om_cost, system_capacity):

    #---- create and run construction financing calculator

    ssc = api.PySSC()
    dat = ssc.data_create()
    try:
        prm = usr_vars['print_messages']
    except KeyError:
        prm = True
    ssc.module_exec_set_print( 1 if prm else 0 )  #0 = no, 1 = yes (print progress updates)


#    total_installed_cost = ssc.data_get_number(dat, 'total_installed_cost')
    C['total_installed_cost'] = total_installed_cost
    set_ssc_data_from_dict(ssc, dat, C)
    constr = ssc.module_create("cb_construction_financing")
    ssc.module_exec(constr, dat)
    const_cost = ssc.data_get_number(dat, "construction_financing_cost")
    
    #------------ configure and run the financial calculations ----------------
    F['ppa_multiplier_model'] = 1  #0=diurnal,1=timestep
    F['dispatch_factors_ts'] = price_mult
    
    #set financials based on results of simulation
    F["construction_financing_cost"] = const_cost
    F['gen'] = gen
    F["total_installed_cost"] = total_installed_cost

    F["om_fixed"] = [om_cost]
    
    F["system_capacity"] = system_capacity

    #update data object with parameters in F
    datfin = ssc.data_create()
    set_ssc_data_from_dict(ssc, datfin, F)
    #create financial
    fin = ssc.module_create("singleowner")
    #run financial
    frets = {}
    finouts = [\
        'lcoe_nom',      #[cents/kWh]  Levelized cost (nominal)
        'lcoe_real',      #[cents/kWh]  Levelized cost (real)
        'ppa',          #[cents/kWh]  PPA price (first year)
        'project_return_aftertax_npv',   #NPV
        'project_return_aftertax_irr',   #IRR
        'total_installed_cost'
    ]

    if ssc.module_exec(fin, datfin) == 0:
        print ("singleowner simulation error")
        idx = 1
        msg = ssc.module_log(fin, 0)
        while (msg != None):
            print ('\t: ' + msg )
            msg = ssc.module_log(fin, idx)
            idx = idx + 1
            
        for fn in finouts:
            frets[fn] = float('nan')
    else:

#        print("\nFinancial calculations completed")
        for fn in finouts:
            cd = ssc.data_get_number(datfin, fn)
            frets[fn] = cd
    
    #clean up    
    ssc.module_free(fin)
    ssc.module_free(constr)
    ssc.data_free(dat)
    ssc.data_free(datfin)
    
    return frets  #returns financials (frets)


#==============================================================================
def regr_mod(x, C):
    #The regression model
    return C[0] + C[1] * exp(C[2]*x)

def regr_ss(C, Z):
    #Calculate total sum of squares error
    s = 0.
    npt = len(Z)/200
    for x in range(0,len(Z),npt):
        zeq = regr_mod(x,C)
        s += (zeq - Z[x])**2
    return s
    
def load_shared_library(libname):
    #decide which platform we're on
    if sys.platform == 'win32' or sys.platform == 'cygwin':
        if 8*struct.calcsize("P") == 64:
            adll = CDLL(os.path.join(basepath, "../../sdk-win64/%s.dll"%libname) )
        else:
            adll = CDLL(os.path.join(basepath, "../../sdk-win32/%s.dll"%libname) )
    elif sys.platform == 'darwin':
        adll = CDLL("%s.dylib"%libname) 
    elif sys.platform in ['linux2', 'linux']:
        adll = CDLL(os.path.join(basepath, "../../sdk-linux/%s.so"%libname) )  # instead of relative path, require user to have on LD_LIBRARY_PATH
    else:
        print( "Platform not supported ", sys.platform)
        return None
    return adll
    
def run_availability(usr_vars):
    """
    Run the long-term stochastic heliostat field availability module to get
    the hourly profile.
    
    This function uses the following components from usr_vars:
        helio_mtf               heliostat mean time to failure
        helio_min_fail_time     heliostat minimum down time after failure
        helio_max_fail_time     heliostat maximum down time after failure
        n_hel                   number of heliostats
        n_om_staff              number of available OM staff for repairs
        om_staff_max_hours_week   maximum hours per week each staff can work on repairs
        n_avail_sim_hours       number of hours to simulate for availability model
        avail_seed              Seed value for random sampling
        n_heliostats_sim        number of heliostats to use in scaled problem
    """
    
    #decide which platform we're on
    adll = load_shared_library('avail_model')
   
    #create the module context
    adll.create_context.restype = c_void_p
    cxt = adll.create_context()
    #Run the simulation
    adll.run.restype = c_void_p
    adll.run(c_void_p(cxt), 
                c_int(usr_vars['helio_mtf']), 
                c_number(usr_vars['helio_min_fail_time']), 
                c_number(usr_vars['helio_max_fail_time']), 
                c_int(usr_vars['n_hel']), 
                c_int(usr_vars['om_staff']), 
                c_number(usr_vars['om_staff_max_hours_week']), 
                c_int(usr_vars['n_avail_sim_hours']),
                c_int(usr_vars['avail_seed']),
                c_int(usr_vars['n_heliostats_sim'])
            )
    #Extract the availability profile
    adll.get_avail_schedule.restype = POINTER(c_number)
    count = c_int()
    parr = adll.get_avail_schedule(c_void_p(cxt), byref(count))
    arr = parr[0:count.value] # extract all at once			
    #Collect summary results
    adll.get_summary_results.restype = c_void_p
    n_repairs = c_int()
    staff_utilization = c_number()
    adll.get_summary_results(c_void_p(cxt), byref(n_repairs), byref(staff_utilization) )
    
    #fit to availability to exponential regression model
    #R = sop.minimize(regr_ss, [.1, .1, -.0001], args=(arr), bounds=[[None,None], [None,None], [-10., 0.]])
#    r_steady = 0. #R.x[0]
    
    #construct a suitable availability schedule from the available data
    if len(arr) > 8759:
        asched = arr[-8760:]  #take the last full year of hours
    else:
        nn = int(len(arr)/8760.)+1  #repeat the vector to generate sufficient data
        asched = (arr*nn)[0:8760]


    #return values
    return {  #"avail_steady":r_steady, 
            "n_repairs":int(n_repairs.value), 
            "staff_utilization":float(staff_utilization.value),
            "avail_sched":asched
            }

            
def run_degradation(usr_vars):
    """
    Run the long-term stochastic heliostat field degradation module to get
    the hourly profile.
    
    This function uses the following components from usr_vars:
        n_hours_sim             Number of hours to simulate
        n_wash_crews            Number of mirror washing crews
        n_heliostat             Number of heliostats in the field
        degr_per_hour           Initial reflectivity degradation rate, per hour
        degr_accel_per_year     Reflectivity degradation acceleration, per year
        degr_replace_limit      Minimum reflectivity threshold for mirror replacement
        soil_per_hour           Soiling rate, per hour
        wash_units_per_hour     Number of units washed per hour per crew
        wash_crew_max_hours_week   Number of working hours per week per crew
        wash_crew_max_hours_day    Number of working hours per day per crew
        degr_seed               Seed value for random sampling
    """
    
    #decide which platform we're on
    adll = load_shared_library('degradation_model')
   
    #create the module context
    adll.create_context.restype = c_void_p
    cxt = adll.create_context()
    #Run the simulation
    adll.run.restype = c_void_p
    
    adll.run(c_void_p(cxt), 
                c_int(usr_vars['n_hours_sim']),
                c_int(usr_vars['n_wash_crews']),
                c_int(usr_vars['n_heliostat']),
                c_number(usr_vars['degr_per_hour']),
                c_number(usr_vars['degr_accel_per_year']),
                c_number(usr_vars['degr_replace_limit']),
                c_number(usr_vars['soil_per_hour']),
                c_number(usr_vars['wash_units_per_hour']),
                c_number(usr_vars['wash_crew_max_hours_week']),
                c_number(usr_vars['wash_crew_max_hours_day']),
                c_int(usr_vars['degr_seed'])
            )
    
    #Extract the soiling profile
    adll.get_soiling_schedule.restype = POINTER(c_number)
    count = c_int()
    parr = adll.get_soiling_schedule(c_void_p(cxt), byref(count))
    arr = parr[0:count.value] # extract all at once			
    
    #construct a suitable soiling schedule from the available data
    if len(arr) > 8759:
        ssched = arr[-8760:]  #take the last full year of hours
    else:
        nn = int(len(arr)/8760.)+1  #repeat the vector to generate sufficient data
        ssched = (arr*nn)[0:8760]

    soil_steady = sum(ssched)/float(len(ssched))
    
    #Extract the degradation profile
    adll.get_degradation_schedule.restype = POINTER(c_number)
    dcount = c_int()
    darr = adll.get_degradation_schedule(c_void_p(cxt), byref(dcount))
    dsched = darr[0:dcount.value] # extract all at once
    
    
    #get number of repairs 
    adll.get_number_repairs.restype = c_int
    nrep = c_int()
    nrep = adll.get_number_repairs(c_void_p(cxt))
    
    #return values
    return {"soil_steady":soil_steady, "n_repairs":int(nrep), "soiling_sched":ssched, "degradation_sched":dsched}
            
#==============================================================================
#==============================================================================




