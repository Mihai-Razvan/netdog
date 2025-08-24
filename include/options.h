#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

typedef struct {
    bool is_server;
    int port;
    char *command;
    char *ip_address;
    char *hostname;
} netdog_options;

#endif //OPTIONS_H
