//
// Created by pbeaujea on 3/15/18.
//

#include "datafile.h"

char** datafile_line_field_positions(char* text, unsigned int num_data) {
    if (text == NULL)
        return NULL;

    char** positions = malloc(num_data * sizeof(char*));

    unsigned int found = 0;
    char* pos = strnextnspace(text);

    while (pos != NULL) {
        if (found >=  num_data) {
            write_log("! line contains more fields than expected");
            break;
        }

        if (*pos == '#') // comment
            break;

        positions[found] = pos;
        found++;
        pos = strnextnspace(strnextspace(pos)); // go to next field
    }

    if (found != num_data - 1) {
        free(positions);
        if (found != 0)
            write_log("! line contains less fields than expected (%d < %d)", found, num_data - 1);

        return NULL;
    }

    return positions;
}