# DAO-Tk Soiling and Degradataion Models (liboptical)

The efficiency of Concentrating Solar Power (CSP) plants depends on the reflectance of the solar field's mirrors, which are negatively impacted by the accumulation of dust and particulate matter over time, or soiling, and permanent damage to the reflective surface, or degradation.  The purpose of the soiling and degradation models in this library is to approximate activity in the optical field that reduces the reflectivity of the mirrors due to these two environmental factors.  

First, using approximations that include a constant soiling and degradation rate per mirror and the expected annual energy output of each heliostat, the model determines the number of wash crews to hire that minimize the costs of capital, materials and labor, plus lost revenues due to soiling and degradation.  Then, a stochastic model steps through time, obtains realizations of soiling and degradation rates at specified intervals, and applies these rates to the heliostats on an hourly basis.  Each wash crew is assigned a subset of the heliostats in the solar field, and in the simulation, rotates through their assigned heliostats, cleaning the mirrors and replacing them as needed.  

The outputs of this model include the operations and maintenance costs associated with the mirrors in the solar field, the number of wash vehicles to procure, and an assignment of heliostats for each wash crew.  Additionally, a schedule of the optical field's relative soiling and degradation loses for each hour are provided. 


## Library Organization


The library's header files are organized as follows: 

### optical_degr.h

Contains the optical soiling and degradation simulation model, as well as the method that obtains the optimal heliostat replacement threshold policy for the mirror (or a single value for all heliostats the field).

In what follows, we separate the degradation model from the simulation model, though they exist within the same header file.  

### optical_structures.h

Contains parameters and data structures used in the optical simulation model and additional methods in optical_degr.h. 

### wash_opt.h

Contains methods that create and solve the heliostat washing problem, the solution of which includes the number of wash crews and the allocation of mirrors to each crew.  The solution is passed as input to the simulation model included in optical_degr.h.

### wash_opt_structure.h

Contains paramters and data structures used by the wash crew optimization model in wash_opt.h.


## Sets


The following sets and indices are used in the descriptions of the inputs and outputs for the three models in this library.  

| Set Index | Description |
| --- | --- |
| h | heliostats |
| t | time periods (e.g., hours) |
| c | wash crews |


## Wash Crew Optimization Model


This section defines the inputs and outputs for the wash crew optimization model.

### Inputs

The inputs to the wash crew optimization model include annual energy output estimates for each heliostat in the solar field design, as well as assumptions on costs, plant revenues, wash crew performance, and plant efficiency.  Some are user-defined inputs within the DAO-tk solution, while others come directly from SSC's SolarPILOT module.

| Parameter | Data Type | Description (units)  | Default Value (if any) | 
| --- | --- | --- | --- |
| wash_crew_capital_cost | Floating-point number | Cost of wash crew vehicle ($)                                     | 100,000 |
| hourly_cost_per_crew   | Floating-point number | Labor and meterials cost ($/crew-hr)                              |      50 |
| discount_rate          | Floating-point number | Annual discount rate applied to revenues (fraction)               |    0.06 |
| wash_rate              | Floating-point number | Wash crew's rate of heliostat cleaning (m^2/hr)                   |   3,680 |
| system_efficiency      | Floating-point number | Plant electricity generated / DNI sent to receiver (fraction)     |   0.298 |
| num_years              | Integer               | Plant lifetime (years)                                            |      25 |
| price_per_kwh          | Floating-point number | Plant revenue per kWh generated ($/kWh)                           |   0.137 |
| vehicle_life           | Integer               | Wash vehicle lifetime (years)                                     |      10 |
| use_uniform_assignment | Boolean               | 1 if assigning an equal number of heliostats to each crew, 0 o.w. |       1 |
| soiling_rate           | Floating-point number | Relative soiling rate per day (1/day)                             |   0.003 |
| hours_per_week         | Floating-point number | Wash crew weekly work limit (hours/week)                          |      70 |
| max_num_crews          | Integer               | Maximum number of crews in first dynamic programming iteration    |      40 |
| h | names         | Integer               | Heliostat identifier | |
| h | x_pos         | Floating-point number | Heliostat location on east-west axis relative to receiver   | |
| h | y_pos         | Floating-point number | Heliostat location on north-south axis relative to receiver | |
| h | mirror_output | Floating-point number | Expected annual energy output (kWh)                         | |
|   | helio_height  | Floating-point number | Heliostat surface height (m) | |
|   | helio_width   | Floating-point number | Heliostat surface width (m) | |

### Outputs

The outputs of the wash crew optimization model are used in both the optical degradation and simulation models.  The number of wash crews is used by the broader DAO-tk solution in calculating operations and maintenance costs.

| Sets | Parameter | Data Type | Description (units)  |
| --- | --- | --- | --- |
| c | assignments | Integer | Index of first heliostat in ordered set h for crew c to wash |
|   | num_wash_crews | Integer | Number of wash crews to assign | 


