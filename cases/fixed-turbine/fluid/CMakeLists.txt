cmake_minimum_required(VERSION 3.10.2)
project(fluid-solver LANGUAGES CXX DESCRIPTION "fluid solver dummy")

find_package(precice REQUIRED CONFIG)

add_executable(fluid-solver fluid-solver.cpp)
set_target_properties(fluid-solver PROPERTIES CXX_STANDARD 11)
target_link_libraries(fluid-solver PRIVATE precice::precice)


enable_testing()
add_test(NAME precice.fluid-solver
  COMMAND ${CMAKE_COMMAND}
  -DDUMMY_EXE=$<TARGET_FILE:fluid-solver>
  -DDUMMY_CONFIG=${CMAKE_CURRENT_SOURCE_DIR}/../precice-config.xml
  -V
  -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake
  )
