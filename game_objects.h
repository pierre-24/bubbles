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
    DA_DEAD,
    DA_INVICIBLE,
    DA_NUMBER
};

#define DRAGON_WIDTH 32
#define DRAGON_HEIGHT 32
#define DRAGON_FRAMERATE 10

#define DRAGON_LIFE 3

#define DRAGON_JUMP 8 // height of jump

typedef struct Dragon_ {
    bool is_bub; // if not, this is Bob
    MapObject* representation;
    Animation* animations[DA_NUMBER]; // animations
    unsigned int score;
    unsigned int life; // 3 at the beginning
    unsigned int max_life; // 3 at the beginning
    bool invincible; // when just killed
    unsigned int invincibility_counter;
} Dragon;

Dragon* dragon_new(MapObject *representation, bool is_bub, Animation **animation);
void dragon_delete(Dragon* dragon);

#define REPRESENTATION_BUB (Position) {1, 1}, 2, 2, true
#define POSITION_BOB (Position) {31, 1}, 2, 2, true

Dragon* create_bub(Image* texture, int y);
Dragon* create_bob(Image* texture, int y);

#endif //BUBBLES_GAME_OBJECTS_H
