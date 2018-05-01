//
// Created by pbeaujea on 3/21/18.
//

#ifndef BUBBLES_LEVELS_H
#define BUBBLES_LEVELS_H

#include "utils.h"
#include "images.h"
#include "object_definitions.h"

#define TILE_WIDTH 16 // [pixels]
#define TILE_HEIGHT 16 // [pixels]

#define MAP_WIDTH 32 // [cases]
#define MAP_HEIGHT 24 // [cases]

#define CONTACT_DISTANCE 4 // [cases²]

typedef struct Position_ {
    /* Represent a position, as a number of case.
     * */
    unsigned int x;
    unsigned int y;
}  Position;

int position_index(Position pos);

typedef struct Level_ {
	/* Represent a level (NULL terminated chained list).
	 *
	 * A level is a map of cases, each case may be occupied by a wall or not.
	 * Each level contain a list of monsters definition (and their positions), used to create the monsters at the beginning of a level.
	 * There is also a bubble endpoint, which is the target of any bubble.
	 * */
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

typedef struct LevelObject_ {
    /* Represent the position (and some other stuffs) of any object that can be on a level.
     *
     * The "real position" of an object is computed as:
     * - `x + (move_forward ? -1 : 1) * counter_x / MOVE_EVERY`,
     * - `y + (is_falling ? 1 : -1) * counter_y / MOVE_EVERY`.
     *
     * A displacement is only possible when `counter_x` is stopped, and a jump only possible when `counter_y` is stopped.
     * During a jump, the object goes up as many time as `counter_jump`.
     *
     * An object can also chase each other. The object can choose where to go every time `counter_chase` is stopped.
     *
     * A object can "fall from above" (the level), to a `target` position. If so, the collision tests are deactivated, and the object fall until it reaches its target.
     * */
    Position position;
    int width;
    int height;

    bool move_forward;
    Counter* counter_x;
    Counter* counter_y;
    Counter* counter_jump;
    Counter* counter_chase;
    
    bool is_falling;

    // for the beginning of a level
	bool falling_from_above;
    Position target_position;
} LevelObject;

LevelObject *level_object_new(Position position, int width, int height);
void level_object_delete(LevelObject *map_object);

LevelObject* level_object_copy(LevelObject *src);

bool level_object_test_left(LevelObject *representation, Level *level);
bool level_object_test_right(LevelObject *representation, Level *level);
bool level_object_test_up(LevelObject *representation, Level *level);
bool level_object_test_down(LevelObject *representation, Level *level);

bool level_object_can_move(LevelObject *obj);
bool level_object_can_jump(LevelObject *obj);

bool level_object_move_left(LevelObject *representation, Level *level);
bool level_object_move_right(LevelObject *representation, Level *level);
bool level_object_jump(LevelObject *representation, Level *level, int jump);
void level_object_adjust(LevelObject *representation, Level *level);

void level_object_chase(LevelObject *moving, LevelObject *target, Level *level, int speed);

#define FALLING_FROM (MAP_HEIGHT + 2)

void map_object_set_falling_from_above(LevelObject *obj, Position target);

typedef struct EffectivePosition_ {
    /* Represent the actual position of an object, as a fractional number of cases.
     * */
	float x;
	float y;
} EffectivePosition;

EffectivePosition level_object_to_effective_position(LevelObject *mobj);
bool level_object_in_collision(LevelObject *a, LevelObject *b);

void compute_real_pixel_positions(LevelObject *obj, int *px, int *py);

#endif //BUBBLES_LEVELS_H
