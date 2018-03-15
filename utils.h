//
// Created by pierre on 3/13/18.
//

#ifndef BUBBLES_UTILS_H
#define BUBBLES_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <GL/glut.h>

#define LOG_FILE "bubbles.log"

void init_log();
void write_log(char* format, ...);
void close_log();

#endif //BUBBLES_UTILS_H
