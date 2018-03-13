#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pixel_ {
	// each color value is between 0 (black) and 255 (full color)
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	bool transparent; // "true" if the pixel is transparent 
} Pixel;

typedef struct Texture_ {
	unsigned int width;
	unsigned int height;
	Pixel* pixels; // dynamically allocated
} Texture;

Texture* texture_new_from_file(FILE* f);
void texture_delete(Texture* texture);

Texture* texture_new_from_text(char* text, Texture* sprite_font);

typedef struct Sprite_ {
	Texture* texture; // pointer to the texture
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
} Sprite;

Sprite* sprite_new(Texture* texture, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void sprite_delete(Sprite* sprite);

#endif
