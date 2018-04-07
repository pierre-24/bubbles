//
// Created by pbeaujea on 3/21/18.
//

#include "game_main.h"
#include "game.h"
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

MapObject *map_object_new(Position position, int width, int height) {
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
    
    obj->move_forward = false;
    obj->is_falling = false;
    
    obj->counter_x = counter_new(MOVE_EVERY, false, true);
    counter_stop(obj->counter_x);
    obj->counter_y = counter_new(MOVE_EVERY, false, true);
    counter_stop(obj->counter_y);
    obj->counter_jump = counter_new(1, false, true);
    counter_stop(obj->counter_jump);
    obj->counter_chase = counter_new(1, false, true);
    counter_stop(obj->counter_chase);
    
    if (obj->counter_x == NULL || obj->counter_y == NULL || obj->counter_jump == NULL) {
		map_object_delete(obj);
		return NULL;
	}

    return obj;
}

void map_object_delete(MapObject *obj) {
    if (obj != NULL) {
		counter_delete(obj->counter_x);
		counter_delete(obj->counter_y);
		counter_delete(obj->counter_jump);
        counter_delete(obj->counter_chase);
        free(obj);

#ifdef VERBOSE_MEM
    printf("-MapObject %p\n", obj);
#endif
	}
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
    obj->counter_x = counter_copy(src->counter_x);
    obj->counter_y = counter_copy(src->counter_y);
    obj->counter_jump = counter_copy(src->counter_jump);
    obj->counter_chase = counter_copy(src->counter_chase);
    
    return obj;
}


bool map_object_test_left(MapObject *obj, Level* level) {
    Position n = {obj->position.x - 1, obj->position.y};

    if (n.x < 0)
        return false;

    for (int i = 0; i < obj->height; ++i) {
        if (level->map[position_index(n)])
            return false;
        n.y += 1;
    }

    return true;
}

bool map_object_test_right(MapObject *obj, Level* level) {
    Position n = {obj->position.x + obj->width, obj->position.y};

    if (n.x >= MAP_WIDTH)
        return false;

    for (int i = 0; i < obj->height; ++i) {
        if (level->map[position_index(n)])
            return false;
        n.y += 1;
    }

    return true;
}


bool map_object_test_up(MapObject *obj, Level* level) {
    Position n = {obj->position.x, obj->position.y + obj->height};

    if (n.y >= MAP_HEIGHT)
        return false;

    return true;
}

bool map_object_test_down(MapObject *obj, Level* level) {
    Position n = {obj->position.x, obj->position.y - 1};

    if (n.y < 0)
        return false;

    for (int i = 0; i < obj->width; ++i) {
        if (level->map[position_index(n)])
            return false;
        n.x += 1;
    }

    return true;
}

bool map_object_can_move(MapObject* obj) {
	return counter_stopped(obj->counter_x);
}

bool map_object_can_jump(MapObject* obj) {
	return !obj->is_falling && counter_stopped(obj->counter_jump);
}

bool map_object_move_left(MapObject *obj, Level *level) {
    obj->move_forward = false;

    if (map_object_test_left(obj, level) && map_object_can_move(obj)) {
        obj->position.x -= 1;
        counter_restart(obj->counter_x, -1);

        if (counter_value(obj->counter_jump) > 2)
            obj->counter_jump->value -= 2;

        return true;
    }

    return false;
}

bool map_object_move_right(MapObject *obj, Level *level) {
    obj->move_forward = true;

    if (map_object_test_right(obj, level) && map_object_can_move(obj)) {
        obj->position.x += 1;
        counter_restart(obj->counter_x, -1);

        if (counter_value(obj->counter_jump) > 2)
            obj->counter_jump->value -= 2;

        return true;
    }

    return false;
}

bool map_object_jump(MapObject *obj, Level *level, int jump) {
	if (jump <= 0)
		return false;
	
    if (map_object_can_jump(obj) && map_object_test_up(obj, level)) {
        counter_stop(obj->counter_y);
        counter_restart(obj->counter_jump, jump);
        return true;
    }

    return false;
}

