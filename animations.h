//
// Created by pierre on 3/20/18.
//

#ifndef BUBBLES_ANIMATIONS_H
#define BUBBLES_ANIMATIONS_H

#include "images.h"
#include "utils.h"

typedef struct Animation_ {
    Sprite* frame;
    struct Animation_* next_frame;
} Animation;

Animation* animation_new();
void animation_delete(Animation* animation);

Animation * animation_add_frame(Animation *animation, Sprite *frame);
Animation * animation_next(Animation *animation);

Animation* animation_copy(Animation* src);

#endif //BUBBLES_ANIMATIONS_H
