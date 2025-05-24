#include "server.h"

#include <arpa/inet.h> /* inet_ntop */
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "logging.h"

#include <stdbool.h>

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

// TODO: this should be parameterized with host and port as strings.
// TODO: use getaddrinfo.
static int server_bind_ipv4(struct server *s, int port)
{
    s->ipv4_sockaddr.sin_addr.s_addr = INADDR_ANY;
    s->ipv4_sockaddr.sin_family = AF_INET;
    s->ipv4_sockaddr.sin_port = htons(port);

    int const rc = bind(s->ipv4_socket_fd, (struct sockaddr *)(&s->ipv4_sockaddr), sizeof(struct sockaddr_in));
    if (rc != 0)
    {
        log_error("bind(fd: %d, port: %d) error: %s", s->ipv4_socket_fd, port, strerror(errno));
        return 1;
    }

    // TODO: this should be parameterized to not just bind to INADDR_ANY
    log_info("ipv4: bound to 0.0.0.0:%d", port);
    return 0;
}

static int server_listen(struct server *s)
{
    int rc = 0;
    rc = listen(s->ipv4_socket_fd, s->ipv4_backlog);

    if (rc != 0)
    {
        log_error("listen(fd: %d) error: %s", s->ipv4_socket_fd, strerror(errno));
        return 1;
    }

    return 0;
}

int server_init(struct server *s)
{
    memset(s, 0, sizeof(struct server));
    s->ipv4_backlog = 5; // TODO: parameterize
    int rc = 0;

    s->ipv4_socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    if (s->ipv4_socket_fd == -1)
    {
        log_error("can't create IPv4 socket (%s)", strerror(errno));
        return 1;
    }

    log_debug("created IPv4 socket (fd: %d)", s->ipv4_socket_fd);

    if (zerod_setsockopt(s->ipv4_socket_fd, SO_REUSEPORT, 1))
    {
        return 1;
    }

    if (server_bind_ipv4(s, 8080))
    {
        // FIXME: close should have checked return value
        close(s->ipv4_socket_fd);
        s->ipv4_socket_fd = 0;
        return 1;
    }

    if (server_listen(s))
    {
        // FIXME: close should have checked return value
        close(s->ipv4_socket_fd);
        s->ipv4_socket_fd = 0;
        return 1;
    }

    return 0;
}



int server_cleanup(struct server *s)
{
    int rc = 0;

    if (s->ipv4_socket_fd != 0)
    {
        rc = close(s->ipv4_socket_fd);

        if (rc == -1)
        {
            log_error("can't close IPv4 socket properly (%s)", strerror(errno));
            return 1;
        }

        log_debug("ipv4: closed socket");
    }

    return 0;
}
