# -*- coding: utf-8 -*-
"""
Real-time dispatch optimization model output processing module
"""
import pyomo.environ as pe
import numpy

class RTDispatchOutputs(object):
    def __init__(self, model):
        self.objective_value = pe.value(model.OBJ)
        self.cycle_on = numpy.array([pe.value(model.y[t]) for t in model.T])
        self.cycle_standby = numpy.array([pe.value(model.ycsb[t]) for t in model.T])
        self.cycle_startup = numpy.array([pe.value(model.ycsu[t]) for t in model.T])
        self.receiver_on = numpy.array([pe.value(model.yr[t]) for t in model.T])
        self.receiver_startup = numpy.array([pe.value(model.yrsu[t]) for t in model.T])
        self.receiver_power = numpy.array([pe.value(model.xr[t]) for t in model.T])
        self.thermal_input_to_cycle = numpy.array([pe.value(model.x[t]) for t in model.T])
        self.electrical_output_from_cycle = numpy.array([pe.value(model.wdot[t]) for t in model.T])
        self.tes_soc = numpy.array([pe.value(model.s[t]) for t in model.T])
        
    def print_outputs(self):
        print(self.objective_value)
        print(self.cycle_on)
        print(self.cycle_standby)
        print(self.cycle_startup)
        print(self.receiver_on)
        print(self.receiver_startup)
        print(self.thermal_input_to_cycle)
        print(self.electrical_output_from_cycle)
        print(self.tes_soc)

