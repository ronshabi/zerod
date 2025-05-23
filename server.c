#include "server.h"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "logging.h"

int server_init(struct server *s)
{
    s->ipv4_socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    if (s->ipv4_socket_fd == -1)
    {
        log_error("can't create IPv4 socket (%s)", strerror(errno));
        return 1;
    }

    log_debug("created IPv4 socket (fd: %d)", s->ipv4_socket_fd);
    return 0;
}

int server_cleanup(struct server *s)
{
    int rc = 0;

    rc = close(s->ipv4_socket_fd);
    if (rc == -1)
    {
        log_error("can't close IPv4 socket properly (%s)", strerror(errno));
        return 1;
    }

    log_debug("closed IPv4 socket");

    return 0;
}
