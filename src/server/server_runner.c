#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "server/server_runner.h"
#include "server/server_client_runner.h"

void wait_connections(int server_socketfd) {
    while (1) {
        socklen_t sockaddr_size = sizeof(struct sockaddr_in);

        struct sockaddr_in client_addr = { 0 };
        int clientfd = accept(server_socketfd, (struct sockaddr*) &client_addr, &sockaddr_size);

        if (clientfd < 0) {
            // handle error
            printf("Failed to create client fd");
        }

        initiate_client(clientfd);
    }
}

static void initiate_client(int clientfd) {
    printf("Client connection received. Client fd: %d\n", clientfd);

    int *clientfd_copy = malloc(sizeof(int));
    *clientfd_copy = clientfd;
    pthread_t *client_thread = malloc(sizeof(pthread_t));
    pthread_create(client_thread, NULL, handle_client, clientfd_copy);
    pthread_detach(*client_thread);

    free(client_thread);
}