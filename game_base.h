//
// Created by pbeaujea on 4/7/18.
//

#ifndef BUBBLES_GAME_BASE_H
#define BUBBLES_GAME_BASE_H

#include "game_objects.h"
#include "score.h"

#define FRAMES_BETWEEN_KEY_REPEAT_IN_SCREEENS 10 // frames
#define FRAMES_BETWEEN_KEY_REPEAT_IN_GAME 4

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
    SCREEN_WELCOME,
    SCREEN_INSTRUCTIONS,
    SCREEN_GAME_OVER,
    SCREEN_WIN,
    SCREEN_UNKNOWN,
    SCREEN_NUMBER
};

#define GAME_ELEMENT_WIDTH 32
#define GAME_ELEMENT_HEIGHT 32

enum {
    GE_ARROW,
    GE_NUMBER
};

typedef struct Game_ {
    bool paused;
    bool main_started;

    // Image
    Image* texture_game;
    Image* texture_items;
    Image* texture_monsters;
    Image* texture_levels;
    Image* texture_dragons;
    Image* texture_screens;

    int current_screen;
    Sprite* screens[SCREEN_NUMBER];

    Sprite* game_elements[GE_NUMBER];
    
    // scores
    Score* scores_list;

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
    int key_interval;
} Game;

void key_down(Game *game, int key);
void key_up(Game *game, int key);
void key_update_interval(Game* game);

bool key_fired(Game* game, int key);

#endif //BUBBLES_GAME_BASE_H
