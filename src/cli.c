#include <stdlib.h>
#include <string.h>

#include "utils/status.h"
#include "cli.h"
#include "options.h"

extern netdog_options netdog_opt;

generic_status parseArguments(int argc, char* argv[]) {

    for (int i = 1; i < argc; i+= 2) {
        char *flag = argv[i];

        // Each command has to start with '-'
        if (flag[0] != '-') {
            return (generic_status) { .return_status = 1, .message = "Invalid option flag format" };
        }

        if (i == argc - 1) {
            return (generic_status) { .return_status = 1, .message = "Option must be given a value" };
        }

        if (strcmp(flag, "-p") == 0) {
            int value = atoi(argv[i + 1]);
            netdog_opt.port = value;
        } else {
            return (generic_status) { .return_status = 1, .message = "Invalid option flag" };
        }
    }

    return (generic_status) { .return_status = 0 };
}