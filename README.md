# Design, Analysis, and Operations Toolkit (DAO-Tk)

This project develops a toolkit for the design, analysis, and optimization of concentrating solar power 
(CSP) systems, including optimization of both physical design variables and operations and maintenance 
(O&M) activity. The software is derived and actively developed under funding from the U.S. Department of 
Energy -- Energy Efficiency and Renewable Energy grant DE-EE000030338 ("Concurrent optimization of capital 
cost and expected O&M").

This work is the result of a collaboration between researchers and industry, including: 
* National Renewable Energy Laboratory 
* Colorado School of Mines
* Northwestern University

Previous contributors include:
* Argonne National Laboratory

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

| Project | Windows Environment Variable |
|---------|------------------|
| [wxWidgets](https://www.wxwidgets.org/downloads) | WXMSW3 |
| [LK](https://github.com/NREL/lk) | LKDIR |
| [WEX](https://github.com/NREL/wex) | WEXDIR |
| [Google Test](https://github.com/google/googletest) | GTEST |
| [SSC](https://github.com/mjwagner2/ssc) <br><i>use the daotk-develop branch</i> | SSCDIR |
| [DAO-Tk](https://github.com/NREL/DAO-Tk) |  |


6. After successfully building all repositories, copy the SSC shared or DLL object from the SSC build folder into the platform-specific DAO-Tk deploy folder. For example, on Linux:
```
cp ~/projects/ssc/build_linux/ssc.so ~/projects/dao-tk/deploy/linux/ssc.so
```

Windows distributions require the following DLL's (in addition to ssc.dll):
``` dbghelp, libcurl, libeay32, libssh2, msvcp120, msvcr120, ssleay32, vcruntime140 ```
These can be located from system path directories or downloaded from [NREL's FTP server](https://pfs.nrel.gov/main.html?download&weblink=2803bc659530b139621bbacc1b80910f&realfilename=nrelapp-win-dynlibs.zip).

## Repository Access

Access is currently limited only to project partners. To obtain access, please send a request to [Mike](mailto://mike.wagner@nrel.gov).

## Contributing

Please see the [contributing guidelines](https://github.com/dao-tk/wiki/contributing-guidelines) page for 
specific information on contributing. 

## License

DAO-Tk's open source code is copyrighted by the U.S. Department of Energy -- Energy Efficiency and Renewable Energy under a [mixed MIT and GPLv3 license](https://github.com/NREL/dao-tk/blob/develop/LICENSE.md). It allows for-profit and not-for-profit organizations to develop and redistribute software based on DAO-Tk under terms of an MIT license and requires that research entities including national laboratories, colleges and universities, and non-profit organizations make the source code of any redistribution publicly available under terms of a GPLv3 license.

## Project organization

The code in this project is organized as follows:

| Folder				| Description |
|-------------------|------------------------------------|
| ./app | Source code for the user interface and scripting routines |
| ./build\_linux | Build and makefiles for linux platforms |
| ./build\_vs2017 | Build and project files for VisualStudio 2017 (v15) |
| ./deploy | Files contained in the distribution package |
| ./deploy/samples | Sample project and script files |
| ./deploy/rs | Libraries used by the models |
| ./deploy/samples | Sample files and LK scripts |
| ./libclearsky | Source code for clear sky irradiance model |
| ./libcluster | Source code for the data clustering model |
| ./libcycle | Source code for the cycle availability and failure model |
| ./liboptical | Source code for the optical degradation and soiling model |
| ./liboptimize | Source code and libraries for the nonlinear design optimization problem |
| ./libsolar | Source code for the solar field failure model |


## External libraries

DAO-Tk utilizes the following open-source libraries:

| Project | Version | Usage |
|---------|---------|-------|
| [RapidJSON](https://github.com/Tencent/rapidjson)  	| v1.1.0 	| JSON read-write library for project files |
| [Eigen](http://eigen.tuxfamily.org)                 				| v3.3.5 	| Eigen is a C++ template library for linear algebra - matrices, vectors, numerical solvers, and related algorithms. |
| [NLOpt](https://nlopt.readthedocs.io/en/latest/)    		| v2.5   	| NLopt is a free/open-source library for nonlinear optimization, providing a common interface for a number of different free optimization routines available online as well as original implementations of various other algorithms. |
| [lp\_solve](http://lpsolve.sourceforge.net/5.5/)     		| v5.5.2.5 | lp\_solve is a free linear (integer) programming solver based on the revised simplex method and the Branch-and-bound method for the integers. |
