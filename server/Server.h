#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <stdio.h>

class Server{
    public:
        int create_connector(char ip[50], int port){
            in_addr local_ip;
            if (inet_pton(AF_INET, ip, &local_ip)<0) {
                perror("invalid ip");
                return -1;
            }
            short local_port = port;

            sockaddr_in local_addr_in = {0};
            local_addr_in.sin_addr = local_ip;
            local_addr_in.sin_port = htons(local_port);
            local_addr_in.sin_family = AF_INET;
            sockaddr *local_addr = (sockaddr *)&local_addr_in;
            int local_addrlen = sizeof(struct sockaddr_in);

            int connector = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); //NONBLOCK?????
            if (bind(connector, (sockaddr*)&local_addr_in, sizeof(local_addr_in)) < 0) {
                perror("bind failed");
                close(connector);
                return -1;
            }
            printf("binding successful\n");
            
            if (listen(connector, 5) < 0) {
                perror("listen failed");
                close(connector);
                return -1;
            }
            printf("listening\n");
            return connector;  //не забыть закрыть коннектор
        }
        virtual int init_server() = 0;
        virtual int loop_server() = 0;
        

};