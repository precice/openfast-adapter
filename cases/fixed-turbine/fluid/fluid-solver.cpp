#include <iostream>
#include <sstream>
#include "/usr/include/precice/SolverInterface.hpp"

int main(int argc, char **argv)
{
  int commRank = 0;
  int commSize = 1;

  using namespace precice;
  using namespace precice::constants;

  std::string configFileName(argv[1]);
  std::string solverName;
  std::string meshName;
  std::string dataWriteName;
  std::string dataReadName;

  if (argc != 2) {
    std::cout << "Usage: ./fluid-solver configFile\n\n";
    std::cout << "Parameter description\n";
    std::cout << "  configurationFile: Path and filename of preCICE configuration\n";
    return 1;
  }

  std::cout << "Running fluid solver with preCICE config file \"" << configFileName << "\" and participant name \"" << solverName << "\".\n";

  solverName    = "Fluid";
  dataWriteName = "Velocity";
  dataReadName  = "Force";   
  meshName      = "Fluid-Mesh";
    
  SolverInterface interface(solverName, configFileName, commRank, commSize);


  int meshID           = interface.getMeshID(meshName);
  int dimensions       = interface.getDimensions();
  int numberOfVertices = 921; // Number of velocity nodes in OpenFAST - 0: hub node 1-922: blade nodes 923: tower base node --> use only blade nodes

  const int readDataID  = interface.getDataID(dataReadName, meshID);
  const int writeDataID = interface.getDataID(dataWriteName, meshID);

  std::vector<double> readData(numberOfVertices * dimensions);
  std::vector<double> writeData(numberOfVertices * dimensions);
  std::vector<double> vertices(numberOfVertices * dimensions);
  std::vector<int>    vertexIDs(numberOfVertices);
  
  // create dummy mesh close to the velocity nodes of OpenFAST
  
  std::vector<double> nodePosition(3);
  std::vector<double> bladeRoot(3);
  std::vector<double> bladeTip(3);
  
  // Blade 1
  std::cout << "Meshing blade 1 \n";
  bladeRoot    = {-5.0, 1.5, 89.5};
  bladeTip     = {-7.7, 62.9, 88.7};
  
  for (int i = 0; i < 307; i++) {
    for (int j = 0; j < dimensions; j++) {
    // interpolate position in x, y and z
      nodePosition[j] = bladeRoot[j] + (bladeTip[j] - bladeRoot[j]) * (i/307);
      vertices.at(j + dimensions * i)  = nodePosition[j];
      readData.at(j + dimensions * i)  = i; // dont care about read data
      
      if (j==0)
      {
        writeData.at(j + dimensions * i) = 9.0; //horizontal wind speed
      } else {
        writeData.at(j + dimensions * i) = 0.0;
      }
    }
  }
  
  // Blade 2
  std::cout << "Meshing blade 2 \n";
  bladeRoot    = {-5.0, -0.7, 90.8};
  bladeTip     = {-7.7, -30.5, 144.4};
  
  for (int i = 307; i < 614; i++) {
    for (int j = 0; j < dimensions; j++) {
      nodePosition[j] = bladeRoot[j] + (bladeTip[j] - bladeRoot[j]) * ((i-307)/307);
      vertices.at(j + dimensions * i)  = nodePosition[j];
      readData.at(j + dimensions * i)  = i; // dont care about read data
      if (j==0)
      {
        writeData.at(j + dimensions * i) = 9.0; //horizontal wind speed
      } else {
        writeData.at(j + dimensions * i) = 0.0;
      }
    }
  }
  
  // Blade 3
  std::cout << "Meshing blade 3 \n";
  bladeRoot    = {-5.0, -0.7, 88.2};
  bladeTip     = {-7.7, -32.1, 35.4};
  
  for (int i = 614; i < 921; i++) {
    for (int j = 0; j < dimensions; j++) {
      nodePosition[j] = bladeRoot[j] + (bladeTip[j] - bladeRoot[j]) * ((i-614)/307);
      vertices.at(j + dimensions * i)  = nodePosition[j];
      readData.at(j + dimensions * i)  = i; // dont care about read data
      if (j==0)
      {
        writeData.at(j + dimensions * i) = 9.0; //horizontal wind speed
      } else {
        writeData.at(j + dimensions * i) = 0.0;
      }
    }
  }

  interface.setMeshVertices(meshID, numberOfVertices, vertices.data(), vertexIDs.data());

  double dt = interface.initialize();

  while (interface.isCouplingOngoing()) {

    if (interface.isActionRequired(actionWriteIterationCheckpoint())) {
      std::cout << "DUMMY: Writing iteration checkpoint\n";
      interface.markActionFulfilled(actionWriteIterationCheckpoint());
    }

    if (interface.isReadDataAvailable()) {
      interface.readBlockVectorData(readDataID, numberOfVertices, vertexIDs.data(), readData.data());
    }

    //for (int i = 0; i < numberOfVertices * dimensions; i++) {
    //  writeData.at(i) = readData.at(i) + 1;
    // }
    
    if (interface.isWriteDataRequired(dt)) {
      interface.writeBlockVectorData(writeDataID, numberOfVertices, vertexIDs.data(), writeData.data());
    }

    dt = interface.advance(dt);

    if (interface.isActionRequired(actionReadIterationCheckpoint())) {
      std::cout << "DUMMY: Reading iteration checkpoint\n";
      interface.markActionFulfilled(actionReadIterationCheckpoint());
    } else {
      std::cout << "Advancing in time\n";
    }
  }

  interface.finalize();
  std::cout << "Closing C++ fluid solver ...\n";

  return 0;
}
