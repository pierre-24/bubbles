#include "textures.h"

Texture* texture_new_from_file(FILE* f)  {
    if (f == NULL)
        return NULL;
    
    char c[3] = {fgetc(f), fgetc(f), '\0'};
    
    if(strcmp(c, "P6") != 0)
        printf("its P6!");
    
    return NULL;
}


