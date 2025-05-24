cmake_minimum_required(VERSION 3.31)
project(zerod C)

set(CMAKE_C_STANDARD 99)

add_executable(zerod
        main.c
        server.c
        server.h
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
