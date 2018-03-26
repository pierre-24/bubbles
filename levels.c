//
// Created by pbeaujea on 3/21/18.
//

#include "levels.h"


Level* level_new(bool** map, Position bubble_endpoint, Sprite* fill_tile, unsigned int num_monsters, MonsterDef** monsters, Position* monster_positions) {
    return NULL;
}
void level_delete(Level* level) {
    if (level != NULL) {
        sprite_delete(level->fill_tile);
        if (level->monster_positions != NULL)
            free(level->monster_positions);

        if (level->monsters != NULL)
            free(level->monsters);

        free(level);

#ifdef VERBOSE_MEM
        printf("-Level %p\n", level);
#endif
    }
}

Level* level_new_from_string(char* buffer, int* position_in_buff, MonsterDef** base_monster_defs) {
    if (buffer == NULL || position_in_buff == NULL || base_monster_defs == NULL)
        return NULL;

    char** positions = malloc(32 * sizeof(char*));
    char* nextstart = buffer + *position_in_buff;
    char* next;
    int err = 0;

    // catch level header
    do {
        err = datafile_line_field_positions(nextstart, 5, positions, &nextstart);
    } while (err == 1); // comment line in the beginning

    if (err != 0) {
        write_log("! level should start with definition (5 fields)");

        *position_in_buff = -1;
        return NULL;
    }

    *position_in_buff = -1;
    return NULL;
}


Level *levels_new_from_file(FILE *f, MonsterDef **base_monster_defs, unsigned int *num_levels) {
    if (f == NULL || base_monster_defs == NULL || num_levels == NULL)
        return NULL;

    char* buffer = file_get_content(f);

    if (buffer == NULL)
        return NULL;

    char** positions = malloc(1 * sizeof(char*));
    char* nextstart = buffer;
    char* next;
    int err = 0;

    // catch number of level
    do {
        err = datafile_line_field_positions(nextstart, 1, positions, &nextstart);
    } while (err == 1); // comment line in the beginning of the file

    if (err != 0) {
        write_log("! file should start with a single field (number)");
        return NULL;
    }

    int num = (int) strtol(positions[0], &next, 0);
    free(positions);

    if (num <= 0 || next == positions[0])
        return NULL;

    printf("%d\n", num);
    *num_levels = (unsigned int) num;

    int index = 0;
    int position_in_buffer = (unsigned int) (next - buffer);
    Level* prev = NULL;
    while(position_in_buffer != -1 && index < num) {
        Level* level = level_new_from_string(buffer, &position_in_buffer, base_monster_defs);
        if (level != NULL) {
            write_log("# - adding level %d", index);
            if (prev != NULL) {
                prev->next = level;
            }

            else
                prev = level;

            index++;
        }
    }

    if (index != num) {
        write_log("! the number of level added is lower than expected (%d < %d)", index, num);
        return NULL;
    }

    free(buffer);

    return NULL;
}