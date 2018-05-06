//
// Created by pierre on 3/13/18.
//

#ifndef BUBBLES_UTILS_H
#define BUBBLES_UTILS_H

// #define VERBOSE_MEM 0

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#include <GL/glut.h>
#include <GL/freeglut.h>

#define LOG_FILE "bubbles.log"

void init_log();
void write_log(char* format, ...);
void close_log();

char* file_get_content(FILE* f);

char *strnextspace(char *str);
char *strnextline(char *str);
char *strnextnspace(char *str);

typedef struct Counter_ {
    /* Define a counter.
     * 
     * A counter ticks until a given value (`max`). 
     * The "direction" is given by `decrement`. The value is between [0;max[, and the counter is set to its "mininmum":
     * - `max` if decrement=true, 
     * - 0 otherwise.
     * A counter may ticks infinitelly (`infinite`): if it gets to the maximum, the counter is set to its "minimum".
     * */
    int value;
    int max;
    bool infinite;
    bool decrement;
} Counter;

Counter* counter_new(int max, bool infinite, bool decrement);
void counter_delete(Counter* counter);

Counter* counter_copy(Counter* counter);

void counter_restart(Counter* counter, int nmax);
void counter_stop(Counter* counter);
bool counter_stopped(Counter* counter);

int counter_tick(Counter* counter);
int counter_value(Counter* counter);

#endif //BUBBLES_UTILS_H
