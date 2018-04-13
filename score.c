#include "score.h"

Score* score_new(unsigned int score, char name[SCORE_NAME_SIZE + 1]) {
    Score* score_obj = malloc(sizeof(Score));

    if (score_obj == NULL) {
        write_log("! cannot allocate score");
        return NULL;
    }

#ifdef VERBOSE_MEM
    printf("+Score %p\n", score_obj);
#endif

    score_obj->score = score;
    memcpy(score_obj->name, name, SCORE_NAME_SIZE + 1);
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
    Score* list = NULL;
    char* text = file_get_content(f), *next = text, *n;
    char name[5];
    name[4] = '\0';
    unsigned  int score;

    while (*next != '\0') {
        memcpy(name, next, 4 * sizeof(char));
        next = strnextnspace(next + 5);

        score = (unsigned  int) strtoul(next, &n, 0);

        if (n != next)
            list = score_insert(list, score, name);

        while (*next != '\0' && *next != '\n')
            next++;

        if (*next == '\n') // go after the line return
            next++;
    }

    free(text);

    return list;
}

bool scores_save_in_file(FILE* f, Score* list) {
    Score* p = list;
    while (p != NULL) {
        fprintf(f, "%s %d\n", p->name, p->score);
        p = p->next;
    }
}

Score* score_insert(Score *list, unsigned int score, char name[SCORE_NAME_SIZE + 1]) {
    write_log("# inserting score for <%s>", name);

    Score* score_obj = score_new(score, name);
    if (list == NULL) {
        return score_obj;
    }

    else {
        Score* t = list, *prev = NULL;
        bool inserted = false;
        while (t != NULL && !inserted) {
            if (score_obj->score > t->score) {
                if (prev != NULL)
                    prev->next = score_obj;
                else
                    list = score_obj;

                score_obj->next = t;
                inserted = true;
            }

            prev = t;
            t = t->next;
        }

        if (!inserted)
            prev->next = score_obj;

        return list;
    }
}

void score_print(Score* list) {
    Score* t = list;
    while (t != NULL) {
        printf("%s : %d\n", t->name, t->score);
        t = t->next;
    }
}