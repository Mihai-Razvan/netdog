#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <netinet/in.h>

#include "utils/comm_utils.h"
#include "options.h"

#define SUCCESS 0
#define ERROR (-1)

extern netdog_options netdog_opt;

int compute_server_port() {
    int port = netdog_opt.port != 0 ? netdog_opt.port : 8080;
    return htons(port);
}

int set_fd_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        verbose_message("Failed to get fd flags for fd %d\n", fd);
        return ERROR;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
        verbose_message("Failed to set the fd as non-blocking for fd %d\n", fd);
        return ERROR;
    }

    return SUCCESS;
}

void verbose_message(const char *format, ...) {
    if (netdog_opt.verbose) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}