#include "socket.h"
#include "logging.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>

int zd_socket_util_setsockopt(int sockfd, int optname, int optval)
{
    const int rc =
        setsockopt(sockfd, SOL_SOCKET, optname, &optval, sizeof(optval));
    if (rc != 0)
    {
        log_error("setsockopt(fd: %d, optname: %d, optval: %d) failed - %s",
                  sockfd, optname, optval, strerror(errno));
        return 1;
    }

    log_debug("setsockopt(fd: %d, optname: %d) to %d", sockfd, optname, optval);
    return 0;
}

int zd_socket_util_setnonblocking(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL);
    if (flags == -1)
    {
        log_error("fcntl(socket %d, F_GETFL) failed: %s", sockfd,
                  strerror(sockfd));
        return 1;
    }

    flags |= O_NONBLOCK;

    int rc = fcntl(sockfd, F_SETFL, flags);
    if (rc == -1)
    {
        log_error("fcntl(socket %d, set +nonblock): %s", sockfd,
                  strerror(sockfd));
        return 1;
    }

    log_debug("%s(%d)", __func__, sockfd);

    return 0;
}