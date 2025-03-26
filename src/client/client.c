#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "client/client.h"
#include "network_exceptions.h"

int sock_client_create(sock_client_t *client, const char *rhost, uint16_t rport, int use_ipv6)
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

    if (connect(client->_socket_descriptor, client->_address,
        use_ipv6 ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in)) == -1)
    {
        fprintf(stderr, "%s Client could not connect to server %s:%u:\t%s", ERROR, rhost, rport, strerror(errno));
        return socket_error_bind;
    }

    return socket_error_success;
}
