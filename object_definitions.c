//
// Created by pbeaujea on 3/15/18.
//

#include "object_definitions.h"

ItemDef *item_def_new(unsigned int points_given, extra_power_t extra_power, Sprite *sprite) {
    /* Create an item definition (and copy the sprite).
     * */
    if (sprite == NULL)
        return NULL;

    ItemDef* def = malloc(sizeof(ItemDef));

    if (def == NULL){
        write_log("! cannot allocate item def");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+ItemDef %p\n", def);
#endif

    def->points_given = points_given;
    def->extra_power = extra_power;
    def->sprite = sprite_copy(sprite);

    if (def->sprite == NULL) {
        item_def_delete(def);
        return NULL;
    }

    return def;
}

void item_def_delete(ItemDef* item) {
    /* Delete an item.
     * */
    if (item != NULL) {
        if (item->sprite != NULL)
            sprite_delete(item->sprite);
        free(item);

#ifdef VERBOSE_MEM
        printf("-ItemDef %p\n", item);
#endif
    }
}

ItemDef** item_defs_from_file(FILE* f, Image* items_texture, unsigned int* size) {
    /* Create an item array (of `size`) from a file.
     *
     * - The file starts with the number of items
     * - For each item, the points given, the extra power, and the sprite position (the sprite size is assumed to be `ITEM_WIDTH`x`ITEM_HEIGHT`).
     * */
    *size = 0;

    if (f == NULL)
        return NULL;

    char* buffer = file_get_content(f);
    char** positions = malloc(4 * sizeof(char*));
    char* nextstart = buffer;
    char* next;
    int err = 0;

    long points_given, extra_power, sx, sy;

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
        err = datafile_line_field_positions(nextstart, 4, positions, &nextstart);

        if (err == 0) {
            write_log("# - adding item %d", index);
            points_given = strtoul(positions[0], &next, 0);
            if (next == positions[0]) {
                write_log("! error while converting number #0");
                continue;
            }

            extra_power = strtoul(positions[1], &next, 0);
            if (next == positions[1]) {
                write_log("! error while converting number #1");
                continue;
            }

            sx = strtoul(positions[2], &next, 0);
            if (next == positions[2]) {
                write_log("! error while converting number #2");
                continue;
            }

            sy = strtoul(positions[3], &next, 0);
            if (next == positions[3]) {
                write_log("! error while converting number #3");
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

            Sprite* sprite = sprite_new(items_texture, (int) sx, (int) sy, ITEM_WIDTH, ITEM_HEIGHT);

            if (sprite == NULL)
                continue;


            definitions[index] = item_def_new((unsigned int) points_given, (extra_power_t) extra_power, sprite);
            sprite_delete(sprite);

            if (definitions[index] != NULL)
                index++;
        }
    }

    if (index != *size) {
        write_log("! the number of item added is lower than expected (%d < %d)", index, *size);
        return NULL;
    }

    free(buffer);
    free(positions);

    return definitions;
}

MonsterDef* monster_def_new(Animation **sprite_animation, unsigned int speed) {
    /* Create a monster definition (copy animations).
     * */
    if (sprite_animation == NULL)
        return NULL;

    MonsterDef* def = malloc(sizeof(MonsterDef));

    if (def == NULL){
        write_log("! cannot allocate monster def");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+MonsterDef %p\n", def);
#endif

    for (int i = 0; i < MA_NUMBER; ++i) {
        def->animation[i] = animation_copy(sprite_animation[i]);

        if (def->animation[i] == NULL) {
            monster_def_delete(def);
            return NULL;
        }
    }

    def->speed = speed;

    return def;
}

void monster_def_delete(MonsterDef* item) {
    /* Delete a monster
     * */
    if (item != NULL) {
        for (int i = 0; i < MA_NUMBER; ++i) {
            animation_delete(item->animation[i]);
        }

        free(item);
#ifdef VERBOSE_MEM
        printf("-MonsterDef %p\n", item);
#endif
    }
}

MonsterDef** monster_defs_from_file(FILE* f, Image* items_texture, unsigned int* size) {
    /* Create a monster array (of `size`) from a file.
     *
     * - The file starts with the number of monsters
     * - For each monster, the speed and the sprite y position.
     *   The sprite size is assumed to be `ITEM_WIDTH`x`ITEM_HEIGHT`, and the different sprites for the animation are assumed to be on the same line.
     *   Two frame per animation, `MA_NUMBER` animations.
     * */
    *size = 0;
    if (f == NULL)
        return NULL;

    char* buffer = file_get_content(f);
    char** positions = malloc(5 * sizeof(char*));
    char* nextstart = buffer;
    char* next;
    int err = 0;
    long speed, sy;

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
    MonsterDef** definitions = malloc(*size * sizeof(MonsterDef*));
    int index = 0;

    Animation* animation[MA_NUMBER];

    while(nextstart != NULL && index < *size) {
        err = datafile_line_field_positions(nextstart, 2, positions, &nextstart);
        if (err == 0) {
            write_log("# - adding item %d", index);

            speed = strtoul(positions[0], &next, 0);
            if (next == positions[0]) {
                write_log("! error while converting number #0");
                continue;
            }

            sy = strtoul(positions[1], &next, 0);
            if (next == positions[1]) {
                write_log("! error while converting number #1");
                continue;
            }

            if (speed < 0) {
                write_log("! negative speed");
                continue;
            }

            for (int i = 0; i < MA_NUMBER; ++i) {
                Sprite* sprite1 =  sprite_new(items_texture, i * 2 * MONSTER_WIDTH, (int) sy, MONSTER_WIDTH, MONSTER_HEIGHT);
                Sprite* sprite2 =  sprite_new(items_texture, (i * 2 + 1) * MONSTER_WIDTH, (int) sy, MONSTER_WIDTH, MONSTER_HEIGHT);

                if (sprite1 == NULL || sprite2 == NULL)
                    continue;

                animation[i] = animation_new(MONSTER_FRAMERATE);
                animation[i] = animation_add_frame(animation[i], sprite1);
                animation[i] = animation_add_frame(animation[i], sprite2);

                sprite_delete(sprite1);
                sprite_delete(sprite2);

                if (animation[i] == NULL)
                    continue;
            }

            definitions[index] = monster_def_new(animation, (unsigned int) speed);

            for (int j = 0; j < MA_NUMBER; ++j) {
                animation_delete(animation[j]);
            }

            if (definitions[index] != NULL)
                index++;
        }
    }

    if (index != *size) {
        write_log("! the number of item added is lower than expected (%d < %d)", index, *size);
        return NULL;
    }

    free(buffer);
    free(positions);

    return definitions;
}