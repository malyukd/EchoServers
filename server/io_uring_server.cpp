#include "io_uring_server.h"
#include <iostream>

int IO_uring_server::init_server()
{
    int ret = io_uring_queue_init_params(4, &ring, &params);
    io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, connector,
                         NULL,
                         NULL, 0);
    // sqe->rw_flags = SOCK_NONBLOCK;
    Operation *op = new Operation();
    op->type = Operation::Type::ACCEPT;
    op->fd = connector;
    io_uring_sqe_set_data(sqe, op);
    io_uring_submit(&ring);

    printf("server initialized\n");
    return 1;
}

int IO_uring_server::prep_read(int fd)
{
    Operation *read_op = new Operation();
    read_op->size = 0;
    io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    read_op->type = Operation::Type::READ;
    read_op->fd = fd;
    sqe->rw_flags = SOCK_NONBLOCK;
    io_uring_prep_recv(sqe, fd, read_op->buff, read_op->BUFFER_SIZE, 0);
    io_uring_sqe_set_data(sqe, read_op);
    return 1;
}

int IO_uring_server::prep_write(int fd, char *buff, ssize_t buff_size)
{
    Operation *write_op = new Operation();
    write_op->size = 0;
    buff[buff_size] = 0;
    printf("recived: %s\n", buff);
    io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    write_op->type = Operation::Type::WRITE;
    write_op->fd = fd;
    write_op->size = buff_size;
    memcpy(write_op->buff, buff, write_op->size);
    write_op->buff[buff_size] = 0;
    io_uring_prep_send(sqe, write_op->fd, write_op->buff, write_op->size, 0);
    io_uring_sqe_set_data(sqe, write_op);
    return 1;
}

int IO_uring_server::prep_accept()
{
    Operation *accept_op = new Operation();
    accept_op->size = 0;
    io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, connector,
                         NULL,
                         NULL, 0);
    sqe->rw_flags = SOCK_NONBLOCK;
    accept_op->type = Operation::Type::ACCEPT;
    accept_op->fd = connector;
    io_uring_sqe_set_data(sqe, accept_op);
    return 1;
}

int IO_uring_server::loop_server()
{
    printf("loop started\n");
    while (true)
    {
        int count = io_uring_peek_batch_cqe(&ring, cqes, 50);
        for (int i = 0; i < count; i++)
        {
            io_uring_cqe *cqe = cqes[i];
            Operation *op = static_cast<Operation *>(io_uring_cqe_get_data(cqe));
            op->size = 0;

            if (!op)
            {
                io_uring_cqe_seen(&ring, cqe);
                continue;
            }

            if (cqe->res >= 0)
            {
                if (op->type == Operation::Type::ACCEPT)
                {
                    int new_fd = cqe->res;
                    if (new_fd > 0)
                    {
                        printf("new connection\n");
                        prep_read(new_fd);
                        prep_accept();
                        delete op;
                    }
                    else
                    {
                        perror("accept");
                    }
                }
                else if (op->type == Operation::Type::READ)
                {
                    op->size = cqe->res;
                    printf("new message\n");
                    if (op->size > 0)
                    {
                        printf("new message\n");
                        prep_write(op->fd, op->buff, op->size);
                    }
                    // else{
                    //     delete op;
                    //     close(fd);
                    //     printf("connection closed\n");
                    // }
                    delete op;
                }
                else if (op->type == Operation::Type::WRITE)
                {
                    printf("sent: %s\n", op->buff);
                    prep_read(op->fd);
                }
                // else{
                //     close(fd);
                //     delete op;
                //     printf("connection closed\n");
                // }
            }
            // else{
            //     close(fd);
            //     delete op;
            //     printf("connection closed\n");
            // }

            io_uring_cqe_seen(&ring, cqe);
        }
        io_uring_submit(&ring);
    }
    return 1;
}