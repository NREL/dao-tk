# DAO-Tk Power Cycle Maintenance and Repair Model Library (libcycle)

This library composes a procedure used to simulate maintenance and component failure events in a Concentrating Solar Power (CSP) production plant, when provided plant specifications and hourly dispatch decisions as input.  In particular, we simulate large component failures and maintenance events, each of which may be prespecified (e.g., a maintenance event occurs for every five thousand hours of operations) or stochastic in nature (e.g., component failure interarrival times for the heat exchanger are exponentially distributed with a mean time of 10,000 hours of use).  These events then serve as inputs to the next iteration of the optimization model in a rolling horizon solution method, i.e.,  a reactive scheduling method that iteratively solves a long-term problem by moving forward the simulation and optimization horizon in each iteration. Our rolling horizon solution method is used to obtain a dispatch solution to year-long problems while accounting for component reliability and maintenance scheduling.  The model may also be used to validate certain inputs to the optimization model, such as the expected cost of a cold start, based on optimized dispatch.

## Library Organization

The library's header files are organized as follows: 

### component.h

Parameters and methods specific to a certain component within the CSP power cycle (e.g., a feedwater heater).  Includes structures for a failure event and a component's status.

### distributions.h

Parameters and methods for each probability distribution we use to generate failure interarrival times via inversion of the cumulative distribution function without dependencies on statistical libraries.     

### failure.h

Members and methods used to describe type of failures that may occur for a given component in the power block.  Members include the operating mode(s) in which a failure may occur, and the probability of failure or lifetime remaining.

### lib_util.h

Contains some dependencies used in SAM, e.g., descriptions of the size_t variable.

### plant_struct.h

Data structures for CSP power cycle parameters, simulation model parameters, and results.

### plant.h

Parameters and methods specific to a the CSP power cycle as a whole (e.g., operating time between maintenance events).

### well512.h

An implementation of the WELL512 pseudo-random number generator we use to generate random variates of the distributions in distributions.h, which, in turn, serve as inputs to the simulation model.

## Inputs

This section defines the sets and inputs that are used for the 

### Sets

| Set Index | Description |
| --- | --- |
| s | Scenarios |
| t | time periods (e.g., hours) |

### Power Cycle Composition Parameters

The component information stores how many of each component type are in the CSP power cycle.  Each component type described here has a collection of failure modes and distributional information on the frequency of failure; we assume that these modes and distributions are fixed, with IEEE and NUREG guidelines as the sources for distributions that govern failure and repair events.  All component parameters are scalars, so no sets are included in the table that follows.

The power cycle composition should be defined by the user, but defaults are included as a guide.

| Parameter | Data Type | Description  | Default Value (if any) | 
| --- | --- | --- | --- |
| num_condenser_trains | Integer | Number of condenser trains in power cycle | 2 |
| fans_per_train | Integer | Number of fans in each condenser train | 30 |
| radiators_per_train | Integer | Number of radiators in each condenser train | 1 |
| num_salt_steam_trains | Integer | Number of salt-to-steam trains in power cycle | 2 |
| num_fwh | Integer | Number of feedwater heaters in power cycle | 6 |
| num_salt_pumps | Integer | Number of molten salt pumps in power cycle | 4 |
| num_salt_pumps_required | Integer | Number of molten salt pumps required to operate power cycle at capacity | 3 |
| num_water_pumps | Integer | Number of water pumps in power cycle | 2 |
| num_turbines |  Integer | Number of turbine-generator shafts in power cycle | 1 |
| condenser_eff_cold | Vector of floating point numbers | Efficiency of condenser according to how many trains are operational for low ambient temperatures | {0,1,1} |
| condenser_eff_hot | Vector of floating point numbers | Efficiency of condenser according to how many trains are operational for high ambient temperatures | {0,0.95,1} |

### Power Cycle Parameters

These plant-level parameters may be user-defined, but have default values as a starting point.

Power cycle parameters are all scalars, so no sets are included in the table that follows.   These can be adjusted via the method "SetPlantAttributes".

