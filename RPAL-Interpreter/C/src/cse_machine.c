#include "../include/cse_machine.h"

CtrlCell *current_env;

CtrlCell *init_cse_machine() {
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

    return cell;
}

CtrlCell *generate_ctrl_structs(Vertex *vertex) {
    if (vertex == NULL) {
        return NULL;
    } else {
        if (vertex->type == T_COND) {
            CtrlCell *B = generate_ctrl_structs(get_left_child(vertex));
            CtrlCell *T = generate_ctrl_structs(get_right_sibling(get_left_child(vertex)));
            CtrlCell *E = generate_ctrl_structs(get_right_sibling(get_right_sibling(get_left_child(vertex))));

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
            CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
            if (cell == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            Tau *tau = (Tau *) malloc(sizeof(Tau));
            if (tau == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            size_t expr_count = 1;
            Vertex *temp = get_right_sibling(get_left_child(vertex));
            while (temp != NULL) {
                expr_count++;
                temp = get_right_sibling(temp);
            }

            CtrlCell **expressions = (CtrlCell **) malloc(expr_count * sizeof(CtrlCell *));
            if (expressions == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            temp = get_left_child(vertex);
            for (int i = 0; i < expr_count; ++i) {
                expressions[i] = generate_ctrl_structs(temp);
            }

            tau->expressions = expressions;

            cell->type = CELL_TAU;
            cell->content.tau = tau;
            cell->prev = cell;
            cell->next = cell;

            return cell;
        } else if (vertex->type == E_LAMBDA) {
            CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
            if (cell == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            
            Lambda *lambda = (Lambda *) malloc(sizeof(Lambda));
            if (lambda == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            char **params;
            Vertex *temp = get_left_child(vertex);
            if (temp->type == V_COMMA) {
                size_t param_cnt = 1;

                temp = get_right_sibling(get_left_child(vertex));
                while (temp != NULL) {
                    param_cnt++;
                    temp = temp->right_sibling;
                }

                params = (char **) malloc(param_cnt * sizeof(char *));
                if (params == NULL) {
                    perror("Memory allocation failed.\n");
                    exit(EXIT_FAILURE);
                }

                temp = get_left_child(get_left_child(vertex));
                for (int i = 0; i < param_cnt; ++i) {
                    params[i] = temp->token->value;
                    temp = temp->right_sibling;
                }

                lambda->params = params;
            } else {
                params = (char **) malloc(sizeof(char *));
                if (params == NULL) {
                    perror("Memory allocation failed.\n");
                    exit(EXIT_FAILURE);
                }
                params[0] = temp->token->value;
            }
            
            lambda->params = params;
            lambda->body = generate_ctrl_structs(get_right_sibling(get_left_child(vertex)));

            cell->type = CELL_LAMBDA;
            cell->content.lambda = lambda;
            cell->prev = cell;
            cell->next = cell;

            return cell;
        } else {
            CtrlCell *cell = (CtrlCell *) malloc(sizeof(CtrlCell));
            if (cell == NULL) {
                perror("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            switch (vertex->type) {
                case T_AUG:
                    cell->type = CELL_AUG;
                    break;
                case B_OR:
                    cell->type = CELL_OR;
                    break;
                case B_AND:
                    cell->type = CELL_AND;
                    break;
                case B_NOT:
                    cell->type = CELL_NOT;
                    break;
                case B_GR:
                    cell->type = CELL_GR;
                    break;
                case B_GE:
                    cell->type = CELL_GE;
                    break;
                case B_LS:
                    cell->type = CELL_LS;
                    break;
                case B_LE:
                    cell->type = CELL_LE;
                    break;
                case B_EQ:
                    cell->type = CELL_EQ;
                    break;
                case B_NE:
                    cell->type = CELL_NE;
                    break;
                case A_ADD:
                    cell->type = CELL_ADD;
                    break;
                case A_SUB:
                    cell->type = CELL_SUB;
                    break;
                case A_NEG:
                    cell->type = CELL_NEG;
                    break;
                case A_MUL:
                    cell->type = CELL_MUL;
                    break;
                case A_DIV:
                    cell->type = CELL_DIV;
                    break;
                case A_EXP:
                    cell->type = CELL_EXP;
                    break;
                case R_GAMMA:
                    cell->type = CELL_GAMMA;
                    break;
                case R_TRUE:
                    cell->type = CELL_TRUE;
                    break;
                case R_FALSE:
                    cell->type = CELL_FALSE;
                    break;
                case R_NIL:
                    cell->type = CELL_NIL;
                    break;
                case R_DUMMY:
                    cell->type = CELL_DUMMY;
                    break;
                case D_EQ:
                    cell->type = CELL_EQUIV;
                    break;
                case Y_STAR:
                    cell->type = CELL_YSTAR;
                    break;
                case NONE:
                    switch (vertex->token->type) {
                        case IDENTIFIER:
                            cell->type = CELL_ID;
                            cell->content.other = vertex->token->value;
                            break;
                        case INTEGER:
                            cell->type = CELL_INT;
                            cell->content.other = vertex->token->value;
                            break;
                        case STRING:
                            cell->type = CELL_STR;
                            cell->content.other = vertex->token->value;
                            break;
                    }
                    break;
            }
            
            CtrlCell *left_child = generate_ctrl_structs(get_left_child(vertex));
            CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex));

            if (left_child != NULL) {
                left_child->prev->next = cell;
                cell->prev = left_child->prev;
                left_child->prev = cell;
                cell->next = left_child;
            } else {
                cell->prev = cell;
                cell->next = cell;
            }

            if (right_sibling != NULL) {
                right_sibling->prev->next = cell;
                cell->prev = right_sibling->prev;
                right_sibling->prev = cell;
                cell->next = right_sibling;
            } else {
                cell->prev = cell;
                cell->next = cell;
            }

            return cell;
        }
    }
}
