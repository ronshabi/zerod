#pragma once

#include <stdio.h>
#include <stdlib.h>

#define VERIFY_NOT_REACHED()                                                   \
    do                                                                         \
    {                                                                          \
        printf("VERIFY_NOT_REACHED: %s\n", __func__);                          \
        __builtin_unreachable();                                               \
    } while (0)

__attribute__((noreturn)) static inline void panic(const char *message)
{
    fprintf(stderr, "[PANIC] %s\n", message);
    exit(1);
}