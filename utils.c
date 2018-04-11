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


Counter* counter_new(int max, bool infinite, bool decrement) {
	Counter* counter = malloc(sizeof(Counter));
	
	if (counter == NULL) {
		write_log("! cannot allocate counter");
		return NULL;
	}

#ifdef VERBOSE_MEM
    printf("+Counter %p\n", counter);
#endif
	
	counter->max = max;
	counter->infinite = infinite;
	counter->decrement = decrement;
	
	counter_restart(counter, -1);
	
	return counter;
}


Counter* counter_copy(Counter* src) {
	Counter* counter = malloc(sizeof(Counter));
	
	if (counter == NULL) {
		write_log("! cannot allocate counter");
		return NULL;
	}

#ifdef VERBOSE_MEM
    printf("+Counter %p (by copy)\n", counter);
#endif

	memcpy(counter, src, sizeof(Counter));
	return counter;
}

void counter_delete(Counter* counter) {
	if (counter != NULL) {
		free(counter);

#ifdef VERBOSE_MEM
    printf("-Counter %p\n", counter);
#endif
	}
}

void counter_restart(Counter* counter, int nmax) {
	if (nmax > 0)
		counter->max = nmax;
		
	if (counter->decrement)
		counter->value = counter->max - 1;
	else
		counter->value = 0;
}

void counter_stop(Counter* counter) {
	if (!counter->decrement)
		counter->value = counter->max - 1;
	else
		counter->value = 0;
}

bool counter_stopped(Counter* counter) {
    if (!counter->decrement)
        return counter->value == counter->max - 1;
    else
        return counter->value == 0;
}

int counter_tick(Counter* counter) {
    if (counter_stopped(counter)) {
        if (counter->infinite)
            counter_restart(counter, -1);
    }

    else {
        counter->value += 1 * (counter->decrement ? -1 : 1);
	}
	
	return counter->value;
}

int counter_value(Counter* counter) {
	return counter->value;
}
