cmake_minimum_required(VERSION 3.10)
project(ServerClient CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(server
    server_.cpp
    io_uring_server.h
    io_uring_server.cpp
    Server.h
    MetricsCollector.h
    MetricsCollector.cpp
)

target_link_libraries(server uring)

