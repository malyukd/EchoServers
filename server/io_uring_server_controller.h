#pragma once

#include "server_controller.h"
#include "io_uring_server.h"

class IO_uring_server_controller
{
public:
    Server *create_server(char ip[50], int port)
    {
        return new IO_uring_server(ip, port);
    }
};