cmake_minimum_required(VERSION 3.13)


###############################################################################
## PROJECT
###############################################################################
project(Pelco_D_Protocol_Parser LANGUAGES CXX)


###############################################################################
## SETTINGS
## basic project settings before use
###############################################################################
# If this project is used as a submodule, the variable should be overridden
# to "OFF" in the top-level application (to disable forced cache rewriting)
option(${PARENT}_SUBMODULE_CACHE_OVERWRITE "Enable forced cache rewriting" ON)
if (${PARENT}_SUBMODULE_CACHE_OVERWRITE)
    SET(REWRITE_FORCE "FORCE")
else()
    SET(REWRITE_FORCE "")
endif()


###############################################################################
## CONFIGURATION
## project configuration
###############################################################################
# API class
SET(${PARENT}_PELCO_D_PROTOCOL_PARSER             ON CACHE BOOL "" ${REWRITE_FORCE})
SET(${PARENT}_PELCO_D_PROTOCOL_PARSER_TESTS       ON CACHE BOOL "" ${REWRITE_FORCE})


## add projects
if (${PARENT}_PELCO_D_PROTOCOL_PARSER)
    add_subdirectory(PelcoDProtocolParser)
endif()

if (${PARENT}_PELCO_D_PROTOCOL_PARSER_TESTS)
    add_subdirectory(PelcoDProtocolParserTests)
endif()

