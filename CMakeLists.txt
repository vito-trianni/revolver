cmake_minimum_required(VERSION 2.8)
project(experiments)

# Set the path where CMake will find additional scripts
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Set the path where CMake will find additional libraries
#SET(CMAKE_C_COMPILER openmpicc)
#SET(CMAKE_CXX_COMPILER openmpicxx)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)

# Find the ARGoS package
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

# Use c++11 std compiler flag
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# Look for the (optional) galib library
#find_package(GALIB)
#if(GALIB_FOUND)
#  include_directories(${GALIB_INCLUDE_DIRS})
#endif(GALIB_FOUND)

# Find the GSL package
find_package(GSL REQUIRED)

# Find Lua
find_package(Lua51 REQUIRED)

# Set ARGoS include dir
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${GSL_INCLUDE_DIR} ${LUA_INCLUDE_DIR})

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

# Descend into the controllers directory
add_subdirectory(utility)

# Descend into the controllers directory
add_subdirectory(controllers)

# Descend into the loop functions directory
add_subdirectory(loop_functions)

# Descend into the revolver directory
add_subdirectory(revolver)

# Descend into the experiments directory
add_subdirectory(obstacle_avoidance)

# Descend into the experiments directory
add_subdirectory(flocking)

# Descend into the experiments directory
add_subdirectory(stick_pulling)

# Descend into the experiments directory
add_subdirectory(specialisation)


