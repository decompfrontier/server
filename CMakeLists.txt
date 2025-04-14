cmake_minimum_required(VERSION 3.21)
project(GimuServer)

set(CMAKE_CXX_STANDARD 17)

if ("${GIMUSRV_FRONTEND}" STREQUAL "STANDALONE")
    set(STANDALONE TRUE)
elseif ("${GIMUSRV_FRONTEND}" STREQUAL "PROXYAPPX")
    set(APPX TRUE)
else()
    message(FATAL_ERROR "Invalid frontend configuration")
endif()

set(SERVER_VERSION_REV 1) # Increment this when a new release happens
set(SERVER_YEAR "2022-205")
set(SERVER_ISSUE_URL "https://www.github.com/decompfrontier/server/issues/")
set(SERVER_HOME_PAGE "https://decompfrontier.github.io/")

find_package(cryptopp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)

# packet generator import
add_library(pkgenruntime INTERFACE 
    packet-generator/runtime/cpp/date.h
    packet-generator/runtime/cpp/glaze_helpers.hpp
)
target_include_directories(pkgenruntime INTERFACE ${CMAKE_CURRENT_LIST_DIR}/packet-generator/runtime/cpp)

add_subdirectory(gimuserver)

if (STANDALONE)
    add_subdirectory(standalone_frontend)
endif()

if (APPX)
    add_subdirectory(game_frontend)
endif()
