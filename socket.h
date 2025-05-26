#pragma once

// Socket utility functions
int zd_socket_util_setsockopt(int sockfd, int optname, int optval);
int zd_socket_util_setnonblocking(int sockfd);