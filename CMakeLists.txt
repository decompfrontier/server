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

# ─── Portable release staging ─────────────────────────────────────────────────
# `cmake --install <build> --config Release --prefix <dir>` populates <dir>
# with a drag-and-drop server folder:
#   <dir>/gimuserverw.exe
#   <dir>/*.dll              (resolved from gimuserverw's runtime DLL deps)
#   <dir>/config.json        (copied from deploy/)
#   <dir>/system/            (copied from deploy/system/)
# game_content/ and gme.sqlite are intentionally NOT bundled: game_content is
# game-owned static assets the operator drags in separately, and gme.sqlite is
# auto-created by MigrationManager on first run.  PDBs are excluded (the
# release at github.com/Seltraeh/server/releases doesn't ship symbols).
if (STANDALONE)
    install(TARGETS gimuserverw
        RUNTIME DESTINATION .
        COMPONENT bf_server
    )

    # $<TARGET_RUNTIME_DLLS:gimuserverw> only catches DLLs whose imported
    # targets have IMPORTED_LOCATION set on the consumer's link line; it
    # misses transitive deps like brotli/openssl/zlib that Drogon pulls in.
    # Glob whatever applocal.ps1 staged beside the exe instead — vcpkg's
    # post-build step is the source of truth for "DLLs this exe needs at
    # runtime".  Requires the build to have been run in a VC env where
    # dumpbin is on PATH (rebuild_release.bat handles that).
    set(_BF_BUILD_BIN_DIR "${CMAKE_BINARY_DIR}/standalone_frontend")
    install(CODE "
        file(GLOB _bf_release_dlls
            \"${_BF_BUILD_BIN_DIR}/\${CMAKE_INSTALL_CONFIG_NAME}/*.dll\")
        if (NOT _bf_release_dlls)
            message(WARNING
                \"No DLLs found beside gimuserverw.exe in the build dir. The \"
                \"portable release will be missing runtime deps. Usually means \"
                \"applocal.ps1 couldn't run (dumpbin not on PATH during build). \"
                \"Rebuild from rebuild_release.bat or a VS x64 Developer prompt.\")
        endif()
        file(INSTALL \${_bf_release_dlls}
            DESTINATION \"\${CMAKE_INSTALL_PREFIX}\"
            TYPE SHARED_LIBRARY)
    " COMPONENT bf_server)

    install(FILES "${CMAKE_SOURCE_DIR}/deploy/config.json"
        DESTINATION .
        COMPONENT bf_server
    )
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/deploy/system"
        DESTINATION .
        COMPONENT bf_server
    )
endif()
