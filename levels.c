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

Level* level_new_from_string(char* buffer, unsigned int position_in_buff, MonsterDef** base_monster_defs) {
    return NULL;
}


Level *levels_new_from_file(FILE *f, MonsterDef **base_monster_defs, unsigned int *num_levels) {
    if (f == NULL || base_monster_defs == NULL || num_levels == NULL)
        return NULL;

    char* buffer = file_get_content(f);

    if (buffer == NULL)
        return NULL;

    char** positions = malloc(32 * sizeof(char*));
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

    if (num <= 0 || next == positions[0])
        return NULL;

    printf("%d\n", num);
    *num_levels = (unsigned int) num;

    return NULL;
}