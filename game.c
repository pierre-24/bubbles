#include "game.h"

Game* game = NULL;

void game_fail_exit() {
    printf("something went wrong (check log), exiting ...\n");
    game_quit();
    exit(-1);
}

Image* load_texture(const char* image_path) {
    FILE* f;

    f = fopen(image_path, "r");
    if (f == NULL) {
        write_log("! cannot open image %s", image_path);
        return NULL;
    }

    else
        write_log("# load image %s", image_path);

    Image* tex = image_new_from_file(f);
    fclose(f);

    return tex;
}

void game_init() {
    srand((unsigned int) time(NULL));
	init_log();

    FILE* f = NULL;

    game = malloc(sizeof(Game));

    write_log("# starting Bubbles!");

    // load textures
    game->texture_items = NULL;
    game->texture_monsters = NULL;
    game->texture_levels = NULL;
    game->texture_dragons = NULL;
    game->texture_screens = NULL;

    game->texture_items = load_texture(TEXTURE_ITEMS);
    if (game->texture_items == NULL)
        game_fail_exit();

    game->texture_monsters = load_texture(TEXTURE_MONSTERS);
    if (game->texture_monsters == NULL)
        game_fail_exit();

    game->texture_levels = load_texture(TEXTURE_LEVELS);
    if (game->texture_levels == NULL)
        game_fail_exit();

    game->texture_dragons = load_texture(TEXTURE_DRAGONS);
    if (game->texture_dragons == NULL)
        game_fail_exit();

    game->texture_screens = load_texture(TEXTURE_SCREENS);
    if (game->texture_screens == NULL)
        game_fail_exit();

    // sprites for screen
    for (int i = 0; i < SCREEN_NUMBER; ++i) {
        game->screens[i] = sprite_new(game->texture_screens, 0, i * WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
        if (game->screens[i] == NULL)
            game_fail_exit();
    }

    // load definition
    game->num_items = 0;
    game->definition_items = NULL;
    game->num_monsters = 0;
    game->definition_monsters = NULL;

    f = fopen(DEFINITION_ITEMS, "r");
    if (f == NULL) {
        write_log("! unable to open item def file %s", DEFINITION_ITEMS);
        game_fail_exit();
    }
    else
        write_log("# opening item def file %s", DEFINITION_ITEMS);

    game->definition_items = item_defs_from_file(f, game->texture_items, &(game->num_items));
    fclose(f);
    if (game->definition_items == NULL || game->num_items == 0) {
        write_log("! no game items, exiting");
        game_fail_exit();
    }

    f = fopen(DEFINITION_MONSTERS, "r");
    if (f == NULL) {
        write_log("! unable to open monster def file %s", DEFINITION_MONSTERS);
        game_fail_exit();
    }
    else
        write_log("# opening monster def file %s", DEFINITION_MONSTERS);

    game->definition_monsters = monster_defs_from_file(f, game->texture_monsters, &(game->num_monsters));
    fclose(f);
    if (game->definition_monsters == NULL || game->num_monsters == 0) {
        write_log("! no game monsters, exiting");
        game_fail_exit();
    }

    // levels
    game->levels = NULL;
    game->num_levels = 0;

    f = fopen(FILE_LEVELS, "r");
    if (f == NULL) {
        write_log("! unable to level file %s", FILE_LEVELS);
        game_fail_exit();
    }
    else
        write_log("# opening level file %s", FILE_LEVELS);

    game->levels = levels_new_from_file(f, game->texture_levels, game->definition_monsters, game->num_monsters,
                                        &(game->num_levels));
    fclose(f);

    if (game->levels == NULL || game->num_levels == 0) {
        write_log("! no levels, exiting");
        game_fail_exit();
    }

    game->current_level = game->levels; // ->next;

    // dragons & monsters & bubbles
    game->bub = create_bub(game->texture_dragons, 0);

    if (game->bub == NULL)
        game_fail_exit();
    
    write_log("# created bub");

    game->monster_list = monsters_new_from_level(game->current_level);
    game->bubble_list = NULL;
    game->item_list = NULL;

    // keys
    for (int i = 0; i < E_SIZE; ++i)
        game->key_pressed[i] = false;

    // openGL
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    write_log("# READY TO PLAY !");

    game->paused = true;
    game->done = false;
    game->current_screen = SCREEN_INSTRUCTIONS; // starts with instruction screen
}

void blit_sprite(Sprite *sprite, int sx, int sy, bool flip_x, bool flip_y) {
    if (sprite != NULL) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sprite->texture_id);

        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 1.0); glVertex2i(sx + (flip_x ? sprite->width : 0), sy + ((flip_y) ? sprite->height : 0));
        glTexCoord2d(0.0, 0.0); glVertex2i(sx + (flip_x ? sprite->width : 0), sy + ((flip_y) ? 0 : sprite->height));
        glTexCoord2d(1.0, 0.0); glVertex2i(sx + (flip_x ? 0 : sprite->width), sy + ((flip_y) ? 0 : sprite->height));
        glTexCoord2d(1.0, 1.0); glVertex2i(sx + (flip_x ? 0 : sprite->width), sy + ((flip_y) ? sprite->height : 0));
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}

