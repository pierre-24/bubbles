//
// Created by pbeaujea on 3/30/18.
//

#include "game_objects.h"

Dragon* dragon_new(Position position, bool is_bub, Animation* animation[DA_NUMBER]) {
    Dragon* dragon = malloc(sizeof(Dragon));

    if (dragon == NULL) {
        write_log("! cannot allocate dragon :(");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Dragon %p\n", dragon);
#endif

    dragon->current_position.x = position.x;
    dragon->current_position.y = position.y;

    dragon->score = 0;
    dragon->life = DRAGON_LIFE;
    dragon->max_life = DRAGON_LIFE;
    dragon->invicible = false;
    dragon->invincibility_counter = 0;
    dragon->is_bub = is_bub;
    dragon->look_right = is_bub;
    dragon->is_moving = 0;
    dragon->is_jumping = 0;
    dragon->is_falling = false;

    for (int i = 0; i < DA_NUMBER; ++i) {
        dragon->animations[i] = animation_copy(animation[i]);
        if (dragon->animations[i] == NULL) {
            dragon_delete(dragon);
            return NULL;
        }
    }

    return dragon;
}

void dragon_delete(Dragon* dragon) {
    if (dragon != NULL) {

        for (int i = 0; i < DA_NUMBER; ++i) {
            if (dragon->animations[i] != NULL)
                animation_delete(dragon->animations[i]);
        }

        free(dragon);

#ifdef VERBOSE_MEM
        printf("-Dragon %p\n", dragon);
#endif
    }
}

Dragon* create_bub(Image* texture, int y) {
    // create animations
    Animation* animations[DA_NUMBER];

    for (int i = 0; i < DA_NUMBER; ++i) {
        Sprite* sprite1 = sprite_new(texture, i * 2 * DRAGON_WIDTH, y, DRAGON_WIDTH, DRAGON_HEIGHT);
        Sprite* sprite2 = sprite_new(texture, (i * 2 + 1) * DRAGON_WIDTH, y, DRAGON_WIDTH, DRAGON_HEIGHT);

        if (sprite1 == NULL || sprite2 == NULL) {
            return NULL;
        }

        animations[i] = animation_new(DRAGON_FRAMERATE);

        if (animations[i] == NULL) {
            sprite_delete(sprite1);
            sprite_delete(sprite2);
            return NULL;
        }

        animations[i] = animation_add_frame(animations[i], sprite1);
        animations[i] = animation_add_frame(animations[i], sprite2);

        sprite_delete(sprite1);
        sprite_delete(sprite2);
    }

    Dragon* dragon = dragon_new((Position) POSITION_BUB , true, animations);

    for (int j = 0; j < DA_NUMBER; ++j)
        animation_delete(animations[j]);

    if (dragon == NULL)
        return NULL;

    return dragon;
}

Dragon* create_bob(Image* texture, int y) {
    return NULL;
}
