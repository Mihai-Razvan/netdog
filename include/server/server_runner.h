#ifndef SERVER_RUNNER_H
#define SERVER_RUNNER_H

#include <sys/socket.h>

void wait_connections(int server_socketfd);
static void initiate_client(int clientfd, struct sockaddr_in*);

#endif //SERVER_RUNNER_H
