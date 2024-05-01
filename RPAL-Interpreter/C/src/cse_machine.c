#include "../include/cse_machine.h"

void initCSEMachine() {
    Env *e0 = (Env *) malloc(sizeof(Env));
    if (e0 == NULL) {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    e0->rename_rules = NULL;
    e0->prev = NULL;

    CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
    if (cell == NULL) {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    cell->type = T_Env;
    cell->content.env = e0;
    cell->prev = cell;
    cell->next = cell;

    currentEnv = cell;
    currentCell = currentEnv;
}

void generateCtrlStructs(Vertex *vertex) {
    if (strncmp(vertex->data, "gamma", 5) == 0) {
        CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
        if (cell == NULL) {
            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        cell->type = T_Gamma;
        cell->content.other = strdup(vertex->data);
        cell->prev = currentCell;
        cell->next = currentCell->next;

        currentCell->next->prev = cell;
        currentCell->next = cell;
    } else if (strncmp(vertex->data, "tau", 3) == 0) {
        size_t elem_cnt = 1;
        Vertex *temp = vertex->left_child->right_sibling;
        while (temp != NULL) {
            elem_cnt++;
            temp = temp->right_sibling;
        }

        Tuple *tuple = (Tuple *) malloc(sizeof(Tuple));
        char **elems = malloc(elem_cnt * sizeof(char*));
        if (elems == NULL) {
            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        } else {
            temp = vertex->left_child;
            for (int i = 0; i < elem_cnt; i++) {
                elems[i] = malloc(10 * sizeof(char));
                if (elems[i] == NULL) {
                    perror("Memory allocation failed.\n");
                    for (int j = 0; j < i; j++) {
                        free(elems[j]);
                    }
                    free(elems);
                    exit(EXIT_FAILURE);
                }
                snprintf(elems[i], 10, "String%d", i);
            }
        }

        CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
        if (cell == NULL) {
            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        cell->type = T_Tuple;
        cell->content.tuple = NULL;
        cell->prev = currentCell;
        cell->next = currentCell->next;

        currentCell->next->prev = cell;
        currentCell->next = cell;
    }
}
