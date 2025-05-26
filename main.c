#include <stdio.h>

#include "logging.h"
#include "server.h"

#include <unistd.h>

int main(void)
{
    struct server s;

    log_info("Starting zerod (pid: %d)", getpid());

    server_init(&s, "8080", AF_INET);

    return 0;
}