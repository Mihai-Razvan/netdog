#include "interpreter.h"
#include "options.h"
#include "server/server_initializer.h"
#include "server/server_runner.h"
#include "stdio.h"

extern netdog_options netdog_opt;

void execute_options() {
    if (netdog_opt.is_server) {
        int server_socketfd = initialize_server();
        wait_connections(server_socketfd);
    } else {

    }
}