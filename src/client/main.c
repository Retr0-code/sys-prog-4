#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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

void player_human(const range_t *range, int *guess)
{
    scanf("%i", guess);
}

void player_machine(const range_t *range, int *guess)
{
    *guess = range->bottom + rand() % (range->top - range->bottom + 1);
}

extern inline int check_ip_version(const char *host)
{
    return strcspn(host, ".") == strlen(host);
}

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4)
    {
        fprintf(stderr, "%s No server specified\n", ERROR);
        return -1;
    }

    const char *host = argv[1];
    int use_ipv6 = check_ip_version(host);
    uint16_t port = atoi(argv[2]);
    if (port == 0)
    {
        fprintf(stderr, "%s Wrong port \"%s\"\n", ERROR, argv[2]);
        return -1;
    }

    typedef void (*player_function_ptr)(const range_t *, int*);
    
    player_function_ptr guess_function = &player_human;
    if (argc == 4)
    {
        switch (*argv[3])
        {
            case 'h':
                guess_function = &player_human;
                break;
            case 'm':
                guess_function = &player_machine;
                break;
            default:
                fprintf(stderr, "%s No such player option '%c'\n", *argv[4]);
                break;
        }
    }

    sock_client_t client;
    if (sock_client_create(&client, host, port, use_ipv6, SOCK_STREAM) != socket_error_success)
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
        guess_function(range_ptr, &guess);
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
        printf("%i is right answer\n", guess);
        break;
    }
    sock_client_stop(&client);

    return 0;
}
