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

    // dragons
    game->bub = create_bub(game->texture_dragons, 0);

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
        glTexCoord2d(0.0, 1.0); glVertex2i(sx + (flip_x ? sprite->width : 0),sy + ((flip_y) ? sprite->height : 0));
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
                    blit_sprite(level->fill_tile, x * LEVEL_WIDTH, y * LEVEL_HEIGHT, 0, 0);
                }
            }
        }
    }
}

void blit_dragon(Dragon *dragon) {

    Animation** animation = &(dragon->animations[DA_NORMAL]);

    if (dragon->is_moving > 0) {
        dragon->is_moving--;
        animation = &(dragon->animations[DA_MOVE]);
    }

    animation_animate(animation);
    blit_animation(*animation, dragon->current_position.x * LEVEL_WIDTH, dragon->current_position.y * LEVEL_HEIGHT, dragon->look_right, false);
}

void game_loop() {
    // KEY MANAGEMENT:
    key_update_interval();

    if (game->key_pressed[E_QUIT]) {
        exit(EXIT_SUCCESS);
    }

    if (game->key_pressed[E_LEFT] && game->key_pressed_interval[E_LEFT] == 0) {
        game->bub->look_right = false;

        if (can_go_left(game->levels, game->bub->current_position, 2, 2)) {
            game->bub->current_position.x -= 1;
            game->bub->is_moving = 3;
        }
    }

    else if (game->key_pressed[E_RIGHT] && game->key_pressed_interval[E_RIGHT] == 0) {
        game->bub->look_right = true;

        if (can_go_right(game->levels, game->bub->current_position, 2, 2)) {
            game->bub->current_position.x += 1;
            game->bub->is_moving = 3;
        }
    }

    if (game->bub->is_jumping == 0 && !game->bub->is_falling && game->key_pressed[E_ACTION_1] && game->key_pressed_interval[E_ACTION_1] == 0) {
        if (can_go_top(game->levels, game->bub->current_position, 2, 2)) {
            game->bub->is_jumping = DRAGON_JUMP - 1;
            game->bub->current_position.y += 1;
        }
    }

    if (game->bub->is_jumping > 0) {

        if (can_go_top(game->levels, game->bub->current_position, 2, 2)) {
            game->bub->current_position.y += 1;
            game->bub->is_jumping--;

            if (game->bub->is_jumping == 0) {
                game->bub->is_falling = true;
            }
        }

        else {
            game->bub->is_jumping = 0;
            game->bub->is_falling = true;
        }

    }

    else if(game->bub->is_falling) {
        if (can_go_bottom(game->levels, game->bub->current_position, 2, 2)) {
            game->bub->current_position.y -= 1;
        }
        else
            game->bub->is_falling = false;
    }

    else if (can_go_bottom(game->levels, game->bub->current_position, 2, 2)) {
        game->bub->is_falling = true;
    }


    // DRAWING:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    blit_level(game->levels); // level
    blit_dragon(game->bub);

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
            keyp = E_BOTTOM;
            break;
        case GLUT_KEY_UP:
            keyp = E_TOP;
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
            keyp = E_BOTTOM;
            break;
        case GLUT_KEY_UP:
            keyp = E_TOP;
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