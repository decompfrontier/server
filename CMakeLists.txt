cmake_minimum_required(VERSION 4.0)
project(GimuServer)

set(CMAKE_CXX_STANDARD 23)

if ("${GIMUSRV_FRONTEND}" STREQUAL "STANDALONE")
    set(STANDALONE TRUE)
elseif ("${GIMUSRV_FRONTEND}" STREQUAL "PROXYAPPX")
    set(APPX TRUE)
else()
    message(FATAL_ERROR "Invalid frontend configuration")
endif()

if (MSVC)
     # TODO: Enable /Wall, /WX and /permissive-
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /MP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8 /MP")
endif()

find_package(cryptopp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED) # c++ JSON abstraction

add_subdirectory(packet-generator/assets/runtime/cpp)
add_subdirectory(gimuserver)

if (STANDALONE)
    add_subdirectory(standalone_frontend)
endif()

if (APPX)
    add_subdirectory(game_frontend)
endif()
