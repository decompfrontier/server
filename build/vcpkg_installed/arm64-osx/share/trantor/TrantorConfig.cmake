# - Config file for the Trantor package
# It defines the following variables
#  TRANTOR_INCLUDE_DIRS - include directories for Trantor
#  TRANTOR_LIBRARIES    - libraries to link against
#  Trantor_FOUND
# This module defines the following IMPORTED target:
# Trantor::Trantor


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was TrantorConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
if(TRUE)
  find_dependency(OpenSSL)
endif()
if(1)
  find_dependency(c-ares CONFIG)
endif()
find_dependency(Threads)
# Compute paths

# Our library dependencies (contains definitions for IMPORTED targets)
get_filename_component(TRANTOR_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
if(NOT TARGET Trantor::Trantor)
  include("${TRANTOR_CMAKE_DIR}/TrantorTargets.cmake")
endif()

get_target_property(TRANTOR_INCLUDE_DIRS Trantor::Trantor INTERFACE_INCLUDE_DIRECTORIES)
set(TRANTOR_LIBRARIES Trantor::Trantor)
