#include <sys/socket.h>
#include <arpa/inet.h>

#include "server/server_initializer.h"
#include "options.h"

#define CONNECTION_QUEUE_LIMIT 10

extern netdog_options netdog_opt;

int initialize_server() {
    int server_socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socketfd < 0) {
        // handle error
    }

    struct sockaddr_in server_addr = { 0 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = compute_server_port();

    if (bind(server_socketfd, (struct sockaddr*) &server_addr,
        sizeof(struct sockaddr_in)) < 0) {
        // handle error
    }

    if (listen(server_socketfd, CONNECTION_QUEUE_LIMIT) < 0) {
        // handle error
    }

    return server_socketfd;
}

static int compute_server_port() {
    int port = netdog_opt.port != 0 ? netdog_opt.port : 8080;
    return htons(port);
}