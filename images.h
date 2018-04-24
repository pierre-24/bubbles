#ifndef BUBBLES_TEXTURES_H
#define BUBBLES_TEXTURES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define TRANSPARENT {255, 0, 175}; // transparent color, a kind of pink

typedef struct Image_ {
    /* Image (array of pixels with a given width and height)
     *
     * The pixels are stored in a one dimentional array (dynamically allocated).
     * A pixel may be accessed as `(y * width + x) * 3 + c`, where `x` and `y` are the position of the pixel, and `c` is the component (red, green, blue or alpha).
     *
     * Note that an image have its origin on the top left corner.
     * */
	unsigned int width;
	unsigned int height;
	GLubyte * pixels; // dynamically allocated
} Image;

Image* image_new_from_file(FILE *f);
void image_delete(Image *image);

typedef struct Sprite_ {
	/* Sprite (part of an image).
	 *
	 * An image generally contains many different sprites, of a given `width` and `height`, located at `x` and `y` in the image.
	 * It corresponds to a texture id in OpenGL.
	 * */
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


#define FONT_MAX_CHAR 256

typedef struct Font_ {
    /* Font (set of sprite, one for each letter).
     *
     * The `char_width` and `char_height` characters are used to blit a string on screen.
     * */
	Image* font;
	Sprite* characters[FONT_MAX_CHAR];
    int char_width;
    int char_height;
} Font;

Font* font_new(Image* font_image, int char_width, int char_height);
void font_delete(Font* font);

void blit_text(Font* font, char* text, int x, int y);

#endif
