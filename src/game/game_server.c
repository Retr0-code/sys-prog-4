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
        printf("%s Game initiated with range [%i; %i] and answer=%i\n",
               INFO, game.range.bottom, game.range.top, game.answer);
        memset(client_address, 0, INET6_ADDRSTRLEN);
        socket_get_address(client_address, &client->_address, client->_use_ipv6);

        printf("%s %s:Client connected\n", INFO, client_address);
        if (game_send_range(client->_socket_descriptor, &game.range) != me_success)
        {
            fprintf(stderr, "%s %s:Sending range:\t%s\n", strerror(errno));
            continue;
        }

        if (game_receive_guess(client->_socket_descriptor, &game.guess) != me_success)
        {
            fprintf(stderr, "%s %s:Receiving client guess:\t%s\n",
                    ERROR, client_address, strerror(errno));
            continue;
        }

        printf("%s %s:Client guess is %i\n", INFO, client_address, game.guess);

        if (game_send_answer(client->_socket_descriptor,
                             game.guess == game.answer ? mt_answer_right : mt_answer_wrong) != me_success)
        {
            fprintf(stderr, "%s %s:Sending answer:\t%s\n", strerror(errno));
            continue;
        }

        printf("%s %s:Client disconnected\n", INFO, client_address);
    }
    return 0;
}
