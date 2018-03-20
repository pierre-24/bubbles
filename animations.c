//
// Created by pierre on 3/20/18.
//

#include "animations.h"

Animation* animation_new() {
    Animation* a = malloc(sizeof(Animation));

    if (a == NULL) {
        write_log("! unable to allocate Animation");
        return NULL;
    }

    a->frame = NULL;
    a->next_frame = a;
}

void animation_delete(Animation* animation) {
    if (animation != NULL) {
        Animation* beg = animation->next_frame;
        Animation* current = animation;
        while (current != beg && current != NULL) {
            Animation* n = current->next_frame;
            sprite_delete(current->frame);
            free(current);
            current = n;
        }
    }
}

Animation * animation_add_frame(Animation *animation, Sprite *frame) {
    // find end
    Animation* p = animation, *q = NULL;
    while (p->next_frame->frame != NULL)
        p = p->next_frame;

    q = p->next_frame;

    Animation* nx = malloc(sizeof(Animation));
    if (nx != NULL) {
        nx->frame = sprite_copy(frame);

        if (nx->frame == NULL)
            return NULL;

        nx->next_frame = q;
        p->next_frame = nx;

        return nx;
    }

    write_log("! unable to alocate new animation");
    return NULL;
}

Sprite* animation_get_frame(Animation *animation) {
    return animation->frame;
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
