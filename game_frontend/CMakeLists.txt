file(GLOB SRC "*.cpp" "*.hpp")
add_library(offlinemod SHARED ${SRC})
target_link_libraries(offlinemod PRIVATE gimuserver)
