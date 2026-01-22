#include "io_uring_server.h"
#include <iostream>


int IO_uring_server::init_server(){
    int ret = io_uring_queue_init_params(4, &ring, &params);
    io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, connector,
                        NULL,
                        NULL, 0);
    // sqe->rw_flags = SOCK_NONBLOCK;
    Operation* op = new Operation();
    op->type = Operation::Type::ACCEPT;
    op->fd = connector;
    io_uring_sqe_set_data(sqe, op);
    io_uring_submit(&ring);
 
    printf("server initialized\n");
    return 1;
}

int IO_uring_server::loop_server(){
    printf("loop started\n");

    ;
    while(true){
        
        int fd;
        int new_fd;
        int count = io_uring_peek_batch_cqe(&ring, cqes, 50);
        for (int i = 0; i < count; i++) {
            io_uring_cqe* cqe = cqes[i];
            Operation* op = static_cast<Operation*>(io_uring_cqe_get_data(cqe));
            fd = op->fd;

            if (!op) {
                io_uring_cqe_seen(&ring, cqe);
                continue;
            }

            if(cqe->res>=0){
                if(op->type == Operation::Type::ACCEPT){
                    new_fd = cqe->res;
                    if(new_fd>0){
                        Operation* read_op = new Operation();
                        io_uring_sqe *sqe = io_uring_get_sqe(&ring);
                        read_op->type = Operation::Type::READ;
                        read_op->fd = new_fd;
                        io_uring_prep_recv(sqe, new_fd, read_op->buff, 100, 0);
                        io_uring_sqe_set_data(sqe, read_op);
                        printf("new connection\n");
                        Operation* next_accept = new Operation();
                        io_uring_sqe *sqe2 = io_uring_get_sqe(&ring);
                        io_uring_prep_accept(sqe2, connector,
                                            NULL,
                                            NULL, 0);
                        sqe2->rw_flags = SOCK_NONBLOCK;
                        next_accept->type = Operation::Type::ACCEPT;
                        next_accept->fd = connector;
                        io_uring_sqe_set_data(sqe2, next_accept);
                        delete op;
                    }else
                            perror("accept");
                }else if (op->type == Operation::Type::READ){
                    int message_len = cqe->res;
                    printf("new message\n");
                    if(message_len>0){
                        Operation* write_op = new Operation();
                        op->buff[message_len] = 0;
                        printf("recived: %s\n", op->buff);
                        io_uring_sqe *sqe = io_uring_get_sqe(&ring);
                        write_op->type = Operation::Type::WRITE;
                        write_op->fd = op->fd;
                        io_uring_prep_send(sqe, op->fd, op->buff, message_len, 0);
                        io_uring_sqe_set_data(sqe, write_op);
                        delete op;  
                    }
                    // else{
                    //     delete op;  
                    //     close(fd);
                    //     printf("connection closed\n");
                    // }
                }else if (op->type == Operation::Type::READ){
                    Operation* next_read = new Operation();
                    printf("send message\n");
                    printf("sent: %s\n", op->buff);
                    next_read->type = Operation::Type::READ;
                    next_read->fd = fd;
                    io_uring_sqe *sqe = io_uring_get_sqe(&ring);
                    io_uring_prep_recv(sqe, fd, next_read->buff, 100, 0);
                    io_uring_sqe_set_data(sqe, next_read);
                    delete op;
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