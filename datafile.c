//
// Created by pbeaujea on 3/15/18.
//

#include "datafile.h"

int datafile_line_field_positions(char* text, unsigned int num_data, char** positions, char** nextstart) {
    /* Set `position` so that each of the case point to the begining of a "field". 
     * A field is separated from another by (at least) one space.
     * A line may contain a "#" character: everything after this character is ignored. 
     * If the line start by "#", the line is ignored.
     *
     * Expect `num_data` fields.
     * Set `nextstart` to the begining of the end of the line.
     *
     * Return 0 if erything is ok, -1 if a pointer is set incorectly, -2 if there is less or more field than expected and 1 if it is just a comment line.
     * So < 0 means error.
     */
    if (text == NULL || positions == NULL || nextstart == NULL)
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