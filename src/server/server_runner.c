#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "server/server_runner.h"
#include "options.h"
#include "server/server_client_runner.h"

extern netdog_options netdog_opt;

void wait_connections(int server_socketfd) {
    while (1) {
        socklen_t sockaddr_size = sizeof(struct sockaddr_in);

        struct sockaddr_in* client_addr = malloc(sizeof(struct sockaddr_in));
        int clientfd = accept(server_socketfd, (struct sockaddr*) client_addr, &sockaddr_size);

        if (clientfd < 0) {
            // handle error
            printf("Failed to create client fd");
        }

        create_client_thread(clientfd, client_addr);
    }
}

static void create_client_thread(int clientfd, struct sockaddr_in* client_addr) {
    printf("Client connection received. Client fd: %d\n", clientfd);

    // This is freed in the handle_client method
    server_client_thread_info* thread_client_info = malloc(sizeof(server_client_thread_info));
    thread_client_info->fd = clientfd;
    thread_client_info->addr = client_addr;

    pthread_t client_thread;
    pthread_create(&client_thread, NULL, handle_client, thread_client_info);
    pthread_detach(client_thread);
}