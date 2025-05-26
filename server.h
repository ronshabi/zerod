#pragma once

#include <netdb.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdint.h>

#define SERVER_BACKLOG 1024

enum server_status
{
    SERVER_OK = 0,
    SERVER_ERROR_OS,
};

struct server
{
    enum server_status      status;
    int                     address_family;
    int                     socket_fd;
    const char             *port_str;
    struct sockaddr_storage sockaddr;
    char                    address_str[INET6_ADDRSTRLEN];
};

void server_init(struct server *s, const char *port, int address_family);
int  server_cleanup(struct server *s);
