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

    // dragons & monsters
    game->bub = create_bub(game->texture_dragons, 0);
    game->monsters_list = monsters_new_from_level(game->current_level);

    if (game->bub == NULL)
        game_fail_exit();

    // keys
    for (int i = 0; i < E_SIZE; ++i)
        game->key_pressed[i] = false;

    // openGL
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    write_log("# READY TO PLAY !");
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

void compute_shifts(MapObject* obj, int* shiftx, int* shifty) {
    float shift_x = ((float) obj->moving_counter) / MAX_MOVING_COUNTER * TILE_WIDTH * (obj->look_right ? -1 : 1);
    float shift_y = 0;

    if (obj->jumping_counter > 0)
        shift_y = (float) (JUMP_EVERY - obj->jumping_counter % JUMP_EVERY) / JUMP_EVERY * TILE_HEIGHT;
    else if (obj->is_falling)
        shift_y = ((float) obj->falling_counter) / FALL_EVERY * TILE_HEIGHT;

    *shiftx = (int) shift_x;
    *shifty = (int) shift_y;
}

void blit_dragon(Dragon *dragon) {
    if (dragon == NULL)
        return;

    Animation** animation = &(dragon->animations[DA_NORMAL]);

    if (dragon->hit)
        animation = &(dragon->animations[DA_HIT]);

    else if (dragon->invincible)
        animation = &(dragon->animations[DA_INVICIBLE]);

    else if (dragon->representation->moving_counter > 0)
        animation = &(dragon->animations[DA_MOVE]);

    animation_animate(animation);

    int shift_x = 0, shift_y = 0;
    compute_shifts(dragon->representation, &shift_x, &shift_y);

    blit_animation(
            *animation,
            dragon->representation->position.x * TILE_WIDTH + shift_x,
            dragon->representation->position.y * TILE_HEIGHT + shift_y,
            dragon->representation->look_right,
            false);
}

void blit_monster(Monster* monster) {
    if (monster == NULL)
        return;

    animation_animate(&(monster->animation));

    int shift_x = 0, shift_y = 0;
    compute_shifts(monster->representation, &shift_x, &shift_y);

    blit_animation(
            monster->animation,
            monster->representation->position.x * TILE_WIDTH + shift_x,
            monster->representation->position.y * TILE_HEIGHT + shift_y,
            monster->representation->look_right,
            false);
}

void game_loop() {
    // KEY MANAGEMENT:
    key_update_interval();

    if (game->key_pressed[E_QUIT])
        exit(EXIT_SUCCESS);

    // eventually move bub:
    map_object_update(game->bub->representation);

    if (!game->bub->hit) {
        if (game->key_pressed[E_LEFT] && game->key_pressed_interval[E_LEFT] == 0)
            map_object_move_left(game->bub->representation, game->current_level);

        else if (game->key_pressed[E_RIGHT] && game->key_pressed_interval[E_RIGHT] == 0)
            map_object_move_right(game->bub->representation, game->current_level);

        if (game->key_pressed[E_ACTION_1] && game->key_pressed_interval[E_ACTION_1] == 0)
            map_object_jump(game->bub->representation, game->current_level, DRAGON_JUMP);
    }

    map_object_adjust(game->bub->representation, game->current_level);

    // move monsters
    Monster* t = game->monsters_list;
    while (t != NULL) {
        map_object_update(t->representation);
        map_object_chase(t->representation, game->bub->representation, game->current_level, t->definition->speed);
        map_object_adjust(t->representation, game->current_level);
        t = t->next;
    }

    // test collisions
    t = game->monsters_list;
    while (t != NULL) {
        if (map_object_in_collision(t->representation, game->bub->representation) && !game->bub->hit && !game->bub->invincible) {
            game->bub->hit = true;
            game->bub->hit_counter = DRAGON_HIT;
        }

        t = t->next;
    }

    // adjust dragon
    dragon_adjust(game->bub);

    // DRAWING:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    blit_level(game->current_level); // level

    t = game->monsters_list;
    while (t != NULL) {
        blit_monster(t);
        t = t->next;
    }

    blit_dragon(game->bub); // dragon

    char buffer[25];
    sprintf(buffer, "%d", game->bub->life);
    glRasterPos2f(2 * TILE_WIDTH, 0 * TILE_HEIGHT);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) buffer);

    sprintf(buffer, "%06d", game->bub->score);
    glRasterPos2f(4 * TILE_WIDTH, 25 * TILE_HEIGHT);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) buffer);

    glutSwapBuffers();
}

void game_quit() {
    // free everything

    // screen
    if (game != NULL) {
        // textures
        image_delete(game->texture_items);
        image_delete(game->texture_monsters);
        image_delete(game->texture_levels);
        image_delete(game->texture_dragons);

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

        // dragons
        dragon_delete(game->bub);
        monster_delete(game->monsters_list);

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