#ifndef SERVER_RUNNER_H
#define SERVER_RUNNER_H

void wait_connections(int server_socketfd);
static void initiate_client(int clientfd);

#endif //SERVER_RUNNER_H
