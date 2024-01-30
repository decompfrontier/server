file(GLOB SRC "*.cpp" "*.hpp")
add_library(gimuserver STATIC ${SRC})
target_link_libraries(gimuserver PUBLIC Drogon::Drogon cryptopp::cryptopp)
target_include_directories(gimuserver PUBLIC .)
