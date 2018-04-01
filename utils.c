//
// Created by pierre on 3/13/18.
//

#include "utils.h"

FILE* log_file = NULL;

void init_log() {
    log_file = fopen(LOG_FILE, "w");
    if (log_file == NULL) {
        printf("! unable to open log file!\n");
        exit(-1);
    }
}

void write_log(char* format, ...) {
    /* The following piece of code is adapted from to http://stackoverflow.com/a/1056423 */

    va_list argptr;
    va_start(argptr, format);

    if (log_file != NULL) {
        vfprintf(log_file, format, argptr);
        fputc('\n', log_file);
    }

    else
        vfprintf(stdout, format, argptr);

    va_end(argptr);
    fflush(log_file);
}

void close_log()  {
    if (log_file != NULL)
        fclose(log_file);

    log_file = NULL;
}

char* file_get_content(FILE* f) {

    fseek (f, 0, SEEK_END);
    long length = ftell (f);
    fseek (f, 0, SEEK_SET);

    char* buffer = malloc(length * sizeof(char));
    if (buffer == NULL) {
        write_log("! cannot allocate buffer to read file (size=%d)", length);
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, (size_t) length, f);

    return buffer;
}

char *strnextspace(char *str) {
    if (str == NULL)
        return NULL;

    char* next = str;

    while(!isspace(*next) && *next != 0)
        next++;

    if(*next == 0)
        return NULL;

    return next;
}

char *strnextline(char *str) {
    if (str == NULL)
        return NULL;

    char* next = str;

    while(*next != '\n' && *next != 0)
        next++;

    if(*next == 0)
        return NULL;

    return next;
}


char *strnextnspace(char *str) {
    if (str == NULL)
        return NULL;

    char* next = str;

    while(isspace(*next) && *next != 0)
        next++;

    if(*next == 0)
        return NULL;

    return next;
}

int factorial(int a)  {
    return 0 == a ? 1 : (a * factorial(a - 1));
}