#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;

using namespace std;

class Client
{
    int sockfd;
    char protocol[50];

public:
    Client(char ip[50], int port, char protocol[50])
    {
        strcpy(this->protocol, protocol);
        this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        sockaddr_in server_addr = {0};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
        {
            perror("inet_pton");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("connect");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("Connected to server\n");
    }
    int loop_client();
    int check_protocol(char *buff, ssize_t buf_size);
    ~Client() { close(sockfd); };
};