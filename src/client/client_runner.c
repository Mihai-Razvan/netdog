#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "utils/comm_utils.h"
#include "client/client_runner.h"

#define BUFFER_SIZE 16384
#define SUCCESS 0
#define ERROR (-1)

void run_client(int socketfd) {

    set_fd_nonblocking(socketfd);
    set_fd_nonblocking(STDIN_FILENO);

    char *buffer = malloc(BUFFER_SIZE);

    while (1) {
        ssize_t read_bytes = read(socketfd, buffer, BUFFER_SIZE);
        if (read_bytes > 0) {
            write(STDOUT_FILENO, buffer, read_bytes);
        } else if (read_bytes == 0) {
            printf("Connection with server was closed\n");
            break;
        } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            printf("Failed to read from buffer\n");
            break;
        }

        read_bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        if (read_bytes > 0) {
            ssize_t sent_bytes = write(socketfd, buffer, read_bytes);
            if (sent_bytes < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
                printf("Error when sending bytes to server\n");
                break;
            }
        } else if (read_bytes < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
            printf("Failed to read from terminal\n");
            break;
        }
    }

    shutdown(socketfd, SHUT_RDWR);
    close(socketfd);
    free(buffer);
}