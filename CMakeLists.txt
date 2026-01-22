cmake_minimum_required(VERSION 3.10)
project(ServerClient CXX)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_subdirectory(server)
add_subdirectory(client)