# preCICE-Openfast Adapter

⚠️ The state of the adapter is experimental.

[OpenFAST](https://openfast.readthedocs.io/en/dev/index.html) is a multi-physics engineering tool for the aero-servo-elastic simulation of wind turbines. It includes modules for the modeling of aerodynamic, structural, and electrical behaviour of on- and offshore wind turbines.

This project aims to couple OpenFAST with CFD simulation tools with the coupling library [preCICE](https://precice.org/). Therefore, a preCICE-OpenFAST Adapter is being developed. The Adapter calls OpenFAST via the C++ API to drive the turbine simulation. Additionally, it calls the preCICE library to communicate and coordinate with CFD solvers like OpenFOAM.

As a first test case, the Adapter should be able to perform the coupled simulation of a single NREL5MW onshore turbine. OpenFAST calculates the forces and internal state of the turbine, while OpenFOAM is used as CFD solver to compute the inflow (see image).

![img](images/openfast-coupling-scheme.png)
Source: [OpenFAST documentation](https://ganesh-openfast.readthedocs.io/en/latest/_images/actuatorLine_illustrationViz.pdf), visited on 07.11.2023

## Dependencies

- [OpenFAST v3.5.0](https://openfast.readthedocs.io/en/main/source/install/index.html)
- [preCICE v3.0.0](https://precice.org/installation-overview.html) by installation from source from the [develop](https://github.com/precice/precice) branch

> [!NOTE]  
> If you are using a different version of OpenFAST and want to run the provided cases, make sure to check if the OpenFAST input files are [suitable](https://openfast.readthedocs.io/en/main/source/user/api_change.html) for your version

## Run a simulation

- Install the adapter
- Select a [simulation case](https://github.com/LeonardWilleke/openfast-adapter/tree/main/cases)
- Compile the necessary scripts
- Run the coupled simulation

Instructions on how to compile the files and run the simulation are given in each case.

## Installation

Clone the repository and switch to the source directory:

```bash
git clone https://github.com/precice/openfast-adapter.git
cd openfast-adapter/src
```

We use **cmake** for the compilation by invoking a make script:

```bash
bash Allmake
```

The adapter is built locally in the `bin` folder. To make it accessible on the whole system, you need to export the installation path after compilation.

Open the `.bashrc` file

```bash
cd ~
nano .bashrc
```

and add the following line:

```bash
export PATH={path_to_repository}/openfast-adapter/bin:$PATH
```

Now run

```bash
source .basrhc
```

and restart your terminal.

You're good to go! If you want to re-compile the adapter, make sure to clean the old installation first by running `src/Allclean`.

## Known installation issues

The installation may fail to detect `OpenFAST.H`, causing the installation process to end with the error message

```bash
fatal error: OpenFAST.H: No such file or directory
    1 | #include <OpenFAST.H>
```

One solution is to change the include command in line 1 of `openfast-adapter.cpp` and provide the direct path to the `OpenFAST.H` file of your OpenFAST installation:

```bash
#include "/yourPath/openfast/install/include/OpenFAST.H"
```

While in no way perfect, this solution adresses the issue and allows to proceed with the installation.

## References

A more detailed description of the concept behind the adapter can be found in this [report](https://pure.tudelft.nl/ws/portalfiles/portal/175757249/willeke24-openfast-adapter.pdf). Please consider citing the report if you are using the adapter.

## License and attribution

Parts of the code in `src/openfast-adapter` were reused from a [OpenFAST C++ API example](https://github.com/OpenFAST/openfast/tree/v3.5.0/glue-codes/openfast-cpp/src/FAST_Prog.cpp), which is licensed under the [Apache 2 license](https://github.com/precice/openfast-adapter/tree/main/thirdparty).

## Development contribution

The initial version of this adapter was developed by [Leonard Willeke](https://github.com/LeonardWilleke) during his research stay at Delft University of Technology in the group for [Wind Energy](https://www.tudelft.nl/en/ae/organisation/departments/flow-physics-and-technology/wind-energy) in collaboration with Evert Wiegant.
