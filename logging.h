#pragma once

enum log_level {
    LOG_INFO,
    LOG_ERROR
};

#define ANSI_RESET "\033[0m"
#define ANSI_RED "\033[91m"
#define ANSI_BOLD "\033[1m"

void log_info(const char *fmt, ...);
void log_error(const char *fmt, ...);
