//
// Created by pbeaujea on 3/15/18.
//

#include "datafile.h"

int datafile_line_field_positions(char* text, unsigned int num_data, char** positions, char** nextstart) {
    if (text == NULL)
        return -1;

    unsigned int found = 0;
    char* pos = strnextnspace(text);
    char* endofline = strnextline(pos);

    while ((pos != NULL) && (*pos != '#') && (found <=  num_data) && (endofline == NULL || pos < endofline) ) {
        positions[found] = pos;
        found++;
        pos = strnextnspace(strnextspace(pos)); // go to next field
    }

    *nextstart = endofline;

    if (found != num_data) {
        if (found == 0) // just a comment line
            return 1;

        if (found < num_data)
            write_log("! line contains less fields than expected (%d < %d)", found, num_data);
        else if (found > num_data)
            write_log("! line contains more fields than expected (%d > %d)", found, num_data);

        return -2;
    }

    return 0;
}