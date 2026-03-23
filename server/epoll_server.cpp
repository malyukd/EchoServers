#include "epoll_server.h"
#include <iostream>

int Epoll_server::check_protocol(char *buff, ssize_t buf_size)
{
    try
    {
        json j = json::parse(buff);
        if (!strcmp(j["protocol"].get<std::string>().c_str(), "epoll"))
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

int Epoll_server::init_server()
{
    epoll_base = epoll_create1(0);
    epoll_fds[0].data.fd = connector;
    epoll_fds[0].events = EPOLLIN;
    epoll_ctl(epoll_base, EPOLL_CTL_ADD, connector, &epoll_fds[0]);
    printf("server initialized\n");
    return 1;
}

int Epoll_server::loop_server(){
    printf("loop started\n");
    while(true){
        int ev_count = epoll_wait(epoll_base, epoll_fds, 50, -1);
        int fd;
        int new_fd;
        epoll_event epoll_fd;
        char message[100];
        ssize_t message_len = 0;
        for(int i=0; i<ev_count; i++){
            if(epoll_fds[i].events & EPOLLIN){
                printf("new event\n");
                fd = epoll_fds[i].data.fd;
                epoll_fd.events = EPOLLIN;
                if(fd==connector){
                    new_fd = accept(connector, 0, 0);
                    if(new_fd>0){
                        epoll_fd.data.fd = new_fd;
                        epoll_fd.events = EPOLLIN;
                        epoll_ctl(epoll_base, EPOLL_CTL_ADD, new_fd, &epoll_fd);
                        printf("new connection\n");
                    }else
                        perror("accept");
                }else{
                    message_len = read(fd, message, sizeof(message) - 1);
                    if(message_len>0){
                        message[message_len] = 0;
                        if (check_protocol(message, message_len)==1)
                        {
                            printf("recived: %s\n", message);
                            write(fd, message, message_len);
                            printf("sent: %s\n", message);
                        }
                        else
                        {
                            close(fd);
                            printf("connection closed\n");
                            epoll_ctl(epoll_base, EPOLL_CTL_DEL, fd, NULL);
                        }
                    }else{
                        close(fd);
                        printf("connection closed\n");
                        epoll_ctl(epoll_base, EPOLL_CTL_DEL, fd, NULL);
                    }
                }
            }else if(epoll_fds[i].events & EPOLLERR || epoll_fds[i].events & EPOLLHUP ){
                close(fd);
                printf("connection closed\n");
                epoll_ctl(epoll_base, EPOLL_CTL_DEL, fd, NULL);
            }
        }
    }
    return 1;
}

