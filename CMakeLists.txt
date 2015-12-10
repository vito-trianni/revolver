cmake_minimum_required(VERSION 2.8)
project(experiments)

# Set the path where CMake will find additional scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Set the path where CMake will find additional libraries
#SET(CMAKE_C_COMPILER openmpicc)
#SET(CMAKE_CXX_COMPILER openmpicxx)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)

# Find the ARGoS package
find_package(PkgConfig)
# pkg_check_modules(ARGOS REQUIRED argos3_simulator)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

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
# find_package(Lua51 REQUIRED)

# Set ARGoS include dir
# include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${GSL_INCLUDE_DIR} ${LUA_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR} ${GSL_INCLUDE_DIR})

# Descend into the utility directory
add_subdirectory(utility)

# Descend into the argos_utility directory
add_subdirectory(argos_utility)

# Descend into the revolver directory
add_subdirectory(src)

