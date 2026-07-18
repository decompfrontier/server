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

# glaze's vcpkg port pins /GL (whole-program opt) and /LTCG on its imported
# target for Release/MinSizeRel.  Those flags propagate through gimuserver to
# every consumer, and MSVC's LTCG code-gen pass chokes on the coroutine
# HANDLEF bodies (e.g. UserInfo.cpp, GachaAction.cpp) with C4737 -> LNK1257.
# Drop the LTCG flags so Release links cleanly.  We give up cross-TU inlining
# from glaze; the tradeoff is the build actually completing.
if (MSVC AND TARGET glaze::glaze)
    set_property(TARGET glaze::glaze PROPERTY INTERFACE_COMPILE_OPTIONS
        "/Zc:preprocessor;/permissive-;/Zc:lambda")
    set_property(TARGET glaze::glaze PROPERTY INTERFACE_LINK_OPTIONS
        "$<$<CONFIG:Release>:/INCREMENTAL:NO>;$<$<CONFIG:MinSizeRel>:/INCREMENTAL:NO>")
endif()

add_subdirectory(packet-generator/assets/runtime/cpp)
add_subdirectory(gimuserver)

if (STANDALONE)
    add_subdirectory(standalone_frontend)
endif()

if (APPX)
    add_subdirectory(game_frontend)
endif()
