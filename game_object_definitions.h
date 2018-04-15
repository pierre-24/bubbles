#ifndef GAME_OBJECT_DEFINITIONS_H
#define GAME_OBJECT_DEFINITIONS_H

#include "images.h"
#include "animations.h"
#include "datafile.h"

// items
typedef enum extra_power_t_ {
	EP_NONE,
	EP_ADD_LIFE,
	EP_ADD_EXTRA_LIFE,
	EP_FULL_HEAL,
	EP_NUMBER
} extra_power_t;

typedef struct ItemDef_ {
	unsigned int points_given;
	Sprite* sprite;
	extra_power_t extra_power;
} ItemDef;

ItemDef *item_def_new(unsigned int points_given, extra_power_t extra_power, Sprite *sprite);
void item_def_delete(ItemDef* item);

#define ITEM_WIDTH 32
#define ITEM_HEIGHT 32

ItemDef** item_defs_from_file(FILE* f, Image* items_texture, unsigned int* size);

// monsters
enum {
    MA_NORMAL,
    MA_ANGRY,
    MA_CAPTURED,
    MA_NUMBER
};

typedef struct MonsterDef_ {
	Animation* animation[MA_NUMBER];
	unsigned int speed; // number of frames between two movements
} MonsterDef;

MonsterDef* monster_def_new(Animation **sprite_animation, unsigned int speed);
void monster_def_delete(MonsterDef* item);

#define MONSTER_WIDTH 32
#define MONSTER_HEIGHT 32
#define MONSTER_FRAMERATE 10

MonsterDef** monster_defs_from_file(FILE* f, Image* items_texture, unsigned int* size);

#endif
