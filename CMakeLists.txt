cmake_minimum_required(VERSION 3.20)
project(GimuServer)

set(CMAKE_CXX_STANDARD 17)

set(SERVER_VERSION_REV 1) # Increment this when a new release happens
set(SERVER_YEAR "2022-2024")
set(SERVER_ISSUE_URL "https://www.github.com/decompfrontier/server/issues/")
set(SERVER_HOME_PAGE "https://decompfrontier.github.io/")

find_package(cryptopp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_subdirectory(gimuserver)
add_subdirectory(standalone_frontend)
add_subdirectory(game_frontend)
