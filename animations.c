//
// Created by pierre on 3/20/18.
//

#include "game.h"
#include "animations.h"

Animation *animation_new(int framerate) {
    /* Create an animation.
     *
     * Only create the sentinel.
     * */
    Animation* a = malloc(sizeof(Animation));

    if (a == NULL) {
        write_log("! unable to allocate Animation");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+animation %p\n", a);
#endif

    a->frame = NULL;
    a->next_frame = a;
    a->framerate = framerate;
    a->counter = framerate;

    return a;
}

void animation_delete(Animation* animation) {
    /* Delete an animation (and the sprites)
     * */
    if (animation != NULL) {
        Animation* p = animation, *n = NULL;

        while (p->frame != NULL)
            p = p->next_frame;

        p = p->next_frame;
        bool was_last = false;

        while (!was_last) {
            n = p->next_frame;

            if (p->frame == NULL)
                was_last = true;
            else
                sprite_delete(p->frame);

            free(p);

#ifdef VERBOSE_MEM
            printf("-animation %p\n", p);
#endif
            p = n;
        }
    }
}

Animation * animation_add_frame(Animation *animation, Sprite *frame) {
    /* Add a frame to the animation.
     *
     * Insert the frame as the last frame of the animation.
     * */

    if (animation == NULL || frame == NULL)
        return NULL;

    // find end
    Animation* p = animation, *q = animation;
    if (p->frame == NULL)
        p = p->next_frame;

    while (p->frame != NULL) {
        q = p;
        p = p->next_frame;
    } // now, p contains the sentinel, q the frame before (or the sentinel, if that's the only frame)

    Animation* nx = malloc(sizeof(Animation));
    if (nx != NULL) {
        nx->frame = sprite_copy(frame);

#ifdef VERBOSE_MEM
        printf("+animation %p (by add)\n", nx);
#endif

        if (nx->frame == NULL)
            return NULL;

        nx->framerate = p->framerate;
        nx->counter = p->framerate;

        nx->next_frame = p;
        q->next_frame = nx;

        return nx;
    }

    write_log("! unable to allocate new animation");
    return NULL;
}

Animation * animation_next(Animation *animation) {
    /* Get the next frame, no matter the value of the counter.
     *
     * Also set the counter for this next frame.
     * */
    if (animation == NULL)
        return NULL;

    if (animation->next_frame == NULL)
        return animation; // return the sentinel if that is the only frame

    Animation* n = animation->next_frame;
    if (n->frame == NULL) // skip sentinel
        n =  n->next_frame;

    n->counter = n->framerate;
    return n;
}

void animation_animate(Animation **animation) {
    /* Tick the counter and set the frame accordingly.
     * */

    if (animation != NULL && *animation != NULL) {
        if ((*(animation))->framerate > 0) {
            if ((*(animation))->counter < 1) {
                *animation = animation_next(*animation);
            }

            else
                (*(animation))->counter--;
        }
    }

}

Animation* animation_copy(Animation* src) {
    /* Copy the animation (and the sprites)
     * */

    if (src == NULL)
        return NULL;

    Animation* p = src;
    Animation* dest = animation_new(src->framerate);

    if (dest == NULL) {
        write_log("! unable to allocate animation for copy");
        return NULL;
    }

    while (p->frame != NULL)
        p = p->next_frame;

    p = p->next_frame;

    while (p->frame != NULL) {
        dest = animation_add_frame(dest, p->frame);
        p = p->next_frame;
    }

    return dest;
}

void blit_animation(Animation *animation, int sx, int sy, bool flip_x, bool flip_y) {
    /* Blit the animation.
     *
     * Take the same parameters as `blit_sprite()`.
     * */
    if (animation != NULL && animation->frame != NULL) {
        blit_sprite(animation->frame, sx, sy, flip_x, flip_y);
    }
}