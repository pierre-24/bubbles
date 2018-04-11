//
// Created by pbeaujea on 4/11/18.
//

#ifndef BUBBLES_SCORE_H
#define BUBBLES_SCORE_H

#include "utils.h"

#define SCORE_NAME_SIZE 4

typedef struct Score_ {
    unsigned int score;
    char name[SCORE_NAME_SIZE];
    struct Score_* next;
} Score;

Score* score_new(unsigned int score, char name[SCORE_NAME_SIZE]);
void score_delete(Score* score);

Score* scores_new_from_file(FILE* f);
bool scores_save_in_file(FILE* f, Score* list);

#endif //BUBBLES_SCORE_H
