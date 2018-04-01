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

int factorial(int a);

#endif //BUBBLES_UTILS_H
