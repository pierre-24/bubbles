//
// Created by pbeaujea on 4/7/18.
//

#ifndef BUBBLES_GAME_BASE_H
#define BUBBLES_GAME_BASE_H

#include "game_objects.h"
#include "score.h"

#define FRAMES_BETWEEN_KEY_REPEAT_IN_SCREEENS 8 // [frames]
#define FRAMES_BETWEEN_KEY_REPEAT_IN_GAME 4 // [frames]

#define WINDOW_WIDTH 512 // [pixels]
#define WINDOW_HEIGHT 432 // [pixels]
#define FPS 60 // [frames per second]

enum {
    /* Define keys
     * */
    E_NONE,
    E_LEFT,
    E_RIGHT,
    E_UP,
    E_DOWN,
    E_ACTION_1, // i.e. "jump"
    E_ACTION_2, // i.e. "blow"
    E_FREEZE,
    E_SHOW_SCORE,
    E_SHOW_CONTROLS,
    E_QUIT,
    E_SIZE
};

enum {
    /* Define screens
     * */
    SCREEN_WELCOME,
    SCREEN_INSTRUCTIONS,
    SCREEN_GAME_OVER,
    SCREEN_WIN,
    SCREEN_SCORE,
    SCREEN_NUMBER
};

#define GAME_ELEMENT_WIDTH 32 // [pixels]
#define GAME_ELEMENT_HEIGHT 32 // [pixels]

#define NEXT_LEVEL_TRANSITION 120 // [frames]
#define UNTIL_NEXT_LEVEL 600 // [frames]

#define FREEZE_EVERY 30 // [frames]

#define MAX_LEVEL_TIME (30 * 60) // [frames]

enum {
    /* Define game elements (some sprites)
     * */
    GE_ARROW,
    GE_NUMBER
};

typedef struct Game_ {
    /* Define the main game structure (normally the only global variable).
     *
     * - All the textures (images), some sprites (for the screens) and font ;
     * - The list of score ;
     * - Definitions of item and monster arrays (and corresponding numbers of elements) ;
     * - Lists of items, monsters and bubbles ;
     * - Key management: `key_pressed` to know wether a given key is pressed, and `key_pressed_interval` to deal with key repetition ;
     * - Level management: the list of levels, the starting, previous current and next level.
     * - Some boolean:
     *   * `paused`: set to true if a screen is active ;
     *   * `main_started`: true if the "main" game is started ;
     *   * `freeze`: true is the game is paused.
     * */
    bool paused;
    bool main_started;
    bool freeze;

    // Image
    Image* texture_game;
    Image* texture_items;
    Image* texture_monsters;
    Image* texture_levels;
    Image* texture_dragons;
    Image* texture_screens;
    Image* texture_font;

    int current_screen;
    Sprite* screens[SCREEN_NUMBER];

    Sprite* game_elements[GE_NUMBER];

    Font* font;
    
    // scores
    Score* scores_list;

    // definitions
    ItemDef** definition_items;
    unsigned int num_items_defined;
    MonsterDef** definition_monsters;
    unsigned int num_monsters_defined;

    // levels
    Level* levels;
    Level* current_level;
    Level* previous_level;
    Level* starting_level;
    unsigned int num_levels;

    Counter* counter_next_level;
    Counter* counter_end_this_level;
    Counter* counter_start_level;

    // dragons & monsters & bubbles & items
    Dragon* bub;
    Monster* monster_list;
    Bubble* bubble_list;
    Item* item_list;

    // keys
    bool key_pressed[E_SIZE];
    Counter* key_counters[E_SIZE];
    int key_interval;
} Game;

void keys_init(Game* game);
void keys_delete(Game* game);
void keys_update_interval(Game *game);
void keys_reset(Game *game);

void key_down(Game *game, int key);
void key_up(Game *game, int key);

bool key_fired(Game* game, int key);

#endif //BUBBLES_GAME_BASE_H
