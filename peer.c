#include "peer.h"

#include <string.h>

void peer_init(struct peer *peer)
{
    memset(peer, 0, sizeof(struct peer));
}

void peer_recv(struct peer *peer)
{
    peer->bytes_received_last = recv(peer->socket_fd, peer->recvbuf, PEER_RECVBUF_SIZE, 0);
    if (peer->bytes_received_last == -1)
    {
        peer->receive_state = PEER_RECEIVE_ERROR;
        return;
    }

    if (peer->bytes_received_last == 0)
    {
        // End of this receive!
        peer->receive_state = PEER_RECEIVE_DONE;
        return;
    }

    peer->receive_state = PEER_RECEIVE_CONTINUE;
    peer->bytes_received_overall += peer->bytes_received_last;
}