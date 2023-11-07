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

  std::cout << "Running dummy fluid solver with preCICE config file \"" << configFileName << "\" and participant name \"" << solverName << "\".\n";

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
  std::vector<double> windSpeed(3);
  
  // set initial wind speed (u, v, w)
  windSpeed = {9, 0, 0};
  
  // Blade 1
  std::cout << "Meshing blade 1 \n";
  bladeRoot    = {-5.0, 1.5, 89.5};
  bladeTip     = {-7.7, 62.9, 88.7};
  
  for (int i = 0; i < 306; i++) {
    for (int j = 0; j < dimensions; j++) {
    // interpolate position in x, y and z
      nodePosition[j] = bladeRoot[j] + (bladeTip[j] - bladeRoot[j]) * (i/307);
      vertices.at(j + dimensions * i)  = nodePosition[j];
      writeData.at(j + dimensions * i) = windSpeed[j];
    }
  }
  
  // Blade 2
  std::cout << "Meshing blade 2 \n";
  bladeRoot    = {-5.0, -0.7, 90.8};
  bladeTip     = {-7.7, -30.5, 144.4};
  
  for (int i = 306; i < 615; i++) {
    for (int j = 0; j < dimensions; j++) {
      nodePosition[j] = bladeRoot[j] + (bladeTip[j] - bladeRoot[j]) * ((i-307)/307);
      vertices.at(j + dimensions * i)  = nodePosition[j];
      writeData.at(j + dimensions * i) = windSpeed[j];
    }
  }
  
  // Blade 3
  std::cout << "Meshing blade 3 \n";
  bladeRoot    = {-5.0, -0.7, 88.2};
  bladeTip     = {-7.7, -32.1, 35.4};
  
  for (int i = 615; i < 921; i++) {
    for (int j = 0; j < dimensions; j++) {
      nodePosition[j] = bladeRoot[j] + (bladeTip[j] - bladeRoot[j]) * ((i-614)/307);
      vertices.at(j + dimensions * i)  = nodePosition[j];
      writeData.at(j + dimensions * i) = windSpeed[j];
    }
  }

  interface.setMeshVertices(meshID, numberOfVertices, vertices.data(), vertexIDs.data());

  double dt = interface.initialize();
  
  if (interface.isActionRequired(actionWriteInitialData())) {
      interface.writeBlockVectorData(writeDataID, numberOfVertices, vertexIDs.data(), writeData.data());    
      interface.markActionFulfilled(actionWriteInitialData());
  }
  
  interface.initializeData();
  
  std::cout << "Force in node four of blade 1: " + std::to_string(readData[9]) + "   " + std::to_string(readData[10]) + "   " + std::to_string(readData[11]) + "\n";
  
  while (interface.isCouplingOngoing()) {

    if (interface.isActionRequired(actionWriteIterationCheckpoint())) {
      std::cout << "DUMMY: Writing iteration checkpoint\n";
      interface.markActionFulfilled(actionWriteIterationCheckpoint());
    }

    if (interface.isReadDataAvailable()) {
      interface.readBlockVectorData(readDataID, numberOfVertices, vertexIDs.data(), readData.data());
    }
    std::cout << "Force in node four of blade 1: " + std::to_string(readData[9]) + "   " + std::to_string(readData[10]) + "   " + std::to_string(readData[11]) + "\n";
    
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
