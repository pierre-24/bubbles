//
// Created by pbeaujea on 4/7/18.
//

#include "game_base.h"

void key_down(Game *game, int key) {
    if (game == NULL)
        return;

    if (key >= E_SIZE || key < 0)
        return;

    game->key_pressed[key] = true;
    game->key_pressed_interval[key] = 1;
}

void key_up(Game *game, int key) {
    if (game == NULL)
        return;

    if (key >= E_SIZE || key < 0)
        return;

    game->key_pressed[key] = false;
}

void key_update_interval(Game* game) {
    if (game == NULL)
        return;

    for (int i = 0; i < E_SIZE; ++i) {
        if (game->key_pressed[i]) {
            if (game->key_pressed_interval[i] <= 0)
                game->key_pressed_interval[i] = game->key_interval;
            else {
                game->key_pressed_interval[i] -= 1;
            }
        }
    }
}

bool key_fired(Game* game, int key) {
    if (key >= E_SIZE || key < 0)
        return false;

    return game->key_pressed[key] && game->key_pressed_interval[key] == 0;
}