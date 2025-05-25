#include "logging.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "panic.h"

#define TIME_BUFFER_LEN 64
#define TIME_FORMAT     "%T"

static const char *log_level_to_string(enum log_level level)
{
    switch (level)
    {
    case LOG_DEBUG:
        return "DEBUG";
    case LOG_INFO:
        return "INFO";
    case LOG_WARN:
        return "WARN";
    case LOG_ERROR:
        return "ERROR";
    default:
        VERIFY_NOT_REACHED();
    }
}

void log_impl(FILE *f, enum log_level level, const char *ansi_color,
              const char *fmt, va_list ap)
{
    int             rc                           = 0;
    struct timespec tp                           = {0};
    struct tm       tm                           = {0};
    char            time_buffer[TIME_BUFFER_LEN] = {0};

    rc = clock_gettime(CLOCK_REALTIME, &tp);
    if (rc == -1)
    {
        panic("clock_gettime returned an error");
    }

    if (gmtime_r(&tp.tv_sec, &tm) == NULL)
    {
        panic("gmtime_r failed in log function");
    }

    if (strftime(time_buffer, TIME_BUFFER_LEN, TIME_FORMAT, &tm) == 0)
    {
        panic("strftime failed in log function");
    }

    if (ansi_color != NULL)
    {
        fprintf(f, "%s [%s%5s%s] ", time_buffer, ansi_color,
                log_level_to_string(level), ANSI_RESET);
    }
    else
    {
        fprintf(f, "%s [%5s] ", time_buffer, log_level_to_string(level));
    }

    vfprintf(f, fmt, ap);
    fputc('\n', f);
}

void log_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_impl(stderr, LOG_ERROR, ANSI_RED, fmt, ap);
    va_end(ap);
}

void log_info(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_impl(stderr, LOG_INFO, NULL, fmt, ap);
    va_end(ap);
}
void log_warn(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_impl(stderr, LOG_WARN, ANSI_YELLOW, fmt, ap);
    va_end(ap);
}

void log_debug(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_impl(stderr, LOG_DEBUG, ANSI_CYAN, fmt, ap);
    va_end(ap);
}
