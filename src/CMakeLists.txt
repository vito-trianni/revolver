find_package(MPI REQUIRED)

add_definitions(${MPI_CXX_COMPILE_FLAGS})
include_directories(${MPI_CXX_INCLUDE_PATH})

set(revolver_IMPLEMENTATIONS
  revolver_main.cpp
  evolution.cpp
  sequential_evolution.cpp
  parallel_evolution.cpp
  population.cpp
  soo_population.cpp
#  moo_population.cpp
  selection_strategy.cpp
  roulette_wheel_selection.cpp
  rank_selection.cpp
  mulambda_selection.cpp
  evaluation_strategy.cpp
  clonal_evaluation.cpp
  aclonal_evaluation.cpp
  genotype.cpp
  )

add_executable(revolver ${revolver_IMPLEMENTATIONS})
target_link_libraries(revolver
#  shark
  evolutionary_lf
  evolutionary_controller
  ${MPI_CXX_LINK_FLAGS}
  ${MPI_CXX_LIBRARIES}
  argos3core_simulator
  argos3plugin_simulator_actuators
  argos3plugin_simulator_dynamics2d
  argos3plugin_simulator_entities
  # argos3plugin_simulator_epuck
  argos3plugin_simulator_footbot
  argos3plugin_simulator_genericrobot
  argos3plugin_simulator_media
  argos3plugin_simulator_qtopengl
  argos3plugin_simulator_sensors
  )

add_definitions(${MPI_CXX_COMPILE_FLAGS})
include_directories(${MPI_CXX_INCLUDE_PATH})

set(invalid_IMPLEMENTATIONS
  invalid_main.cpp 
  )

add_executable(invalid ${invalid_IMPLEMENTATIONS})
target_link_libraries(invalid
  evolutionary_lf
  evolutionary_controller
  utility
  ${MPI_CXX_LINK_FLAGS}
  ${MPI_CXX_LIBRARIES}
  argos3core_simulator
  argos3plugin_simulator_actuators
  argos3plugin_simulator_dynamics2d
  argos3plugin_simulator_entities
  # argos3plugin_simulator_epuck
  argos3plugin_simulator_footbot
  argos3plugin_simulator_genericrobot
  argos3plugin_simulator_media
  argos3plugin_simulator_qtopengl
  argos3plugin_simulator_sensors
  )



