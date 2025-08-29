#ifndef CLI_H
#define CLI_H

#include "utils/status.h"

generic_status parse_arguments(int argc, const char* argv[]);
static generic_status parse_single_arguments(const char* argv[], int flag_index);
static generic_status parse_double_arguments(int argc, const char* argv[], int flag_index);
static void print_help();

#endif //CLI_H
