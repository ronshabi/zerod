#pragma once

#include <netinet/ip.h>
#include <stdint.h>

enum connection_status
{
    CONNECTION_ACTIVE = 0,
    CONNECTION_CLOSED
};

struct connection
{
    enum connection_status  status;
    int                     socket_fd;
    struct sockaddr_storage sockaddr;
    uint64_t                sockaddr_len;
};