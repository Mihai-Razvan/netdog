#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils/status.h"
#include "cli.h"
#include "options.h"

#define PARSING_ERROR (-1)
#define PARSING_NO_EVALUATION 1
#define PARSING_SUCCESS 0

extern netdog_options netdog_opt;

generic_status parse_arguments(int argc, const char* argv[]) {

    for (int flag_index = 1; flag_index < argc; flag_index++) {
        const char *flag = argv[flag_index];
        // Each command has to start with '-'
        if (flag[0] != '-') {
            return (generic_status) { .return_status = PARSING_ERROR, .message = "Invalid option flag format" };
        }

        generic_status parse_status = parse_single_arguments(argv, flag_index);

        if (parse_status.return_status == PARSING_NO_EVALUATION) {
            parse_status = parse_double_arguments(argc, argv, flag_index);
            if (parse_status.return_status == PARSING_ERROR) {
                return parse_status;
            }

            flag_index++;
        } else if (parse_status.return_status == PARSING_ERROR) {
            return parse_status;
        }

        if (parse_status.return_status == PARSING_NO_EVALUATION) {
            return (generic_status) { .return_status = PARSING_ERROR, .message = "Invalid option flag" };
        }
    }

    return (generic_status) { .return_status = PARSING_SUCCESS };
}

static generic_status parse_single_arguments(const char* argv[], int flag_index) {
    const char *flag = argv[flag_index];

    if (strcmp(flag, "-l") == 0) {
        netdog_opt.is_server = true;
    } else {
        return (generic_status) { .return_status = PARSING_NO_EVALUATION };
    }

    return (generic_status) { .return_status = PARSING_SUCCESS };
}

static generic_status parse_double_arguments(int argc, const char* argv[], int flag_index) {
    const char *flag = argv[flag_index];

    if (flag_index == argc - 1) {
        return (generic_status) { .return_status = PARSING_ERROR, .message = "Flag must be given a value" };
    }

    if (strcmp(flag, "-p") == 0) {
        int value = atoi(argv[flag_index + 1]);
        netdog_opt.port = value;
    } else  if (strcmp(flag, "-e") == 0) {
        char *value = argv[flag_index + 1];
        netdog_opt.command = value;
    } else {
        return (generic_status) { .return_status = PARSING_NO_EVALUATION };
    }

    return (generic_status) { .return_status = PARSING_SUCCESS };
}