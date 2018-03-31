#ifndef BUBBLES_GAME_H
#define BUBBLES_GAME_H

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "game_object_definitions.h"
#include "levels.h"
#include "game_objects.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 432

#define TEXTURE_ITEMS "assets/items.ppm"
#define TEXTURE_MONSTERS "assets/monsters.ppm"
#define TEXTURE_LEVELS "assets/levels.ppm"
#define TEXTURE_DRAGONS "assets/dragons.ppm"

#define DEFINITION_ITEMS "data/items.txt"
#define DEFINITION_MONSTERS "data/monsters.txt"

#define FILE_LEVELS "data/levels.txt"

#define FRAMES_BETWEEN_KEY_REPEAT 4 // frames

enum {
    E_NONE,
    E_LEFT,
    E_RIGHT,
    E_TOP,
    E_BOTTOM,
    E_ACTION_1, // i.e. "jump"
    E_ACTION_2, // i.e. "blow"
    E_PAUSE,
    E_SHOW_SCORE,
    E_SHOW_CONTROLS,
    E_QUIT,
    E_SIZE
};

void game_special_key_down(int key, int x, int y);
void game_special_key_up(int key, int x, int y);
void game_key_down(unsigned char key, int x, int y);
void game_key_up(unsigned char key, int x, int y);

void key_update_interval();

// macro to get pixel component position from game screen (note: invert y so that it is located above)
#define _SR(i, j) (((WINDOW_HEIGHT - (j))*WINDOW_WIDTH+(i))*4)

typedef struct Game_ {

    // Image
    Image* texture_items;
    Image* texture_monsters;
    Image* texture_levels;
    Image* texture_dragons;

    // definitions
    ItemDef** definition_items;
    unsigned int num_items;
    MonsterDef** definition_monsters;
    unsigned int num_monsters;

    // levels
    Level* levels;
    unsigned int num_levels;

    // dragons
    Dragon* bub;

    // keys
    bool key_pressed[E_SIZE];
    int key_pressed_interval[E_SIZE];
} Game;

void game_init();
void game_loop();
void game_quit();

#endif
