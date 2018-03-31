//
// Created by pbeaujea on 3/21/18.
//

#include "levels.h"

int position_index(Position pos) {
    if (pos.x >= MAP_WIDTH || pos.y >= MAP_HEIGHT) {
        return -1;
    }

    return pos.y * MAP_WIDTH + pos.x;
}


Level* level_new(bool *map, Position bubble_endpoint, Sprite *fill_tile, unsigned int num_monsters,
                 MonsterDef **monsters, Position *monster_positions) {

    if (fill_tile == NULL || monster_positions == NULL || monsters == NULL)
        return NULL;

    Level* level = malloc(sizeof(Level));

    if (level == NULL) {
        write_log("! cannot allocate level");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Level %p\n", level);
#endif

    level->bubble_endpoint = bubble_endpoint;
    level->fill_tile = sprite_copy(fill_tile);
    level->num_monsters = num_monsters;

    memcpy(level->map, map, MAP_HEIGHT * MAP_WIDTH * sizeof(bool));

    if (level->fill_tile == NULL) {
        level_delete(level);
        return NULL;
    }

    level->monster_positions = malloc(num_monsters * sizeof(Position));
    level->monsters = malloc(num_monsters * sizeof(MonsterDef*));

    if (level->monsters == NULL || level->monster_positions == NULL) {
        write_log("! unable to allocate monsters space");
        level_delete(level);
        return NULL;
    }

    memcpy(level->monster_positions, monster_positions, num_monsters * sizeof(Position));
    memcpy(level->monsters, monsters, num_monsters * sizeof(MonsterDef*));

    level->next = NULL;

    return level;
}
void level_delete(Level* level) {
    Level* next = level, *t = NULL;
    while (next != NULL) {
        t = next->next;

        sprite_delete(next->fill_tile);

        if (next->monster_positions != NULL)
            free(next->monster_positions);

        if (next->monsters != NULL)
            free(next->monsters);

        free(next);

#ifdef VERBOSE_MEM
        printf("-Level %p\n", level);
#endif

        next = t;
    }
}

Level *level_new_from_string(char *buffer, int *position_in_buff, Image *image_level, MonsterDef **base_monster_defs,
                             int num_monster_defs) {
    if (buffer == NULL || position_in_buff == NULL || base_monster_defs == NULL || image_level == NULL)
        return NULL;

    char** positions = malloc(MAP_WIDTH * sizeof(char*));
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

    long num_monster, posx, posy, sx, sy, index = 0, id_monster;
    num_monster = strtoul(positions[0], &next, 0);
    if (next == positions[0]) {
        write_log("! error while converting number #0");
        *position_in_buff = -1;
        return NULL;
    }

    posx = strtoul(positions[1], &next, 0);
    if (next == positions[1]) {
        write_log("! error while converting number #1");
        *position_in_buff = -1;
        return NULL;
    }

    posy = strtoul(positions[2], &next, 0);
    if (next == positions[2]) {
        write_log("! error while converting number #2");
        *position_in_buff = -1;
        return NULL;
    }

    sx = strtoul(positions[3], &next, 0);
    if (next == positions[3]) {
        write_log("! error while converting number #3");
        *position_in_buff = -1;
        return NULL;
    }

    sy = strtoul(positions[4], &next, 0);
    if (next == positions[4]) {
        write_log("! error while converting number #4");
        *position_in_buff = -1;
        return NULL;
    }

    if (num_monster < 0) {
        write_log("! negative number of monster ?");
        *position_in_buff = -1;
        return NULL;
    }

    if (posx < 0 || posy < 0) {
        write_log("! negative positions for bubble ?");
        *position_in_buff = -1;
        return NULL;
    }

    Position bubble_pos = {(unsigned  int) posx, (unsigned  int) posy};

    MonsterDef** monsters = malloc(num_monster * sizeof(MonsterDef*));
    Position* monsters_positions = malloc(num_monster * sizeof(Position));

    if (monsters_positions == NULL || monsters == NULL) {
        write_log("! unable to allocate data for level");
        *position_in_buff = -1;
        return NULL;
    }

    Sprite* sprite_tile = sprite_new(image_level, (int) sx, (int) sy, TILE_WIDTH, TILE_HEIGHT);

    if (sprite_tile == NULL) {
        *position_in_buff = -1;
        return NULL;
    }

    // catch monsters
    while(nextstart != NULL && index < num_monster) {
        err = datafile_line_field_positions(nextstart, 3, positions, &nextstart);
        if (err == 0) {
            write_log("# - adding monster %d", index);

            id_monster = strtoul(positions[0], &next, 0);
            if (next == positions[0]) {
                write_log("! error while converting number #0");
                *position_in_buff = -1;
                return NULL;
            }

            if (id_monster < 0 || id_monster >= num_monster_defs) {
                write_log("! invalid id %d (max is %d)", id_monster, num_monster_defs);
                continue;
            }

            posx = strtoul(positions[1], &next, 0);
            if (next == positions[1]) {
                write_log("! error while converting number #1");
                continue;
            }

            posy = strtoul(positions[2], &next, 0);
            if (next == positions[2]) {
                write_log("! error while converting number #2");
                continue;
            }

            if (posx < 0 || posy < 0) {
                write_log("! negative positions for monster ?");
                continue;
            }

            monsters[index] = base_monster_defs[id_monster];
            monsters_positions[index].x = (unsigned int) posx;
            monsters_positions[index].y = (unsigned int) posy;

            index++;
        }
    }

    if (index != num_monster) {
        write_log("! the number of monster added is lower than expected (%d < %d)", index, num_monster);
        *position_in_buff = -1;
        return NULL;
    }

    // catch map
    bool map[MAP_HEIGHT * MAP_WIDTH];

    index = 0;

    while(nextstart != NULL && index < MAP_HEIGHT) {
        err = datafile_line_field_positions(nextstart, MAP_WIDTH, positions, &nextstart);
        if (err == 0) {
            write_log("# - adding map line %d", index);
            for (unsigned int i = 0; i < MAP_WIDTH; ++i) {
                map[position_index((Position) {i, (unsigned int) (MAP_HEIGHT - index-1)})] = (*(positions[i]) != '0');
            }

            index++;
        }
    }

    if (index != MAP_HEIGHT) {
        write_log("! the number of line added is lower than expected (%d < %d)", index, MAP_HEIGHT);
        *position_in_buff = -1;
        return NULL;
    }

    Level* level = level_new(map, bubble_pos, sprite_tile, (unsigned) num_monster, monsters, monsters_positions);

    free(positions);
    free(monsters);
    free(monsters_positions);
    sprite_delete(sprite_tile);

    if (level == NULL) {
        *position_in_buff = -1;
        return NULL;
    }

    *position_in_buff = (int) (nextstart - buffer);
    return level;
}


Level *levels_new_from_file(FILE *f, Image *image_level, MonsterDef **base_monster_defs, int num_monster_defs,
                            unsigned int *num_levels) {
    if (f == NULL || base_monster_defs == NULL || num_levels == NULL || image_level == NULL)
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

    if (num <= 0 || next == positions[0])
        return NULL;

    *num_levels = (unsigned int) num;

    int index = 0;
    int position_in_buffer = (unsigned int) (next - buffer);
    Level* prev = NULL, *beg = NULL;
    while(position_in_buffer != -1 && index < num) {
        Level* level = level_new_from_string(buffer, &position_in_buffer, image_level, base_monster_defs, num_monster_defs);
        if (level != NULL) {
            write_log("# - adding level %d", index);
            if (beg != NULL) {
                prev->next = level;
                prev = prev->next;
            }

            else {
                prev = level;
                beg = prev;
            }

            index++;
        }
    }

    if (index != num) {
        write_log("! the number of level added is lower than expected (%d < %d)", index, num);
        return NULL;
    }

    free(positions);
    free(buffer);

    return beg;
}

MapObject* map_object_new(Position position, int width, int height, bool alive) {
    MapObject* obj = malloc(sizeof(MapObject));

    if (obj == NULL) {
        write_log("! can not allocate MapObject");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+MapObject %p\n", obj);
#endif

    obj->position.x = position.x;
    obj->position.y = position.y;

    obj->width = width;
    obj->height = height;
    obj->alive = alive;

    if (alive) {
        obj->jumping_counter = 0;
        obj->moving_counter = 0;
        obj->falling_counter = 0;
        obj->is_falling = false;
        obj->look_right = false;
    }

    return obj;
}

void map_object_delete(MapObject *map_object) {
    if (map_object != NULL)
        free(map_object);
}

MapObject* map_object_copy(MapObject *src) {
    if (src == NULL)
        return NULL;

    MapObject* obj = malloc(sizeof(MapObject));

    if (obj == NULL) {
        write_log("! can not allocate MapObject");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+MapObject %p (by copy)\n", obj);
#endif

    memcpy(obj, src, sizeof(MapObject));
    return obj;
}

void map_object_update(MapObject* object) {
    if (object->alive) {
        if(object->moving_counter > 0)
            object->moving_counter--;
        if (object->jumping_counter > 0)
            object->jumping_counter--;
    }
}

bool map_object_test_left(MapObject *representation, Level* level) {
    if (representation->alive) {
        Position n = {representation->position.x - 1, representation->position.y};

        if (n.x < 0)
            return false;

        for (int i = 0; i < representation->height; ++i) {
            if (level->map[position_index(n)])
                return false;
            n.y += 1;
        }

        return true;
    }

    return false;
}

bool map_object_test_right(MapObject *representation, Level* level) {
    if (representation->alive) {
        Position n = {representation->position.x + representation->width, representation->position.y};

        if (n.x >= MAP_WIDTH)
            return false;

        for (int i = 0; i < representation->height; ++i) {
            if (level->map[position_index(n)])
                return false;
            n.y += 1;
        }

        return true;
    }

    return false;
}


bool map_object_test_up(MapObject *representation, Level* level) {
    if (representation->alive) {
        Position n = {representation->position.x, representation->position.y + representation->height};

        if (n.y >= MAP_HEIGHT)
            return false;

        return true;
    }

    return false;
}

bool map_object_test_down(MapObject *representation, Level* level) {
    if (representation->alive) {
        Position n = {representation->position.x, representation->position.y - 1};

        if (n.y < 0)
            return false;

        for (int i = 0; i < representation->width; ++i) {
            if (level->map[position_index(n)])
                return false;
            n.x += 1;
        }

        return true;
    }

    return false;
}

bool map_object_move_left(MapObject *representation, Level *level) {
    if (representation->alive) {
        representation->look_right = false;

        if (map_object_test_left(representation, level) && representation->moving_counter == 0) {
            representation->position.x -= 1;
            representation->moving_counter = MAX_MOVING_COUNTER;

            if (representation->jumping_counter > 2 * JUMP_EVERY)
                representation->jumping_counter -= 2 * JUMP_EVERY;

            return true;
        }
    }

    return false;
}

bool map_object_move_right(MapObject *representation, Level *level) {
    if (representation->alive) {
        representation->look_right = true;

        if (map_object_test_right(representation, level) && representation->moving_counter == 0) {
            representation->position.x += 1;
            representation->moving_counter = MAX_MOVING_COUNTER;

            if (representation->jumping_counter > 2 * JUMP_EVERY)
                representation->jumping_counter -= 2 * JUMP_EVERY;

            return true;
        }
    }

    return false;
}

bool map_object_jump(MapObject *representation, Level *level, int jump) {
    if (representation->alive) {
        if (representation->jumping_counter == 0 && !representation->is_falling &&
            map_object_test_up(representation, level)) {
            representation->jumping_counter = jump * JUMP_EVERY;
            return true;
        }
    }

    return false;
}

void map_object_adjust(MapObject *representation, Level* level) {
    if (representation->alive) {
        if (representation->jumping_counter > 0) {
            if (map_object_test_up(representation, level)) {
                if (representation->jumping_counter % JUMP_EVERY == 0)
                    representation->position.y += 1;
                representation->jumping_counter--;
            } else
                representation->jumping_counter = 0;
        } else if (representation->is_falling) {
            if (map_object_test_down(representation, level)) {
                if (representation->falling_counter == 0) {
                    representation->position.y -= 1;
                    representation->falling_counter = FALL_EVERY - 1;
                } else
                    representation->falling_counter--;
            } else
                representation->is_falling = false;
        } else if (map_object_test_down(representation, level)) {
            representation->is_falling = true;
            representation->falling_counter = 0;
        }
    }
}

