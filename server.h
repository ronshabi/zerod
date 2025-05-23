#pragma once

struct server {
    int ipv4_socket_fd;
};

int server_init(struct server *s);
int server_cleanup(struct server *s);
