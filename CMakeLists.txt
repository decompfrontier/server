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
# HANDLEF bodies (UserInfo.cpp, GachaAction.cpp) with C4737 -> LNK1257.
# Drop the LTCG flags so Release links cleanly.  We give up cross-TU inlining
# from glaze; the tradeoff is the build actually completing.
if (MSVC AND TARGET glaze::glaze)
    set_property(TARGET glaze::glaze PROPERTY INTERFACE_COMPILE_OPTIONS
        "/Zc:preprocessor;/permissive-;/Zc:lambda")
    set_property(TARGET glaze::glaze PROPERTY INTERFACE_LINK_OPTIONS
        "$<$<CONFIG:Release>:/INCREMENTAL:NO>;$<$<CONFIG:MinSizeRel>:/INCREMENTAL:NO>")
endif()

add_subdirectory(packet-generator/assets/runtime/cpp)

# KDL -> C++ codegen. The packet-generator Rust CLI compiles assets/all.kdl
# into a single flat header (all.hpp) that gimuserver includes. The first
# build compiles the Rust generator itself (one-time, a few minutes); every
# subsequent build only re-runs it when a .kdl file has changed.
find_program(CARGO_EXECUTABLE cargo
    DOC "Rust/Cargo toolchain, required to build packet-generator"
)
if (NOT CARGO_EXECUTABLE)
    message(FATAL_ERROR
        "cargo not found on PATH. Install Rust from https://rustup.rs/ "
        "and re-run CMake configure."
    )
endif()

set(PKGEN_SRC_DIR "${CMAKE_SOURCE_DIR}/packet-generator")
set(PKGEN_ENTRY "assets/all.kdl")
# App.hpp includes <gimuserver/packets/all.hpp>, so emit directly into the
# source tree under gimuserver/packets/. That directory has a .gitignore
# (*.hpp) that keeps the generated header out of version control.
set(PKGEN_OUT_DIR "${CMAKE_SOURCE_DIR}/gimuserver/packets")
set(PKGEN_OUT_HEADER "${PKGEN_OUT_DIR}/all.hpp")

file(GLOB_RECURSE KDL_SCHEMAS CONFIGURE_DEPENDS
    "${PKGEN_SRC_DIR}/assets/*.kdl"
)

file(MAKE_DIRECTORY "${PKGEN_OUT_DIR}")

# The archive schema (assets/archive.kdl) generates a second header consumed
# by gimuserver/archive/*Archiver; gimuserver/archive/.gitignore keeps it out
# of version control just like packets/all.hpp.
set(PKGEN_ARCHIVE_ENTRY "assets/archive.kdl")
set(PKGEN_ARCHIVE_OUT_DIR "${CMAKE_SOURCE_DIR}/gimuserver/archive")
set(PKGEN_ARCHIVE_OUT_HEADER "${PKGEN_ARCHIVE_OUT_DIR}/archive.hpp")

add_custom_command(
    OUTPUT "${PKGEN_OUT_HEADER}" "${PKGEN_ARCHIVE_OUT_HEADER}"
    COMMAND "${CARGO_EXECUTABLE}" run --release --
        generate --cxx --glaze
        -i "${PKGEN_ENTRY}"
        -o "${PKGEN_OUT_DIR}"
    COMMAND "${CARGO_EXECUTABLE}" run --release --
        generate --cxx --glaze
        -i "${PKGEN_ARCHIVE_ENTRY}"
        -o "${PKGEN_ARCHIVE_OUT_DIR}"
    WORKING_DIRECTORY "${PKGEN_SRC_DIR}"
    DEPENDS ${KDL_SCHEMAS}
    COMMENT "Regenerating C++ packet headers from KDL schemas"
    VERBATIM
)

add_custom_target(pkgen_generate ALL
    DEPENDS "${PKGEN_OUT_HEADER}" "${PKGEN_ARCHIVE_OUT_HEADER}"
    SOURCES ${KDL_SCHEMAS}
)

message(STATUS
    "packet-generator: first build compiles the Rust generator (~2-5 min); "
    "subsequent builds only re-run it when .kdl files change."
)

add_subdirectory(gimuserver)

if (STANDALONE)
    add_subdirectory(standalone_frontend)
endif()

if (APPX)
    add_subdirectory(game_frontend)
endif()
