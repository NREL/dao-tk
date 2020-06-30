# -*- coding: utf-8 -*-
"""
Data structures for the real-time dispatch model
"""

import pyomo.environ as pe
#import re



def buildParamsFromAMPLFile(filename):
    params_dict = {}
    f = open(filename, 'r')
    lines = f.readlines()
    in_param = False
    for line in lines:
        if line.startswith("let"):
            in_param = False
            splitline = line.split(" ")
            key = splitline[1]
            if key in ["T","day_of_year"]:
                val = int(splitline[-1][:-2])
            else:
                val = float(splitline[-1][:-2])
            params_dict[key] = val
            in_param = False
        elif line.startswith("param "):  #If parameter header line, start
            in_param = True
            splitline = line.split(" ")
            param_key = splitline[1]
            params_dict[param_key] = {}
        elif in_param:
            if line[0] == ";":
                in_param = False
                param_key = None
            else: 
                splitline = line.split("\t")
                params_dict[param_key][int(splitline[0])] = float(splitline[-1])
        
    return params_dict

if __name__ == "__main__":
    filename = "./input_files/data_energy.dat"
    params_dict = buildParamsFromAMPLFile(filename)
    print("T: ",params_dict["T"])
    print("eff_frac: \n",params_dict["eff_frac"])
    print("wnet_lim_min: ",params_dict["wnet_lim_min"])
            
            