#include <stdio.h>

#include "cli.h"
#include "options.h"
#include "utils/status.h"

extern netdog_options netdog_opt;

int main(int argc, char* argv[]) {

    generic_status parse_status = parseArguments(argc, argv);

    if (parse_status.return_status != 0) {
        printf("Error while parsing arguments: %s\n", parse_status.message);
    }

    printf("Port %d\n", netdog_opt.port);
}