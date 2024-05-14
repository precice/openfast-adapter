cmake_minimum_required(VERSION 3.10.2)
project(openfast-adapter LANGUAGES CXX DESCRIPTION "preCICE Adapter for OpenFAST")

find_package(OpenFAST REQUIRED CONFIG)
find_package(yaml-cpp REQUIRED CONFIG)
find_package(precice REQUIRED CONFIG)

add_executable(openfast-adapter ./openfast-adapter.cpp)
set_target_properties(openfast-adapter PROPERTIES CXX_STANDARD 11)

target_include_directories(openfast-adapter PRIVATE ${OpenFAST_INCLUDE_DIRS})
target_link_libraries(openfast-adapter PRIVATE aerodyn14lib aerodynlib beamdynlib
 elastodynlib extptfm_mckflib feamlib foamfastlib hydrodynlib icedynlib icefloelib
 ifwlib maplib moordynlib nwtclibs openfastcpplib openfastlib openfast_postlib
 openfast_prelib orcaflexlib scfastlib sctypeslib servodynlib subdynlib precice::precice yaml-cpp)
