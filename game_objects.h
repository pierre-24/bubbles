//
// Created by pbeaujea on 3/30/18.
//

#ifndef BUBBLES_GAME_OBJECTS_H
#define BUBBLES_GAME_OBJECTS_H

#include "utils.h"
#include "game_object_definitions.h"
#include "levels.h"

enum {
    DA_NORMAL,
    DA_MOVE,
    DA_BLOW,
    DA_HIT,
    DA_INVICIBLE,
    DA_NUMBER
};

#define DRAGON_WIDTH 32 // [pixels]
#define DRAGON_HEIGHT 32 // [pixels]
#define DRAGON_FRAMERATE 10 // [frames]

#define DRAGON_LIFE 3

#define DRAGON_JUMP 6 // [height] of jump
#define DRAGON_INVINCIBILITY 120 // [frames]
#define DRAGON_HIT 60 // [frames]
#define DRAGON_BLOW 4 // [frames]

typedef struct Dragon_ {
    bool is_bub; // if not, this is Bob
    MapObject* map_object;
    Position respawn_position; // to respawn
    Animation* animations[DA_NUMBER]; // animations
    unsigned int score;
    int life; // 3 at the beginning
    int max_life; // 3 at the beginning

    bool hit; // hit!
    bool invincible; // when just killed
    Counter* counter_hit;
    Counter* counter_blow;
    Counter* counter_invincible;
} Dragon;

Dragon* dragon_new(MapObject *map_object, bool is_bub, Animation **animation);
void dragon_delete(Dragon* dragon);

void dragon_adjust(Dragon *dragon, Level *level);

void blit_dragon(Dragon *dragon, bool frozen, int shift_y);

#define POSITION_BUB (Position) {3, 1}

Dragon* create_bub(Image* texture, int y);

/*
Dragon* create_bob(Image* texture, int y);
#define POSITION_BOB (Position) {27, 1}*/

#define MONSTER_WIDTH 32
#define MONSTER_HEIGHT 32
#define MONSTER_FRAMERATE 10
#define MONSTER_JUMP 6 // height of jump
#define BUB_Y 0

typedef struct Monster_ {
    MapObject* map_object;
    MonsterDef* definition;
    Animation* animation[MA_NUMBER];
    bool angry;
    bool in_bubble;
    struct Monster_* next;
} Monster;

Monster* monster_new(MapObject* map_object, MonsterDef* definition);
void monster_delete(Monster* monster);

Monster* monsters_new_from_level(Level* level);
Monster* monster_kill(Monster* list, Monster* monster);
void monsters_adjust(Monster* list, Level* level, MapObject* target);

bool test_collide_other_monsters(Monster* moving, Monster* list, MapObject* npos);

void blit_monster(Monster *monster, bool frozen);

#define ITEM_JUMP 8
#define ITEM_INVULNERABILITY 30

typedef struct Item_ {
    MapObject* map_object;
    ItemDef* definition;
    bool go_right;
    Counter* counter_invulnerability;
    struct Item_* next;
} Item;

Item* item_new(MapObject* map_object, ItemDef* definition);
void item_delete(Item* item);

Item *item_create(MapObject *position, Item *list, ItemDef **definitions, int num_item_definitions, Level *level,
                  bool look_right);
Item* dragon_consume_item(Dragon* dragon, Item* list, Item* item);

void items_adjust(Item* list, Level* level);

void blit_item(Item* item);

typedef struct Bubble_ {
    MapObject* map_object;
    Animation* animation;

    Monster* captured; // NULL in the beginning

    Counter* counter_momentum; // counter until it moves on its own
    Counter* counter_time_left; // counter until it auto-burst
    EffectivePosition force;
    struct Bubble_* next; // NULL terminated
} Bubble;

#define BUBBLE_WIDTH 32
#define BUBBLE_HEIGHT 32
#define BUBBLE_FRAMERATE 10

#define BUBBLE_MOMENTUM 10
#define BUBBLE_TIME 600

#define BUBBLE_X 0
#define BUBBLE_Y 32

#define BUBBLE_K_POS .5f
#define BUBBLE_K_INTER 1.f
#define BUBBLE_REQ_POS .5f
#define BUBBLE_REQ_INTER 3.f
#define BUBBLE_MIN_FORCES 5.f // avoid shakiness of the bubbles

Bubble *bubble_new(MapObject *map_object, Image *texture, bool go_right);
void bubble_delete(Bubble* bubble);

Bubble* bubbles_adjust(Bubble *bubble_list, Level *level, Position final_position);

Bubble* dragon_blow(Dragon* dragon, Bubble* bubble_list, Image* texture);
Bubble *bubble_burst(Bubble *bubble_list, Bubble *bubble, bool free_monster);

void blit_bubble(Bubble *bubble, bool frozen);

#endif //BUBBLES_GAME_OBJECTS_H
