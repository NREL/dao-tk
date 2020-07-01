# -*- coding: utf-8 -*-
"""
Replicate Case studies developed in AMPL by John Cox
"""
import dispatch_model
import dispatch_params


cases = {'Yu':    [ 5, 10, 20, 10, 10, 10],
         'Yu0': [ 0,  0,  0,  0,  5, 10],
         'Yd':    [10, 10, 10,  5, 10, 20],
         'Yd0': [ 0,  5, 10,  0,  0,  0],
         'y0':    [ 0,  0,  0,  1,  1,  1],
         'wdot0': [0]*3 + [22000]*3}

def runCase(case_idx):
    params = dispatch_params.buildParamsFromAMPLFile("./input_files/data_energy.dat")
    include = {"pv":False,"battery":False,"persistence":False,"force_cycle":True}
    rt = dispatch_model.RealTimeDispatchModel(params,include)
    rt.model.Yu = cases['Yu'][case_idx]
    rt.model.Yu0 = cases['Yu0'][case_idx]
    rt.model.Yd = cases['Yd'][case_idx]
    rt.model.Yd0 = cases['Yd0'][case_idx]
    rt.model.y0 = cases['y0'][case_idx]
    rt.model.wdot0 = cases['wdot0'][case_idx]
    rt.model.Qrsb = 10
    rt.model.Qb = 10
    results = rt.solveModel()
    rt.printCycleOutput()
    return rt.model.OBJ.expr()
    
objs = []
for i in range(1,2):
    objs.append(runCase(i))