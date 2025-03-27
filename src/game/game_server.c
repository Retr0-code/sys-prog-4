#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "game/game.h"

#define BUFFERSIZE 4096
#define MIN_RANGE 3
#define MAX_RANGE (20 - MIN_RANGE + 1)

static void game_init(guess_number_t *game)
{
    srand(time(NULL));
    int range = MIN_RANGE + rand() % MAX_RANGE;
    game->range.bottom = rand();
    game->range.top = game->range.bottom + range;
    game->answer = game->range.bottom + rand() % (game->range.top - game->range.bottom + 1);
}

int game_run(sock_server_t *server, client_interface_t *client)
{
    if (server == NULL || client == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    guess_number_t game;
    net_message_t message;
    int status = me_success;
    char client_address[INET6_ADDRSTRLEN];
    while (sock_server_listen_connection(server, client) == socket_error_success)
    {
        game_init(&game);
        memset(client_address, 0, INET6_ADDRSTRLEN);
        socket_get_address(client_address, &client->_address, client->_use_ipv6);

        printf("%s:Client connected\n", client_address);
        if (game_send_range(client->_socket_descriptor, &game.range) != me_success)
        {
            perror("Error sending range");
            continue;
        }

        if (game_receive_guess(client->_socket_descriptor, &game.guess) != me_success)
        {
            perror("Error receiving client guess");
            continue;
        }

        if (game_send_answer(client->_socket_descriptor,
            game.guess ? mt_answer_right : mt_answer_wrong) != me_success)
        {
            perror("Error sending answer");
            continue;
        }

        printf("%s:Client disconnected\n", client_address);
    }
    return 0;
}
