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

    dragon->respawn_position = dragon->representation->position;

    dragon->score = 0;
    dragon->life = DRAGON_LIFE;
    dragon->max_life = DRAGON_LIFE;
    dragon->is_bub = is_bub;


    dragon->invincible = false;
    dragon->invincibility_counter = 0;
    dragon->hit = false;
    dragon->hit_counter = 0;

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

void dragon_adjust(Dragon* dragon) {
    if (dragon->hit) {
        dragon->hit_counter--;

        if (dragon->hit_counter == 0) {

            dragon->hit = false;
            dragon->invincible = true;
            dragon->invincibility_counter = DRAGON_INVINCIBILITY;

            dragon->representation->position = dragon->respawn_position;
            dragon->life--;
        }
    }

    else if (dragon->invincible) {
        dragon->invincibility_counter--;
        if (dragon->invincibility_counter == 0)
            dragon->invincible = false;
    }
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

    monster->next = NULL;

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
        printf("-Monster %p\n", next);
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

Bubble *bubble_new(MapObject *representation, Image *texture, bool go_right) {
    if (representation == NULL || texture == NULL)
        return NULL;

    Bubble* bubble = malloc(sizeof(Bubble));

    if (bubble == NULL) {
        write_log("! unable to allocate bubble");
        return NULL;
    }


#ifdef VERBOSE_MEM
    printf("+Bubble %p\n", bubble);
#endif

    bubble->next = NULL;

    bubble->representation = map_object_copy(representation);
    bubble->representation->look_right = go_right;

    bubble->animation = animation_new(BUBBLE_FRAMERATE);

    Sprite* sprite1 = sprite_new(texture, BUBBLE_X, BUBBLE_Y, BUBBLE_WIDTH, BUBBLE_HEIGHT),
            *sprite2 = sprite_new(texture, BUBBLE_X + BUBBLE_WIDTH, BUBBLE_Y, BUBBLE_WIDTH, BUBBLE_HEIGHT);

    bubble->animation = animation_add_frame(bubble->animation, sprite1);
    bubble->animation = animation_add_frame(bubble->animation, sprite2);

    sprite_delete(sprite1);
    sprite_delete(sprite2);

    if (bubble->representation == NULL || bubble->animation == NULL) {
        bubble_delete(bubble);
        return NULL;
    }

    bubble->captured = NULL;
    bubble->momentum = BUBBLE_MOMENTUM;
    bubble->time_left = BUBBLE_TIME;
    bubble->translate_counter = 0;
    bubble->translating = false;

    return bubble;
}

void bubble_delete(Bubble* bubble) {
    Bubble* next = bubble, *t = NULL;
    while (next != NULL) {
        t = next->next;

        map_object_delete(next->representation);
        animation_delete(next->animation);

        free(next);
        next = t;

#ifdef VERBOSE_MEM
        printf("-Bubble %p\n", next);
#endif
    }
}

bool collide_previous_bubble(Bubble *a, Bubble *list) {
    Bubble* t = list;

    while (t != NULL) {
        if (t == a)
            break;

        if (map_object_in_collision(t->representation, a->representation))
            return true;

        t = t->next;
    }

    return false;
}

Bubble* adjust_bubbles(Bubble* bubble_list, Level* level, Position final_position) {
    Bubble* bubble = bubble_list, *prev = NULL, *first = bubble_list;

    while (bubble != NULL)  {
        if (bubble->representation->moving_counter == 0 && bubble->translate_counter == 0) {
            if(!collide_previous_bubble(bubble, first)) {
                if (bubble->momentum > 0) {
                    if (bubble->representation->look_right) {
                        if (map_object_test_right(bubble->representation, level)) {
                            map_object_move_right(bubble->representation, level);
                            bubble->momentum--;
                        }
                        else
                            bubble->momentum = 0;
                    }

                    else {
                        if (map_object_test_left(bubble->representation, level)) {
                            map_object_move_left(bubble->representation, level);
                            bubble->momentum--;
                        }
                        else
                            bubble->momentum = 0;
                    }
                } else {
                    if (bubble->representation->position.y != final_position.y) {
                        bubble->representation->position.y += 1 * (bubble->representation->position.y < final_position.y ? 1 : -1);
                        bubble->translate_counter = BUBBLE_TRANSLATE_EVERY;
                        bubble->go_up = bubble->representation->position.y < final_position.y;
                        bubble->translating = true;
                    }

                    else if (bubble->representation->position.x != final_position.x) {
                        if (bubble->representation->position.x < final_position.x)
                            map_object_move_right(bubble->representation, level);
                        else
                            map_object_move_left(bubble->representation, level);

                        bubble->translating = false;
                    }
                }
            }
        }

        else {
            if (bubble->representation->moving_counter > 0)
                bubble->representation->moving_counter--;
            if (bubble->translate_counter > 0)
                bubble->translate_counter--;
        }

        bubble->time_left--;

        if (bubble->time_left == 0) {
            if (prev == NULL) {
                first = bubble->next;
                prev = first;
            }
            else {
                prev->next = bubble->next;
            }
            bubble->next = NULL;
            bubble_delete(bubble);
            bubble = prev;

        } else {
            prev = bubble;
            bubble = bubble->next;
        }
    }

    return first;
}

Bubble* dragon_blow(Dragon* dragon, Bubble* bubble_list, Image* texture) {
    Position p = dragon->representation->position;
    p.x += dragon->representation->look_right ? 1 : -1;

    MapObject* m = map_object_new(p, BUBBLE_WIDTH / TILE_WIDTH, BUBBLE_HEIGHT / TILE_HEIGHT, true);
    Bubble* bubble = bubble_new(m, texture, dragon->representation->look_right);

    if (bubble == NULL) {
        return bubble_list;
    }

    map_object_delete(m);

    Bubble* last = bubble_list;
    if (last != NULL) {
        while (last->next != NULL)
            last = last->next;
        last->next = bubble;
        return bubble_list;
    } else
        return bubble;
}