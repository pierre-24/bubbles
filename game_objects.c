//
// Created by pbeaujea on 3/30/18.
//

#include "game_objects.h"

Dragon* dragon_new(MapObject *map_object, bool is_bub, Animation **animation) {
    Dragon* dragon = malloc(sizeof(Dragon));

    if (dragon == NULL) {
        write_log("! cannot allocate dragon :(");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Dragon %p\n", dragon);
#endif

    dragon->map_object = map_object_copy(map_object);

    if (dragon->map_object == NULL) {
        dragon_delete(dragon);
        return NULL;
    }

    dragon->respawn_position = dragon->map_object->position;

    dragon->score = 0;
    dragon->life = DRAGON_LIFE;
    dragon->max_life = DRAGON_LIFE;
    dragon->is_bub = is_bub;


    dragon->invincible = false;
    dragon->hit = false;

    dragon->counter_blow = counter_new(DRAGON_BLOW, false, true);
    counter_stop(dragon->counter_blow);

    dragon->counter_hit = counter_new(DRAGON_HIT, false, true);
    dragon->counter_invincible = counter_new(DRAGON_INVINCIBILITY, false, true);

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

        map_object_delete(dragon->map_object);

        counter_delete(dragon->counter_blow);
        counter_delete(dragon->counter_hit);
        counter_delete(dragon->counter_invincible);


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

    MapObject* obj = map_object_new(POSITION_BUB, DRAGON_WIDTH / TILE_WIDTH, DRAGON_HEIGHT / TILE_HEIGHT);

    if (obj == NULL)
        return NULL;

    Dragon* dragon = dragon_new(obj, true, animations);

    for (int j = 0; j < DA_NUMBER; ++j)
        animation_delete(animations[j]);

    map_object_delete(obj);

    if (dragon == NULL)
        return NULL;

    dragon->map_object->move_forward = true;

    return dragon;
}

Dragon* create_bob(Image* texture, int y) {
    Dragon* dragon = create_bub(texture, y);
    if (dragon != NULL) {
        dragon->is_bub = false;
        dragon->map_object->move_forward = false;
    }

    return dragon;
}

void dragon_adjust(Dragon *dragon, Level *level) {
    map_object_adjust(dragon->map_object, level);

    counter_tick(dragon->counter_blow);
    counter_tick(dragon->counter_invincible);
    counter_tick(dragon->counter_hit);

    if (dragon->hit) {

        if (counter_value(dragon->counter_hit) == 0) {

            dragon->hit = false;
            dragon->invincible = true;
            counter_restart(dragon->counter_invincible, -1);

            dragon->map_object->position = dragon->respawn_position;
            dragon->life--;
        }
    }

    else if (dragon->invincible) {
        if (counter_value(dragon->counter_invincible) == 0)
            dragon->invincible = false;
    }
}

Monster* monster_new(MapObject* map_object, MonsterDef* definition) {
    Monster* monster = malloc(sizeof(Monster));

    if (monster == NULL) {
        write_log("! unable to allocate monster");
        return NULL;
    }


#ifdef VERBOSE_MEM
    printf("+Monster %p\n", monster);
#endif

    monster->next = NULL;

    monster->map_object = map_object_copy(map_object);

    for (int i = 0; i < MA_NUMBER; ++i) {
        monster->animation[i] = animation_copy(definition->animation[i]);
        if (monster->animation[i] == NULL) {
            monster_delete(monster);
            return NULL;
        }
    }

    if (monster->map_object == NULL) {
        monster_delete(monster);
        return NULL;
    }

    monster->in_bubble = false;
    monster->angry = false;
    monster->definition = definition;

    return monster;
}

void monster_delete(Monster* monster) {
    Monster* next = monster, *t = NULL;
    while (next != NULL) {
        t = next->next;

        map_object_delete(next->map_object);

        for (int i = 0; i < MA_NUMBER; ++i) {
            animation_delete(next->animation[i]);
        }

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
        MapObject* obj = map_object_new(level->monster_positions[i], MONSTER_WIDTH / TILE_WIDTH,
                                        MONSTER_HEIGHT / TILE_HEIGHT);
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

Monster* monster_kill(Monster* list, Monster* monster) {
    Monster* m = list, *prev = NULL, *first = list;

    while (m != NULL)  {
        if (m == monster) {
            if (prev == NULL) {
                first = m->next;
            }
            else {
                prev->next = m->next;
            }

            m->next = NULL;
            monster_delete(monster);

            break;
        }

        prev = m;
        m = m->next;
    }

    return first;
}

Item* item_new(MapObject* map_object, ItemDef* definition) {
    if(map_object == NULL || definition == NULL)
        return NULL;

    Item* item = malloc(sizeof(Item));

    if (item == NULL) {
        write_log("! cannot allocate item");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Item %p\n", item);
#endif

    item->map_object = map_object_copy(map_object);

    if (item->map_object == NULL) {
        item_delete(item);
        return NULL;
    }

    item->definition = definition;
    item->next = NULL;

    return item;
}

void item_delete(Item* item) {
    Item* i = item, *t = NULL;
    while (i != NULL) {
        if (i->map_object != NULL)
            map_object_delete(i->map_object);

        t = i->next;
        free(i);
        i = t;

#ifdef VERBOSE_MEM
        printf("-Item %p\n", item);
#endif
    }
}

Item *create_item(MapObject *position, Item *list, ItemDef *definitions[], int num_item_definitions, Level *level,
                  bool look_right) {

    if (position == NULL || num_item_definitions == 0)
        return list;

    int item_index = 0;
    int prev = 0;
    int max = num_item_definitions * (num_item_definitions + 1) / 2;
    int random = rand() % max;

    for (int i = 0; i < num_item_definitions; i++) {
        if (random < prev + num_item_definitions - i) {
            item_index = i;
            break;
        }
        else
            prev += num_item_definitions - i;
    }

    MapObject* m = map_object_copy(position);

    if (m == NULL)
        return list;

    Item* item = item_new(m, definitions[item_index]);
    map_object_delete(m);

    if (item == NULL)
        return list;

    item->go_right = look_right;
    map_object_jump(item->map_object, level, ITEM_JUMP);

    if (item->go_right)
        map_object_move_right(item->map_object, level);
    else
        map_object_move_left(item->map_object, level);

    Item* last = list;
    if (last == NULL)
        return item;

    else {
        while (last->next != NULL)
            last = last->next;

        last->next = item;

        return list;
    }
}

Item* dragon_consume_item(Dragon* dragon, Item* list, Item* item) {
    dragon->score += item->definition->points_given;

    Item* it = list, *prev = NULL, *first = list;

    while (it != NULL)  {
        if (it == item) {
            if (prev == NULL) {
                first = it->next;
            }
            else {
                prev->next = it->next;
            }

            it->next = NULL;
            item_delete(item);

            break;
        }

        prev = it;
        it = it->next;
    }

    return first;
 
}

void items_adjust(Item* list, Level* level) {
    Item* it = list;

    while (it != NULL) {
        map_object_adjust(it->map_object, level);

        if (map_object_can_move(it->map_object) && (counter_value(it->map_object->counter_jump) > 0 || it->map_object->is_falling)) {
            if (it->go_right)
                map_object_move_right(it->map_object, level);
            else
                map_object_move_left(it->map_object, level);
        }
        it = it->next;
    }
}

Bubble *bubble_new(MapObject *map_object, Image *texture, bool go_right) {
    if (map_object == NULL || texture == NULL)
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

    bubble->map_object = map_object_copy(map_object);
    bubble->map_object->move_forward = go_right;

    bubble->animation = animation_new(BUBBLE_FRAMERATE);

    Sprite* sprite1 = sprite_new(texture, BUBBLE_X, BUBBLE_Y, BUBBLE_WIDTH, BUBBLE_HEIGHT),
            *sprite2 = sprite_new(texture, BUBBLE_X + BUBBLE_WIDTH, BUBBLE_Y, BUBBLE_WIDTH, BUBBLE_HEIGHT);

    bubble->animation = animation_add_frame(bubble->animation, sprite1);
    bubble->animation = animation_add_frame(bubble->animation, sprite2);

    sprite_delete(sprite1);
    sprite_delete(sprite2);

    if (bubble->map_object == NULL || bubble->animation == NULL) {
        bubble_delete(bubble);
        return NULL;
    }

    bubble->captured = NULL;
    bubble->counter_momentum = counter_new(BUBBLE_MOMENTUM, false, true);
    bubble->counter_time_left = counter_new(BUBBLE_TIME, false, true);

    return bubble;
}

void bubble_delete(Bubble* bubble) {
    Bubble* next = bubble, *t = NULL;
    while (next != NULL) {
        t = next->next;

        map_object_delete(next->map_object);
        animation_delete(next->animation);

        counter_delete(next->counter_momentum);
        counter_delete(next->counter_time_left);

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

        if (map_object_in_collision(t->map_object, a->map_object))
            return true;

        t = t->next;
    }

    return false;
}

Bubble *bubble_burst(Bubble *bubble_list, Bubble *bubble, bool free_monster) {
    Bubble* b = bubble_list, *prev = NULL, *first = bubble_list;

    while (b != NULL)  {
        if (b == bubble) {
            if (prev == NULL) {
                first = b->next;
            }
            else {
                prev->next = b->next;
            }

            if (bubble->captured != NULL && free_monster) {
                bubble->captured->in_bubble = false;
                bubble->captured->map_object->position = bubble->map_object->position;
                bubble->captured->map_object->is_falling = true;
                bubble->captured->angry = true;
            }

            b->next = NULL;
            bubble_delete(bubble);

            break;
        }

        prev = b;
        b = b->next;
    }

    return first;
}

Bubble* adjust_bubbles(Bubble* bubble_list, Level* level, Position final_position) {
    Bubble* bubble = bubble_list, *t = NULL, *first = bubble_list;

    while (bubble != NULL)  {
        counter_tick(bubble->map_object->counter_x);
        counter_tick(bubble->map_object->counter_y);
        counter_tick(bubble->counter_time_left);

        if (map_object_can_move(bubble->map_object) && counter_value(bubble->map_object->counter_y) == 0) {
            if(!collide_previous_bubble(bubble, first)) {
                if (counter_value(bubble->counter_momentum) > 0) {
                    if (bubble->map_object->move_forward) {
                        if (map_object_move_right(bubble->map_object, level))
                            counter_tick(bubble->counter_momentum);
                        else
                            counter_stop(bubble->counter_momentum);
                    }

                    else {
                        if (map_object_move_left(bubble->map_object, level))
                            counter_tick(bubble->counter_momentum);
                        else
                            counter_stop(bubble->counter_momentum);
                    }
                } else {
                    if (bubble->map_object->position.y != final_position.y) {
                        bubble->map_object->is_falling = bubble->map_object->position.y > final_position.y;
                        bubble->map_object->position.y += 1 * (bubble->map_object->is_falling ? -1 : 1);
                        counter_restart(bubble->map_object->counter_y, -1);
                    }

                    else if (bubble->map_object->position.x != final_position.x) {
                        bubble->map_object->move_forward = bubble->map_object->position.x < final_position.x;
                        bubble->map_object->position.x += 1 * (bubble->map_object->move_forward ? 1 : -1);
                        counter_restart(bubble->map_object->counter_x, -1);

                        bubble->map_object->is_falling = false;
                    }
                }
            }
        }

        if (counter_value(bubble->counter_time_left) == 0) {
            t = bubble->next;
            first = bubble_burst(first, bubble, true);
            bubble = t;
        } else
            bubble = bubble->next;
    }

    return first;
}

Bubble* dragon_blow(Dragon* dragon, Bubble* bubble_list, Image* texture) {
    if (dragon == NULL || texture == NULL)
        return bubble_list;

    counter_restart(dragon->counter_blow, -1);

    Position p = dragon->map_object->position;
    p.x += dragon->map_object->move_forward ? 1 : -1;

    MapObject* m = map_object_new(p, BUBBLE_WIDTH / TILE_WIDTH, BUBBLE_HEIGHT / TILE_HEIGHT);
    Bubble* bubble = bubble_new(m, texture, dragon->map_object->move_forward);

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
