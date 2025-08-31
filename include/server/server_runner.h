#ifndef SERVER_RUNNER_H
#define SERVER_RUNNER_H

void wait_tcp_connections(int server_socketfd);
void wait_udp_connections(int server_socketfd);
static void create_client_thread(int clientfd, struct sockaddr_in*);

#endif //SERVER_RUNNER_H
