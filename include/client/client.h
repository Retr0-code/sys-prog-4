#pragma once
#include <stdint.h>
#include <sys/socket.h>

#include "sockutils.h"

typedef struct
{
    struct sockaddr *_address;
    int             _socket_descriptor;
    int             _use_ipv6;
} sock_client_t;

int sock_client_create(sock_client_t *client, const char *rhost, uint16_t rport, int use_ipv6);
