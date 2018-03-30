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

typedef struct Dragon_ {
    Position current_position;
    unsigned int score;
    unsigned int life; // 3 at the begining
    unsigned int max_life; // 3 at the begining
    bool invicible; // when just killed
    unsigned int invincibility_counter;
    bool is_bub; // if not, this is Bob
    Animation* animations[DA_NUMBER]; // animations
} Dragon;

Dragon* dragon_new(Position position, bool is_bub, Animation* animation[DA_NUMBER]);
void dragon_delete(Dragon* dragon);

#define POSITION_BUB {1, 1}
#define POSITION_BOB {31, 1}

Dragon* create_bob(Image* texture, int y);
Dragon* create_bub(Image* texture, int y);

#endif //BUBBLES_GAME_OBJECTS_H
