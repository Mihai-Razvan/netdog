#include <stdio.h>

#include "cli.h"
#include "options.h"
#include "utils/status.h"
#include "interpreter.h"

extern netdog_options netdog_opt;

int main(int argc, const char* argv[]) {

    generic_status parse_status = parse_arguments(argc, argv);

    if (parse_status.return_status != 0) {
        printf("Error while parsing arguments: %s\n", parse_status.message);
    }

    printf("Server listening to port %d; command executed %s\n", netdog_opt.port, netdog_opt.command);

    execute_options();
}