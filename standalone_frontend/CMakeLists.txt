file(GLOB SRC "*.cpp" "*.hpp")
add_executable(gimuserverw ${SRC})
target_link_libraries(gimuserverw PRIVATE gimuserver)

# In Debug builds, bake the absolute path to deploy/config.json into the
# binary so F5 from VS (or any launcher) always finds the right config
# regardless of working directory.  Release builds keep the default
# "./config.json" so the deployed package still works portably.
target_compile_definitions(gimuserverw PRIVATE
    $<$<CONFIG:Debug>:GIMU_DEFAULT_CONFIG_PATH="${CMAKE_SOURCE_DIR}/deploy/config.json">
)
