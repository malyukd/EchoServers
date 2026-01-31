#pragma once

#include "server_controller.h"
#include "epoll_server.h"

class Epoll_server_controller
{
public:
    Server *create_server(char ip[50], int port)
    {
        return new Epoll_server(ip, port);
    }
};