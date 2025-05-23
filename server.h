#pragma once

struct server {
    int socket_fd;
};

int server_init(struct server *s);
