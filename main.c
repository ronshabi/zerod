#include <stdio.h>

#include "logging.h"
#include "server.h"

int main(void) {
    struct server s;

    log_info("starting zerod");

    if (server_init(&s))
    {
        goto err;
    }

    if (server_cleanup(&s))
    {
        goto err;
    }

err:
    return 1;
}