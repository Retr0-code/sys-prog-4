#pragma once
#include <stdint.h>

enum message_type_e
{
    mt_close,
    mt_range,
    mt_guess,
    mt_answer,
    mt_retry
};

enum answer_e
{
    a_right,
    a_less,
    a_more
};

typedef struct
{
    int32_t  type;
    uint32_t length;
    char     *body;
} __attribute__((aligned(4), packed)) net_message_t;

enum message_error_e
{
    me_success,
    me_invalid_args,
    me_wrong_type,
    me_wrong_length,
    me_bad_socket,
    me_send,
    me_receive,
    me_peer_end
};

int message_send(int socket_fd, int type, size_t data_length, const char *data);

int message_receive(int socket_fd, int type, size_t data_length, char **data);
