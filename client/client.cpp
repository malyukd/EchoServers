#include "client.h"
#include <iostream>

int Client::check_protocol(char *buff, ssize_t buf_size)
{
    try
    {
        json j = json::parse(buff);
        if (!strcmp(j["protocol"].get<std::string>().c_str(), "io_uring"))
        {
            memset(buff, 0, buf_size);
            strncpy(buff, j["message"].get<std::string>().c_str(), buf_size-1);
            buff[buf_size - 1] = '\0';
            return 1;
        }
        else
        {
            return -1;
        }
    }
    catch (std::exception& e)
    {
        return -1;
    }
}


int Client::loop_client()
{
    printf("loop started\n");
    string message;
    char buffer[1024];
    ssize_t len;

    while (true)
    {
        cout << "Enter message (or 'exit' to quit): ";
        getline(cin, message);
        json j;
        j["protocol"]= this->protocol;        
        j["message"] = message;
        message = j.dump(4);

        if (message == "exit")
        {
            break;
        }

        ssize_t sent = write(sockfd, message.c_str(), message.size());
        if (sent <= 0)
        {
            perror("write");
            break;
        }

        len = read(sockfd, buffer, sizeof(buffer) - 1);
        if (len > 0)
        {
            buffer[len] = 0;
            if(check_protocol(buffer, len)==1){
                printf("Received: %s\n", buffer);
            }else{
                printf("incorrect protocol\n");
                break;
            }
            
        }
        else if (len == 0)
        {
            printf("Server closed connection\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }

    close(sockfd);
    printf("Connection closed\n");
    return 1;
}