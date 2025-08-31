#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/comm_utils.h"
#include "server/server_initializer.h"
#include "options.h"

#define CONNECTION_QUEUE_LIMIT 10

extern netdog_options netdog_opt;

int initialize_server() {
    int socket_type = netdog_opt.is_udp ? SOCK_DGRAM : SOCK_STREAM;
    int server_socketfd = socket(AF_INET, socket_type, 0);

    if (server_socketfd < 0) {
        verbose_message("Failed to create server fd\n");
        exit(1);
    }

    struct sockaddr_in server_addr = { 0 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = compute_server_port();

    if (bind(server_socketfd, (struct sockaddr*) &server_addr,
        sizeof(struct sockaddr_in)) < 0) {
        verbose_message("Failed to initialize bind server to port \n");
        exit(1);
    }

    // We need to listen only for TCP connections
    if (socket_type == SOCK_STREAM && listen(server_socketfd, CONNECTION_QUEUE_LIMIT) < 0) {
        verbose_message("Failed to put the server in listening mode\n");
        exit(1);
    }

    verbose_message("Listening to port %d\n", ntohs(server_addr.sin_port));

    return server_socketfd;
}