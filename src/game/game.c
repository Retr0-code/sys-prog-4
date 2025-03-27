#include "game/game.h"
#include "game/protocol.h"

int game_send_range(int client_fd, const range_t *range)
{
    return message_send(client_fd, mt_range, sizeof(range_t), range);
}

int game_receive_range(int client_fd, range_t *range)
{
    return message_receive(client_fd, mt_range, sizeof(range_t), &range);
}

int game_send_guess(int client_fd, const int *guess)
{
    return message_send(client_fd, mt_guess, sizeof(int), guess);
}

int game_receive_guess(int client_fd, int *guess)
{
    return message_receive(client_fd, mt_guess, sizeof(int), &guess);
}

int game_send_answer(int client_fd, int answer)
{
    return message_send(client_fd, answer, 0, NULL);
}

int game_receive_answer(int client_fd)
{
    return message_receive(client_fd, mt_answer_right, 0, NULL);
}
