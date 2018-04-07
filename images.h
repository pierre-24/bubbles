#ifndef BUBBLES_TEXTURES_H
#define BUBBLES_TEXTURES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define TRANSPARENT {255, 0, 175};

typedef struct Image_ {
	unsigned int width;
	unsigned int height;
	GLubyte * pixels; // dynamically allocated
} Image;

Image* image_new_from_file(FILE *f);
void image_delete(Image *image);

typedef struct Sprite_ {
	Image* image; // pointer to the image
	int x;
	int y;
	int width;
	int height;
	GLuint texture_id;
} Sprite;

Sprite* sprite_new(Image *image, int x, int y, int w, int h);
void sprite_delete(Sprite* sprite);

Sprite* sprite_copy(Sprite* origin);

void blit_sprite(Sprite *sprite, int sx, int sy, bool flip_x, bool flip_y);

#endif
