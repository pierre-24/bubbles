#include "game.h"

Game* game = NULL;
Animation* animation;

void game_fail_exit() {
    printf("something went wrong (check log), exiting ...\n");
    game_quit();
    exit(-1);
}

Image* load_texture(const char* image_path) {
    FILE* f;

    f = fopen(image_path, "r");
    if (f == NULL) {
        write_log("! cannot open image %s", TEXTURE_ITEMS);
        return NULL;
    }

    else
        write_log("# load image %s", TEXTURE_ITEMS);

    Image* tex = image_new_from_file(f);
    fclose(f);

    return tex;
}

void game_init() {
	init_log();

    game = malloc(sizeof(Game));

    game->texture_items = NULL;
    game->num_items = 0;
    game->definition_items = NULL;

    write_log("# starting Bubbles!");

    // load textures
    game->texture_items = load_texture(TEXTURE_ITEMS);
    if (game->texture_items == NULL)
        game_fail_exit();

    // load definition
    FILE* f = fopen(DEFINITION_ITEMS, "r");
    if (f == NULL) {
        write_log("! unable to open item def file %s", DEFINITION_ITEMS);
        game_fail_exit();
    }
    else
        write_log("# opening item def file %s", DEFINITION_ITEMS);

    game->definition_items = item_defs_from_file(f, game->texture_items, &(game->num_items));
    if (game->definition_items == NULL || game->num_items == 0) {
        write_log("! no game items, exiting");
        game_fail_exit();
    }
    fclose(f);

    animation = animation_new();
    animation = animation_add_frame(animation, game->definition_items[0]->sprite);
    animation = animation_add_frame(animation, game->definition_items[1]->sprite);

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
    if (animation != NULL) {
        blit_sprite(animation->frame, sx, sy);
    }
}

int pos = 0;

void game_loop() {

    // DRAWING
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    blit_animation(animation, 0, pos);
    pos = (pos +1) % WINDOW_HEIGHT;

    if (pos % 25 == 0)
        animation = animation_next(animation);

    glutSwapBuffers();
}

void game_quit() {
    // free everything

    // screen
    if (game != NULL) {
        // textures
        image_delete(game->texture_items);

        // definitions
        if (game->definition_items != NULL)  {
            for (int i = 0; i < game->num_items; ++i) {
                item_def_delete(game->definition_items[i]);
            }

            free(game->definition_items);
        }

        // and finally:
        free(game);
    }

    // tmp
    animation_delete(animation);

    write_log("# quitting Bubbles!");

	close_log();
}

