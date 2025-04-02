#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "server/server.h"
#include "game/game_server.h"

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

static sock_server_t *server_static = NULL;
static client_interface_t *client_static = NULL;

void server_stop_handler(int singal)
{
    client_interface_close(client_static);
    sock_server_close(server_static);
}

extern inline int check_ip_version(const char *host)
{
    return strcspn(host, ".") == strlen(host);
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "%s No interface specified\n", ERROR);
        return -1;
    }

    const char *host = argv[1];
    uint16_t port = atoi(argv[2]);
    int use_ipv6 = check_ip_version(host);
    int max_tries = atoi(argv[3]);
    if (max_tries <= 0)
        return -1;

    sock_server_t server;
    client_interface_t client;
    if (sock_server_create(
            &server, host, port, use_ipv6, SOCK_STREAM) != socket_error_success)
        return -1;

    server_static = &server;
    client_static = &client;
    signal(SIGINT, &server_stop_handler);
    signal(SIGTERM, &server_stop_handler);

    game_run(&server, &client, max_tries);

    return 0;
}
