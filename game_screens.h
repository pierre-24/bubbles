//
// Created by pbeaujea on 4/7/18.
//

#ifndef BUBBLES_GAME_SCREENS_H
#define BUBBLES_GAME_SCREENS_H

#include "game_base.h"
#include "game_main.h"

void game_set_screen(Game* game, int screen);

void game_simple_screen_input_management(Game *game, bool return_to_game);
void game_simple_screen_draw(Game *game);


#define WS_BASE 182 // [pixels]
#define WS_SHIFT 38 // [pixels]

void game_welcome_screen_input_management(Game *game);
void game_welcome_screen_draw(Game *game);


#define WL_BASE_X (WINDOW_WIDTH / 2)
#define WL_BASE_Y (WINDOW_HEIGHT / 2)
#define WL_FRAMERATE 30 // [frames]

void game_win_loose_screen_input_management(Game *game);
void game_win_loose_screen_draw(Game *game);

#define SCORE_SHIFT 2 // [frames]

void game_score_screen_draw(Game *game);

#endif //BUBBLES_GAME_SCREENS_H
