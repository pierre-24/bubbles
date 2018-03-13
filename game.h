#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>

#include "game_object_definitions.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 432

typedef struct Game_ {
    GLubyte* screen;
} Game;


void game_init();
void game_loop();
void game_quit();


#endif
