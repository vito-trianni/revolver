find_package(MPI REQUIRED)

add_definitions(${MPI_CXX_COMPILE_FLAGS})
include_directories(${MPI_CXX_INCLUDE_PATH})

set(revolver_IMPLEMENTATIONS
  revolver_main.cpp
  evolution.cpp
  sequential_evolution.cpp
  ../evolutionary_problems/trivial/trivial_evolution.cpp
  parallel_evolution.cpp
  population.cpp
  soo_population.cpp
#  moo_population.cpp
  mating_population.cpp
  selection_strategy.cpp
  roulette_wheel_selection.cpp
  rank_selection.cpp
  mulambda_selection.cpp
  evaluation_strategy.cpp
  clonal_evaluation.cpp
  aclonal_evaluation.cpp
  mating_evaluation.cpp
  )

add_executable(revolver ${revolver_IMPLEMENTATIONS})
target_link_libraries(revolver
   utility
   argos_utility
#  shark
  ${MPI_CXX_LINK_FLAGS}
  ${MPI_CXX_LIBRARIES}
  )

add_definitions(${MPI_CXX_COMPILE_FLAGS})
include_directories(${MPI_CXX_INCLUDE_PATH})

set(trivialMating_IMPLEMENTATIONS
  ../evolutionary_problems/trivial_mating/invalid_main.cpp
  ../evolutionary_problems/trivial_mating/simulator.cpp
)

add_executable(trivialMating ${trivialMating_IMPLEMENTATIONS})
  target_link_libraries(trivialMating
  argos_utility
  utility
  ${MPI_CXX_LINK_FLAGS}
  ${MPI_CXX_LIBRARIES}
)

set(standaloneTrivialMating_IMPLEMENTATIONS
  ../evolutionary_problems/trivial_mating/standalone_main.cpp
  ../evolutionary_problems/trivial_mating/simulator.cpp
)

add_executable(standaloneTrivialMating ${standaloneTrivialMating_IMPLEMENTATIONS})
  target_link_libraries(standaloneTrivialMating
  argos_utility
  utility
)


