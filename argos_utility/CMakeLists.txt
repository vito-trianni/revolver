set(HEADERS_UTILITY_CONFIGURATION_TINYXML
  configuration/tinyxml/ticpp.h
  configuration/tinyxml/ticpprc.h
  configuration/tinyxml/tinystr.h
  configuration/tinyxml/tinyxml.h)

add_library(argos_utility SHARED
  string_utilities.cpp
  logging/argos_log.cpp
  math/rng.cpp
  # plugins/dynamic_loading.cpp
  configuration/command_line_arg_parser.cpp
  ${HEADERS_UTILITY_CONFIGURATION_TINYXML}
  configuration/tinyxml/ticpp.cpp
  configuration/tinyxml/tinystr.cpp
  configuration/tinyxml/tinyxml.cpp
  configuration/tinyxml/tinyxmlerror.cpp
  configuration/tinyxml/tinyxmlparser.cpp
)


TARGET_LINK_LIBRARIES(argos_utility ${GSL_LIBRARIES})