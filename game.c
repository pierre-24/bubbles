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

    // openGL
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    write_log("# READY TO PLAY !");
}

void blit_sprite(Sprite *sprite, int sx, int sy) {
    if (sprite != NULL) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sprite->texture_id);

        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 1.0); glVertex2i(sx,sy);
        glTexCoord2d(0.0, 0.0); glVertex2i(sx, sy+sprite->height);
        glTexCoord2d(1.0, 0.0); glVertex2i(sx + sprite->width, sy+sprite->height);
        glTexCoord2d(1.0, 1.0); glVertex2i(sx + sprite->width, sy);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}

void blit_animation(Animation* animation, int sx, int sy) {
    if (animation != NULL && animation->frame != NULL) {
        blit_sprite(animation->frame, sx, sy);
    }
}

void blit_level(Level* level) {
    if (level != NULL) {
        for (unsigned int y = 0; y < MAP_HEIGHT; ++y) {
            for (unsigned int x = 0; x < MAP_WIDTH; ++x) {
                if (level->map[position_index((Position) {x, y})]) {
                    blit_sprite(level->fill_tile, x * SPRITE_LEVEL_WIDTH, y * SPRITE_LEVEL_HEIGHT);
                }
            }
        }
    }
}

int pos = 0;

void game_loop() {

    // DRAWING
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    blit_level(game->levels);

    animation_animate(&(game->bub->animations[DA_NORMAL]));
    blit_animation(game->bub->animations[DA_NORMAL], WINDOW_WIDTH - pos, 0);
    pos = (pos +1) % WINDOW_WIDTH;

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

