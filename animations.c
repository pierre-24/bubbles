//
// Created by pierre on 3/20/18.
//

#include "animations.h"

Animation *animation_new(int framerate) {
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
    // find end
    Animation* p = animation, *q = NULL;
    while (p->frame != NULL)
        p = p->next_frame;

    q = p->next_frame;

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

        nx->next_frame = q;
        p->next_frame = nx;

        return nx;
    }

    write_log("! unable to allocate new animation");
    return NULL;
}

Animation * animation_next(Animation *animation) {
    if (animation == NULL)
        return NULL;

    if (animation->next_frame == NULL)
        return animation;

    Animation* n = animation->next_frame;
    if (n->frame == NULL)
        return n->next_frame;

    return n;
}

void animation_animate(Animation **animation) {
    if ((*(animation))->framerate > 0) {
        if ((*(animation))->counter < 1) {
            *animation = animation_next(*animation);
            (*(animation))->counter = (*(animation))->framerate;
        }

        else
            (*(animation))->counter--;
    }
}

Animation* animation_copy(Animation* src) {
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