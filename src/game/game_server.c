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

int game_run(sock_server_t *server, client_interface_t *client, size_t max_tries)
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
        game.tries = max_tries;
        printf("%s Game initiated with range [%i; %i] and answer=%i\n",
               INFO, game.range.bottom, game.range.top, game.answer);
        memset(client_address, 0, INET6_ADDRSTRLEN);
        socket_get_address(client_address, &client->_address, client->_use_ipv6);

        printf("%s %s:Client connected\n", INFO, client_address);
        if (game_send_client_settings(
                client->_socket_descriptor, (game_client_settings_t *)&game) != me_success)
        {
            fprintf(stderr, "%s %s:Sending range:\t%s\n", strerror(errno));
            continue;
        }

        do
        {
            printf("%s:Client try #%i\n", client_address, max_tries - game.tries);
            if (game_receive_guess(client->_socket_descriptor, &game.guess) != me_success)
            {
                fprintf(stderr, "%s %s:Receiving client guess:\t%s\n",
                        WARNING, client_address, strerror(errno));
                break;
            }

            printf("%s %s:Client guess is %i\n", INFO, client_address, game.guess);

            int answer = a_right;
            if (game.guess - game.answer > 0)
                answer = a_more;
            else
                answer = game.guess - game.answer ? a_less : a_right;

            if (game_send_answer(client->_socket_descriptor, answer) != me_success)
            {
                fprintf(stderr, "%s %s:Sending answer:\t%s\n", ERROR, client_address, strerror(errno));
                break;
            }
        } while (--game.tries);

        if (!game.tries)
            printf("%s:Client exceeded amount of tries\n", client_address);

        client_interface_close(client);
        printf("%s %s:Client disconnected\n", INFO, client_address);
    }
    return 0;
}
