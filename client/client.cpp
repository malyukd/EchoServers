#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>       // read, write, close
#include <arpa/inet.h>    // sockaddr_in, inet_pton
#include <sys/socket.h>   // socket, connect
#include <cstring>        // strlen

using namespace std;

int main(int argc, char** argv)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888); // Порт сервера
    

    if (inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    

    printf("Connected to server\n");

    string message;
    char buffer[1024];
    ssize_t len;

    while (true) {
        cout << "Enter message (or 'exit' to quit): ";
        getline(cin, message);
        
        if (message == "exit") {
            break;
        }

        ssize_t sent = write(sockfd, message.c_str(), message.size());
        if (sent <= 0) {
            perror("write");
            break;
        }

        len = read(sockfd, buffer, sizeof(buffer) - 1);
        if (len > 0) {
            buffer[len] = 0;
            printf("Received: %s\n", buffer);
        } else if (len == 0) {
            printf("Server closed connection\n");
            break;
        } else {
            perror("read");
            break;
        }
    }

    close(sockfd);
    printf("Connection closed\n");
    return 0;
}