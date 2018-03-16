//
// Created by pbeaujea on 3/15/18.
//

#include "game_object_definitions.h"

ItemDef *item_def_new(unsigned int points_given, extra_power_t extra_power, Sprite *sprite) {
    if (sprite == NULL)
        return NULL;

    ItemDef* def = malloc(sizeof(ItemDef));

    if (def == NULL){
        write_log("! cannot allocate item def");
        return NULL;
    }

    def->points_given = points_given;
    def->extra_power = extra_power;
    def->sprite = sprite;

    return def;
}

void item_def_delete(ItemDef* item) {
    if (item != NULL) {
        if (item->sprite != NULL)
            sprite_delete(item->sprite);

        free(item);
    }
}

ItemDef** item_defs_from_file(FILE* f, Image* items_texture, unsigned int* size) {
    *size = 0;

    if (f == NULL)
        return NULL;

    char* buffer = file_get_content(f);
    char** positions = malloc(6 * sizeof(char*));
    char* nextstart = buffer;
    int err = 0;

    // catch number of items
    do {
        err = datafile_line_field_positions(nextstart, 1, positions, &nextstart);
    } while (err == 1); // comment line in the beginning of the file

    if (err != 0) {
        write_log("! file should start with a single field (number)");
        return NULL;
    }

    int num = (int) strtol(positions[0], NULL, 10);

    if (num <= 0)
        return NULL;

    *size = (unsigned int) num;
    ItemDef** definitions = malloc(*size * sizeof(ItemDef*));
    int index = 0;

    while(nextstart != NULL && index < *size) {
        err = datafile_line_field_positions(nextstart, 6, positions, &nextstart);
        if (err == 0) {
            definitions[index] = NULL;
            index++;
        }
    }

    printf("%d\n", index);

    free(buffer);
    free(positions);

    return definitions;
}