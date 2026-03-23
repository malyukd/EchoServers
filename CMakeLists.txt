cmake_minimum_required(VERSION 3.10)
project(ServerClient2 CXX)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(nlohmann_json REQUIRED)

include_directories(include)

add_subdirectory(server)
add_subdirectory(client)