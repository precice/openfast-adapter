# preCICE-Openfast Adapter

OpenFAST[https://openfast.readthedocs.io/en/dev/index.html] is a multi-physics engineering tool for the aero-servo-elastic simulation of wind turbines. It includes modules for the modeling of aerodynamic, structural, and electrical behaviour of on- and offshore wind turbines as well as the control strategy.

This project aims to couple OpenFAST with CFD simulation tools with the coupling library preCICE[https://precice.org/]. To this end, a preCICE-OpenFAST Adapter is being developed. The Adapter calls OpenFAST via the C++ API to drive the simulation. Additionally, it calls the preCICE library to communicate and coordinate with CFD solvers like OpenFOAM.