void map_object_adjust(MapObject *obj, Level* level) {
    counter_tick(obj->counter_x);
    counter_tick(obj->counter_y);
    counter_tick(obj->counter_chase);

    if (!counter_stopped(obj->counter_jump)) {
        if (map_object_test_up(obj, level)) {
            if (counter_stopped(obj->counter_y)) {
				counter_tick(obj->counter_jump);
                obj->position.y += 1;
				counter_restart(obj->counter_y, -1);
			}
        }
        else
			counter_stop(obj->counter_jump);
    } else if (obj->is_falling) {
        if (map_object_test_down(obj, level)) {
            if (counter_stopped(obj->counter_y)) {
                obj->position.y -= 1;
                counter_restart(obj->counter_y, -1);
            }
        } else {
            obj->is_falling = false;
            counter_stop(obj->counter_y);
		}
    } else if (map_object_test_down(obj, level) && counter_stopped(obj->counter_y)) {
        obj->is_falling = true;
    }
}

void map_object_chase(MapObject *moving, MapObject *target, Level *level, int speed) {
    if (map_object_can_move(moving)) {
        if (counter_stopped(moving->counter_chase)) {
            counter_restart(moving->counter_chase, speed);

            if (moving->position.y > target->position.y){
                // look for a hole
                bool look_left = true, look_right = true;
                MapObject* m = map_object_copy(moving);

                for (int i = 1; i < MAP_HEIGHT; ++i) {
                    if(look_right) {
                        m->position = (Position) {moving->position.x + i, moving->position.y};
                        if (!map_object_test_right(m, level)) {
                            look_right = false;
                            continue;
                        }

                        if (map_object_test_down(m, level)) {
                            map_object_move_right(moving, level);
                            break;
                        }
                    }

                    if(look_left) {
                        m->position = (Position) {moving->position.x - i, moving->position.y};

                        if (!map_object_test_left(m, level)) {
                            look_left = false;
                            continue;
                        }

                        if (map_object_test_down(m, level)) {
                            map_object_move_left(moving, level);
                            break;
                        }
                    }
                }

                map_object_delete(m);
            }

            else {
                bool action = false;

                if (moving->position.y < target->position.y && map_object_can_jump(target)) {
                    action = map_object_jump(moving, level, MONSTER_JUMP);
                }
                if (!action && moving->position.x < target->position.x) {
                    action = map_object_move_right(moving, level);
                }

                if (!action && moving->position.x > target->position.x) {
                    map_object_move_left(moving, level);
                }

            }
        }
    }
}

EffectivePosition map_object_to_effective_position(MapObject* mobj) {
	EffectivePosition p;
	p.x = (float) mobj->position.x;
	p.y = (float) mobj->position.y;
	
	float xp = (float) counter_value(mobj->counter_x) / MOVE_EVERY;
	float yp = (float) counter_value(mobj->counter_y) / MOVE_EVERY; 
	
	if (mobj->move_forward)
		p.x -= xp;
	else
		p.x += xp;
	
	if (mobj->is_falling)
		p.y += yp;
	else
		p.y -= yp;
	
	return p;
}

bool map_object_in_collision(MapObject* a, MapObject* b) {
	EffectivePosition pa = map_object_to_effective_position(a), pb = map_object_to_effective_position(b);

    double square_dist = pow(pa.x - pb.x, 2) + pow(pa.y - pb.y, 2);

    if (square_dist < CONTACT_DISTANCE)
        return true;
    else
        return false;
}

void blit_level(Level* level) {
    if (level != NULL) {
        for (unsigned int y = 0; y < MAP_HEIGHT; ++y) {
            for (unsigned int x = 0; x < MAP_WIDTH; ++x) {
                if (level->map[position_index((Position) {x, y})]) {
                    blit_sprite(level->fill_tile, x * TILE_WIDTH, y * TILE_HEIGHT, 0, 0);
                }
            }
        }

        // repeat the bottom on top
        for (unsigned int x = 0; x < MAP_WIDTH; ++x) {
            if (level->map[position_index((Position) {x, 0})]) {
                blit_sprite(level->fill_tile, x * TILE_WIDTH, MAP_HEIGHT * TILE_HEIGHT, 0, 0);
            }
        }
    }
}

void compute_real_pixel_positions(MapObject *obj, int *px, int *py) {
    EffectivePosition p = map_object_to_effective_position(obj);

    *px = (int) (p.x * TILE_WIDTH);
    *py = (int) (p.y * TILE_HEIGHT);
}