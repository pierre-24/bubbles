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

            if (points_given < 0) {
                write_log("! negative amount of points");
                continue;
            }

            if (extra_power < 0 || extra_power >= EP_NUMBER) {
                write_log("! unknown extra power");
                continue;
            }

            Sprite* sprite = sprite_new(items_texture, (int) sx, (int) sy, SPRITE_ITEM_WIDTH, SPRITE_ITEM_HEIGHT);

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

MonsterDef* monster_def_new(Animation *sprite_animation, unsigned int speed) {
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

    def->speed = speed;
    def->animation = animation_copy(sprite_animation);

    if (def->animation == NULL) {
        monster_def_delete(def);
        return NULL;
    }

    return def;
}

void monster_def_delete(MonsterDef* item) {
    if (item != NULL) {
        animation_delete(item->animation);
        free(item);
#ifdef VERBOSE_MEM
        printf("-MonsterDef %p\n", item);
#endif
    }
}

MonsterDef** monster_defs_from_file(FILE* f, Image* items_texture, unsigned int* size) {
    *size = 0;
    if (f == NULL)
        return NULL;

    char* buffer = file_get_content(f);
    char** positions = malloc(5 * sizeof(char*));
    char* nextstart = buffer;
    char* next;
    int err = 0;
    long speed, s1x, s1y, s2x, s2y;

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

    Sprite* animation_frames[2];
    Animation* animation = NULL;

    while(nextstart != NULL && index < *size) {
        err = datafile_line_field_positions(nextstart, 5, positions, &nextstart);
        if (err == 0) {
            write_log("# - adding item %d", index);

            speed = strtoul(positions[0], &next, 0);
            if (next == positions[0]) {
                write_log("error while converting number #0");
                continue;
            }

            s1x = strtoul(positions[1], &next, 0);
            if (next == positions[1]) {
                write_log("error while converting number #1");
                continue;
            }

            s1y = strtoul(positions[2], &next, 0);
            if (next == positions[2]) {
                write_log("error while converting number #2");
                continue;
            }

            s2x = strtoul(positions[3], &next, 0);
            if (next == positions[3]) {
                write_log("error while converting number #3");
                continue;
            }

            s2y = strtoul(positions[4], &next, 0);
            if (next == positions[4]) {
                write_log("error while converting number #4");
                continue;
            }

            if (speed < 0) {
                write_log("! negative speed");
                continue;
            }

            animation = animation_new();
            if (animation == NULL)
                continue;

            animation_frames[0] = sprite_new(items_texture, (int) s1x, (int) s1y, SPRITE_MONSTER_WIDTH, SPRITE_MONSTER_HEIGHT);
            animation_frames[1] = sprite_new(items_texture, (int) s2x, (int) s2y, SPRITE_MONSTER_WIDTH, SPRITE_MONSTER_HEIGHT);

            if (animation_frames[0] == NULL || animation_frames[1] == NULL)
                continue;

            animation = animation_add_frame(animation, animation_frames[0]);
            animation = animation_add_frame(animation, animation_frames[1]);

            if (animation == NULL)
                continue;

            definitions[index] = monster_def_new(animation, (unsigned int) speed);

            animation_delete(animation);
            sprite_delete(animation_frames[0]);
            sprite_delete(animation_frames[1]);

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