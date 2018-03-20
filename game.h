#ifndef BUBBLES_GAME_H
#define BUBBLES_GAME_H

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "game_object_definitions.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 432

#define TEXTURE_ITEMS "assets/items.ppm"
#define TEXTURE_MONSTERS "assets/monsters.ppm"

#define DEFINITION_ITEMS "data/items.txt"
#define DEFINITION_MONSTERS "data/monsters.txt"

// macro to get pixel component position from game screen (note: invert y so that it is located above)
#define _SR(i, j) (((WINDOW_HEIGHT - (j))*WINDOW_WIDTH+(i))*4)

typedef struct Game_ {

    // Image
    Image* texture_items;
    Image* texture_monsters;

    // definitions
    ItemDef** definition_items;
    unsigned int num_items;
    MonsterDef** definition_monsters;
    unsigned int num_monsters;
} Game;

void game_init();
void game_loop();
void game_quit();


#endif
