#include "score.h"

Score* score_new(unsigned int score, char name[SCORE_NAME_SIZE]) {
    Score* score_obj = malloc(sizeof(Score));

    if (score_obj == NULL) {
        write_log("! cannot allocate score");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Score %p\n", score_obj);
#endif

    score_obj->score = score;
    memcpy(score_obj->name, name, SCORE_NAME_SIZE);
    score_obj->next = NULL;

    return score_obj;
}

void score_delete(Score* score) {
    Score* next = score, *p = NULL;

    while (next != NULL) {
        p = next->next;
        free(next);
        next = p;


#ifdef VERBOSE_MEM
        printf("-Score %p\n", next);
#endif
    }

}

Score* scores_new_from_file(FILE* f) {

}

bool scores_save_in_file(FILE* f, Score* list) {

}

Score* score_add(Score* list, unsigned int score, char name[SCORE_NAME_SIZE]) {
    if (list == NULL) {
        Score* score_obj = score_new(score, name);
        return score_obj;
    }

    else {
        return list;
    }
}