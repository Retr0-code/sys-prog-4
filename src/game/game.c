#include <time.h>
#include <errno.h>
#include <stdlib.h>

#include "game/game.h"
#include "game/protocol.h"

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

static int game_send_range(int client_fd, const range_t *range)
{
    net_message_t message = { mt_range, sizeof(range_t), NULL };
    int status = send(client_fd,
            &message,
            sizeof(net_message_t) - sizeof(range_t),
            MSG_MORE);
    status = send(client_fd,
            range,
            message.message_length,
            0);

    return -1;
}

int game_run(sock_server_t *server, client_interface_t *client)
{
    if (server == NULL || client == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    char buffer[BUFFERSIZE];
    char client_address[INET6_ADDRSTRLEN];
    guess_number_t game;
    net_message_t message;
    while (sock_server_listen_connection(&server, &client) == socket_error_success)
    {
        game_init(&game);
        memset(client_address, 0, INET6_ADDRSTRLEN);
        if (client->_use_ipv6)
            inet_ntop(AF_INET6, &client->_address.addr_v6.sin6_addr, client_address, client->_address_len);
        else
            inet_ntop(AF_INET, &client->_address.addr_v4.sin_addr, client_address, client->_address_len);

        printf("%s:Client connected\n", client_address);
        memset(buffer, 0, BUFFERSIZE);
        ssize_t received = recv(client->_socket_descriptor, buffer, BUFFERSIZE, 0);
        if (received == 0)
        {
            client_interface_close(&client);
            continue;
        }
        if (received == -1)
        {
            perror("Error receiving");
            continue;
        }
        if (game_send_range(client->_socket_descriptor, &game.range) == -1)
        {
            perror("Error sending range");
            continue;
        }

        printf("%s:Client disconnected\n", client_address);
    }
    return 0;
}