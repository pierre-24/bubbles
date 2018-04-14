//
// Created by pbeaujea on 4/7/18.
//

#ifndef BUBBLES_GAME_MAIN_H
#define BUBBLES_GAME_MAIN_H

#include "game_objects.h"
#include "game_base.h"

#define BLOW_EVERY 60

void game_main_start(Game *game);
void game_next_level(Game *game);
void game_setup_current_level(Game *game);

void game_main_input_management(Game *game);
void game_main_update_states(Game *game);
void game_main_draw(Game *game);

#endif //BUBBLES_GAME_MAIN_H
