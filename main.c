#include <stdio.h>

#include "logging.h"
#include "server.h"

int main(void) {
    struct server s;

    log_info("starting zerod");

    server_init(&s, "8080");

    return 0;
}