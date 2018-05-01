//
// Created by pbeaujea on 4/7/18.
//

#include "game_base.h"

void key_down(Game *game, int key) {
    /* GLUT callback: set a key to be down
     * */
    if (game != NULL) {
        if (key >= E_SIZE || key < 0)
            return;

        game->key_pressed[key] = true;
        counter_restart(game->key_counters[key], game->key_interval);

    }
}

void key_up(Game *game, int key) {
    /* GLUT callback: set a key to be up
     * */
    if (game != NULL) {
        if (key >= E_SIZE || key < 0)
            return;

        game->key_pressed[key] = false;
    }
}

void keys_update_interval(Game *game) {
    /* Tick the counters for which the keys are up
     * */
    if (game != NULL) {
        for (int i = 0; i < E_SIZE; ++i) {
            if (game->key_pressed[i])
                counter_tick(game->key_counters[i]);
        }

    }
}

bool key_fired(Game* game, int key) {
    /* Test if a key is up and the counter stopped.
     * */
    if (game != NULL) {
        if (key >= E_SIZE || key < 0)
            return false;

        return game->key_pressed[key] && counter_stopped(game->key_counters[key]);
    }

    return false;
}

void keys_reset(Game *game) {
    /* Set all the keys to down
     * */
    if (game != NULL) {
        for (int i = 0; i < E_SIZE; ++i)
            game->key_pressed[i] = false;
    }
}

void keys_init(Game* game) {
    /* Set up the counters for the keys
     * */
    if (game != NULL) {
        for (int i = 0; i < E_SIZE; ++i) {
            game->key_counters[i] = counter_new(game->key_interval, true, false);
            counter_stop(game->key_counters[i]);
        }

        keys_reset(game);
    }
}
void keys_delete(Game* game) {
    /* Delete the counters for the keys
     * */
    if (game != NULL) {
        for (int i = 0; i < E_SIZE; ++i)
            counter_delete(game->key_counters[i]);
    }
}