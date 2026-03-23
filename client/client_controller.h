#pragma once

#include "client.h"

class Client_controller
{
public:
    Client_controller(){};
    Client *create_client(char ip[50], int port, char protocol[50]){
        return new Client(ip, port, protocol);
    };
    
};