## Degradation Model 


This section defines the inputs and outputs for the heliostat degradation and replacement model. 


### Inputs

Some of the degradation model inputs are common to, or derived from, the inputs to the wash crew optimization models (e.g., soiling rate, wash rate, annual output per heliostat).  Inputs unique to the degradation and simulation models are in the table below.

| Sets | Parameter | Data Type | Description (units)  | Default Value (if any) | 
| --- | --- | --- | --- | --- |
|   | degr_loss_per_hr  | Floating-point number >= 0 | Heliostat reflectivity degradation rate (1/hr)                       | 3.139e-6 |
|   | degr_accel_per_year   | Floating-point number >= 0 | Labor and meterials cost ($/crew-hr)                              |      50 |
|   | use_mean_replacement_threshold | Boolean               | 1 if optimizing a single replacement threshold, 0 o.w. |       0 |
|   | use_fixed_replacement_threshold           |  Boolean               | 1 if using a fixed replacement threshold, 0 o.w. |       0 |
|   | replacement_threshold | Floating-point number  >= 0 | Fixed degradation-based loss threshold for heliostat replacement (fraction, if used) | 0.75 | 
|   | heliostat_refurbish_cost | Floating-point number  | Cost to replace a heliostat mirror ($) | 23.06/m^2 |
| h | mirror_output | Floating-point number | Expected annual energy output (kWh)                         | |
|   | annnual_profit_per_kwh | Floating-point number | Expected revenue per unit energy collected ($/kWh)                         | |

### Outputs

Outputs of the heliostat degradation model are used in the simulation model.

| Sets | Parameter | Data Type | Description (units)  | 
| --- | --- | --- | --- | 
| h  | replacement_threshold | Floating-point number  >= 0 |  degradation-based loss threshold for replacement of heliostat h (fraction, if used) |


## Simulation Model 


This section defines the inputs and outputs for the simulation model, which steps through time, updating the soiling and degradation rates of each mirror at regular intervals which the hired wash crews clean their assigned heliostats.  Field reflectivity characteristics are then calculated. 

### Inputs
 
The inputs for the simulation model compose the outputs of both the wash crew and replacement optimziation models, but also include common inputs with these models, as well as additional unique inputs. 
 
| Sets | Parameter | Data Type | Description (units)  | Default Value (if any) | 
| --- | --- | --- | --- | --- |
|   | n_wash_crews | Integer | Number of wash crews assigned |  |
| c | assignments | Integer | Index of first heliostat in ordered set h for crew c to wash | |
|   | n_hr_sim | Integer | Simulation time horizon (hours)  | 306,600 |
|   | soil_loss_per_hr  | Floating-point number >= 0 | Soiling rate (1/hr)                       | 3.139e-6 |
|   | degr_loss_per_hr  | Floating-point number >= 0 | Heliostat reflectivity degradation rate (1/hr)                       | 3.139e-6 |
|   | degr_accel_per_year   | Floating-point number >= 0 | Labor and meterials cost ($/crew-hr)                              |      50 |
|   |   | hours_per_day          | Floating-point number >= 0 | Wash crew shift length (hrs/day)                                 |    10 |
|   | seed              | Integer  >= 0 | Random number generator starting seed                    |   3,680 |
|   | refl_sim_interval      | Integer  >= 0               | Plant electricity generated / DNI sent to receiver (fraction)     |   0.298 |
|   | soil_sim_interval      | Integer  >= 0              | Plant lifetime (years)                                            |      25 |
| h  | replacement_threshold | Floating-point number  >= 0 |  degradation-based loss threshold for replacement of heliostat h (fraction, if used) |  | 
| h | mirror_output | Floating-point number | Expected annual energy output (kWh)                         | |

### Outputs

The outputs of the simulation model are used to provide optical field performance characteristics and operations and management cost parameters to the broader DAO-tk solution.

| Sets | Parameter | Data Type | Description (units)  | 
| --- | --- | --- | --- | 
|   | n_replacements | Integer | Number of heliostat replacements over simulation time horizon  |
|   | heliostat_refurbish_cost | Floating-point number | NPV of heliostat replacement costs ($)    |
|   | heliostat_refurbish_cost_y1 | Floating-point number | Mean annual cash flow of heliostat replacement costs ($/yr)    |
|   | avg_soil | Floating-point number | Average relative soiling losses in solar field, weighted by heliostat productivity (fraction) |
|   | avg_degr | Floating-point number | Average relative degradataion losses in solar field, weighted by heliostat productivity (fraction) |
| t | soil_schedule | Floating-point number | hourly relative soiling losses in field (fraction) |
| t | degr_schedule | Floating-point number | hourly relative degradation losses in field (fraction) |
| t | repl_schedule | Integer | hourly number of heliostats replaced in solar field |
