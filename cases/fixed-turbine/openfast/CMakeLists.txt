cmake_minimum_required(VERSION 3.10.2)
project(coupled-openfast LANGUAGES CXX DESCRIPTION "Dummy Adapter for OpenFAST")

find_package(OpenFAST REQUIRED CONFIG)
find_package(yaml-cpp REQUIRED CONFIG)
find_package(precice REQUIRED CONFIG)

add_executable(coupled-openfast coupled-openfast.cpp)
set_target_properties(coupled-openfast PROPERTIES CXX_STANDARD 11)
#target_link_libraries(dummy-openfast PRIVATE precice::precice)

target_link_libraries(coupled-openfast PRIVATE aerodyn14lib aerodynlib beamdynlib
 elastodynlib extptfm_mckflib feamlib foamfastlib hydrodynlib icedynlib icefloelib
 ifwlib maplib moordynlib nwtclibs openfastcpplib openfastlib openfast_postlib
 openfast_prelib orcaflexlib scfastlib sctypeslib servodynlib subdynlib precice::precice yaml-cpp)
#target_link_libraries(dummy-openfast yaml-cpp)
