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
    } else if (strcmp(flag, "-v") == 0) {
        netdog_opt.verbose = true;
    } else if (strcmp(flag, "-u") == 0) {
        netdog_opt.is_udp = true;
    } else if (strcmp(flag, "-help") == 0) {
        print_help();
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
        netdog_opt.port = atoi(argv[flag_index + 1]);
    } else  if (strcmp(flag, "-e") == 0) {
        netdog_opt.command = argv[flag_index + 1];
    } else if (strcmp(flag, "-a") == 0) {
        netdog_opt.ip_address = argv[flag_index + 1];
    } else if (strcmp(flag, "-h") == 0) {
        netdog_opt.hostname = argv[flag_index + 1];
    } else {
        return (generic_status) { .return_status = PARSING_NO_EVALUATION };
    }

    return (generic_status) { .return_status = PARSING_SUCCESS };
}

static void print_help() {
    printf("\n");
    printf("Usage: nd [options]\n");
    printf("\n");
    printf("Options:\n");
    printf("  -l                 Run in server mode and listen for incoming connections\n");
    printf("  -v                 Enable verbose output\n");
    printf("  -u                 Use UDP sockets\n");
    printf("  -p <port>          Specify the port to listen on (server) or connect to (client)\n");
    printf("  -e <command>       Execute a command upon connection (e.g., /bin/bash)\n");
    printf("  -a <ip_address>    Connect to a specific IP address (client mode)\n");
    printf("  -h <hostname>      Connect to a hostname (client mode)\n");
    printf("  -help              Show the man page\n");
    printf("\n");
}