#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#include "utils/comm_utils.h"
#include "client/client_initializer.h"
#include "options.h"

extern netdog_options netdog_opt;

int initialize_client() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0) {
        verbose_message("Failed to create client fd\n");
        exit(1);
    }

    struct sockaddr_in server_addr = { 0 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = compute_server_port();

    if (netdog_opt.ip_address != NULL) {
        if (inet_pton(AF_INET, netdog_opt.ip_address, &server_addr.sin_addr) <= 0) {
            verbose_message("Invalid ip address\n");
            exit(1);
        }
    }

    if (netdog_opt.hostname != NULL) {
        server_addr = *resolve_hostname();
    }

    if (connect(socketfd, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in))) {
        verbose_message("Failed to connect client\n");
        exit(1);
    }

    verbose_message("Connected to to: %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    return socketfd;
}

struct sockaddr_in* resolve_hostname() {
    char port[20];
    sprintf(port, "%d", netdog_opt.port);
    struct addrinfo* address = NULL;

    if(!getaddrinfo(netdog_opt.hostname, port, 0, &address)) {
        for (struct addrinfo *rp = address; rp != NULL; rp = rp->ai_next) {
            if (rp->ai_family == AF_INET) {
                struct sockaddr_in* addr = (struct sockaddr_in*) rp->ai_addr;
                verbose_message("Resolved hostname to: %s:%d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
                return addr;
            }
        }
    }

    verbose_message("Failed to resolve hostname\n");
    exit(1);
}
