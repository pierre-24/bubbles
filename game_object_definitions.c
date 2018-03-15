//
// Created by pbeaujea on 3/15/18.
//

#include "game_object_definitions.h"

ItemDef* item_def_new(unsigned int points_given, Sprite* sprite, extra_power_t extra_power) {
    return NULL;
}

void item_def_delete(ItemDef* item) {
    if (item != NULL)
        free(item);
}

ItemDef** item_defs_from_file(FILE* f, Texture* items_texture, unsigned int* size) {
    *size = 0;

    if (f == NULL)
        return NULL;

    char* buffer = file_get_content(f);
    char** positions = datafile_line_field_positions(buffer, 6);

    free(buffer);

    return NULL;
}