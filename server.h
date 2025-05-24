#pragma once

#include <netdb.h>
#include <stdint.h>
#include <netinet/ip.h>

#include "peer.h"

struct server {
    int ipv4_socket_fd;
    int ipv4_backlog;
    struct sockaddr_in ipv4_sockaddr;

    // Currently we have only one peer, until we can parse basic http.
    struct peer peer;
};

int server_init(struct server *s);
int server_cleanup(struct server *s);
int server_eventloop(struct server *s);
