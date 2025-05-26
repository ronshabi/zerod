#include <arpa/inet.h> // inet_ntop
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h> // getaddrinfo, freeaddrinfo
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "logging.h"
#include "server.h"

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

    // memcpy to server's sockaddr!
    memcpy(&s->sockaddr, res->ai_addr, res->ai_addrlen);

    const char *ntop_rv = NULL;
    ntop_rv = inet_ntop(res->ai_family, addr_ptr, s->address_str, addr_len);
    assert(ntop_rv);

    freeaddrinfo(res);
    return rc;
}

static int setup_socket(struct server *s)
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
    if (zd_socket_util_setsockopt(fd, SO_REUSEPORT, 1))
    {
        return 1;
    }

    // Make socket nonblocking
    if (zd_socket_util_setnonblocking(fd))
    {
        return 1;
    }

    return 0;
}

static int setup_bind(struct server *s)
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

static int setup_listen(struct server *s)
{
    if (listen(s->socket_fd, 0) != 0)
    {
        log_error("Listen failed");
        return 1;
    }

    log_debug("Listening on %s:%s", s->address_str, s->port_str);
    return 0;
}

static int setup_epoll(struct server *s)
{
    s->epoll_fd = epoll_create1(0);
    if (s->epoll_fd == -1)
    {
        log_error("Failed to setup epoll, epoll_create1 failed: %s",
                  strerror(errno));
        return 1;
    }

    s->epoll_ev.events  = EPOLLIN | EPOLLOUT | EPOLLET;
    s->epoll_ev.data.fd = s->socket_fd;

    int epoll_ctl_result =
        epoll_ctl(s->epoll_fd, EPOLL_CTL_ADD, s->socket_fd, &s->epoll_ev);

    if (epoll_ctl_result == -1)
    {
        log_error("Failed to setup epoll, epoll_ctl failed: %s",
                  strerror(errno));
        return 1;
    }

    log_debug("Setup epoll on listening socket %d -> epoll_fd = %d",
              s->socket_fd, s->epoll_fd);

    return 0;
}

void server_process_one_event(struct server *s, int event_index)
{
    log_debug("Processing event #%d", event_index);

    struct epoll_event current_event = s->epoll_events[event_index];

    if (current_event.data.fd == s->socket_fd)
    {
        log_debug("\t> got event on listening socket! should accept");
    }
}

void server_process_events(struct server *s)
{
    log_debug("Starting server event loop");

    while (s->status == SERVER_OK)
    {
        log_debug("EventLoop: Trying to pull events");
        // Pull the events
        int nfds =
            epoll_wait(s->epoll_fd, s->epoll_events, SERVER_MAX_EVENTS, -1);

        log_debug("EventLoop: %d events pulled", nfds);

        if (nfds == -1)
        {
            log_error("Can't pull events, epoll_wait failed: %s",
                      strerror(errno));
            s->status = SERVER_ERROR_EVENTLOOP;
            return;
        }

        // Process the events
        for (int n = 0; n < nfds; ++n)
        {
            ev_do_one(s, n);
        }
    }
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

    if (server_setup_epoll(s))
    {
        s->status = SERVER_ERROR_OS;
        return;
    }

    server_process_events(s);
}
