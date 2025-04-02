#pragma once
#include <threads.h>

#include "game/game.h"

typedef struct thread_node_t
{
    thrd_t thread;
    struct thread_node_t *next;
} thread_node_t;

int game_run(
    struct sock_server_t *server,
    struct client_interface_t *client,
    size_t max_tries,
    thread_node_t **threads);

void threads_close(thread_node_t *threads);
