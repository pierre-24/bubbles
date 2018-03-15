#ifndef BUBBLES_TEXTURES_H
#define BUBBLES_TEXTURES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define TRANSPARENT {255, 0, 175};

typedef struct Pixel_ {
	// each color value is between 0 (black) and 255 (full color)
	GLubyte values[3];
	bool transparent; // "true" if the pixel is transparent 
} Pixel;

typedef struct Texture_ {
	unsigned int width;
	unsigned int height;
	Pixel* pixels; // dynamically allocated
} Texture;

Texture* texture_new_from_file(FILE* f);
void texture_delete(Texture* texture);

Pixel* texture_get_pixel(Texture* texture, int x, int y);

typedef struct Sprite_ {
	Texture* texture; // pointer to the texture
	int x;
	int y;
	int width;
	int height;
} Sprite;

Sprite* sprite_new(Texture *texture, int x, int y, int w, int h);
void sprite_delete(Sprite* sprite);

#endif
