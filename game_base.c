//
// Created by pbeaujea on 4/7/18.
//

#include "game_base.h"

void key_down(Game *game, int key) {
    if (key >= E_SIZE)
        return;

    game->key_pressed[key] = true;
    game->key_pressed_interval[key] = 1;
}

void key_up(Game *game, int key) {
    if (key >= E_SIZE)
        return;

    // printf("→ up: %d\n", key);

    game->key_pressed[key] = false;
}

void key_update_interval(Game* game) {
    for (int i = 0; i < E_SIZE; ++i) {
        if (game->key_pressed[i]) {
            if (game->key_pressed_interval[i] <= 0)
                game->key_pressed_interval[i] = FRAMES_BETWEEN_KEY_REPEAT;
            else {
                game->key_pressed_interval[i] -= 1;
            }
        }
    }
}