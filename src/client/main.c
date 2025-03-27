#include <errno.h>
#include <stdio.h>
#include <signal.h>

#include "client/client.h"
#include "network_exceptions.h"
#include "game/game.h"
#include "game/protocol.h"

#ifndef USE_IPV6
#ifndef HOST
#define HOST "127.0.0.1"
#endif
#else
#ifndef HOST
#define HOST "::1%lo"
#endif
#endif

#ifndef PORT
#define PORT 4444
#endif

#define BUFFERSIZE 4096

static sock_client_t *client_static = NULL;

void client_stop_handler(int singal)
{
    sock_client_stop(client_static);
}

int main(int argc, char **argv)
{
    sock_client_t client;
        if (sock_client_create(&client, HOST, PORT,
#ifdef USE_IPV6
                           1,
#else
                           0,
#endif
    SOCK_STREAM) != socket_error_success)
        return -1;

    client_static = &client;
    signal(SIGINT, &client_stop_handler);
    signal(SIGTERM, &client_stop_handler);
    
    if (sock_client_connect(&client) != socket_error_success)
        return -1;

    range_t range;
    range_t *range_ptr = &range;
    int guess = 0;
    int status = me_success;
    while (1)
    {
        status = game_receive_range(client._socket_descriptor, &range);
        switch (status)
        {
            case me_receive:
                fprintf(stderr, "%s Something went wrong while receiving:\t%s\n", ERROR, strerror(errno));
                break;
            case me_peer_end:
                fprintf(stderr, "%s Server closed connection\n", ERROR);
                break;
            default:
                break;
        }
        if (status != me_success)
            break;
        printf("Guess the number in range [%i; %i]> ", range.bottom, range.top);
        // Choose a player option (machine or human)
        // scanf("%i\n", &guess);
        if (game_send_guess(client._socket_descriptor, &guess) != me_success)
        {
            fprintf(stderr, "%s Something went wrong while sending:\t%s\n", ERROR, strerror(errno));
            break;
        }

        if (game_receive_answer(client._socket_descriptor) == me_wrong_type)
        {
            printf("%i is wrong answer\n", guess);
            break;
        }
    }
    sock_client_stop(&client);

    return 0;
}
