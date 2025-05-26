cmake_minimum_required(VERSION 3.31)
project(zerod C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_COLOR_MAKEFILE ON)

add_executable(zerod
        buffer.c
        buffer.h
        main.c
        server.c
        server.h
        socket.c
        socket.h
        logging.c
        logging.h
        panic.h
)

target_compile_options(zerod PRIVATE
    "-Wall"
    "-Wextra"
    "-Wformat"
    "-g"
    "-O1"
    "-fsanitize=address"
)

target_link_options(zerod PRIVATE
    "-fsanitize=address"
)

enable_testing()
add_executable(test_buffer tests/test_buffer.c buffer.c buffer.h)

target_compile_options(test_buffer PRIVATE
    "-Wall"
    "-Wextra"
    "-Wformat"
    "-g"
    "-O1"
    "-fsanitize=address"
)

target_link_options(test_buffer PRIVATE
    "-fsanitize=address"
)

add_test(NAME test_buffer
        COMMAND test_buffer)

