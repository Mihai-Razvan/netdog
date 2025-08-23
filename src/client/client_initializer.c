#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "utils/comm_utils.h"
#include "client/client_initializer.h"
#include "options.h"

extern netdog_options netdog_opt;

int initialize_client() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0) {
        printf("Failed to create client fd\n");
        exit(1);
    }

    struct sockaddr_in server_addr = { 0 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = compute_server_port();

    if (connect(socketfd, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in))) {
        printf("Failed to connect client\n");
        exit(1);
    }

    return socketfd;
}