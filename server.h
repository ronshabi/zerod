#pragma once

#include <netdb.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdint.h>

#define SERVER_MAX_CONNECTIONS 1000

enum server_status
{
    SERVER_OK = 0,
    SERVER_ERROR_OS,
};

struct server {
    enum server_status status;
    bool ipv4_enabled;
    bool ipv6_enabled;
    char ipv4_address[INET_ADDRSTRLEN];
    char ipv6_address[INET6_ADDRSTRLEN];
    int ipv4_socket_fd;
    int ipv4_backlog;
    struct sockaddr_in ipv4_sockaddr;
    struct sockaddr_in6 ipv6_sockaddr;
};

void server_init(struct server *s, const char *port);
int server_cleanup(struct server *s);
