import objective_function as obj_mod
import json
#import numpy
#import datetime

#create variables and objective function instances
obj = obj_mod.objective()
var = obj_mod.Variables()


#set the value of any parameters
obj.params.is_optimize = False  #re-optimize field geometry?
obj.params.solar_resource_file = '../../scripts/USA CA Daggett (TMY2).csv'
obj.params.ampl_data_dir = "../../scripts/ampl/"  
obj.params.is_ampl_engine = False
obj.params.is_dispatch = True
obj.params.print_messages = False
obj.params.check_max_flux = True

obj.params.degr_per_hour = 1.e-7
obj.params.soil_per_hour = 1.e-4
obj.params.helio_mtf = 20000

#set the value of any variables
var.D_rec.value =  27.991
var.rec_height.value =  24.472
var.N_panels.value =  14
var.P_ref.value =  142.256
var.h_tower.value =  235.003
var.degr_replace_limit.value =  0.771
var.design_eff.value =  0.4456
var.dni_des.value =  746.201
var.n_wash_crews.value = 3 #5 
var.om_staff.value = 5 #20 
var.solarm.value =  4.100
var.tshours.value =  15.391

#var.D_rec.value =  15.
#var.rec_height.value =  18.
#var.N_panels.value =  16
#var.P_ref.value =  65.
#var.h_tower.value =  150.
#var.degr_replace_limit.value =  0.771
#var.design_eff.value =  0.42
#var.dni_des.value =  950.
#var.n_wash_crews.value = 3 #5 
#var.om_staff.value = 5 #20 
#var.solarm.value =  1.5
#var.tshours.value =  6.0


if not var.validate():
    raise Exception("Validation error")

#>>> --------- ... we can run the entire process with just Z -------

##run everything at once
#drets = obj.D(var)
#drets = json.load(open("design.txt",'r'))

#z = obj.Z(var, printout=True)    # Run full simulation

#clusters = obj.setup_clusters(Nclusters = 40)    
#z = obj.Z(var, D=drets, cluster_inputs=clusters, printout=True)   # Run simulation from pre-computed clusters

#z = obj.Z(var, D=drets, Nclusters=40, printout=True)  # Create clusters and run simulation

##>>> ---- or we can either break up the calls as follows... ------------

#true if we need to generate a new field
new_design = True
#new_design = False

if new_design:
    #run the design module
    D = obj.D(var)  #updates self.design
    json.dump(D, open("design.txt",'w'))
        
    #Heliostat availability and OM
    M = obj.M(var, D['design'])
    json.dump(M, open("solar.txt",'w'))
    
    #Heliostat soiling and degradation
    O = obj.O(var, D['design'])
    json.dump(O, open("optical.txt",'w'))

else:
    #load existing from JSON
    D = json.load(open("design.txt",'r'))
    M = json.load(open("solar.txt",'r'))
    O = json.load(open("optical.txt",'r'))


asc = M.pop('avail_sched')
ss = O.pop("soiling_sched")
ds = O.pop("degradation_sched")
#calculate explicit terms
E = obj.E(var)

#run a simulation
#S = obj.S(D['design'], var, sf_avail=asc, sf_soil=ss, sf_degr=ds)   # Run full simulation

#run a simulation with clustering
#clusters = obj.setup_clusters(Nclusters = 40)    
#S = obj.S(D['design'], var, sf_avail=asc, sf_soil=ss, sf_degr=ds, cluster_inputs = clusters)   # Run simulation from pre-computed clusters 

S = obj.S(D['design'], var, sf_avail=asc, sf_soil=ss, sf_degr=ds, Nclusters = 20)   # Create clusters and run simulation
json.dump(S, open("simulation.txt",'w'))
#S = json.load(open("simulation.txt",'r'))

#evaluate the objective based on these results
Z = obj.Z(var, D=D, M=M, O=O, S=S, E=E, printout=True)
#print Z[1]['ppa']
    
