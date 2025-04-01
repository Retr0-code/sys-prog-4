#pragma once

#include "server/server.h"
#include "server/client.h"
#include "game/protocol.h"

typedef struct
{
    int bottom;
    int top;
} range_t;

typedef struct
{
    range_t range;
    int     answer;
    int     guess;
} guess_number_t;

int game_send_range(int client_fd, const range_t *range);

int game_receive_range(int client_fd, range_t *range);

int game_send_guess(int client_fd, const int *guess);

int game_receive_guess(int client_fd, int *guess);

int game_send_answer(int client_fd, int answer);

int game_receive_answer(int client_fd, int answer);
