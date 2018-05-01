//
// Created by pbeaujea on 4/7/18.
//

#include "game.h"

void game_main_start(Game *game) {
    /* Starts a new "main" game by setting the level (and the keys).
     * */
    if (game != NULL) {
        dragon_delete(game->bub);
        game->bub = create_bub(game->texture_dragons, BUB_Y);

        if (game->bub != NULL) {
            game->key_interval = FRAMES_BETWEEN_KEY_REPEAT_IN_GAME;
            game->main_started = true;

            game->current_level = NULL;
            game_next_level(game);

            write_log("# READY TO PLAY !");
        }
    }
}

void game_next_level(Game *game) {
    /* Go to next level by setting the counter for the transition.
     * */
    if (game != NULL) {
        if (game->current_level == NULL) {
            game->current_level = game->starting_level;
            game_setup_current_level(game);
        }
        else {
            game->previous_level = game->current_level;
            game->current_level = game->current_level->next;
            counter_restart(game->counter_next_level, -1);
        }
    }
}

void game_setup_current_level(Game *game) {
    /* Actually set up the level: create monsters (delete previous), delete previous item and bubbles.
     * Also set bub to fall from above.
     * */
    if (game != NULL) {
        if (game->current_level == NULL) {
            game_set_screen(game, SCREEN_WIN);
            game->main_started = false;
            return;
        }

        else {
            monster_delete(game->monster_list);
            game->monster_list = monsters_new_from_level(game->current_level);

            bubble_delete(game->bubble_list);
            game->bubble_list = NULL;

            item_delete(game->item_list);
            game->item_list = NULL;

            counter_restart(game->counter_start_level, -1);

            level_object_set_falling_from_above(game->bub->map_object, game->bub->respawn_position);
        }
    }
}

void game_main_input_management(Game *game) {
    /* Input management for the main game.
     *
     * Note that the keys only works if bub is not hit or falling from above, and if the level is not changing.
     * */
    if (game != NULL) {
        if (key_fired(game, E_FREEZE)) {
            game->freeze = !game->freeze;
            game->key_counters[E_FREEZE]->max = FREEZE_EVERY; // change interval
        }

        if (counter_stopped(game->counter_next_level) && game->current_level != NULL && !game->bub->hit && !game->bub->map_object->falling_from_above && !game->freeze) {
            if (key_fired(game, E_LEFT))
                level_object_move_left(game->bub->map_object, game->current_level);

            if (key_fired(game, E_RIGHT))
                level_object_move_right(game->bub->map_object, game->current_level);

            if (key_fired(game, E_ACTION_1))
                level_object_jump(game->bub->map_object, game->current_level, DRAGON_JUMP);

            if (key_fired(game, E_ACTION_2)) {
                game->bubble_list = dragon_blow(game->bub, game->bubble_list, game->texture_levels);
                game->key_counters[E_ACTION_2]->max = BLOW_EVERY; // change interval
            }

            if (key_fired(game, E_SHOW_CONTROLS))
                game_set_screen(game, SCREEN_INSTRUCTIONS);

            if (key_fired(game, E_SHOW_SCORE))
                game_set_screen(game, SCREEN_SCORE);
        }

    }
}

