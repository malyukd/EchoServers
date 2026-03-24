#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "client_controller.h"
#include <thread>
#include <iostream>





int main(int argc, char **argv)
{
    char ip[50] = "0.0.0.0";
    Client_controller client_controller;
    char protocol[50] = "io_uring";
    Client *client = client_controller.create_client(ip, 8888, protocol);
    client->loop_client();
    printf("loop starting\n");
    exit(0);
}