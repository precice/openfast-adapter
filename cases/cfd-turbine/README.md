## Setup

This case couples a single NREL5MW turbine to OpenFOAM

## Available solvers

- *openfast*: A first coupled code `coupled-openfast`, which calls both the preCICE and OpenFAST API. The input file `cDriver.i` specifies the OpenFAST `.fst` file and some simulation metadata.
- *fluid*: A OpenFOAM simulation of the flow field. The turbine is represented via the actuator-line method with the OpenFOAM library `turbinesFoam`.

## Preparing the simulation

To compile `coupled-openfast` with `cmake`, open a terminal and run:

```bash
cd openfast
cmake .
make
```

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

To visualize the whole flow region, go to the `fluid` Terminal and type:

```bash
paraFoam
```

The aerodynamic effect of the turbine on the flow field becomes visible.

To visualize only the subregion where the turbine is modeled, use the commands:

```bash
foamToVTK -cellSet turbine
paraFoam
```

Now use Paraview to open the file `turbine.vtm.series` in the `VTK` subfolder. 

The aerodynamic effect of the single blades on the near flow field becomes visible.

## License and attribution

Parts of this code were reused from a [OpenFAST C++ API example](https://github.com/OpenFAST/openfast/tree/v3.5.0/glue-codes/openfast-cpp/src/FAST_Prog.cpp), which is licensed under the [Apache 2 license](https://github.com/LeonardWilleke/openfast-adapter/thirdparty/LICENSE.txt).
