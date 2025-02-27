# - Config file for the Drogon package
# It defines the following variables
#  DROGON_INCLUDE_DIRS - include directories for Drogon
#  DROGON_LIBRARIES    - libraries to link against
#  DROGON_EXECUTABLE   - the drogon_ctl executable
#  Drogon_FOUND
# This module defines the following IMPORTED target:
# Drogon::Drogon


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was DrogonConfig.cmake.in                            ########

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

find_dependency(Jsoncpp REQUIRED)
find_dependency(Trantor REQUIRED)
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" AND NOT WIN32)
find_dependency(UUID REQUIRED)
endif(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" AND NOT WIN32)
find_dependency(ZLIB REQUIRED)
if()
find_dependency(PostgreSQL)
endif()
if(TRUE)
find_dependency(SQLite3)
endif()
if()
find_dependency(MySQL)
endif()
if()
find_dependency(Boost)
find_package(Boost COMPONENTS filesystem system)
endif()
if(TRUE)
find_dependency(Brotli)
endif()
if()
find_dependency(coz-profiler)
endif()
if()
find_dependency(hiredis CONFIG)
endif()
if(0)
find_dependency(yaml-cpp)
endif()
if(OFF)
find_dependency(Threads)
endif()
if(ON)
find_dependency(Filesystem)
find_package(Filesystem COMPONENTS Final REQUIRED)
endif()


# Our library dependencies (contains definitions for IMPORTED targets)

get_filename_component(DROGON_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
if(NOT TARGET Drogon::Drogon)
  include("${DROGON_CMAKE_DIR}/DrogonTargets.cmake")
  include("${DROGON_CMAKE_DIR}/DrogonUtilities.cmake")
  include("${DROGON_CMAKE_DIR}/ParseAndAddDrogonTests.cmake")
endif()

get_target_property(DROGON_INCLUDE_DIRS Drogon::Drogon INTERFACE_INCLUDE_DIRECTORIES)
set(DROGON_LIBRARIES Drogon::Drogon)
set(DROGON_EXECUTABLE drogon_ctl)
