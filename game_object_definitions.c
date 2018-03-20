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
    char* next;
    int err = 0;

    long points_given, extra_power, sx, sy, sw, sh;

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
            write_log("# - adding item %d", index);
            points_given = strtoul(positions[0], &next, 0);
            if (next == positions[0]) {
                write_log("error while converting number #0");
                continue;
            }

            extra_power = strtoul(positions[1], &next, 0);
            if (next == positions[1]) {
                write_log("error while converting number #1");
                continue;
            }

            sx = strtoul(positions[2], &next, 0);
            if (next == positions[2]) {
                write_log("error while converting number #2");
                continue;
            }

            sy = strtoul(positions[3], &next, 0);
            if (next == positions[3]) {
                write_log("error while converting number #3");
                continue;
            }

            sw = strtoul(positions[4], &next, 0);
            if (next == positions[4]) {
                write_log("error while converting number #4");
                continue;
            }

            sh = strtoul(positions[5], &next, 0);
            if (next == positions[5]) {
                write_log("error while converting number #5");
                continue;
            }

            if (points_given < 0) {
                write_log("! negative amount of points");
                continue;
            }

            if (extra_power < 0 || extra_power >= EP_NUMBER) {
                write_log("! unknown extra power");
                continue;
            }

            Sprite* sprite = sprite_new(items_texture, (int) sx, (int) sy, (int) sw, (int) sh);

            if (sprite == NULL)
                continue;

            // printf("→ %ld %ld %ld %ld %ld %ld\n", points_given, extra_power, sx, sy, sw, sh);

            definitions[index] = item_def_new((unsigned int) points_given, (extra_power_t) extra_power, sprite);
            index++;
        }
    }

    free(buffer);
    free(positions);

    return definitions;
}