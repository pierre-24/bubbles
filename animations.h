//
// Created by pierre on 3/20/18.
//

#ifndef BUBBLES_ANIMATIONS_H
#define BUBBLES_ANIMATIONS_H

#include "images.h"
#include "utils.h"

typedef struct Animation_ {
    /* Define an animation.
     * 
     * An animation is a circular chained list, which contain a set of sprites (frames) that are shown for a given time (`framerate`).
     * The sentinel of this list is a structure with a NULL `frame`.
     * The `counter` value helps to determine wheter to change frame or not.
     */
    Sprite* frame;
    struct Animation_* next_frame;
    int framerate;
    int counter;
} Animation;

Animation *animation_new(int framerate);
void animation_delete(Animation* animation);

Animation * animation_add_frame(Animation *animation, Sprite *frame);
Animation * animation_next(Animation *animation);
void animation_animate(Animation **animation);

Animation* animation_copy(Animation* src);

void blit_animation(Animation *animation, int sx, int sy, bool flip_x, bool flip_y);

#endif //BUBBLES_ANIMATIONS_H
