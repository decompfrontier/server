cmake_minimum_required(VERSION 3.20)
project(GimuServer)

set(CMAKE_CXX_STANDARD 17)

find_package(cryptopp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)

add_subdirectory(gimuserver)
add_subdirectory(pc_frontend)
add_subdirectory(android_frontend)
