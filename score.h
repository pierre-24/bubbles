//
// Created by pbeaujea on 4/11/18.
//

#ifndef BUBBLES_SCORE_H
#define BUBBLES_SCORE_H

#include "utils.h"

#define SCORE_NAME_SIZE 4
#define BUFF_SCORE 255

typedef struct Score_ {
    /* Define a score.
     *
     * A score is a name (of `SCORE_NAME_SIZE` characters) and a (positive) integer score.
     * This is a NULL terminated chained list.
     * */
    unsigned int score;
    char name[SCORE_NAME_SIZE + 1];
    struct Score_* next;
} Score;

Score* score_new(unsigned int score, char name[SCORE_NAME_SIZE  + 1]);
void score_delete(Score* score);

Score* scores_new_from_file(FILE* f);

void scores_save_in_file(FILE *f, Score *list);

Score* score_insert(Score *list, unsigned int score, char name[SCORE_NAME_SIZE + 1]);
void score_print(Score* list);

#endif //BUBBLES_SCORE_H
