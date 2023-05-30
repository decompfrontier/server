file(GLOB SRC "*.cpp" "*.hpp")
add_executable(gimuserverw ${SRC})
target_link_libraries(gimuserverw PRIVATE gimuserver)
