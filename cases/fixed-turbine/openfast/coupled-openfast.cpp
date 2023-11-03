// ** LICENSE NOTICE **
// This file is based on the script "FAST_Prog.cpp" distributed with OpenFAST v3.5.0 under a Apache 2.0 license.
// The following functions were taken from previous work: checkFileExists(), readTurbineData(), readInputFile()
// The following functions were adapted or extended: main()
// The original code can be found here: https://github.com/OpenFAST/openfast/tree/v3.5.0/glue-codes/openfast-cpp/src/FAST_Prog.cpp

#include "/home/leonard/Tools/openfast/install/include/OpenFAST.H"
#include "/usr/local/include/yaml-cpp/yaml.h"
#include <iostream>
#include <mpi.h> 
#include <sstream>
#include "/usr/include/precice/SolverInterface.hpp"


// --------------- Helper functions to read input files for OpenFAST -----------------------

inline bool checkFileExists(const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

void readTurbineData(int iTurb, fast::fastInputs & fi, YAML::Node turbNode) {
    //Read turbine data for a given turbine using the YAML node
    fi.globTurbineData[iTurb].TurbID = turbNode["turb_id"].as<int>();
    fi.globTurbineData[iTurb].FASTInputFileName = turbNode["FAST_input_filename"].as<std::string>();
    fi.globTurbineData[iTurb].FASTRestartFileName = turbNode["restart_filename"].as<std::string>();
    if (turbNode["turbine_base_pos"].IsSequence() ) {
        fi.globTurbineData[iTurb].TurbineBasePos = turbNode["turbine_base_pos"].as<std::vector<double> >();
    }
    if (turbNode["turbine_hub_pos"].IsSequence() ) {
        fi.globTurbineData[iTurb].TurbineHubPos = turbNode["turbine_hub_pos"].as<std::vector<double> >();
    }
    fi.globTurbineData[iTurb].numForcePtsBlade = turbNode["num_force_pts_blade"].as<int>();
    fi.globTurbineData[iTurb].numForcePtsTwr = turbNode["num_force_pts_tower"].as<int>();
    if (turbNode["nacelle_cd"]) fi.globTurbineData[iTurb].nacelle_cd = turbNode["nacelle_cd"].as<float>();
    if (turbNode["nacelle_area"]) fi.globTurbineData[iTurb].nacelle_area = turbNode["nacelle_area"].as<float>();
    if (turbNode["air_density"]) fi.globTurbineData[iTurb].air_density = turbNode["air_density"].as<float>();
}

void readInputFile(fast::fastInputs & fi, std::string cInterfaceInputFile, double * tEnd) {

    fi.comm = MPI_COMM_WORLD;

    // Check if the input file exists and read it
    if ( checkFileExists(cInterfaceInputFile) ) {

        YAML::Node cDriverInp = YAML::LoadFile(cInterfaceInputFile);

        fi.nTurbinesGlob = cDriverInp["nTurbinesGlob"].as<int>();

        if (fi.nTurbinesGlob > 0) {

            if(cDriverInp["dryRun"]) {
                fi.dryRun = cDriverInp["dryRun"].as<bool>();
            } 

            if(cDriverInp["debug"]) {
                fi.debug = cDriverInp["debug"].as<bool>();
            } 

            if(cDriverInp["simStart"]) {
                if (cDriverInp["simStart"].as<std::string>() == "init") {
                    fi.simStart = fast::init;
                } else if(cDriverInp["simStart"].as<std::string>() == "trueRestart") {
                    fi.simStart = fast::trueRestart;
                } else if(cDriverInp["simStart"].as<std::string>() == "restartDriverInitFAST") {
                    fi.simStart = fast::restartDriverInitFAST;
                } else {
                    throw std::runtime_error("simStart is not well defined in the input file");
                }
            }

            fi.tStart = cDriverInp["tStart"].as<double>();
            *tEnd = cDriverInp["tEnd"].as<double>();
            fi.nEveryCheckPoint = cDriverInp["nEveryCheckPoint"].as<int>();
            fi.dtFAST = cDriverInp["dtFAST"].as<double>();
            fi.tMax = cDriverInp["tMax"].as<double>(); // tMax is the total duration to which you want to run FAST. This should be the same or greater than the max time given in the FAST fst file. Choose this carefully as FAST writes the output file only at this point if you choose the binary file output.

            if(cDriverInp["superController"]) {
                fi.scStatus = cDriverInp["superController"].as<bool>();
                fi.scLibFile = cDriverInp["scLibFile"].as<std::string>();
            }

            fi.globTurbineData.resize(fi.nTurbinesGlob);
            for (int iTurb=0; iTurb < fi.nTurbinesGlob; iTurb++) {
                if (cDriverInp["Turbine" + std::to_string(iTurb)]) {
                    readTurbineData(iTurb, fi, cDriverInp["Turbine" + std::to_string(iTurb)] );
                } else {
                    throw std::runtime_error("Node for Turbine" + std::to_string(iTurb) + " not present in input file or I cannot read it");
                }
            }

        } else {
            throw std::runtime_error("Number of turbines <= 0 ");
        }

    } else {
        throw std::runtime_error("Input file " + cInterfaceInputFile + " does not exist or I cannot access it");
    }
}


// --------------- main function ------------------------------------------------------
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: ./coupled-openfast preciceConfigFile cDriverFile\n\n";
        std::cout << "Parameter description\n";
        std::cout << "  preciceConfigFile: Path and filename of preCICE configuration (.xml)\n";
        std::cout << "  cDriverFile: Path and filename of OpenFAST C++ configuration (.i)\n\n\n";
    }
    
    
    // --------------- Initialize OpenFAST ------------------------------------------------------
    
    int iErr;
    int nProcs ;
    int rank;
    std::vector<double> point (3, 0.0); 
    double time_cp;
    
    // variables for data exchange
    std::vector<double> force(3, 0.0);
    std::vector<double> coords(3);
    std::vector<double> velocity(3, 10.0);
    int iNode = 0;
    int iTurb = 0;
    
    // preCICE initializes MPI too --> does preCICE check if MPI already running? No error so far
    iErr = MPI_Init(NULL,NULL);
    //iErr = MPI_Comm_size( MPI_COMM_WORLD, &nProcs);
    //iErr = MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    
    // preCICE controls the time now
    double tEnd ; // This doesn't belong in the FAST - C++ interface 
    //int ntEnd ; // This doesn't belong in the FAST - C++ interface
    

    std::string cDriverInputFile=argv[2];
    fast::OpenFAST FAST;
    fast::fastInputs fi ;
    try {
        readInputFile(fi, cDriverInputFile, &tEnd);
    } catch( const std::runtime_error & ex) {
        std::cerr << ex.what() << std::endl ;
        std::cerr << "Program quitting now" << std::endl ;
        return 1;
    }

    // Calculate the last time step
    //ntEnd = tEnd/fi.dtFAST;

    FAST.setInputs(fi);
    FAST.allocateTurbinesToProcsSimple(); 
    // Or allocate turbines to procs by calling "setTurbineProcNo(iTurbGlob, procId)" for turbine.

    FAST.init();
    
    if (FAST.isTimeZero()) FAST.solution0();
    
    // --------------- Initialize preCICE ------------------------------------------------------

    using namespace precice;
    using namespace precice::constants;
    
    int commRank = 0;
    int commSize = 1;

    std::string configFileName  = argv[1];
    
    std::string solverName      = "Solid";
    std::string meshName        = "Solid-Mesh";
    std::string dataWriteName   = "Force";
    std::string dataReadName    = "Velocity";

    std::cout << "DUMMY: Running OpenFAST dummy with preCICE config file \"" << configFileName << "\" and participant name \"" << solverName << "\".\n";

    SolverInterface interface(solverName, configFileName, commRank, commSize);
    
    // --------------- Set up the mesh ------------------------------------------------------

    int meshID           = interface.getMeshID(meshName);
    int dimensions       = interface.getDimensions();
    int numberOfVertices = FAST.get_numForcePts(iTurb);

    const int readDataID  = interface.getDataID(dataReadName, meshID);
    const int writeDataID = interface.getDataID(dataWriteName, meshID);

    std::vector<double> readData(numberOfVertices * dimensions);
    std::vector<double> writeData(numberOfVertices * dimensions);
    std::vector<double> vertices(numberOfVertices * dimensions);
    std::vector<int>    vertexIDs(numberOfVertices);
    
    //Initialize mesh with data from FAST
    for (int i = 0; i < numberOfVertices; i++) {
      for (int j = 0; j < dimensions; j++) {
        // positions
        FAST.getForceNodeCoordinates(coords, i, iTurb);
        vertices.at(j + dimensions * i)  = coords[j];
        // velocity - How to initialize? Data should come from Fluid Participant. Maybe initialize via preCICE, but probably later in the code
        readData.at(j + dimensions * i)  = 0.0;
        // force
        FAST.getForce(force, i, iTurb);
        writeData.at(j + dimensions * i) = force[j];
      }
    }

    interface.setMeshVertices(meshID, numberOfVertices, vertices.data(), vertexIDs.data());

    double dt = interface.initialize();
    
    double time = 0.0;

    
    // --------------- Main loop ------------------------------------------------------
    while (interface.isCouplingOngoing()) {

        if (interface.isActionRequired(actionWriteIterationCheckpoint())) {
          std::cout << "Writing iteration checkpoint\n";
          time_cp = time;
          interface.markActionFulfilled(actionWriteIterationCheckpoint());
        }
        
        
        // read data
        if (interface.isReadDataAvailable()) {
          interface.readBlockVectorData(readDataID, numberOfVertices, vertexIDs.data(), readData.data());
        }
        
        // set data in FAST
        //FAST.setVelocity(...);
        
        // there are no velocity points defined on the turbine. maybe this is the problem?
        // Do I have to initialize the velocity and force nodes, eg with coordinates and values?
        // This would make sense and could explain why retrieving values from the nodes is currently failing
        std::cout << std::to_string(FAST.get_numVelPts(iTurb)) + "\n";
        
        //for(int i=0; i < FAST.get_numVelPts(iTurb); i++) {
          // Get velocity node co-ordinates at time step 'n+1'
          //FAST.getVelNodeCoordinates(currentCoords, i, iTurb);
          //Sample velocity from CFD solver at currentCoords into sampleVel here
          // Set velocity at the velocity nodes at time step 'n+1'
          //FAST.setVelocity(sampleVel, i, iTurb);
          //std::cout << std::to_string(i) + "\n";
        //}
        
        std::cout << "Velocity to FAST:\n";
        for (int i = 0; i < 3; i++) { 
          std::cout << std::to_string(velocity[i]) + "\n";
        }
   
        
        // calculate next time step
        FAST.step();
        
        
        for(int i=0; i < FAST.get_numForcePts(iTurb); i++) {
         // Get actuator node co-ordinates at time step 'n+1'
         //FAST.getForceNodeCoordinates(currentCoords, i, iTurb);
         //Move the actuator point to this co-ordinate if nece++++~+*ssary
         // Get force at actuator node at time step 'n+1'
         //FAST.getForce(currentForce, i, iTurb);
         
         }
        
        
        
        
        
        std::cout << "Number of actuator points for Force: " + std::to_string(FAST.get_numForcePts(iTurb)) + "\n";
        // get data from FAST
        //FAST.getForceNodeCoordinates(point, iNode, iTurb);
        FAST.getForce(force, 9, iTurb);
        //FAST.getChord(5, iTurb);
        //std::cout << "Coords from FAST:\n";
        //for (int i = 0; i < 3; i++) { 
        //  std::cout << std::to_string(currentCoords[i]) + "\n";
        //}
        
        std::cout << "Force from FAST:\n";
        for (int i = 0; i < 3; i++) { 
          std::cout << std::to_string(force[i]) + "\n";
        }

        // write data
        for (int i = 0; i < numberOfVertices * dimensions; i++) {
          writeData.at(i) = readData.at(i) + 1;
        }
        
        
        if (interface.isWriteDataRequired(dt)) {
          interface.writeBlockVectorData(writeDataID, numberOfVertices, vertexIDs.data(), writeData.data());
        }

        // advance the simulation
        dt = interface.advance(dt);

        if (interface.isActionRequired(actionReadIterationCheckpoint())) {
          std::cout << "Reading iteration checkpoint\n";
          time = time_cp;
          interface.markActionFulfilled(actionReadIterationCheckpoint());
        } else {
          std::cout << "Advancing in time\n";
          time = time + dt;
        }
    }
    
    std::cerr << "Close FAST" << std::endl ;
    FAST.end() ;
    interface.finalize();

    return 0;

}
