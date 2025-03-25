#include <stdio.h>
#include <signal.h>

#include "protocol.h"
#include "server/server.h"

#define BUFFERSIZE 4096

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

typedef struct
{
    int x;
    int y;
} int_vector2d_t;

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

    char buffer[BUFFERSIZE];
    char client_address[INET6_ADDRSTRLEN];
    while (sock_server_listen_connection(&server, &client) == socket_error_success)
    {
        memset(client_address, 0, INET6_ADDRSTRLEN);
        if (client._use_ipv6)
            inet_ntop(AF_INET6, &client._address.addr_v6.sin6_addr, client_address, client._address_len);
        else
            inet_ntop(AF_INET, &client._address.addr_v4.sin_addr, client_address, client._address_len);

        printf("%s:Client connected\n", client_address);
        memset(buffer, 0, BUFFERSIZE);
        ssize_t received = recv(client._socket_descriptor, buffer, BUFFERSIZE, 0);
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
        if (send(client._socket_descriptor, &msg_status_check, sizeof(net_message_t), 0) == -1)
        {
            perror("Client socket status");
        }
        printf("%s:Client disconnected\n", client_address);
    }
    return 0;
}
