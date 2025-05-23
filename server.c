#include "server.h"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "logging.h"

int server_init(struct server *s)
{
    log_debug("calling socket()");
    s->socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    if (s->socket_fd == -1)
    {
        log_error("can't create IPv4 socket (errno: %s)", strerror(errno));
        return 1;
    }

    log_debug("created IPv4 socket (fd: %d)", s->socket_fd);
    return 0;
}
