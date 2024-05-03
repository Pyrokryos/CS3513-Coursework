#include "../include/cse_machine.h"

void initCSEMachine() {
    Env *env = (Env *) malloc(sizeof(Env));
    if (env == NULL) {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    env->rename_rules = NULL;
    env->prev = NULL;

    CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
    if (cell == NULL) {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    cell->type = CELL_ENV;
    cell->content.env = env;
    cell->prev = cell;
    cell->next = cell;

    current_env = cell;
}

CtrlCell *generateCtrlStructs(Vertex *vertex) {
    if (vertex == NULL) {
        return NULL;
    } else {
        if (vertex->type == T_COND) {
        CtrlCell *B = generateCtrlStructs(get_left_child(vertex));
        CtrlCell *T = generateCtrlStructs(get_right_sibling(get_left_child(vertex)));
        CtrlCell *E = generateCtrlStructs(get_right_sibling(get_right_sibling(get_left_child(vertex))));

        CtrlCell *_then = (CtrlCell *) malloc(sizeof(CtrlCell));
        if (_then == NULL) {
            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        _then->type = CELL_DELTA;
        _then->content.delta = (Delta *) malloc(sizeof(Delta));

        CtrlCell *_else = (CtrlCell *) malloc(sizeof(CtrlCell));
        if (_else == NULL) {
            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        _else->type = CELL_DELTA;
        _else->content.delta = (Delta *) malloc(sizeof(Delta));

        CtrlCell *beta = (CtrlCell *) malloc(sizeof(CtrlCell));
        if (beta == NULL) {
            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        beta->type = CELL_BETA;
        beta->content.other = NULL;

        _then->content.delta->cell = T;
        _else->content.delta->cell = E;

        _then->next = _else;
        _else->prev = _then;

        _else->next = beta;
        beta->prev = _else;

        beta->next = B;
        _then->prev = B->prev;
        B->prev = beta;

        return _then;
        } else if (vertex->type == T_TAU) {
            size_t elem_cnt = 1;

            Vertex *temp = vertex->left_child->right_sibling;
            while (temp != NULL) {
                elem_cnt++;
                temp = temp->right_sibling;
            }

            Tau *tau = (Tau *) malloc(sizeof(Tau));
            if (tau == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            char **elems = malloc(elem_cnt * sizeof(char*));
            if (elems == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            temp = vertex->left_child;
            for (int i = 0; i < elem_cnt; ++i) {
                elems[i] = vertex->token->value;
                temp = temp->right_sibling;
            }

            tau->elem_cnt = elem_cnt;
            tau->elems = elems;

            CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
            if (cell == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            cell->type = CELL_TAU;
            cell->content.tau = tau;
            cell->prev = cell;
            cell->next = cell;

            return cell;
        } else if (vertex->type == E_LAMBDA) {
            Lambda *lambda = (Lambda *) malloc(sizeof(Lambda));
            if (lambda == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
            if (cell == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            cell->type = CELL_LAMBDA;
            cell->content.lambda = lambda;

            Vertex *temp = vertex->left_child;
            if (temp->type == T_TAU) {
                lambda->params = generateCtrlStructs(temp)->content.tau;
            } else {
                lambda->params = NULL;
            }

            lambda->cell = generateCtrlStructs(get_right_sibling(temp));

            cell->prev = cell;
            cell->next = cell;

            return cell;
        } else {
            CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
            if (cell == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            cell->type = CELL_OTHER;
            cell->content.other = vertex->token->value;
            cell->prev = cell;
            cell->next = cell;

            return cell;
        }
    }
}
