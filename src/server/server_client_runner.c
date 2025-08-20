#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>

#include "options.h"
#include "server/server_client_runner.h"

#define READ_BUFFER_SIZE 16384

extern netdog_options netdog_opt;

void handle_client(void *attr) {
    server_client_thread_info* client_thread_info = attr;
    int clientfd = client_thread_info->fd;
    struct sockaddr_in *client_addr = client_thread_info->addr;
    free(attr);

    int server_thread_infd = -1;
    int server_thread_outfd = STDOUT_FILENO;

    if (netdog_opt.command != NULL) {
        execute_command(&server_thread_infd, &server_thread_outfd);
    }

    // This is freed in the handle_writing_thread method
    server_client_communication_info* write_com_info = malloc(sizeof(server_client_communication_info));
    write_com_info->communicationfd = server_thread_infd;
    write_com_info->socketfd = clientfd;
    write_com_info->client_addr = client_addr;
    pthread_t write_thread;
    pthread_create(&write_thread, NULL, handle_writing_thread, write_com_info);
    pthread_detach(write_thread);

    // This is freed in the handle_reading_thread method
    server_client_communication_info* read_com_info = malloc(sizeof(server_client_communication_info));
    read_com_info->communicationfd = server_thread_outfd;
    read_com_info->socketfd = clientfd;
    read_com_info->client_addr = client_addr;
    pthread_t read_thread;
    pthread_create(&read_thread, NULL, handle_reading_thread, read_com_info);
    pthread_detach(read_thread);
}

static void handle_writing_thread(void *attr) {
    server_client_communication_info* read_com_info = attr;
    int server_thread_infd = read_com_info->communicationfd;
    int clientfd = read_com_info->socketfd;
    struct sockaddr_in *client_addr  = read_com_info->client_addr;
    free(attr);

    char *buffer = malloc(READ_BUFFER_SIZE);

    while (1) {
        if (server_thread_infd < 0) {
            continue;
        }

        ssize_t read_bytes = read(server_thread_infd, buffer, READ_BUFFER_SIZE);
        ssize_t sent_bytes = sendto(clientfd, buffer, read_bytes, 0, (struct sockaddr*) client_addr, sizeof(struct sockaddr_in));
        if (sent_bytes < 0) { // It means the socket was closed, so communication ended
            break;
        }
    }

    free(buffer);
}

static void handle_reading_thread(void *attr) {
    server_client_communication_info* read_com_info = attr;
    int server_thread_outfd = read_com_info->communicationfd;
    int clientfd = read_com_info->socketfd;
    free(attr);

    char *buffer = malloc(READ_BUFFER_SIZE);

    while (1) {
        ssize_t read_bytes = recv(clientfd, buffer, READ_BUFFER_SIZE, 0);
        if (read_bytes < 0) {
            printf("Failed to read from buffer for client fd %d\n", clientfd);
            break;
        }

        // The client closed the connection
        if (read_bytes <= 0) {
            break;
        }

        write(server_thread_outfd, buffer, read_bytes);
    }

    close(clientfd);
    free(buffer);
    printf("Closed connection for client fd %d\n", clientfd);
}

static void execute_command(int* server_thread_infd, int* server_thread_outfd) {
    // Parent = server process --- Child = process executing given command
    int in_pipe[2]; // Parent reads, child writes
    int out_pipe[2]; // Parent writes, child reads
    if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0) {
        // handle error
    }

    pid_t pid = fork();
    if (pid < 0) {
         printf("Failed to fork\n");
    }

    if (pid == 0) {
        execute_command_child(in_pipe, out_pipe);
    } else {
        execute_command_parent(server_thread_infd, server_thread_outfd, in_pipe, out_pipe);
   //     wait(&pid);
    }
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