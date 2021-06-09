cmake_minimum_required(VERSION 3.13)



###############################################################################
## 3RD-PARTY
## dependencies for the project
###############################################################################
project(PelcoDProtocolParserTests LANGUAGES CXX)



###############################################################################
## SETTINGS
## basic 3rd-party settings before use
###############################################################################

# Disable self-overwriting of parameters inside included subdirectories.
SET(${PARENT}_SUBMODULE_CACHE_OVERWRITE OFF CACHE BOOL "" FORCE)



###############################################################################
## CONFIGURATION
## 3rd-party submodules configuration
##############################################################################
SET(${PARENT}_PELCO_D_DEMO_APPLICATION             ON CACHE BOOL "" FORCE)


if (${PARENT}_PELCO_D_DEMO_APPLICATION)
    add_subdirectory(PelcoDDemoApplication)
endif()
