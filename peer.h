#pragma once

#define PEER_RECVBUF_SIZE 1024
#include <stdint.h>
#include <netinet/ip.h>

enum peer_receive_state
{
    PEER_RECEIVE_READY = 0,
    PEER_RECEIVE_CONTINUE, // Received one chunk, there may be more
    PEER_RECEIVE_DONE,
    PEER_RECEIVE_ERROR
};

struct peer
{
    int socket_fd;
    struct sockaddr_storage sockaddr;
    socklen_t sockaddr_len;
    uint8_t recvbuf[PEER_RECVBUF_SIZE];
    ssize_t bytes_received_last;
    ssize_t bytes_received_overall;
    enum peer_receive_state receive_state;
};

void peer_init(struct peer *peer);
void peer_recv(struct peer *peer);
