## Setup

This case couples a single NREL5MW turbine in OpenFAST to a dummy turbine in OpenFOAM. The two models don't match in dimension but are used to show the concept.

The fluid solver writes `Velocity` from the flow to the solid solver. Ideally, the solid solver would return the variable `Force`, which is currently not possible with the OpenFOAM adapter. For now, the force data is sent to the Fluid as `Pressure`. This leads to a wrong calculation, but is done to explore the data exchange and mapping between the simulation tools.

An open challenge is the mapping between the volume-based cellSet `turbine` in OpenFOAM and the line-based turbine model in OpenFAST (see pictures below).

## Available solvers

- *openfast*: An OpenFAST simulation of a single NREL 5MW turbine coupled to preCICE by the code `openfast-adapter`. The input files `preciceInput.yaml` and `openfastInput.yaml` specify the simulation case by pointing to other files such as `nrel5mw.fst`.
- *fluid*: A OpenFOAM simulation of the flow field

Remark: For future experiments, the OpenFOAM library [turbinesFoam](https://github.com/turbinesFoam/turbinesFoam) may be interesting to represent wind turbines using the actuator-line method. The necessary system files are included in this setup but not activated.

## Additional Dependencies

- [OpenFOAM v2206](https://www.openfoam.com/news/main-news/openfoam-v2206)
- [preCICE-OpenFOAM](https://precice.org/adapter-openfoam-get.html) adapter by installation from source from the [develop](https://github.com/precice/openfoam-adapter) branch

## Running the Simulation

Open two separate terminals and start both participants by calling:

```bash
cd openfast
./run.sh
```

and

```bash
cd fluid
./run.sh
```

## Visualization

**OpenFOAM**

To visualize the whole flow region, go to the `fluid` Terminal and type:

```bash
paraFoam
```

The aerodynamic effect of the turbine on the flow field becomes visible.

To visualize only the subregion where the turbine data from OpenFAST is applied, use the commands:

```bash
foamToVTK -cellSet turbine
paraFoam
```

Now use Paraview to open the file `turbine.vtm.series` in the `VTK` subfolder. 

The aerodynamic effect of the single blades on the near flow field becomes visible.

![OpenFOAM Turbine mesh with force data](images/openfoam-turbine-mesh.png)

**OpenFAST**

OpenFAST generates VTK files to investigate the turbine mesh. The files are stored in the same folder as the `.fst` file. Starting from the `openfast` terminal, type:

```bash
cd ./nrel5mw
paraview
```

Now use Paraview to open all files in the `vtk` subfolder. You should see the line representation of the turbine as used by the Actuator Line Method.

![OpenFAST Turbine mesh](images/openfast-turbine-mesh.png)
