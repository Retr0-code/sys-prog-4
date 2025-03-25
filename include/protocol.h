#pragma once
#include <stdint.h>

enum message_type_e
{
    mt_range,
    mt_guess,
    mt_answer_right,
    mt_answer_wrong
};

typedef struct
{
    int32_t  message_type;
    uint32_t message_length;
    char     *message;
} __attribute__((aligned(4), packed)) net_message_t;
