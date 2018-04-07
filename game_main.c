//
// Created by pbeaujea on 4/7/18.
//

#include "game.h"

void game_main_input_management(Game *game) {
    if (!game->bub->hit) {
        if (game->key_pressed[E_LEFT] && game->key_pressed_interval[E_LEFT] == 0)
            map_object_move_left(game->bub->map_object, game->current_level);

        else if (game->key_pressed[E_RIGHT] && game->key_pressed_interval[E_RIGHT] == 0)
            map_object_move_right(game->bub->map_object, game->current_level);

        if (game->key_pressed[E_ACTION_1] && game->key_pressed_interval[E_ACTION_1] == 0)
            map_object_jump(game->bub->map_object, game->current_level, DRAGON_JUMP);

        if (game->key_pressed[E_ACTION_2] && game->key_pressed_interval[E_ACTION_2] == 0) {
            game->bubble_list = dragon_blow(game->bub, game->bubble_list, game->texture_levels);
            game->key_pressed_interval[E_ACTION_2] = BLOW_EVERY;
        }
    }
}

void game_main_update_states(Game *game) {
    // test collisions with items
    Item* it = game->item_list, *x = NULL;
    while (it != NULL) {
        if (map_object_in_collision(it->map_object, game->bub->map_object) && counter_stopped(it->counter_invulnerability)) {
            x = it->next;
            game->item_list = dragon_consume_item(game->bub, game->item_list, it);
            it = x;
        }
        else
            it = it->next;
    }

    // test collision between dragon and bubbles (generate item)
    Bubble* b = game->bubble_list, * t;
    while (b != NULL) {
        if (map_object_in_collision(game->bub->map_object, b->map_object) && counter_value(b->counter_momentum) < BUBBLE_MOMENTUM - 2) {
            t = b->next;
            if (b->captured != NULL) {
                game->monster_list = monster_kill(game->monster_list, b->captured);
                game->item_list = create_item(b->map_object, game->item_list, game->definition_items,
                                              game->num_items, game->current_level, game->bub->map_object->move_forward);
            }

            game->bubble_list = bubble_burst(game->bubble_list, b, false);
            b = t;
        }
        else
            b = b->next;
    }

    // test collisions with monster
    Monster* m = game->monster_list;
    while (m != NULL) {
        if (!m->in_bubble) {
            if (map_object_in_collision(m->map_object, game->bub->map_object) && !game->bub->hit && !game->bub->invincible) {
                game->bub->hit = true;
                counter_restart(game->bub->counter_hit, -1);
            }

            // eventually capture monster in bubbles
            b = game->bubble_list;
            while (b != NULL) {
                if (map_object_in_collision(m->map_object, b->map_object) && b->captured == NULL && b->counter_momentum > 0) {
                    m->in_bubble = true;
                    b->captured = m;
                    break;
                }

                b = b->next;
            }
        }

        m = m->next;
    }

    // adjust everything
    monsters_adjust(game->monster_list, game->current_level, game->bub->map_object);
    dragon_adjust(game->bub, game->current_level);
    game->bubble_list = bubbles_adjut(game->bubble_list, game->current_level, game->current_level->bubble_endpoint);
    items_adjust(game->item_list, game->current_level);

    // check if not win
    if (game->bub->life < 0) {
        game->paused = true;
        game->done = true;
        game->current_screen = SCREEN_GAME_OVER;
    }

    else if(game->monster_list == NULL && game->item_list == NULL){
        game->paused = true;
        game->done = true;
        game->current_screen = SCREEN_WIN;
    }
}

void game_main_draw(Game *game) {
    blit_level(game->current_level); // level

    // monsters
    Monster* m = game->monster_list;
    while (m != NULL) {
        if (!m->in_bubble)
            blit_monster(m);
        m = m->next;
    }

    blit_dragon(game->bub); // dragon

    // bubbles
    Bubble* b = game->bubble_list;
    while (b != NULL) {
        blit_bubble(b);
        b = b->next;
    }

    // items
    Item* it = game->item_list;
    while (it != NULL) {
        blit_item(it);
        it = it->next;
    }

    char buffer[25];
    sprintf(buffer, "%d", game->bub->life);
    glRasterPos2f(2 * TILE_WIDTH, 0 * TILE_HEIGHT);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) buffer);

    sprintf(buffer, "%06d", game->bub->score);
    glRasterPos2f(4 * TILE_WIDTH, 25 * TILE_HEIGHT);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) buffer);
}