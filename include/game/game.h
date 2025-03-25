#pragma once

#include "server/server.h"
#include "server/client.h"

typedef struct
{
    int bottom;
    int top;
} range_t;

typedef struct
{
    range_t range;
    int     answer;
} guess_number_t;

int game_run(struct sock_server_t *server, struct client_interface_t *client);
