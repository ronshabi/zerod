#include <stdio.h>

#include "logging.h"
#include "server.h"

int main(void) {
    struct server s;
    int rc = 0;

    rc = server_init(&s);
    return rc;
}