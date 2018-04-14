//
// Created by pbeaujea on 3/21/18.
//

#ifndef BUBBLES_LEVELS_H
#define BUBBLES_LEVELS_H

#include "utils.h"
#include "images.h"
#include "game_object_definitions.h"

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define MAP_WIDTH 32 // cases
#define MAP_HEIGHT 24 // cases

#define CONTACT_DISTANCE 3 // [cases²]

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

void blit_level(Level *level, int y_shift);

#define MOVE_EVERY 4 // [frames]

typedef struct MapObject_ {
    Position position;
    int width;
    int height;

    bool move_forward;
    Counter* counter_x;
    Counter* counter_y;
    Counter* counter_jump;
    Counter* counter_chase;
    
    bool is_falling;

    // for the begining of a level
	bool falling_from_above;
    Position target_position;
} MapObject;

MapObject *map_object_new(Position position, int width, int height);
void map_object_delete(MapObject *map_object);

MapObject* map_object_copy(MapObject *src);

bool map_object_test_left(MapObject *representation, Level* level);
bool map_object_test_right(MapObject *representation, Level* level);
bool map_object_test_up(MapObject *representation, Level* level);
bool map_object_test_down(MapObject *representation, Level* level);

bool map_object_can_move(MapObject* obj);
bool map_object_can_jump(MapObject* obj);

bool map_object_move_left(MapObject *representation, Level *level);
bool map_object_move_right(MapObject *representation, Level *level);
bool map_object_jump(MapObject *representation, Level *level, int jump);
void map_object_adjust(MapObject *representation, Level* level);

void map_object_chase(MapObject *moving, MapObject *target, Level *level, int speed);

#define FALLING_FROM (MAP_HEIGHT + 2)

void map_object_set_falling_from_above(MapObject *obj, Position target);

typedef struct EffectivePosition_ {
	float x;
	float y;
} EffectivePosition;

EffectivePosition map_object_to_effective_position(MapObject* mobj);
bool map_object_in_collision(MapObject* a, MapObject* b);

void compute_real_pixel_positions(MapObject *obj, int *px, int *py);

#endif //BUBBLES_LEVELS_H
