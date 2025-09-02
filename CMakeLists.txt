cmake_minimum_required(VERSION 3.29)
project(netdog C)

set(CMAKE_C_STANDARD 11)

add_executable(netdog
        src/main.c
        src/options.c
        src/cli.c
        src/interpreter.c
        src/server/server_initializer.c
        src/server/server_runner.c
        src/server/server_client_runner.c
        src/client/client_initializer.c
        src/client/client_runner.c
        src/utils/comm_utils.c
)

target_include_directories(netdog PRIVATE include)
set_target_properties(netdog PROPERTIES OUTPUT_NAME "nd")