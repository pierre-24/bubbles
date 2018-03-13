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
}

void close_log()  {
    if (log_file != NULL)
        fclose(log_file);

    log_file = NULL;
}