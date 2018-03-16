#include "textures.h"

#define MAGIC_WHITESPACE "%*[ \n\t]"

Texture* texture_new_from_file(FILE* f)  {
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
    Texture* texture = malloc(sizeof(Texture));

    if (texture == NULL) {
        write_log("! cannot alocate texture");
        return NULL;
    }

    unsigned int max;

    fscanf(f, "%u" MAGIC_WHITESPACE "%u" MAGIC_WHITESPACE "%u" MAGIC_WHITESPACE, &(texture->width), &(texture->height), &max);

    if (max != 255) {
        write_log("weird color space: %d", max);
        return NULL;
    }

    // then, the data
    texture->pixels = calloc(texture->width * texture->height * 4, sizeof(GLubyte));

    if (texture->pixels == NULL) {
        write_log("! cannot allocate pixels data (%dx%d)", texture->width, texture->height);
        texture_delete(texture);
        return NULL;
    }

    char transp[3] = TRANSPARENT;

    for (int y=0; y < texture->height; y++) {
        for (int x = 0; x < texture->width; ++x) {
            texture->pixels[(y * texture->width + x) * 4 + 0] = (GLubyte) fgetc(f);
            texture->pixels[(y * texture->width + x) * 4 + 1] = (GLubyte) fgetc(f);
            texture->pixels[(y * texture->width + x) * 4 + 2] = (GLubyte) fgetc(f);

            if(memcmp(transp, texture->pixels + (y * texture->width + x) * 4, 3) == 0)
                texture->pixels[(y * texture->width + x) * 4 + 3] = 0;
            else
                texture->pixels[(y * texture->width + x) * 4 + 3] = 255;
        }
    }

    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLubyte* x = malloc(8 * 8 * 4 * sizeof(GLubyte));
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, /*texture->width, texture->height*/ 0, GL_RGBA, GL_UNSIGNED_BYTE, x);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 8, 8, /*texture->width, texture->height,*/ GL_RGBA, GL_UNSIGNED_BYTE, x);
    free(x);

    // ok :)
    return texture;
}

void texture_delete(Texture* texture) {
    if (texture != NULL) {
        if (texture->pixels != NULL)
            free(texture->pixels);
        free(texture);
    }
}

GLubyte * texture_get_pixel(Texture* texture, int x, int y) {
    if (x >= texture->width || y >= texture->height || x < 0 || y < 0)
        return NULL;

    return texture->pixels+((y * texture->width + x) * 4);
}

Sprite* sprite_new(Texture *texture, int x, int y, int w, int h) {
    if (texture == NULL)
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
        w = texture->width - x;
    }

    if (h < 0) {
        h = texture->height - y;
    }

    if (x + w > texture->width) {
        if (x >= texture->width)
            x = texture->width;

        w = texture->width - x;
    }

    if (y + h > texture->height) {
        if (y >= texture->height)
            y = texture->height;

        h = texture->height - y;
    }

    Sprite* sprite = malloc(sizeof(Sprite));

    if(sprite == NULL) {
        write_log("! cannot allocate sprite");
        return NULL;
    }

    sprite->x = x;
    sprite->y = y;
    sprite->width = w;
    sprite->height = h;
    sprite->texture = texture;

    return sprite;
}

void sprite_delete(Sprite* sprite) {
    if (sprite != NULL)
        free(sprite);
}

