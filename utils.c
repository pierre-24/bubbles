//
// Created by pierre on 3/13/18.
//

#include "utils.h"

FILE* log_file = NULL;

void init_log() {
    /* Open the log file, once and for all.
     * */
    log_file = fopen(LOG_FILE, "w");
    if (log_file == NULL) {
        printf("! unable to open log file!\n");
        exit(-1);
    }
}

void write_log(char* format, ...) {
    /* Write a line into the log file. Use a printf-like syntax (thanks to `vfprintf()`).
     * 
     * Note: he following piece of code is adapted from to http://stackoverflow.com/a/1056423 
     * */

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
    /* Close the log file 
     * */
    if (log_file != NULL)
        fclose(log_file);

    log_file = NULL;
}

char* file_get_content(FILE* f) {
    /* Read the whole content of a file (`f`) and return a string.
     * The programmer must delete the string after use.
     * */

    long curr_pos = ftell(f); // get current
    fseek (f, 0, SEEK_END); // go to end
    long length = ftell(f) - curr_pos;
    fseek (f, curr_pos, SEEK_SET); // go back to current

    char* buffer = malloc((length + 1) * sizeof(char));
    if (buffer == NULL) {
        write_log("! cannot allocate buffer to read file (size=%d)", length);
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, (size_t) length, f);
    buffer[length] = '\0';

    return buffer;
}

char *strnextspace(char *str) {
    /* Return the adress of the next space (according to `isspace()`, wich includes `\t`, `\n`, ..) in the string, or NULL if it reaches the end of the string.
     * Note that the search starts from the current position.
     * */
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
    /* Return the adress of the next `\n` in the string, or NULL if it reaches the end of the string.
     * Note that the search starts from the current position.
     * */
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
    /* Return the adress of the next non-space character (according to `!isspace()`) in the string, or NULL if it reaches the end of the string.
     * Note that the search starts from the current position.
     * */
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
    /* Create a counter
     * */
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
    /* Copy a counter. 
     * */
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
    /* Delete a counter
     * */
	if (counter != NULL) {
		free(counter);

#ifdef VERBOSE_MEM
    printf("-Counter %p\n", counter);
#endif
	}
}

void counter_restart(Counter* counter, int nmax) {
    /* Restart (set to its "minimum" position) a counter.
     * The `nmax` set a new `max` if > 0 (otherwise it use the current maximum).
     * */
	if (nmax > 0)
		counter->max = nmax;

    counter->value = 0;
}

void counter_stop(Counter* counter) {
    /* Stop (set to its "maximum" position) a counter.
     * */

    counter->value = counter->max - 1;
}

bool counter_stopped(Counter* counter) {
    /* Test if a counter value equals its "maximum".
     * */

    return counter->value == counter->max - 1;
}

int counter_tick(Counter* counter) {
    /* Increase (or decrease) the value.
     * Also restart the counter if stopped, but infinite.
     * */
    if (counter_stopped(counter)) {
        if (counter->infinite)
            counter_restart(counter, -1);
    }

    else
        counter->value += 1;
	
	return counter_value(counter);
}

int counter_value(Counter* counter) {
    /* Get the counter current value.
     * */
    if (!counter->decrement)
	    return counter->value;
    else
        return counter->max - counter->value - 1;
}

void custom_usleep(int msecs) {
    /* Mimic the usleep() function, since it is not part of the C99 standard.
     *
     * Uses select, which looks whether a file descriptor becomes available or not.
     * But fd=0 does not!
     *
     * Solution due to https://stackoverflow.com/a/264378.
     * */
    // printf("sleep for %d\n", msecs);
    struct timeval tv;
    tv.tv_sec = msecs / 1000;
    tv.tv_usec = (msecs % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
}
