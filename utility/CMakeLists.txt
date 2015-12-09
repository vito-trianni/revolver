add_library(utility SHARED
  control_parameters.cpp
  evaluation_config.cpp
)
target_link_libraries(utility
  argos3core_simulator
  argos3plugin_simulator_actuators
  argos3plugin_simulator_footbot
  argos3plugin_simulator_genericrobot
  argos3plugin_simulator_sensors)
