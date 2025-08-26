#ifndef COMM_UTILS_H
#define COMM_UTILS_H

int compute_server_port();
int set_fd_nonblocking(int fd);
void verbose_message(const char *format, ...);

#endif //COMM_UTILS_H
