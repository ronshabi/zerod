#pragma once

#include <netdb.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/epoll.h>

#define SERVER_BACKLOG    1024
#define SERVER_MAX_EVENTS 10

enum server_status
{
    SERVER_OK = 0,
    SERVER_ERROR_OS,
    SERVER_ERROR_EVENTLOOP,
};

struct server
{
    enum server_status      status;
    int                     address_family;
    int                     socket_fd;
    const char             *port_str;
    struct sockaddr_storage sockaddr;
    char                    address_str[INET6_ADDRSTRLEN];

    int                epoll_fd;
    struct epoll_event epoll_ev;
    struct epoll_event epoll_events[SERVER_MAX_EVENTS];
};

void server_init(struct server *s, const char *port, int address_family);
int  server_cleanup(struct server *s);
