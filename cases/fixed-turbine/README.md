## Setup

This case couples a single NREL5MW turbine with a fixed rotor to a dummy fluid solver. It is very handy to get an insight into the C++ APIs of preCICE and OpenFAST, as well as for debugging.

## Available solvers

- *openfast*: A first coupled code `coupled-openfast`, which calls both the preCICE and OpenFAST API. The input file `cDriver.i` specifies the OpenFAST `.fst` file and some simulation metadata.
- *fluid*: An example fluid solver which creates a dummy mesh, writes velocity data on it and reads force data from OpenFAST

## Compiling the code

To compile `coupled-openfast` with `cmake`, open a terminal and run:

```bash
cd openfast
cmake .
make
```

Repeat the same procedure for the `fluid-solver`.

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

## License and attribution

Parts of this code were reused from a [OpenFAST C++ API example](https://github.com/OpenFAST/openfast/tree/v3.5.0/glue-codes/openfast-cpp/src/FAST_Prog.cpp), which is licensed under the [Apache 2 license](https://github.com/LeonardWilleke/openfast-adapter/thirdparty/LICENSE.txt).
