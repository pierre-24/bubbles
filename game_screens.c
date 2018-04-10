//
// Created by pbeaujea on 4/7/18.
//

#include "game_screens.h"

void game_set_screen(Game* game, int screen) {
    if (screen >= 0 && screen < SCREEN_NUMBER) {
        game->current_screen = screen;
        game->paused = true;
    }
}

void game_simple_screen_input_management(Game *game, bool return_to_game) {

    if (game->key_pressed[E_ACTION_1]) {
        if (return_to_game)
            game->paused = false;
        else
            game_set_screen(game, SCREEN_WELCOME);

        game->key_pressed[E_ACTION_1] = false;
    }
}

void game_simple_screen_draw(Game *game) {
    blit_sprite(game->screens[game->current_screen], 0, 0, false, false);
}


void game_welcome_screen_input_management(Game *game, int* action) {
    game->key_interval = FRAMES_BETWEEN_KEY_REPEAT_IN_SCREEENS;

    if (game->key_pressed[E_ACTION_1]) {
        if (*action == 0) {
            game_main_start(game, game->current_level);
            game->paused = false;
        } else if (*action == 1) {
            game_set_screen(game, SCREEN_INSTRUCTIONS);
        }

        game->key_pressed[E_ACTION_1] = false;
    }

    if (key_fired(game, E_UP) || key_fired(game, E_DOWN)) {
        *action += game->key_pressed[E_UP] ? -1 : 1;

        if (*action == -1)
            *action = 2;

        *action %= 3;
    }
}

void game_welcome_screen_draw(Game *game, int* action) {
    blit_sprite(game->screens[SCREEN_WELCOME], 0, 0, false, false);
    blit_sprite(game->game_elements[GE_ARROW], 100, WS_BASE - *action * WS_SHIFT, false, false);
}