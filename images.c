#include "game.h"
#include "images.h"

#define MAGIC_WHITESPACE "%*[ \n\t]"

Image* image_new_from_file(FILE *f)  {
    // read a PPM (see http://netpbm.sourceforge.net/doc/ppm.html)
    if (f == NULL)
        return NULL;
    
    char c[3] = {fgetc(f), fgetc(f), '\0'};

    // first line should be "P6"
    if(strcmp(c, "P6") != 0) {
        write_log("image is not a P6!");
        return NULL;
    }

    fgetc(f);

    // second line is a comment
    if(fgetc(f) == '#') {
        while (fgetc(f) != '\n')
            continue;
    }

    // then comes the width, height and maximum value
    Image* image = malloc(sizeof(Image));

    if (image == NULL) {
        write_log("! cannot allocate image");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+image %p\n", image);
#endif

    unsigned int max;

    fscanf(f, "%u" MAGIC_WHITESPACE "%u" MAGIC_WHITESPACE "%u" MAGIC_WHITESPACE, &(image->width), &(image->height), &max);

    if (max != 255) {
        write_log("weird color space: %d", max);
        return NULL;
    }

    // then, the data
    image->pixels = calloc(image->width * image->height * 4, sizeof(GLubyte));

    if (image->pixels == NULL) {
        write_log("! cannot allocate pixels data (%dx%d)", image->width, image->height);
        image_delete(image);
        return NULL;
    }

    char transp[3] = TRANSPARENT;

    for (int y=0; y < image->height; y++) {
        for (int x = 0; x < image->width; ++x) {
            image->pixels[(y * image->width + x) * 4 + 0] = (GLubyte) fgetc(f);
            image->pixels[(y * image->width + x) * 4 + 1] = (GLubyte) fgetc(f);
            image->pixels[(y * image->width + x) * 4 + 2] = (GLubyte) fgetc(f);

            if(memcmp(transp, image->pixels + (y * image->width + x) * 4, 3) == 0)
                memset(image->pixels + (y * image->width + x) * 4, 0, 3 * sizeof(GLubyte)); // transparent black pixel
            else
                image->pixels[(y * image->width + x) * 4 + 3] = 255;
        }
    }

    // ok :)
    return image;
}

void image_delete(Image *image) {
    if (image != NULL) {
        if (image->pixels != NULL)
            free(image->pixels);
        free(image);

#ifdef VERBOSE_MEM
        printf("-image %p\n", image);
#endif
    }
}

Sprite* sprite_new(Image *image, int x, int y, int w, int h) {
    if (image == NULL)
        return NULL;

    if (x < 0) {
        w += x;
        x = 0;
    }

    if (y < 0) {
        h += y;
        y = 0;
    }

    if (w < 0) {
        w = image->width - x;
    }

    if (h < 0) {
        h = image->height - y;
    }

    if (x + w > image->width) {
        if (x >= image->width)
            x = image->width;

        w = image->width - x;
    }

    if (y + h > image->height) {
        if (y >= image->height)
            y = image->height;

        h = image->height - y;
    }

    // printf("→ %d %d %d %d\n", x, y, w, h);

    Sprite* sprite = malloc(sizeof(Sprite));

    if(sprite == NULL) {
        write_log("! cannot allocate sprite");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+sprite %p\n", sprite);
#endif

    sprite->x = x;
    sprite->y = y;
    sprite->width = w;
    sprite->height = h;
    sprite->image = image;

    // create data buffer and fill it with sub image
    GLubyte* data = malloc(sprite->width * sprite->height * 4 * sizeof(GLubyte));

    for (int i = 0; i < sprite->height; ++i)
        memcpy(data + (i*sprite->width) * 4, image->pixels + ((sprite->y+i) * image->width + sprite->x) * 4, (size_t) sprite->width * 4);

    // create the opengl texture
    glGenTextures(1, &(sprite->texture_id));
    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite->width, sprite->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    free(data);
    return sprite;
}

void sprite_delete(Sprite* sprite) {
    if (sprite != NULL) {
        free(sprite);

#ifdef VERBOSE_MEM
        printf("-sprite %p\n", sprite);
#endif
    }
}

Sprite* sprite_copy(Sprite* origin) {
    Sprite* dest = malloc(sizeof(Sprite));

    if (dest == NULL) {
        write_log("! unable to copy Sprite");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+sprite %p (by copy)\n", dest);
#endif

    memcpy(dest, origin, sizeof(Sprite));
    return dest;
}

void blit_sprite(Sprite *sprite, int sx, int sy, bool flip_x, bool flip_y) {
    if (sprite != NULL) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sprite->texture_id);

        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 1.0); glVertex2i(sx + (flip_x ? sprite->width : 0), sy + ((flip_y) ? sprite->height : 0));
        glTexCoord2d(0.0, 0.0); glVertex2i(sx + (flip_x ? sprite->width : 0), sy + ((flip_y) ? 0 : sprite->height));
        glTexCoord2d(1.0, 0.0); glVertex2i(sx + (flip_x ? 0 : sprite->width), sy + ((flip_y) ? 0 : sprite->height));
        glTexCoord2d(1.0, 1.0); glVertex2i(sx + (flip_x ? 0 : sprite->width), sy + ((flip_y) ? sprite->height : 0));
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}

Font* font_new(Image* font_image, int char_width, int char_height) {
    if (font_image == NULL)
        return NULL;

    Font* font = malloc(sizeof(Font));

    if (font == NULL) {
        write_log("! cannot allocate font");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Font\n", font);
#endif

    font->char_width = char_width;
    font->char_height = char_height;
    font->font = font_image;

    memset(font->characters, 0, FONT_MAX_CHAR * sizeof(Sprite*));

    int char_per_line = font_image->width / char_width;
    int imx, imy;

    for (int i = 0; i < FONT_MAX_CHAR; ++i) {
        imx = (i % char_per_line) * char_width;
        imy = i / char_per_line * char_height;

        font->characters[i] = sprite_new(font_image, imx, imy, char_width, char_height);

        if (font->characters[i] == NULL) {
            font_delete(font);
            return NULL;
        }
    }

    return font;
}

void font_delete(Font* font) {
    if (font != NULL) {
        for (int i = 0; i < 256; ++i) {
            sprite_delete(font->characters[i]);
        }

        free(font);
#ifdef VERBOSE_MEM
        printf("-Font\n", font);
#endif
    }
}

void blit_text(Font* font, char* text, int x, int y) {
    if (font != NULL && text != NULL) {
        int tx = 0;
        int ty = 0;

        char* p = text;
        while (*p != '\0') {
            if (*p == '\n') {
                tx = 0;
                ty += font->char_height;
            } else {
                blit_sprite(font->characters[(int) *p], x + tx, y + ty, false, false);
                tx += font->char_width;
            }

            p++;
        }
    }
}