void blit_animation(Animation *animation, int sx, int sy, bool flip_x, bool flip_y) {
    if (animation != NULL && animation->frame != NULL) {
        blit_sprite(animation->frame, sx, sy, flip_x, flip_y);
    }
}

void blit_level(Level* level) {
    if (level != NULL) {
        for (unsigned int y = 0; y < MAP_HEIGHT; ++y) {
            for (unsigned int x = 0; x < MAP_WIDTH; ++x) {
                if (level->map[position_index((Position) {x, y})]) {
                    blit_sprite(level->fill_tile, x * TILE_WIDTH, y * TILE_HEIGHT, 0, 0);
                }
            }
        }

        // repeat the bottom on top
        for (unsigned int x = 0; x < MAP_WIDTH; ++x) {
            if (level->map[position_index((Position) {x, 0})]) {
                blit_sprite(level->fill_tile, x * TILE_WIDTH, MAP_HEIGHT * TILE_HEIGHT, 0, 0);
            }
        }
    }
}

void compute_real_positions(MapObject* obj, int* shiftx, int* shifty) {
    EffectivePosition p = map_object_to_effective_position(obj);

    *shiftx = (int) (p.x * TILE_WIDTH);
    *shifty = (int) (p.y * TILE_HEIGHT);
}

void blit_dragon(Dragon *dragon) {
    if (dragon != NULL) {
        Animation** animation = &(dragon->animations[DA_NORMAL]);

        if (dragon->hit)
            animation = &(dragon->animations[DA_HIT]);

        else if (dragon->invincible)
            animation = &(dragon->animations[DA_INVICIBLE]);

        else if (counter_value(game->bub->counter_blow) != 0)
            animation = &(dragon->animations[DA_BLOW]);

        else if (!map_object_can_move(dragon->map_object))
            animation = &(dragon->animations[DA_MOVE]);

        animation_animate(animation);

        int shift_x = 0, shift_y = 0;
        compute_real_positions(dragon->map_object, &shift_x, &shift_y);

        blit_animation(
                *animation,
                shift_x,
                shift_y,
                dragon->map_object->move_forward,
                false);
    }
}

void blit_monster(Monster* monster) {
    if (monster != NULL && !monster->in_bubble) {
        Animation** animation = &(monster->animation[MA_NORMAL]);

        if (monster->angry)
            animation = &(monster->animation[MA_ANGRY]);

        animation_animate(animation);

        int shift_x = 0, shift_y = 0;
        compute_real_positions(monster->map_object, &shift_x, &shift_y);

        blit_animation(
                *animation,
                shift_x,
                shift_y,
                monster->map_object->move_forward,
                false);

    }
}

void blit_bubble(Bubble* bubble) {
    if (bubble != NULL) {
        Animation** animation = &(bubble->animation);

        if (bubble->captured != NULL)
            animation = &(bubble->captured->animation[MA_CAPTURED]);

        animation_animate(animation);

        int shift_x = 0, shift_y = 0;
        compute_real_positions(bubble->map_object, &shift_x, &shift_y);

        blit_animation(
                *animation,
                shift_x,
                shift_y,
                false,
                false);
    }
}

void blit_item(Item* item) {
    if (item != NULL) {
        blit_sprite(item->definition->sprite, item->map_object->position.x * TILE_WIDTH, item->map_object->position.y * TILE_HEIGHT, false, false);
    }
}

