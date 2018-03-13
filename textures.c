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
    unsigned int max;

    fscanf(f, "%u" MAGIC_WHITESPACE "%u" MAGIC_WHITESPACE "%u" MAGIC_WHITESPACE, &(texture->width), &(texture->height), &max);

    if (max != 255) {
        write_log("weird color space: %d", max);
        return NULL;
    }

    // then, the data
    texture->pixels = malloc(texture->width * texture->height * sizeof(Pixel));

    char transp[3] = TRANSPARENT;

    for (int y=0; y < texture->height; y++) {
        for (int x = 0; x < texture->width; ++x) {
            texture->pixels[y * texture->width + x].red = (GLubyte) fgetc(f);
            texture->pixels[y * texture->width + x].green = (GLubyte) fgetc(f);
            texture->pixels[y * texture->width + x].blue = (GLubyte) fgetc(f);
            texture->pixels[y * texture->width + x].transparent = false;

            if(memcmp(transp, &(texture->pixels[y * texture->width + x]), 3) == 0)
                texture->pixels[y * texture->width + x].transparent = true;
        }
    }

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

Pixel* texture_get_pixel(Texture* texture, int x, int y) {
    if (x >= texture->width || y >= texture->height || x < 0 || y < 0)
        return NULL;

    return &(texture->pixels[y * texture->width + x]);
}


