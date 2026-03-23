cmake_minimum_required(VERSION 3.10)
project(ServerClient CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(server
    server_.cpp
    epoll_server.h
    epoll_server.cpp
    io_uring_server.h
    io_uring_server.cpp
    Server.h
    server_controller.h
    io_uring_server_controller.h
    epoll_server_controller.h
    MetricsCollector.h
    MetricsCollector.cpp
)

target_link_libraries(server nlohmann_json::nlohmann_json)
target_link_libraries(server uring)

