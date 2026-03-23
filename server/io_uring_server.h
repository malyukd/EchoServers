#pragma once

#include "server.h"
#include <liburing.h>

struct Operation
{
        enum class Type
        {
                ACCEPT,
                READ,
                WRITE
        };
        Type type;
        int fd;
        size_t size;
        const static int BUFFER_SIZE = 100;
        char buff[BUFFER_SIZE];
};

class IO_uring_server : public Server
{
        int connector;
        io_uring_params params = {0};
        io_uring ring;
        io_uring_cqe *cqes[50];

public:
        IO_uring_server(char ip[50], int port) { connector = create_connector(ip, port); }
        int init_server() override;
        int loop_server() override;
        int check_protocol(char *buff, ssize_t buf_size) override;
        int prep_read(int fd);
        int prep_write(int fd, char *buff, ssize_t buff_size);
        int prep_accept();
        ~IO_uring_server() { close(connector); };
};