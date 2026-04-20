file(GLOB_RECURSE SRC "*.cpp" "*.hpp")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${SRC})
add_library(gimuserver STATIC ${SRC})

# The generated header (packets/all.hpp) is produced by pkgen_generate,
# declared at the project root. Everything in gimuserver transitively
# depends on it via App.hpp, so pin the build-order edge here.
add_dependencies(gimuserver pkgen_generate)

target_link_libraries(gimuserver PUBLIC
    # third-party libraries
    Drogon::Drogon
    cryptopp::cryptopp
    unofficial::sqlite3::sqlite3
    glaze::glaze
    # project
    pkgen_cpp
)

target_include_directories(gimuserver
    PUBLIC
        ../
    PRIVATE
        .
)

target_precompile_headers(gimuserver PRIVATE App.hpp)

if (MSVC)
    target_compile_options(gimuserver PUBLIC "/constexpr:steps8388608")
endif()
