#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#include "options.h"
#include "utils/comm_utils.h"
#include "server/server_client_runner.h"

#define BUFFER_SIZE 16384
#define SUCCESS 0
#define ERROR (-1)

extern netdog_options netdog_opt;

void handle_client(void *attr) {
    server_client_thread_info* client_thread_info = attr;
    int clientfd = client_thread_info->fd;
    free(attr);

    set_fd_nonblocking(clientfd);

    int server_thread_infd = -1;
    int server_thread_outfd = STDOUT_FILENO;

    if (netdog_opt.command != NULL) {
        if (execute_command(&server_thread_infd, &server_thread_outfd) < 0) {
            close_connection(3, (int[]) { clientfd, server_thread_infd, server_thread_outfd }, 0,  (char* []) {});
        }
    }

    if (server_thread_infd >= 0) {
        if (set_fd_nonblocking(server_thread_infd) < 0) {
            close_connection(3, (int[]) { clientfd, server_thread_infd, server_thread_outfd }, 0,  (char* []) {});
        }
    }

    char *buffer = malloc(BUFFER_SIZE);

    while (1) {
        ssize_t read_bytes = read(clientfd, buffer, BUFFER_SIZE);
        if (read_bytes > 0) {
            write(server_thread_outfd, buffer, read_bytes);
        } else if (read_bytes == 0) {
            verbose_message("Connection was closed by client for fd %d\n", clientfd);
            break;
        } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            verbose_message("Failed to read from buffer for client fd %d\n", clientfd);
            break;
        }

        read_bytes = read(server_thread_infd, buffer, BUFFER_SIZE);
        if (read_bytes > 0) {
            ssize_t sent_bytes = write(clientfd, buffer, read_bytes);
            if (sent_bytes < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
                verbose_message("Error when sending bytes to client fd %d\n", clientfd);
                break;
            }
        } else if (read_bytes < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
            verbose_message("Failed to read from buffer for server thread in fd %d\n", server_thread_infd);
            break;
        }
    }

    close_connection(3, (int[]) { clientfd, server_thread_infd, server_thread_outfd }, 1,  (char* []) { buffer });
}

static void close_connection(int fds_count, int fds[], int buffers_count, char* buffers[]) {

    shutdown(fds[0], SHUT_RDWR);
    close(fds[0]);

    for (int i = 1; i < fds_count; i++) {
        close(fds[i]);
    }

    for (int i = 0; i < buffers_count; i++) {
        free(buffers[i]);
    }

    verbose_message("Closed connection for client fd %d\n", fds[0]);
}

static int execute_command(int* server_thread_infd, int* server_thread_outfd) {
    // Parent = server process --- Child = process executing given command
    int in_pipe[2]; // Parent reads, child writes
    int out_pipe[2]; // Parent writes, child reads
    if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0) {
        return ERROR;
    }

    pid_t pid = fork();
    if (pid < 0) {
        return ERROR;
    }

    if (pid == 0) {
        execute_command_child(in_pipe, out_pipe);
    } else {
        execute_command_parent(server_thread_infd, server_thread_outfd, in_pipe, out_pipe);
    }

    return SUCCESS;
}

static void execute_command_parent(int* server_thread_infd, int* server_thread_outfd, int in_pipe[2], int out_pipe[2]) {
    *server_thread_outfd = out_pipe[1];
    *server_thread_infd = in_pipe[0];
}

static void execute_command_child(int in_pipe[2], int out_pipe[2]) {
    dup2(out_pipe[0], STDIN_FILENO);
    dup2(in_pipe[1], STDOUT_FILENO);
    execv(netdog_opt.command, NULL);
}