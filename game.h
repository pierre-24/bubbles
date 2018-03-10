#ifndef GAME_H
#define GAME_H

#include <GL/glut.h>

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 432

typedef struct Game_ {
    GLubyte* screen;
} Game;


void game_init();
void game_loop();
void game_quit();


#endif
