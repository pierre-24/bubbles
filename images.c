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
                image->pixels[(y * image->width + x) * 4 + 3] = 0;
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

