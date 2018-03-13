#include "game.h"

Game* game = NULL;

Texture* tex = NULL;

void game_init() {
	init_log();

    game = malloc(sizeof(Game));

    write_log("# starting Bubbles!");
    
    // create screen
    game->screen = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * 3 * sizeof(GLubyte));
    
    FILE* f = fopen("assets/test.ppm", "r");
    tex = texture_new_from_file(f);
    fclose(f);
}

void blit_texture(GLubyte *screen, int sx, int sy, Texture *texture, int tx, int ty, int tw, int th) {
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

            Pixel* pixel = texture_get_pixel(texture, tx + i, ty + j);

            if (pixel == NULL || pixel->transparent)
                continue;

            screen[_SR(i + sx, j + sy)] = pixel->red;
            screen[_SG(i + sx, j + sy)] = pixel->green;
            screen[_SB(i + sx, j + sy)] = pixel->blue;
        }
    }
}

int pos = 0;

void game_loop() {

    // DRAWING
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2i(0, 0);

    memset(game->screen, 0, WINDOW_WIDTH * WINDOW_HEIGHT * 3); // clear game screen

    blit_texture(game->screen, 175, pos, tex, 0, 0, -1, -1);
    pos = (pos + 1) % WINDOW_HEIGHT;
	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, game->screen);
	glutSwapBuffers();
}

void game_quit() {
    // free everything
    free(game->screen);
    free(game);

    texture_delete(tex);

    write_log("# quitting Bubbles!");

	close_log();
}

