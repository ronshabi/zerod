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
    "-O1"
    "-fsanitize=address"
)

target_link_options(zerod PRIVATE
    "-fsanitize=address"
)
