#ifndef SERVER_RUNNER_H
#define SERVER_RUNNER_H

#include <sys/socket.h>

void wait_connections(int server_socketfd);
static void create_client_thread(int clientfd, struct sockaddr_in*);

#endif //SERVER_RUNNER_H