| Parameter | Data Type | Description (Units) | Default Value (if any) |
| --- | --- | --- | --- | 
| maintenance_interval	|	Floating point number	|	Operation time before maintenance (h)	| 5,000 | 
| maintenance_duration	|	Floating point number	|	Maintenance event duration (h)	| 168 |
| downtime_threshold	|	Floating point number	|	Length of downtime required to qualify as a cold start (h)	| 24 |
| hours_to_maintenance | Floating point number | Operation duration before the next maintenance event (h) | 5,000 | 
| is_online | Boolean | true if plant is currently online, false otherwise | false |
| is_on_standby | Boolean | true if plant is currently on standby, false otherwise | false |
| power_output | Floating point number | Plant power cycle output at start of simulation (W) | 0 |
| capacity | Floating point number | Upper bound for power cycle output (W) | 500,000 |
| temp_threshold | Floating point number | Threshold for two condenser streams required for cooling the plant (Celsius) | 20 | 
| time_online | Floating point number | Time elapsed since plant was last offline or in standby (h) | 0 |
| time_in_standby | Floating point number | Time elapsed since plant was last offline or generating power (h) | 0 |
| downtime | Floating point number | Time elapsed since plant was last in standby or generating power (h) | 0 |


### Policy parameters

These parameters indicate when the simulation model shuts the power cycle down for unplanned maintenance.  These can be adjusted via the method "SetPlantAttributes".

| Parameter | Data Type | Description (Units) | Default Value (if any) |
| shutdown_capacity | Floating point number | Capacity threshold for power cycle immediate shutdown (fraction) | 0.3 |
| no_restart_capacity | Floating point number | Capacity threshold for maintenance at next power cycle shutdown (fraction) | 0.8 |
| shutdown_efficiency | Floating point number | Efficiency threshold for power cycle immediate shutdown (fraction) | 0.7 |
| no_restart_efficiency | Floating point number | Efficiency threshold for maintenance at next power cycle shutdown (fraction) | 0.9 |

### Simulation Parameters

These are not expected to be user-defined inputs, but are required to run the model.

Cycle simulation parameters are all scalars, so no sets are included in the table that follows.   These can be adjusted via the method "SetSimulationParameters" or by directly adjusting the structure "m_sim_params".

| Parameter | Data Type | Description (Units) | Default (if any) |
| --- | --- | --- | --- | 
| read_periods | Integer | Number of time periods that are read-only | 0 |
| sim_length | Integer | Length of simulation time horizon, in periods | 48 | 
| steplength	|	Floating point number	|	Time period length (h)	| 1 |
| epsilon | Floating point number | Threshold on lifetimes for which a failure occurs | 1e-10 |
| print_output | Boolean | True if information on failure events is printed to the console, false o.w. | false |
| num_scenarios | Integer between 1 and 100 | Number of scenarios in simulation | 1 | 
| hourly_labor_cost | Floating point number | Hourly labor cost for repair of a failed component ($) | 35 |
| stop_at_first_repair | Boolean | True if cycle model terminates after first repair, false o.w. | false |
| stop_at_first_failure | Boolean | True if cycle model terminates after first failure, false o.w. | false |

### Dispatch Parameters

These are the inputs that come from the optimization model and SAM.

| Sets | Parameter | Data Type | Description (Units) |
| --- | --- | --- | --- | 
| t | cycle_power | Floating point number | Cycle power output in time period t (W) |
| t | ambient_temperature | Floating point number | Ambient temperature in time period t (Celsius) | 
| t | standby | Binary | 1 if the power cycle is in standby, 0 otherwise |

## Outputs

The cycle simulation model provides the following outputs: 
 
| Sets | Parameter | Data Type | Description |
| --- | --- | --- | --- | 
| s, t | cycle_capacity | Floating point number | Relative capacity of system due to component failures (fraction of total capacity) |
| s, t | cycle_efficiency | Floating point number | Relative efficiency of system due to component failures (fraction of total capacity) |
| t | avg_cycle_capacity | Floating point number | Effective capacity of system due to component failures (fraction of total capacity) |
| t | avg_cycle_efficiency | Floating point number | Effective capacity of system due to component failures (fraction of total capacity) |
| s | labor_costs | Floating point number | Total labor cost of component repairs for scenario s ($) |
| s | turbine_efficiency | Floating point number | Relative efficiency of system due to turbine aging (fraction of total capacity) |
| s | turbine_capacity | Floating point number | Relative capacity of system due to turbine aging (fraction of total capacity) |
| s | period_of_last_failure | Integer | Time period in which last compnent failure occurred | 
| s | period_of_last_repair | Integer | Time period in which last compnent failure occurred | 
|  | avg_labor_cost | Floating point number | Average total labor cost for component repairs ($) |
|  | avg_turbine_efficiency | Floating point number | Average relative efficiency of system due to turbine aging (fraction of total capacity) |
|  | avg_turbine_capacity | Floating point number | Average relative capacity of system due to turbine aging (fraction of total capacity) |
|  | expected_time_to_failure | Floating point number | Estimated runtime to the next component failure (h) |
|  | expected_starts_to_failure | Floating point number | Estimated number of scycle starts to the next component failure |




