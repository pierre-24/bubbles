#include "game.h"

Game* game = NULL;

Sprite* sp = NULL;

void game_fail_exit() {
    printf("something went wrong (check log), exiting ...\n");
    game_quit();
    exit(-1);
}

Texture* load_texture(const char* image_path) {
    FILE* f;

    f = fopen(image_path, "r");
    if (f == NULL) {
        write_log("! cannot open texture %s", TEXTURE_ITEMS);
        return NULL;
    }

    else
        write_log("# load texture %s", TEXTURE_ITEMS);

    Texture* tex = texture_new_from_file(f);
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

    // openGL state stuffs
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DITHER);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_LOGIC_OP);
    glDisable(GL_STENCIL_TEST);
    glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
    glPixelTransferi(GL_RED_SCALE, 1);
    glPixelTransferi(GL_RED_BIAS, 0);
    glPixelTransferi(GL_GREEN_SCALE, 1);
    glPixelTransferi(GL_GREEN_BIAS, 0);
    glPixelTransferi(GL_BLUE_SCALE, 1);
    glPixelTransferi(GL_BLUE_BIAS, 0);
    glPixelTransferi(GL_ALPHA_SCALE, 1);
    glPixelTransferi(GL_ALPHA_BIAS, 0);
}

void blit_texture(GLubyte *screen, int sx, int sy, Texture *texture, int tx, int ty, int tw, int th) {
    if (screen != NULL && texture != NULL) {
        if ((tx + tw) > texture->width || tw < 0)
            tw = texture->width - tx;
        if ((ty + th) > texture->height || th < 0)
            th = texture->height - ty;

        for (int j = 0; j < th; ++j) {
            if ((j + sy) > WINDOW_HEIGHT || (j + sy) < 0)
                continue;

            for (int i = 0; i < tw; ++i) {
                if ((i + sx) > WINDOW_WIDTH || (i + sx) < 0)
                    continue;

                GLubyte * pixel = texture_get_pixel(texture, tx + i, ty + j);

                memcpy(&(screen[_SR(i + sx, j + sy)]), pixel, 4 * sizeof(GLubyte));
            }
        }
    }
}

void blit_sprite(GLubyte *screen, int sx, int sy, Sprite *sprite) {
    if (sprite != NULL)
        blit_texture(screen, sx, sy, sprite->texture, sprite->x, sprite->y, sprite->width, sprite->height);
}

int pos = 0;

void game_loop() {

    // DRAWING
	// glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2i(0, 0);

    memset(game->screen, 0, WINDOW_WIDTH * WINDOW_HEIGHT * 4); // clear game screen

    blit_sprite(game->screen, 175, pos, sp);
    pos = (pos + 1) % WINDOW_HEIGHT;
	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, game->screen);
	glutSwapBuffers();
}

void game_quit() {
    // free everything

    // screen
    if (game != NULL) {
        if (game->screen != NULL)
            free(game->screen);

        // textures
        texture_delete(game->texture_items);

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

