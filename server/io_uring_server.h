#include "Server.h"
#include <liburing.h>

struct Operation {
        enum class Type { ACCEPT, READ, WRITE, TIMEOUT };
        Type type;
        int fd;
        static constexpr size_t BUFFER_SIZE = 100;
        char buff[BUFFER_SIZE];
        size_t data_size; 
};

class IO_uring_server: public Server{
    int connector;
    io_uring_params params = {0};
    io_uring ring;
    io_uring_cqe* cqes[50];

    public:
        IO_uring_server(char ip[50], int port){ connector = create_connector(ip, port);}
        int init_server() override;
        int loop_server() override;
        ~IO_uring_server(){close(connector);};
};