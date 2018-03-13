#include "game.h"

Game* game = NULL;


void game_init() {
    game = malloc(sizeof(Game));
    
    // create screen
    game->screen = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * 3 * sizeof(GLubyte));
    
    FILE* f = fopen("assets/test.ppm", "r");
    Texture* tex = texture_new_from_file(f);
    fclose(f);
}

void game_loop() {

    // DRAWING
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2i(0, 0);
	
	for(int i = 0; i < WINDOW_HEIGHT; i++) {
	    for(int j= 0; j < WINDOW_WIDTH; j++) {
	        game->screen[(i*WINDOW_WIDTH+j)*3] = (GLubyte) (i % 256); //+ j % 128);
	        game->screen[(i*WINDOW_WIDTH+j)*3+1] = (GLubyte) (j % 256); //+ j % 128)
	        game->screen[(i*WINDOW_WIDTH+j)*3+2] = (GLubyte) (j % 128 + i % 128) ; //+ j % 128)
	    }
	}
	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, game->screen);
	glutSwapBuffers();
}

void game_quit() {
    // free everything
    free(game->screen);
    free(game);
}