void game_loop_input_management() {
    // KEY MANAGEMENT:
    key_update_interval();

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

void game_loop_update_states() {
    // test collisions with items
    Item* it = game->item_list, *x = NULL;
    while (it != NULL) {
        if (map_object_in_collision(it->map_object, game->bub->map_object)) {
            x = it->next;
            game->item_list = dragon_consume_item(game->bub, game->item_list, it);
            it = x;
        }
        else
            it = it->next;
    }

    // test collision between dragon and bubbles (generate item)
    Bubble* b = game->bubble_list;
    Bubble* t;
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

    // move monsters
    m = game->monster_list;
    while (m != NULL) {
        if (!m->in_bubble) {
            map_object_chase(m->map_object, game->bub->map_object, game->current_level, m->definition->speed / (m->angry ? 2 : 1));
            map_object_adjust(m->map_object, game->current_level);
        }

        m = m->next;
    }

    // adjust dragon & bubbles & items
    dragon_adjust(game->bub, game->current_level);
    game->bubble_list = adjust_bubbles(game->bubble_list, game->current_level, game->current_level->bubble_endpoint);
    items_adjust(game->item_list, game->current_level);
}


void game_loop_draw() {
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

void game_loop() {
    if (game->key_pressed[E_QUIT])
        exit(EXIT_SUCCESS);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    if (!game->paused) {
        game_loop_input_management();
        game_loop_update_states();
        game_loop_draw();


        if (game->bub->life < 0) {
            game->paused = true;
            game->done = true;
            game->current_screen = SCREEN_GAME_OVER;
        }

        /*else if(game->monster_list == NULL && game->item_list == NULL){
            game->paused = true;
            game->done = true;
            game->current_screen = SCREEN_WIN;
        }*/
    }

    else {
        blit_sprite(game->screens[game->current_screen], 0, 0, false, false);

        if (game->key_pressed[E_ACTION_1]) {
            game->paused = false;
            game->key_pressed[E_ACTION_1] = false;

            if (game->done)
                exit(EXIT_SUCCESS);
        }
    }

    glutSwapBuffers();

}

void game_quit() {
    // screen
    if (game != NULL) {
        // textures
        image_delete(game->texture_items);
        image_delete(game->texture_monsters);
        image_delete(game->texture_levels);
        image_delete(game->texture_dragons);
        image_delete(game->texture_screens);

        for (int j = 0; j < SCREEN_NUMBER; ++j)
            sprite_delete(game->screens[j]);

        // definitions
        if (game->definition_items != NULL)  {
            for (int i = 0; i < game->num_items; ++i) {
                item_def_delete(game->definition_items[i]);
            }

            free(game->definition_items);
        }

        if (game->definition_monsters != NULL)  {
            for (int i = 0; i < game->num_monsters; ++i) {
                monster_def_delete(game->definition_monsters[i]);
            }

            free(game->definition_monsters);
        }

        // levels
        level_delete(game->levels);

        // dragons & monster & stuffs
        dragon_delete(game->bub);
        monster_delete(game->monster_list);
        bubble_delete(game->bubble_list);
        item_delete(game->item_list);

        // and finally:
        free(game);
    }

    write_log("# quitting Bubbles!");

	close_log();
}

void key_down(int key) {
    if (key >= E_SIZE)
        return;

    game->key_pressed[key] = true;
    game->key_pressed_interval[key] = 1;
}

void key_up(int key) {
    if (key >= E_SIZE)
        return;

    // printf("→ up: %d\n", key);

    game->key_pressed[key] = false;

}

void game_special_key_down(int key, int x, int y) {
    // printf("down(s): %d (%d)\n", key, GLUT_KEY_DOWN);

    int keyp;

    switch (key) {
        case GLUT_KEY_DOWN:
            keyp = E_DOWN;
            break;
        case GLUT_KEY_UP:
            keyp = E_UP;
            break;
        case GLUT_KEY_LEFT:
            keyp = E_LEFT;
            break;
        case GLUT_KEY_RIGHT:
            keyp = E_RIGHT;
            break;
        default:
            keyp = E_NONE;
            break;
    }

    key_down(keyp);
}

void game_special_key_up(int key, int x, int y) {
    // printf("up(s): %d\n", key);

    int keyp;

    switch (key) {
        case GLUT_KEY_DOWN:
            keyp = E_DOWN;
            break;
        case GLUT_KEY_UP:
            keyp = E_UP;
            break;
        case GLUT_KEY_LEFT:
            keyp = E_LEFT;
            break;
        case GLUT_KEY_RIGHT:
            keyp = E_RIGHT;
            break;
        default:
            keyp = E_NONE;
            break;
    }

    key_up(keyp);
}

void game_key_down(unsigned char key, int x, int y) {
    // printf("down keyboard: %c\n", key);

    int keyp;

    switch (key) {
        case 'x':
            keyp = E_ACTION_2;
            break;
        case ' ':
            keyp = E_ACTION_1;
            break;
        case 'p':
            keyp = E_PAUSE;
            break;
        case 'm':
            keyp = E_SHOW_SCORE;
            break;
        case 'c':
            keyp = E_SHOW_CONTROLS;
            break;
        case 27: // esc key
            keyp = E_QUIT;
            break;
        default:
            keyp = E_NONE;
            break;
    }

    key_down(keyp);
}

void game_key_up(unsigned char key, int x, int y) {
    // printf("down keyboard: %c\n", key);

    int keyp;

    switch (key) {
        case 'x':
            keyp = E_ACTION_2;
            break;
        case ' ':
            keyp = E_ACTION_1;
            break;
        case 'p':
            keyp = E_PAUSE;
            break;
        case 'm':
            keyp = E_SHOW_SCORE;
            break;
        case 'c':
            keyp = E_SHOW_CONTROLS;
            break;
        default:
            keyp = E_NONE;
            break;
    }

    key_up(keyp);
}

void key_update_interval() {
    for (int i = 0; i < E_SIZE; ++i) {
        if (game->key_pressed[i]) {
            if (game->key_pressed_interval[i] <= 0)
                game->key_pressed_interval[i] = FRAMES_BETWEEN_KEY_REPEAT;
            else {
                game->key_pressed_interval[i] -= 1;
            }
        }
    }
}
