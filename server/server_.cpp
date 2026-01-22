#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "epoll_server.h"
#include "io_uring_server.h"
#include <thread>
#include <iostream>
#include "MetricsCollector.h"

MetricsCollector g_metrics;
bool program = true;

void console_input() {
    std::string line;
    while (program) {  
        if (std::getline(std::cin, line)) {
            if (line == "metrics") {
                  auto m = g_metrics.get_metrics();
            std::cout << "RPS: " << m.rps
                << ", MB/s: " << m.mbps
                << ", Total: " << m.total_requests << "\n";
            } else {
                std::cout << "Ты ввёл: " << line << "\n";
            }
        }
    }
}

int main(int argc, char** argv)
{
    char ip[50] = "0.0.0.0";
    IO_uring_server a = IO_uring_server(ip, 8888);
    a.init_server();
    printf("loop starting\n");
    a.loop_server();
    exit(0);
}