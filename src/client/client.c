#include <errno.h>
#include <stdlib.h>

#include "client/client.h"
#include "network_exceptions.h"

int sock_client_init(sock_client_t *client, const char *rhost, uint16_t rport, int use_ipv6)
{
    if (client == NULL || rhost == NULL || rport == 0)
    {
        errno = EINVAL;
        return -1;
    }

    client->_use_ipv6 = (use_ipv6 != 0);
    sa_family_t af = use_ipv6 ? AF_INET6 : AF_INET;
    if ((client->_socket_descriptor = socket(af, SOCK_STREAM, 0)) == -1)
        return socket_error_init;

    connect(client->_socket_descriptor, );
    return socket_error_success;
}
