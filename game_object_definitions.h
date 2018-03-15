#ifndef GAME_OBJECT_DEFINITIONS_H
#define GAME_OBJECT_DEFINITIONS_H

#include "textures.h"
#include "datafile.h"

// items
typedef enum extra_power_t_ {
	EP_NONE,
	EP_ADD_LIFE,
	EP_ADD_EXTRA_LIFE,
	// some other powers
	EP_NUMBER
} extra_power_t;

typedef struct ItemDef_ {
	unsigned int points_given;
	Sprite* sprite;
	extra_power_t extra_power;
} ItemDef;


ItemDef* item_def_new(unsigned int points_given, Sprite* sprite, extra_power_t extra_power);
void item_def_delete(ItemDef* item);

ItemDef** item_defs_from_file(FILE* f, Texture* items_texture, unsigned int* size);

// monsters
typedef struct MonsterDef_ {
	Sprite* sprite_animation[2];  // two sprites for the animation
	unsigned int speed; // number of frames between two movements
} MonsterDef;

MonsterDef* monster_def_new(Sprite* sprite_animation[2], unsigned int speed);
void imonster_def_delete(MonsterDef* item);

MonsterDef** monster_defs_from_file(FILE* f, Texture* items_texture, unsigned int* size);

#endif
