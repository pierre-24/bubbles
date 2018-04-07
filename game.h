#ifndef BUBBLES_GAME_H
#define BUBBLES_GAME_H

#include <stdio.h>
#include <stdlib.h>

#include "game_main.h"
#include "game_screens.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 432

#define TEXTURE_ITEMS "assets/items.ppm"
#define TEXTURE_MONSTERS "assets/monsters.ppm"
#define TEXTURE_LEVELS "assets/levels.ppm"
#define TEXTURE_DRAGONS "assets/dragons.ppm"
#define TEXTURE_SCREENS "assets/screens.ppm"
#define TEXTURE_GAME "assets/game.ppm"

#define DEFINITION_ITEMS "data/items.txt"
#define DEFINITION_MONSTERS "data/monsters.txt"

#define FILE_LEVELS "data/levels.txt"

void game_special_key_down(int key, int x, int y);
void game_special_key_up(int key, int x, int y);
void game_key_down(unsigned char key, int x, int y);
void game_key_up(unsigned char key, int x, int y);

void game_init();
void game_loop();
void game_quit();

#endif
