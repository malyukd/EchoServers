cmake_minimum_required(VERSION 3.10)
project(ServerClient CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(client
    client_.cpp
    client.h
    client_controller.h
    client.cpp
)

target_link_libraries(client nlohmann_json::nlohmann_json)

