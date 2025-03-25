#include <stdio.h>
#include <signal.h>

#include "server/server.h"

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

static sock_server_t *server_static;
static client_interface_t *client_static;

void server_stop_handler(int singal)
{
    client_interface_close(client_static);
    sock_server_close(server_static);
}

int main(int argc, char **argv)
{
    sock_server_t server;
    client_interface_t client;
    if (sock_server_create(&server, HOST, PORT,
#ifdef USE_IPV6
                           1,
#else
                           0,
#endif
                           SOCK_STREAM) != socket_error_success)
    {
        perror("Error initializing server");
        return -1;
    }

    server_static = &server;
    client_static = &client;
    signal(SIGINT, &server_stop_handler);
    signal(SIGTERM, &server_stop_handler);
    return 0;
}
