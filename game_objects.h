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

#define DRAGON_WIDTH 32
#define DRAGON_HEIGHT 32
#define DRAGON_FRAMERATE 10

#define DRAGON_LIFE 3

#define DRAGON_JUMP 6 // [height] of jump
#define DRAGON_INVINCIBILITY 120 // [frames]
#define DRAGON_HIT 60 // [frames]
#define DRAGON_BLOW 4 // [frames]

typedef struct Dragon_ {
    bool is_bub; // if not, this is Bob
    MapObject* representation;
    Position respawn_position; // to respawn
    Animation* animations[DA_NUMBER]; // animations
    unsigned int score;
    unsigned int life; // 3 at the beginning
    unsigned int max_life; // 3 at the beginning

    bool hit; // hit!
    int hit_counter;
    bool invincible; // when just killed
    int invincibility_counter;
    int blow_counter;
} Dragon;

Dragon* dragon_new(MapObject *representation, bool is_bub, Animation **animation);
void dragon_delete(Dragon* dragon);

void dragon_adjust(Dragon *dragon, Level *level);

#define POSITION_BUB (Position) {3, 1}
#define POSITION_BOB (Position) {27, 1}

Dragon* create_bub(Image* texture, int y);
Dragon* create_bob(Image* texture, int y);

#define MONSTER_WIDTH 32
#define MONSTER_HEIGHT 32
#define MONSTER_FRAMERATE 10
#define MONSTER_JUMP 6 // height of jump

typedef struct Monster_ {
    MapObject* representation;
    MonsterDef* definition;
    Animation* animation[MA_NUMBER];
    bool angry;
    bool in_bubble;
    struct Monster_* next;
} Monster;

Monster* monster_new(MapObject* representation, MonsterDef* definition);
void monster_delete(Monster* monster);

Monster* monsters_new_from_level(Level* level);
Monster* monster_kill(Monster* list, Monster* monster);

#define ITEM_JUMP 6

typedef struct Item_ {
    MapObject* representation;
    ItemDef* definition;
    bool go_right;
    struct Item_* next;
} Item;

Item* item_new(MapObject* representation, ItemDef* definition);
void item_delete(Item* item);

Item *create_item(MapObject *position, Item *list, ItemDef *definitions[], int num_item_definitions, Level *level,
                  bool look_right);
Item* dragon_consume_item(Dragon* dragon, Item* list, Item* item);

void items_adjust(Item* list, Level* level);

#define BUBBLE_TRANSLATE_EVERY 4

typedef struct Bubble_ {
    MapObject* representation;
    Animation* animation;
    int momentum; // counter until it moves on its own
    int time_left; // counter until it auto-burst
    Monster* captured; // NULL in the beginning
    int translate_counter;
    bool go_up;
    bool translating;
    struct Bubble_* next; // NULL terminated
} Bubble;

#define BUBBLE_WIDTH 32
#define BUBBLE_HEIGHT 32
#define BUBBLE_FRAMERATE 10

#define BUBBLE_MOMENTUM 10
#define BUBBLE_TIME 500

#define BUBBLE_X 0
#define BUBBLE_Y 32

Bubble *bubble_new(MapObject *representation, Image *texture, bool go_right);
void bubble_delete(Bubble* bubble);

Bubble* adjust_bubbles(Bubble* bubble_list, Level* level, Position final_position);

Bubble* dragon_blow(Dragon* dragon, Bubble* bubble_list, Image* texture);
Bubble *bubble_burst(Bubble *bubble_list, Bubble *bubble, bool free_monster);

#endif //BUBBLES_GAME_OBJECTS_H
