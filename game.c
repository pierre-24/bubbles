#include "game.h"

Game* game = NULL;

Sprite* sp = NULL;

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
    game->screen = NULL;

    game->texture_items = NULL;
    game->definition_items = NULL;

    write_log("# starting Bubbles!");
    
    // create screen
    game->screen = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * 4 * sizeof(GLubyte));

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
    fclose(f);

    // tmp:
    sp = sprite_new(game->texture_items, 0, 0, -1, -1);

    // openGL
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void blit_sprite(Sprite *sprite, int sx, int sy) {
    if (sprite != NULL) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sp->texture_id);

        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 1.0); glVertex2i(sx,sy);
        glTexCoord2d(0.0, 0.0); glVertex2i(sx, sy+sprite->height);
        glTexCoord2d(1.0, 0.0); glVertex2i(sx + sprite->width, sy+sprite->height);
        glTexCoord2d(1.0, 1.0); glVertex2i(sx + sprite->width, sy);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

}

int pos = 0;

void game_loop() {

    // DRAWING
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    blit_sprite(sp, 0, pos);
    pos = (pos +1) % WINDOW_HEIGHT;

	glutSwapBuffers();
}

void game_quit() {
    // free everything

    // screen
    if (game != NULL) {
        if (game->screen != NULL)
            free(game->screen);

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

    sprite_delete(sp);

    write_log("# quitting Bubbles!");

	close_log();
}

