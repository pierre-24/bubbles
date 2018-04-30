//
// Created by pbeaujea on 3/21/18.
//

#include "game_main.h"
#include "game.h"
#include "levels.h"

int position_index(Position pos) {
    /* Return the position in the 1D array from the 2D position.
     * */
    if (pos.x >= MAP_WIDTH || pos.y >= MAP_HEIGHT) {
        return -1;
    }

    return pos.y * MAP_WIDTH + pos.x;
}


Level* level_new(bool *map, Position bubble_endpoint, Sprite *fill_tile, unsigned int num_monsters,
                 MonsterDef **monsters, Position *monster_positions) {
    /* Create and return a level.
     * */

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
    /* Delete a level.
     * */
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
    /* Create a level from a string.
     *
     * The string contains different lines:
     *
     * 1. The first line contains the number of monsters, the position of the bubble and the sprite position (sprite size is assumed to be `TILE_WIDTH`x`TILE_HEIGHT`.
     * 2. For each monster (one line per monster), its id (definition) and its initial position.
     * 3. `MAP_HEIGHT` line, containing `MAP_WIDTH` 0 or 1 (anything different from 0 actually means "wall").
     * */
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
    /* Create a series of levels from a file.
     *
     * The file starts by the number of level. Then, `level_new_from_string()` reads the levels.
     * */
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
            write_log("# >>> adding level %d", index);
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

LevelObject *level_object_new(Position position, int width, int height) {
    /* Create and return a new level object.
     * */
    LevelObject* obj = malloc(sizeof(LevelObject));

    if (obj == NULL) {
        write_log("! can not allocate LevelObject");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+LevelObject %p\n", obj);
#endif

    obj->position.x = position.x;
    obj->position.y = position.y;

    obj->width = width;
    obj->height = height;
    
    obj->move_forward = false;
    obj->is_falling = false;

    obj->falling_from_above = false;
    obj->target_position = (Position) {0, 0};
    
    obj->counter_x = counter_new(MOVE_EVERY, false, true);
    counter_stop(obj->counter_x);
    obj->counter_y = counter_new(MOVE_EVERY, false, true);
    counter_stop(obj->counter_y);
    obj->counter_jump = counter_new(1, false, true);
    counter_stop(obj->counter_jump);
    obj->counter_chase = counter_new(1, false, true);
    counter_stop(obj->counter_chase);
    
    if (obj->counter_x == NULL || obj->counter_y == NULL || obj->counter_jump == NULL) {
        level_object_delete(obj);
		return NULL;
	}

    return obj;
}

void level_object_delete(LevelObject *obj) {
    /* Delete a level object.
     * */
    if (obj != NULL) {
		counter_delete(obj->counter_x);
		counter_delete(obj->counter_y);
		counter_delete(obj->counter_jump);
        counter_delete(obj->counter_chase);
        free(obj);

#ifdef VERBOSE_MEM
    printf("-LevelObject %p\n", obj);
#endif
	}
}

LevelObject* level_object_copy(LevelObject *src) {
    /* Copy a level object (and its counters).
     * */
    if (src == NULL)
        return NULL;

    LevelObject* obj = malloc(sizeof(LevelObject));

    if (obj == NULL) {
        write_log("! can not allocate LevelObject");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+LevelObject %p (by copy)\n", obj);
#endif

    memcpy(obj, src, sizeof(LevelObject));
    obj->counter_x = counter_copy(src->counter_x);
    obj->counter_y = counter_copy(src->counter_y);
    obj->counter_jump = counter_copy(src->counter_jump);
    obj->counter_chase = counter_copy(src->counter_chase);
    
    return obj;
}


bool level_object_test_left(LevelObject *obj, Level *level) {
    /* Test if an object can go left.
     * */
    Position n = {obj->position.x - 1, obj->position.y};

    if (n.x < 0)
        return false;

    for (int i = 0; i < obj->height; ++i) {
        n.x = obj->position.x - 1;
        for (int j = 0; j < obj->width; ++j) {
            if (level->map[position_index(n)])
                return false;
            n.x += 1;
        }
        n.y += 1;
    }

    return true;
}

bool level_object_test_right(LevelObject *obj, Level *level) {
    /* Test if an object can go right.
     * */
    Position n = {obj->position.x + 1, obj->position.y};

    if (n.x >= MAP_WIDTH)
        return false;

    for (int i = 0; i < obj->height; ++i) {
        n.x = obj->position.x + 1;
        for (int j = 0; j < obj->width; ++j) {
            if (level->map[position_index(n)])
                return false;
            n.x += 1;
        }
        n.y += 1;
    }

    return true;
}


bool level_object_test_up(LevelObject *obj, Level *level) {
    /* Test if an object can go up.
     * */
    Position n = {obj->position.x, obj->position.y + obj->height};

    if (n.y >= MAP_HEIGHT)
        return false;

    return true;
}

bool level_object_test_down(LevelObject *obj, Level *level) {
    /* Test if an object can go down.
     * */
    Position n = {obj->position.x, obj->position.y - 1};

    if (n.y < 0)
        return false;

    for (int i = 0; i < obj->width; ++i) {
        n.y = obj->position.y-1;
        for (int j = 0; j < obj->height; ++j) {
            if (level->map[position_index(n)])
                return false;
            n.y += 1;
        }
        n.x += 1;
    }

    return true;
}

bool level_object_can_move(LevelObject *obj) {
    /* Test if an object can perform a movement in left or right direction (but not if the movement is possible)
     * */
	return counter_stopped(obj->counter_x);
}

bool level_object_can_jump(LevelObject *obj) {
    /* Test if an object can perform a jump (but not if the jump is possible).
     * */
	return !obj->is_falling && counter_stopped(obj->counter_jump);
}

bool level_object_move_left(LevelObject *obj, Level *level) {
    /* Move an object to the left, if possible.
     *
     * Return true if the movement was performed, false otherwise.
     * */
    obj->move_forward = false;

    if (level_object_test_left(obj, level) && level_object_can_move(obj)) {
        obj->position.x -= 1;
        counter_restart(obj->counter_x, -1);

        if (counter_value(obj->counter_jump) > 2)
            obj->counter_jump->value -= 2;

        return true;
    }

    return false;
}

bool level_object_move_right(LevelObject *obj, Level *level) {
    /* Move an object to the right, if possible.
     *
     * Return true if the movement was performed, false otherwise.
     * */
    obj->move_forward = true;

    if (level_object_test_right(obj, level) && level_object_can_move(obj)) {
        obj->position.x += 1;
        counter_restart(obj->counter_x, -1);

        if (counter_value(obj->counter_jump) > 2)
            obj->counter_jump->value -= 2;

        return true;
    }

    return false;
}

bool level_object_jump(LevelObject *obj, Level *level, int jump) {
    /* Jump, if possible.
     *
     * Return true if the movement was performed, false otherwise.
     * */
	if (jump <= 0)
		return false;
	
    if (level_object_can_jump(obj) && level_object_test_up(obj, level)) {
        counter_stop(obj->counter_y);
        counter_restart(obj->counter_jump, jump);
        return true;
    }

    return false;
}

void level_object_adjust(LevelObject *obj, Level *level) {
    /* "Adjust" an object: update counters, make it fall if needed, stop falling if needed.
     * */
    counter_tick(obj->counter_x);
    counter_tick(obj->counter_y);
    counter_tick(obj->counter_chase);

    if (!counter_stopped(obj->counter_jump)) {
        if (level_object_test_up(obj, level)) {
            if (counter_stopped(obj->counter_y)) {
				counter_tick(obj->counter_jump);
                obj->position.y += 1;
				counter_restart(obj->counter_y, -1);
			}
        }
        else
			counter_stop(obj->counter_jump);
    } else if (obj->is_falling) {
        if (!obj->falling_from_above) {
            if (level_object_test_down(obj, level)) {
                if (counter_stopped(obj->counter_y)) {
                    if (obj->position.y == 0)
                        obj->position.y = MAP_HEIGHT;

                    obj->position.y -= 1;
                    counter_restart(obj->counter_y, -1);
                }
            } else {
                obj->is_falling = false;
                counter_stop(obj->counter_y);
            }
        }

        else if (counter_stopped(obj->counter_y)) {
            if (obj->position.y > obj->target_position.y) {
                obj->position.y -= 1;
                counter_restart(obj->counter_y, -1);
            } else
                obj->falling_from_above = false;
        }

    } else if (level_object_test_down(obj, level) && counter_stopped(obj->counter_y)) {
        obj->is_falling = true;
    }
}

void level_object_chase(LevelObject *moving, LevelObject *target, Level *level, int speed) {
    /* Make an object chase another.
     *
     * Every `speed` frame, set the direction in order to get closer to the target.
     *
     * Note that if the target is below the object, it will find the closer hole to get closer.
     * */
    if (level_object_can_move(moving) && !moving->falling_from_above && !target->falling_from_above) {
        if (counter_stopped(moving->counter_chase)) {
            counter_restart(moving->counter_chase, speed);

            if (moving->position.y > target->position.y){
                // look for a hole
                bool look_left = true, look_right = true;
                LevelObject* m = level_object_copy(moving);

                for (int i = 1; i < MAP_HEIGHT; ++i) {
                    if(look_right) {
                        m->position = (Position) {moving->position.x + i, moving->position.y};
                        if (!level_object_test_right(m, level)) {
                            look_right = false;
                            continue;
                        }

                        if (level_object_test_down(m, level)) {
                            level_object_move_right(moving, level);
                            break;
                        }
                    }

                    if(look_left) {
                        m->position = (Position) {moving->position.x - i, moving->position.y};

                        if (!level_object_test_left(m, level)) {
                            look_left = false;
                            continue;
                        }

                        if (level_object_test_down(m, level)) {
                            level_object_move_left(moving, level);
                            break;
                        }
                    }
                }

                level_object_delete(m);
            }

            else {
                bool action = false;

                if (moving->position.y < target->position.y && level_object_can_jump(target)) {
                    action = level_object_jump(moving, level, MONSTER_JUMP);
                }
                if (!action && moving->position.x < target->position.x) {
                    action = level_object_move_right(moving, level);
                }

                if (!action && moving->position.x > target->position.x) {
                    level_object_move_left(moving, level);
                }

            }
        }
    }
}

void map_object_set_falling_from_above(LevelObject *obj, Position target) {
    /* Set the object in order to "fall from above" the level, to a given `target` position.
     * */
    obj->falling_from_above = true;
    obj->target_position = target;
    obj->position = target;
    obj->position.y = FALLING_FROM;
    obj->is_falling = true;
}

EffectivePosition level_object_to_effective_position(LevelObject *mobj) {
    /* From the position and the `counter_x`/`counter_y` counters, get the effective position.
     * */
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

bool level_object_in_collision(LevelObject *a, LevelObject *b) {
    /* Test if objects are in collision, but not using a bounding box, but a minimal `sqrt(CONTACT_DISTANCE)` distance between the two objects.
     *
     * */
	EffectivePosition pa = level_object_to_effective_position(a), pb = level_object_to_effective_position(b);

    double square_dist = pow(pa.x - pb.x, 2) + pow(pa.y - pb.y, 2);

    if (square_dist < CONTACT_DISTANCE)
        return true;
    else
        return false;
}

void blit_level(Level *level, int y_shift) {
    /* Blit a whole level on the screen, using `fill_tile`.
     * */
    if (level != NULL) {
        for (unsigned int y = 0; y < MAP_HEIGHT; ++y) {
            for (unsigned int x = 0; x < MAP_WIDTH; ++x) {
                if (level->map[position_index((Position) {x, y})]) {
                    blit_sprite(level->fill_tile, x * TILE_WIDTH, y * TILE_HEIGHT + y_shift, 0, 0);
                }
            }
        }

        // repeat the bottom on top
        for (unsigned int x = 0; x < MAP_WIDTH; ++x) {
            if (level->map[position_index((Position) {x, 0})]) {
                blit_sprite(level->fill_tile, x * TILE_WIDTH, MAP_HEIGHT * TILE_HEIGHT + y_shift, 0, 0);
            }
        }
    }
}

void compute_real_pixel_positions(LevelObject *obj, int *px, int *py) {
    /* Get the position in pixels on screen, from the effective position.
     * */
    EffectivePosition p = level_object_to_effective_position(obj);

    *px = (int) (p.x * TILE_WIDTH);
    *py = (int) (p.y * TILE_HEIGHT);
}