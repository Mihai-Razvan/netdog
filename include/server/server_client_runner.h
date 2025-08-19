#ifndef SERVER_CLIENT_RUNNER_H
#define SERVER_CLIENT_RUNNER_H

#include <netinet/in.h>

typedef struct {
    int fd;
    struct sockaddr_in* addr;
} server_client_thread_info;

void * handle_client(void *attr);
static void execute_command(int* server_thread_infd, int* server_thread_outfd);
static void execute_command_parent(int* server_thread_infd, int* server_thread_outfd, int in_pipe[2], int out_pipe[2]);
static void execute_command_child(int in_pipe[2], int out_pipe[2]);

#endif //SERVER_CLIENT_RUNNER_H