//
// Created by pbeaujea on 4/7/18.
//

#ifndef BUBBLES_GAME_SCREENS_H
#define BUBBLES_GAME_SCREENS_H

#include "game_base.h"
#include "game_main.h"

void game_simple_screen_input_management(Game *game, bool return_to_game);
void game_simple_screen_draw(Game *game);


#define WS_BASE 182
#define WS_SHIFT 38

void game_welcome_screen_input_management(Game *game, int* action);
void game_welcome_screen_draw(Game *game, int* action);

#endif //BUBBLES_GAME_SCREENS_H