void game_main_update_states(Game *game) {
    /* Update the main game:
     *
     * 1. Test if the level is not changing, if it is not the time to change it and if the player is not loosing ;
     * 2. Adjust everything ;
     * 3. Send a monster after the player if to slow ;
     * 4. Test collision with item ;
     * 5. Test collision wit bubbles (eventually burst bubbles) ;
     * 6. Test collision between monster and bub, and monster and bubble (eventually capture).
     * */
    if (game != NULL) {
        if (!counter_stopped(game->counter_next_level)) {
            counter_tick(game->counter_next_level);

            if (counter_stopped(game->counter_next_level))
                game_setup_current_level(game);
        }

        else if (game->bub->life < 0) { // loose ?
            game->main_started = false;
            game_set_screen(game, SCREEN_GAME_OVER);
        }

        else if(game->monster_list == NULL && (counter_stopped(game->counter_end_this_level) || game->item_list == NULL)) // next level?
            game_next_level(game);

        else if (!game->freeze && game->current_level != NULL) { // ok, game is running
            counter_tick(game->counter_end_this_level);
            counter_tick(game->counter_start_level);

            // adjust everything
            monsters_adjust(game->monster_list, game->current_level, game->bub->map_object);
            dragon_adjust(game->bub, game->current_level);
            items_adjust(game->item_list, game->current_level);
            game->bubble_list = bubbles_adjust(game->bubble_list, game->current_level, game->current_level->bubble_endpoint);

            if (counter_stopped(game->counter_start_level) && game->monster_list != NULL) { // to slow, send a monster after the player
                Monster* last = game->monster_list;
                while (last->next != NULL)
                    last = last->next;

                last->next = monster_new(game->bub->map_object, game->definition_monsters[game->num_monsters_defined - 1]);
                level_object_set_falling_from_above(last->next->map_object, game->bub->map_object->position);

                counter_restart(game->counter_start_level, -1);
            }

            // test collisions with items
            Item* it = game->item_list, *x = NULL;
            while (it != NULL) {
                if (level_object_in_collision(it->map_object, game->bub->map_object) && counter_stopped(it->counter_invulnerability)) {
                    x = it->next;
                    game->item_list = dragon_consume_item(game->bub, game->item_list, it);
                    it = x;
                }
                else
                    it = it->next;
            }

            // test collision between dragon and bubbles (burst bubbles & generate item)
            Bubble* b = game->bubble_list, * t;
            bool monsters_alive = game->monster_list != NULL;

            while (b != NULL) {
                if (level_object_in_collision(game->bub->map_object, b->map_object) && counter_value(b->counter_momentum) < BUBBLE_MOMENTUM - 2) {
                    t = b->next;
                    if (b->captured != NULL) {
                        game->monster_list = monster_kill(game->monster_list, b->captured);
                        game->item_list = item_create(b->map_object, game->item_list, game->definition_items,
                                                      game->num_items_defined, game->current_level,
                                                      counter_value(game->counter_start_level));
                    }

                    game->bubble_list = bubble_burst(game->bubble_list, b, false);
                    b = t;
                }
                else
                    b = b->next;
            }

            if (monsters_alive && game->monster_list == NULL) // all monsters were killed
                counter_restart(game->counter_end_this_level, -1);

            // test collisions with monster
            if (!game->bub->map_object->falling_from_above) {
                Monster* m = game->monster_list;
                while (m != NULL) {
                    if (!m->in_bubble && !m->map_object->falling_from_above) {
                        if (level_object_in_collision(m->map_object, game->bub->map_object) && !game->bub->hit && !game->bub->invincible) {
                            game->bub->hit = true;
                            counter_restart(game->bub->counter_hit, -1);
                        }

                        // eventually capture monster in bubbles
                        b = game->bubble_list;
                        while (b != NULL) {
                            if (level_object_in_collision(m->map_object, b->map_object) && b->captured == NULL && b->counter_momentum > 0) {
                                m->in_bubble = true;
                                b->captured = m;
                                break;
                            }

                            b = b->next;
                        }
                    }

                    m = m->next;
                }
            }
        }
    }


}

void game_main_draw(Game *game) {
    /* Draw the main game:
     *
     * If not transitioning between levels, draw level, monsters, dragon, bubbles, items, score and life
     * (in that order, even if the only things that matter is that the level is drawn first).
     *
     * If transitioning, draw both levels (or black if there is no previous [starting the game] or current [ending the game] level).
     * */
    if (game != NULL) {
        // monsters
        if (counter_stopped(game->counter_next_level)) {
            if (game->current_level != NULL) {
                blit_level(game->current_level, 0); // level

                Monster* m = game->monster_list;
                while (m != NULL) {
                    if (!m->in_bubble)
                        blit_monster(m, game->freeze);
                    m = m->next;
                }

                blit_dragon(game->bub, game->freeze, 0); // dragon

                // bubbles
                Bubble* b = game->bubble_list;
                while (b != NULL) {
                    blit_bubble(b, game->freeze);
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
                blit_text(game->font, buffer,  TILE_WIDTH / 2, 0);

                sprintf(buffer, "%06d", game->bub->score);
                blit_text(game->font, buffer, 4 * TILE_WIDTH, 25 * TILE_HEIGHT);
            }

        }

        else {
            if (game->previous_level != NULL) {
                int sy = counter_value(game->counter_next_level) * WINDOW_HEIGHT / NEXT_LEVEL_TRANSITION;
                blit_level(game->previous_level, sy);

                dragon_adjust(game->bub, game->previous_level);
                blit_dragon(game->bub, NULL, sy); // dragon
            }

            if (game->current_level != NULL)
                blit_level(game->current_level, -WINDOW_HEIGHT + counter_value(game->counter_next_level) * WINDOW_HEIGHT / NEXT_LEVEL_TRANSITION);
        }

        if (game->freeze)
            blit_text(game->font, "PAUSED", 15 * TILE_WIDTH, 25 * TILE_HEIGHT);
    }

}
