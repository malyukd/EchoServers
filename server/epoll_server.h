#include "Server.h"
#include <sys/epoll.h>

class Epoll_server: public Server{
    int connector;
    int epoll_base;
    epoll_event epoll_fds[50];
  

    public:
        Epoll_server(char ip[50], int port){ connector = create_connector(ip, port);}
        int init_server() override;
        int loop_server() override;
        ~Epoll_server(){close(connector);};
};