//
// Created by pbeaujea on 4/7/18.
//

#ifndef BUBBLES_GAME_BASE_H
#define BUBBLES_GAME_BASE_H

#include "game_objects.h"

#define FRAMES_BETWEEN_KEY_REPEAT 4 // frames

enum {
    E_NONE,
    E_LEFT,
    E_RIGHT,
    E_UP,
    E_DOWN,
    E_ACTION_1, // i.e. "jump"
    E_ACTION_2, // i.e. "blow"
    E_PAUSE,
    E_SHOW_SCORE,
    E_SHOW_CONTROLS,
    E_QUIT,
    E_SIZE
};

enum {
    SCREEN_INSTRUCTIONS,
    SCREEN_GAME_OVER,
    SCREEN_WIN,
    SCREEN_UNKNOWN,
    SCREEN_NUMBER
};


typedef struct Game_ {
    bool paused;
    bool done;
    int current_screen;
    Sprite* screens[SCREEN_NUMBER];

    // Image
    Image* texture_items;
    Image* texture_monsters;
    Image* texture_levels;
    Image* texture_dragons;
    Image* texture_screens;

    // definitions
    ItemDef** definition_items;
    unsigned int num_items;
    MonsterDef** definition_monsters;
    unsigned int num_monsters;

    // levels
    Level* levels;
    Level* current_level;
    unsigned int num_levels;

    // dragons & monsters & bubbles & items
    Dragon* bub;
    Monster* monster_list;
    Bubble* bubble_list;
    Item* item_list;

    // keys
    bool key_pressed[E_SIZE];
    int key_pressed_interval[E_SIZE];
} Game;

void key_down(Game *game, int key);
void key_up(Game *game, int key);
void key_update_interval(Game* game);

#endif //BUBBLES_GAME_BASE_H
