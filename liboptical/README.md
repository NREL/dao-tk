# DAO-Tk Soiling and Degradataion Models (liboptical)

The efficiency of Concentrating Solar Power (CSP) plants depends on the reflectance of the solar field's mirrors, which are negatively impacted by the accumulation of dust and particulate matter over time, or soiling, and permanent damage to the reflective surface, or degradation.  The purpose of the soiling and degradation models in this library is to approximate activity in the optical field that reduces the reflectivity of the mirrors due to these two environmental factors.  

First, using approximations that include a constant soiling and degradation rate per mirror and the expected annual energy output of each heliostat, the model determines the number of wash crews to hire that minimize the costs of capital, materials and labor, plus lost revenues due to soiling and degradation.  Then, a stochastic model steps through time, obtains realizations of soiling and degradation rates at specified intervals, and applies these rates to the heliostats on an hourly basis.  Each wash crew is assigned a subset of the heliostats in the solar field, and in the simulation, rotates through their assigned heliostats, cleaning the mirrors and replacing them as needed.  

The outputs of this model include the operations and maintenance costs associated with the mirrors in the solar field, the number of wash vehicles to procure, and an assignment of heliostats for each wash crew.  Additionally, a schedule of the optical field's relative soiling and degradation loses for each hour are provided. 


## Library Organization

The library's header files are organized as follows: 

### wash_opt_structure.h