file(GLOB SRC "*.cpp" "*.hpp")
add_executable(gimuserverw ${SRC})
target_link_libraries(gimuserverw PRIVATE gimuserver)

# The debug CLI (DebugCli.cpp/.hpp) is a local-only dev tool kept out of the
# repo (git-ignored). When it's present, the GLOB above compiles it and we
# define BF_DEBUG_CLI so main.cpp wires it in; upstream builds without the file
# just omit the CLI. Re-run CMake configure after adding/removing the file.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/DebugCli.cpp")
    target_compile_definitions(gimuserverw PRIVATE BF_DEBUG_CLI)
    message(STATUS "standalone_frontend: debug CLI enabled (DebugCli.cpp present)")
endif()

# In Debug builds, bake the absolute path to deploy/config.json into the
# binary so F5 from VS (or any launcher) always finds the right config
# regardless of working directory.  Release builds keep the default
# "./config.json" so the deployed package still works portably.
target_compile_definitions(gimuserverw PRIVATE
    $<$<CONFIG:Debug>:GIMU_DEFAULT_CONFIG_PATH="${CMAKE_SOURCE_DIR}/deploy/config.json">
)
