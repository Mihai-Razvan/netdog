#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h>

#include "options.h"
#include "server/server_client_runner.h"

#define READ_BUFFER_SIZE 1024

extern netdog_options netdog_opt;

void * handle_client(void *attr) {
    server_client_thread_info* client_thread_info = (server_client_thread_info*) attr;
    int clientfd = client_thread_info->fd;
    struct sockaddr_in *client_addr = client_thread_info->addr;
    socklen_t sockaddr_size = sizeof(struct sockaddr_in);
    free(attr);

    char *buffer = malloc(READ_BUFFER_SIZE);
    int server_thread_infd = -1;
    int server_thread_outfd = STDOUT_FILENO;
    execute_command(&server_thread_infd, &server_thread_outfd);

    while (1) {
        ssize_t read_bytes = recv(clientfd, buffer, READ_BUFFER_SIZE, 0);
        if (read_bytes < 0) {
            printf("Failed to read from buffer for client fd %d\n", clientfd);
            break;
        }

        // The client closed the connection
        if (read_bytes == 0) {
            break;
        }

        write(server_thread_outfd, buffer, read_bytes);

        if (server_thread_infd >= 0) {
            read_bytes = read(server_thread_infd, buffer, READ_BUFFER_SIZE);
            sendto(clientfd, buffer, read_bytes, 0, (struct sockaddr*) client_addr, sockaddr_size);
        }
    }

    close(clientfd);
    free(buffer);
    printf("Closed connection for client fd %d\n", clientfd);
}

static void execute_command(int* server_thread_infd, int* server_thread_outfd) {
    if (netdog_opt.command == NULL) {
        return;
    }

    // Parent = server process --- Child = process executing given command
    int in_pipe[2]; // Parent reads, child writes
    int out_pipe[2]; // Parent writes, child reads
    if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0) {
        // handle error
    }

    pid_t pid = fork();
    if (pid < 0) {
        // handle error
    }

    if (pid == 0) {
        execute_command_child(in_pipe, out_pipe);
    } else {
        execute_command_parent(server_thread_infd, server_thread_outfd, in_pipe, out_pipe);
        wait(&pid);
    }
}

static void execute_command_parent(int* server_thread_infd, int* server_thread_outfd, int in_pipe[2], int out_pipe[2]) {
    *server_thread_outfd = out_pipe[1];
    *server_thread_infd = in_pipe[0];
}

static void execute_command_child(int in_pipe[2], int out_pipe[2]) {
    dup2(out_pipe[0], STDIN_FILENO);
    dup2(in_pipe[1], STDOUT_FILENO);
  //  execv(netdog_opt.command, NULL);
    char *args[] = {"/bin/bash", NULL};
    execv("/bin/bash", args);
}