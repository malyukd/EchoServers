#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "epoll_server_controller.h"
#include "io_uring_server_controller.h"
#include <thread>
#include <iostream>
#include "MetricsCollector.h"

MetricsCollector g_metrics;
bool program = true;

void console_input()
{
    std::string line;
    while (program)
    {
        if (std::getline(std::cin, line))
        {
            if (line == "metrics")
            {
                auto m = g_metrics.get_metrics();
                std::cout << "RPS: " << m.rps
                          << ", MB/s: " << m.mbps
                          << ", Total: " << m.total_requests << "\n";
            }
            else
            {
                std::cout << "Ты ввёл: " << line << "\n";
            }
        }
    }
}

int main(int argc, char **argv)
{
    char ip[50] = "0.0.0.0";
    Epoll_server_controller epoll_server_controller;
    Server *server = epoll_server_controller.create_server(ip, 8888);
    server->init_server();
    printf("loop starting\n");
    server->loop_server();
    exit(0);
}