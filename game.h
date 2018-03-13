#ifndef BUBBLES_GAME_H
#define BUBBLES_GAME_H

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "game_object_definitions.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 432

// macro to get pixel component position from game screen (note: invert y so that it is located above)
#define _SR(i, j) (((WINDOW_HEIGHT - (j))*WINDOW_WIDTH+(i))*3+0)
#define _SG(i, j) (((WINDOW_HEIGHT - (j))*WINDOW_WIDTH+(i))*3+1)
#define _SB(i, j) (((WINDOW_HEIGHT - (j))*WINDOW_WIDTH+(i))*3+2)

typedef struct Game_ {
    GLubyte* screen;
} Game;


void game_init();
void game_loop();
void game_quit();


#endif
