file(GLOB SRC "*.cpp" "*.hpp")
add_library(offlinemod SHARED ${SRC})
target_link_libraries(offlinemod PRIVATE gimuserver)

if (APPX)
    target_compile_definitions(offlinemod PRIVATE -D__APPX__)
elseif (ANDROID)
    target_compile_definitions(offlinemod PRIVATE -D__ANDROID__)
endif()
