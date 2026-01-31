#pragma once

#include "server.h"

class Server_controller
{
public:
    Server_controller();
    virtual Server *create_server(char ip[50], int port) = 0;

private:
    ~Server_controller();
    
};