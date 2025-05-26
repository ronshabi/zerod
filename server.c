#include "server.h"

#include "logging.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <arpa/inet.h> // inet_ntop
#include <netdb.h>     // getaddrinfo, freeaddrinfo
#include <stdbool.h>

#include <sys/socket.h>

//
// Static functions
//
static int zerod_setsockopt(int fd, int optname, int optval)
{
    const int rc = setsockopt(fd, SOL_SOCKET, optname, &optval, sizeof(optval));
    if (rc != 0)
    {
        log_error("setsockopt(fd: %d, optname: %d, optval: %d) failed - %s", fd,
                  optname, optval, strerror(errno));
        return 1;
    }

    log_debug("setsockopt(fd: %d, optname: %d) to %d", fd, optname, optval);
    return 0;
}

static int server_setup_getaddrinfo(struct server *s)
{
    int              rc    = 0;
    struct addrinfo  hints = {0};
    struct addrinfo *res   = NULL;

    hints.ai_family   = s->address_family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    rc = getaddrinfo(NULL, s->port_str, &hints, &res);
    if (rc != 0)
    {
        if (rc == EAI_SYSTEM)
        {
            log_error("getaddrinfo failed, system error: %s", strerror(errno));
        }
        else
        {
            log_error("getaddrinfo failed: %s", gai_strerror(rc));
        }
        return 1;
    }

    if (res == NULL)
    {
        log_error("getaddrinfo failed, no results");
        return 1;
    }

    // The first one should be alright
    const void *addr_ptr = NULL;
    uint64_t    addr_len = 0;

    if (res->ai_family == AF_INET)
    {
        addr_ptr = &((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
        addr_len = INET_ADDRSTRLEN;
    }
    else
    {
        addr_ptr = &((struct sockaddr_in6 *)(res->ai_addr))->sin6_addr;
        addr_len = INET6_ADDRSTRLEN;
    }

    const char *ntop_rv = NULL;
    ntop_rv = inet_ntop(res->ai_family, addr_ptr, s->address_str, addr_len);
    assert(ntop_rv);

    freeaddrinfo(res);
    return rc;
}

static int server_setup_socket(struct server *s)
{
    int fd = socket(s->address_family, SOCK_STREAM, 0);
    if (fd == -1)
    {
        log_error("Failed to obtain socket: %s", strerror(errno));
        return 1;
    }

    s->socket_fd = fd;
    log_debug("Obtained socket file descriptor %d for %s:%s", s->socket_fd,
              s->address_str, s->port_str);

    // Try to setsockopt
    if (zerod_setsockopt(fd, SO_REUSEPORT, 1))
    {
        return 1;
    }

    return 0;
}

static int server_setup_bind(struct server *s)
{
    if (bind(s->socket_fd, (struct sockaddr *)&s->sockaddr,
             sizeof(s->sockaddr)) != 0)
    {
        log_error("Bind to %s:%s failed: %s", s->address_str, s->port_str,
                  strerror(errno));
        return 1;
    }

    log_debug("Bind sockfd %d to %s:%s succeeded", s->socket_fd, s->address_str,
              s->port_str);

    return 0;
}

static int server_setup_listen(struct server *s)
{
    if (listen(s->socket_fd, 0) != 0)
    {
        log_error("Listen failed");
        return 1;
    }

    log_debug("Listening on %s:%s", s->address_str, s->port_str);
    return 0;
}

void server_process_events(struct server *s)
{
}

//
// Public functions
//

void server_init(struct server *s, const char *port, int address_family)
{
    memset(s, 0, sizeof(struct server));

    s->address_family = address_family;
    s->port_str       = port;

    if (server_setup_getaddrinfo(s))
    {
        s->status = SERVER_ERROR_OS;
        return;
    }

    if (server_setup_socket(s))
    {
        s->status = SERVER_ERROR_OS;
        return;
    }

    if (server_setup_bind(s))
    {
        s->status = SERVER_ERROR_OS;
        return;
    }

    if (server_setup_listen(s))
    {
        s->status = SERVER_ERROR_OS;
        return;
    }
}
