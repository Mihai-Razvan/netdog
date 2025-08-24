cmake_minimum_required(VERSION 3.29)
project(netdog C)

set(CMAKE_C_STANDARD 11)

add_executable(netdog
    src/main.c src/options.c src/cli.c src/interpreter.c
        include/interpreter.h
        include/server/server_initializer.h
        src/server/server_initializer.c
        include/server/server_runner.h
        src/server/server_runner.c
        include/server/server_client_runner.h
        src/server/server_client_runner.c
        include/client/client_initializer.h
        src/client/client_initializer.c
        include/client/client_runner.h
        src/client/client_runner.c
        include/utils/comm_utils.h
        src/utils/comm_utils.c
)

target_include_directories(netdog PRIVATE include)
set_target_properties(netdog PROPERTIES OUTPUT_NAME "nd")