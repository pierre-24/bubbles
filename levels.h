//
// Created by pbeaujea on 3/21/18.
//

#ifndef BUBBLES_LEVELS_H
#define BUBBLES_LEVELS_H

#include "utils.h"
#include "images.h"
#include "game_object_definitions.h"

#define SPRITE_LEVEL_WIDTH 16
#define SPRITE_LEVEL_HEIGHT 16

#define MAP_WIDTH 32
#define MAP_HEIGHT 24

typedef struct Position_ {
    unsigned int x;
    unsigned int y;
}  Position;

int position_index(Position pos);

typedef struct Level_ {
    bool map[MAP_HEIGHT * MAP_WIDTH];
    Position bubble_endpoint;
    Sprite* fill_tile;
    unsigned int num_monsters;
    MonsterDef** monsters; // dynamically allocated
    Position* monster_positions; // dynamically allocated
    struct Level_* next; // NULL terminated
} Level;

Level* level_new(bool *map, Position bubble_endpoint, Sprite *fill_tile, unsigned int num_monsters,
                 MonsterDef **monsters, Position *monster_positions);
void level_delete(Level* level);

Level *level_new_from_string(char *buffer, int *position, Image *image_level, MonsterDef **base_monster_defs,
                             int num_monster_defs);
Level *levels_new_from_file(FILE *f, Image *image_level, MonsterDef **base_monster_defs, int num_monster_defs,
                            unsigned int *num_levels);

#endif //BUBBLES_LEVELS_H
