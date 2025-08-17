#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "server/server_client_runner.h"

#define READ_BUFFER_SIZE 1024

void * handle_client(void *attr) {
    int clientfd = *(int*) attr;
    free(attr);

    char *buffer = malloc(READ_BUFFER_SIZE);

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

        write(STDOUT_FILENO, buffer, read_bytes);
    }

    close(clientfd);
    free(buffer);
    printf("Closed connection for client fd %d\n", clientfd);
}