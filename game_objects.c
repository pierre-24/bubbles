//
// Created by pbeaujea on 3/30/18.
//

#include "game_objects.h"

Dragon* dragon_new(MapObject *representation, bool is_bub, Animation **animation) {
    Dragon* dragon = malloc(sizeof(Dragon));

    if (dragon == NULL) {
        write_log("! cannot allocate dragon :(");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Dragon %p\n", dragon);
#endif

    dragon->representation = map_object_copy(representation);

    if (dragon->representation == NULL) {
        dragon_delete(dragon);
        return NULL;
    }

    dragon->score = 0;
    dragon->life = DRAGON_LIFE;
    dragon->max_life = DRAGON_LIFE;
    dragon->invincible = false;
    dragon->invincibility_counter = 0;
    dragon->is_bub = is_bub;

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

        map_object_delete(dragon->representation);

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

    MapObject* obj = map_object_new(POSITION_BUB, DRAGON_WIDTH / TILE_WIDTH, DRAGON_HEIGHT / TILE_HEIGHT, true);

    if (obj == NULL)
        return NULL;

    Dragon* dragon = dragon_new(obj, true, animations);

    for (int j = 0; j < DA_NUMBER; ++j)
        animation_delete(animations[j]);

    map_object_delete(obj);

    if (dragon == NULL)
        return NULL;

    return dragon;
}

Dragon* create_bob(Image* texture, int y) {
    Dragon* dragon = create_bub(texture, y);
    if (dragon != NULL)
        dragon->is_bub = false;

    return dragon;
}

Monster* monster_new(MapObject* representation, MonsterDef* definition) {
    Monster* monster = malloc(sizeof(Monster));

    if (monster == NULL) {
        write_log("! unable to allocate monster");
        return NULL;
    }


#ifdef VERBOSE_MEM
    printf("+Monster %p\n", monster);
#endif

    monster->representation = map_object_copy(representation);
    monster->animation = animation_copy(definition->animation);

    if (monster->representation == NULL || monster->animation == NULL) {
        monster_delete(monster);
        return NULL;
    }

    monster->in_bubble = false;
    monster->angry = false;
    monster->invincible = false;
    monster->definition = definition;
    monster->next = NULL;

    return monster;
}

void monster_delete(Monster* monster) {
    Monster* next = monster, *t = NULL;
    while (next != NULL) {
        t = next->next;

        map_object_delete(next->representation);
        animation_delete(next->animation);

        free(next);
        next = t;

#ifdef VERBOSE_MEM
        printf("-Monster %p\n", monster);
#endif
    }
}

Monster* monsters_new_from_level(Level* level) {
    Monster* m = NULL, *t = NULL, *beg = NULL;
    for (int i = 0; i < level->num_monsters; ++i) {
        MapObject* obj = map_object_new(level->monster_positions[i], MONSTER_WIDTH / TILE_WIDTH, MONSTER_HEIGHT / TILE_HEIGHT, true);
        if (obj == NULL)
            continue;

        t = monster_new(obj, level->monsters[i]);

        if (t == NULL)
            continue;

        if (m == NULL) {
            m = t;
            beg = t;
        }
        else {
            m->next = t;
            m = t;
        }

        free(obj);
    }

    return beg;
}