# Design, Analysis, and Operations Toolkit (DAO-Tk)

This project develops a toolkit for the design, analysis, and optimization of concentrating solar power 
(CSP) systems, including optimization of both physical design variables and operations and maintenance 
(O&M) activity. The software is derived and actively developed under funding from the U.S. Department of 
Energy -- Energy Efficiency and Renewable Energy grant DE-EE000030338 ("Concurrent optimization of capital 
cost and expected O&M").

This work is the result of a collaboration between researchers and industry, including: 
* National Renewable Energy Laboratory (Prime awardee, Dr. Mike Wagner lead) 
* Colorado School of Mines, Prof. Alexandra Newman 
* Northwestern University, Prof. David Morton 
* Argonne National Laboratory, Dr. Sven Leyffer 
* SolarReserve(TM), Charles Diep and Jolyon Dent

For project documentation, please vist the [DAO-Tk wiki](https://github.com/NREL/dao-tk/wiki).

The desktop version of DAO-Tk for Windows or Linux builds from the following open source projects:

* [wxWidgets](https://www.wxwidgets.org/) is a cross-platform graphical user interface platform used for SAM's user interface, and for the development tools included with SSC (SDKtool) and LK (LKscript). The current version of SAM uses wxWidgets 3.1.1.

* [LK](https://github.com/nrel/lk) is a scripting language that is integrated into SAM and allows users to add functionality to the program.

* [WEX](https://github.com/nrel/wex) is a set of extensions to wxWidgets for custom user-interface elements used by SAM, and by LKscript and DView, which are integrated into SAM.

* [SSC](https://github.com/nrel/ssc) is a set of "compute modules" that simulate different kinds of power systems and financial structures. It can be run directly using the [SAM Software Development Kit](https://sam.nrel.gov/sdk). **If you are looking for the algorithms underlying the models, they are located in this repository.**

* [Google Test](https://github.com/google/googletest) is a C++ test framework that enables comprehensive unit-testing of software.  Contributions to the project will eventually be required to have associated unit tests written in this framework.

* This repository, **DAO-Tk**, provides the user interface to assign values to inputs of the computational modules, run the modules in the correct order, and display calculation results. It also includes tools for editing LK scripts, viewing field layout and receiver flux map data, and performing multi-dimensional system optimization.


## Quick Steps for Building DAO-Tk

For detailed build instructions see the [wiki](https://github.com/NREL/DAO-Tk/wiki), with specific instructions for:

* [Windows](https://github.com/NREL/DAO-Tk/wiki/Windows-Build)
* [Linux](https://github.com/NREL/DAO-Tk/wiki/Linux-Build)

These are the general quick steps you need to follow to set up your computer for developing DAO-Tk:

1. Set up your development tools:

    * Windows: Visual Studio 2017 Community or other editions available [here](https://www.visualstudio.com/).
    * Linux: g++ compiler available [here](http://www.cprogramming.com/g++.html) or as part of the Linux distribution.

2. Download the [wxWidgets 3.1.1 source code](https://www.wxwidgets.org/downloads/) for your operating system.

3. Build wxWidgets.

4. In Windows, create the WXMSW3 environment variable on your computer to point to the wxWidgets installation folder, or Linux, create the dynamic link `/usr/<USERNAME>/local/bin/wx-config-3` to point to `/path/to/wxWidgets/bin/wx-config`.

5. As you did for wxWidgets, for each of the following projects, clone (download) the repository, build the project, and then (Windows only) create an environment variable pointing to the project folder. Build the projects in the following order, and assign the environment variable for each project before you build the next one:

<table>
<tr><th>Project</th><th>Repository URL</th><th>Windows Environment Variable</th></tr>
<tr><td>wxWidgets</td><td>https://www.wxwidgets.org/downloads</td><td>WXMSW3</td></tr>
<tr><td>LK</td><td>https://github.com/NREL/lk</td><td>LKDIR</td></tr>
<tr><td>WEX</td><td>https://github.com/NREL/wex</td><td>WEXDIR</td></tr>
<tr><td>Google Test</td><td>https://github.com/google/googletest</td><td>GTEST</td></tr>
<tr><td>SSC</td><td>https://github.com/NREL/ssc<br><i>use the csp_consolidate branch</i></td><td>SSCDIR</td></tr>
<tr><td>DAO-Tk</td><td>https://github.com/NREL/DAO-Tk</td><td></td></tr>
</table>

## Repository Access

Access is currently limited only to project partners. The project will be made public and open-source at a 
later date. To grant others access, please send a request to [Mike](mailto://mike.wagner@nrel.gov).

## Contributing

Please see the [contributing guidelines](https://github.com/dao-tk/wiki/contributing-guidelines) page for 
specific information on contributing. 

## License

This work is not currently publicly available and should not be disseminated. A license governing usage 
will be developed and posted at a later date. 

## Project organization

The code in this project is organized as follows:

<table>
<tr><th>Folder</th><th>Description</th></tr>
<tr><td>./app</td><td>Source code for the user interface</td></tr>
<tr><td>./build_linux</td><td>Build and makefiles for linux platforms</td></tr>
<tr><td>./build_vs2017</td><td>Build and project files for VisualStudio 2017 (v15)</td></tr>
<tr><td>./deploy</td><td>Files contained in the distribution package</td></tr>
<tr><td>./deploy/samples</td><td>Sample project and script files</td></tr>
<tr><td>./deploy/libraries</td><td>Libraries used by the models</td></tr>
<tr><td>./deploy/site_data</td><td>Site data used by the models</td></tr>
<tr><td>./doc</td><td>Documentation and resources</td></tr>
<tr><td>./libcluster</td><td>Source code for the data clustering model</td></tr>
<tr><td>./libcycle</td><td>Source code for the cycle availability and failure model</td></tr>
<tr><td>./libopt</td><td>Source code and libraries for the nonlinear design optimization problem</td></tr>
<tr><td>./liboptical</td><td>Source code for the optical degradation and soiling model</td></tr>
<tr><td>./libsolar</td><td>Source code for the solar field failure model</td></tr>
</table>
