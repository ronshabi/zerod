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

static void server_setup_ipv4_address(struct server *s, struct addrinfo *ai)
{
    // Copy the address
    assert(ai->ai_addrlen == sizeof(struct sockaddr_in));
    memcpy(&s->ipv4_sockaddr, ai->ai_addr, sizeof(struct sockaddr_in));

    // Convert the address into a string, and save it in server buffer
    const struct sockaddr_in *in = (struct sockaddr_in *)ai->ai_addr;

    const char *rv = inet_ntop(AF_INET, &in->sin_addr.s_addr, s->ipv4_address,
                               sizeof(s->ipv4_address));

    // Assert the conversion to a string is successful.
    assert(rv);

    // Enable IPv4
    s->ipv4_enabled = true;
}

static void server_setup_ipv6_address(struct server *s, struct addrinfo *ai)
{
    // Copy the address
    assert(ai->ai_addrlen == sizeof(struct sockaddr_in6));
    memcpy(&s->ipv6_sockaddr, ai->ai_addr, sizeof(struct sockaddr_in6));

    // Convert the address into a string, and save it in server buffer
    const struct sockaddr_in6 *in = (struct sockaddr_in6 *)ai->ai_addr;

    const char *rv = inet_ntop(AF_INET6, &in->sin6_addr, s->ipv6_address,
                               sizeof(s->ipv6_address));

    // Assert the conversion to a string is successful.
    assert(rv);

    // Enable IPv6
    s->ipv6_enabled = true;
}

static int server_getaddrinfo(struct server *s, const char *port)
{
    int rc                = 0;
    struct addrinfo hints = {0};
    struct addrinfo *res  = NULL;

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    rc = getaddrinfo(NULL, port, &hints, &res);
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

    for (struct addrinfo *p = res; p; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            if (s->ipv4_enabled == false)
            {
                server_setup_ipv4_address(s, p);
            }
        }
        else if (p->ai_family == AF_INET6)
        {
            if (s->ipv6_enabled == false)
            {
                server_setup_ipv6_address(s, p);
            }
        }
        else
        {
            log_warn("getaddrinfo returned an unknown address family %d",
                     p->ai_family);
        }
    }

    if (!s->ipv4_enabled && !s->ipv6_enabled)
    {
        log_error("No IPv4 or IPv6 bindable-addresses were found");
        rc = 1;
    }

    if (s->ipv4_enabled)
    {
        log_info("IPv4: Found bind address %s:%s", s->ipv4_address, port);
    }

    if (s->ipv6_enabled)
    {
        log_info("IPv6: Found bind address %s:%s", s->ipv6_address, port);
    }

    freeaddrinfo(res);
    return rc;
}

//
// Public functions
//

void server_init(struct server *s, const char *port)
{
    memset(s, 0, sizeof(struct server));

    if (server_getaddrinfo(s, port))
    {
        log_error("No addresses were found on port %s", port);
        s->status = SERVER_ERROR_OS;
        return;
    }

}
