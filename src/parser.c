#include "../include/parser.h"

Vertex* parse(TokenStream* stream) {
    curr = stream->head;
    queue = create_empty_queue();

    E();

    return queue->head->vertex;
}

void AST(Vertex* vertex) {
    depth_first_left_to_right_traversal(vertex, 0);
}

// E -> ’let’ D ’in’ E | ’fn’ Vb+ ’.’ E | Ew
static void E() {
    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "let", 3) == 0
    ) {
        Vertex* let = create_vertex("let");

        curr = curr->next;
        D();

        add_left_child(let, queue->head->vertex);
        dequeue(queue);

        if (
            curr != NULL &&
            curr->token->type == KEYWORD &&
            strncmp(curr->token->value, "in", 2) == 0
        ) {
            curr = curr->next;
            E();

            add_right_sibling(get_left_child(let), queue->head->vertex);
            dequeue(queue);

            enqueue(queue, let);
        } else {
            perror("E: 'in' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "fn", 2) == 0
    ) {
        Vertex* lambda = create_vertex("lambda");

        curr = curr->next;

        size_t iter = 0;
        Vertex* temp = lambda;
        while (
            curr != NULL &&
            curr->token->type == IDENTIFIER ||
            curr->token->type == PUNCTUATION && strncmp(curr->token->value, "(", 1) == 0
        ) {
            Vb();

            if (iter == 0) {
                add_left_child(temp, queue->head->vertex);
            } else {
                add_right_sibling(temp, queue->head->vertex);
            }
            temp = dequeue(queue);

            iter++;
        }
        if (iter == 0) {
            exit(EXIT_FAILURE);
        }

        if (
            curr != NULL &&
            curr->token->type == OPERATOR &&
            strncmp(curr->token->value, ".", 1) == 0
        ) {
            curr = curr->next;
            E();

            add_right_sibling(temp, queue->head->vertex);
            dequeue(queue);

            enqueue(queue, lambda);
        } else {
            perror("E: '.' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        Ew();
    }
}

// Ew ->  T ’where’ Dr | T
static void Ew() {
    T();

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "where", 5) == 0
    ) {
        Vertex* where = create_vertex("where");

        add_left_child(where, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        Dr();

        add_right_sibling(get_left_child(where), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, where);
    }
}

// T -> Ta ( ’,’ Ta )+ | Ta
static void T() {
    Ta();

    size_t iter = 0;
    Vertex* tau;
    Vertex* temp;
    while (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value, ",", 1) == 0
    ) {
        if (iter == 0) {
            tau = create_vertex("tau");

            add_left_child(tau, queue->head->vertex);
            temp = dequeue(queue);
        }

        curr = curr->next;
        Ta();

        add_right_sibling(temp, queue->head->vertex);
        temp = dequeue(queue);

        iter++;
    }
    if (iter > 0) {
        enqueue(queue, tau);
    }
}

// Ta -> Ta ’aug’ Tc | Tc
static void Ta() {
    Tc();

    size_t iter = 0;
    Vertex* aug;
    Vertex* temp;
    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "aug", 3) == 0
    ) {
        aug = create_vertex("aug");

        add_left_child(aug, queue->head->vertex);
        temp = dequeue(queue);

        curr = curr->next;
        Tc();

        add_right_sibling(temp, queue->head->vertex);
        temp = dequeue(queue);

        enqueue(queue, aug);

        iter++;
    }
}

// Tc -> B ’->’ Tc ’|’ Tc | B
static void Tc() {
    B();

    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "->", 2) == 0
    ) {
        Vertex* arrow = create_vertex("->");

        add_left_child(arrow, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        Tc();

        add_right_sibling(get_left_child(arrow), queue->head->vertex);
        Vertex* temp = dequeue(queue);

        if (
            curr != NULL &&
            curr->token->type == OPERATOR &&
            strncmp(curr->token->value, "|", 1) == 0
        ) {
            curr = curr->next;
            Tc();

            add_right_sibling(temp, queue->head->vertex);
            dequeue(queue);

            enqueue(queue, arrow);
        } else {
            perror("Tc: '|' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

// B -> B ’or’ Bt | Bt
static void B() {
    Bt();

    size_t iter = 0;
    Vertex* or;
    Vertex* temp;
    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "or", 2) == 0
    ) {
        or = create_vertex("or");

        add_left_child(or, queue->head->vertex);
        temp = dequeue(queue);

        curr = curr->next;
        Bt();

        add_right_sibling(temp, queue->head->vertex);
        temp = dequeue(queue);

        enqueue(queue, or);

        iter++;
    }
}

// Bt -> Bt ’&’ Bs | Bs
static void Bt() {
    Bs();

    int iter = 0;
    Vertex* and;
    Vertex* temp;
    while (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "&", 1) == 0
    ) {
        and = create_vertex("&");

        add_left_child(and, queue->head->vertex);
        temp = dequeue(queue);

        curr = curr->next;
        Bs();

        add_right_sibling(temp, queue->head->vertex);
        temp = dequeue(queue);

        enqueue(queue, and);

        iter++;
    }
}

// Bs -> ’not’ Bp | Bp
static void Bs() {
    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "not", 3) == 0
    ) {
        Vertex* not = create_vertex("not");

        curr = curr->next;
        Bp();

        add_left_child(not, queue->head->vertex);
        dequeue(queue);

        enqueue(queue, not);
    } else {
        Bp();
    }
}

/*
    Bp  -> A (’gr’ | ’>’ ) A
        -> A (’ge’ | ’>=’) A
        -> A (’ls’ | ’<’ ) A
        -> A (’le’ | ’<=’) A
        -> A ’eq’ A
        -> A ’ne’ A
        -> A ;
*/
static void Bp() {
    A();

    if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "gr", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, ">", 1) == 0
        )
    ) {
        Vertex* gr = create_vertex("gr");

        add_left_child(gr, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        A();

        add_right_sibling(get_left_child(gr), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, gr);
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "ge", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, ">=", 2) == 0
        )
    ) {
        Vertex* ge = create_vertex("ge");

        add_left_child(ge, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        A();

        add_right_sibling(get_left_child(ge), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, ge);
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "ls", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "<", 1) == 0
        )
    ) {
        Vertex* ls = create_vertex("ls");

        add_left_child(ls, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        A();

        add_right_sibling(get_left_child(ls), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, ls);
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "le", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "<=", 2) == 0
        )
    ) {
        Vertex* le = create_vertex("le");

        add_left_child(le, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        A();

        add_right_sibling(get_left_child(le), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, le);
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "eq", 2) == 0
    ) {
        Vertex* eq = create_vertex("eq");

        add_left_child(eq, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        A();

        add_right_sibling(get_left_child(eq), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, eq);
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "ne", 2) == 0
    ) {
        Vertex* ne = create_vertex("ne");

        add_left_child(ne, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        A();

        add_right_sibling(get_left_child(ne), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, ne);
    }
}

/*
    A   -> A ’+’ At
        -> A ’-’ At
        -> ’+’ At
        -> ’-’ At
        -> At ;
*/
static void A() {
    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "+", 1) == 0
    ) {
        curr = curr->next;
        At();
    } else if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "-", 1) == 0 &&
        strncmp(curr->token->value, "->", 2) != 0
    ) {
        Vertex* neg = create_vertex("neg");

        curr = curr->next;
        At();

        add_left_child(neg, queue->head->vertex);
        dequeue(queue);

        enqueue(queue, neg);
    } else {
        At();
    }

    size_t iter = 0;
    Vertex* add_sub;
    Vertex* temp;
    while (
        curr != NULL && (
            curr->token->type == OPERATOR && strncmp(curr->token->value, "+", 1) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "-", 1) == 0 && strncmp(curr->token->value, "->", 2) != 0
        )
    ) {
        add_sub = create_vertex(curr->token->value);

        add_left_child(add_sub, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        At();

        add_right_sibling(get_left_child(add_sub), queue->head->vertex);
        temp = dequeue(queue);

        enqueue(queue, add_sub);

        iter++;
    }
}

/*
    At  -> At ’*’ Af
        -> At ’/’  Af
        -> Af ;
*/
static void At() {
    Af();

    size_t iter = 0;
    Vertex* mul_div;
    Vertex* temp;
    while (
        curr != NULL && (
            curr->token->type == OPERATOR && strncmp(curr->token->value, "*", 1) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "/", 1) == 0
        )
    ) {
        mul_div = create_vertex(curr->token->value);

        add_left_child(mul_div, queue->head->vertex);
        temp = dequeue(queue);

        curr = curr->next;
        Af();

        add_right_sibling(temp, queue->head->vertex);
        temp = dequeue(queue);

        enqueue(queue, mul_div);
    }
}

/*
    Af -> Ap ’**’ Af
       -> Ap ;
*/
static void Af() {
    Ap();

    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "**", 2) == 0
    ) {
        Vertex* power = create_vertex("**");

        add_left_child(power, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        Af();

        add_right_sibling(get_left_child(power), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, power);
    }
}

/*
    Ap -> Ap ’@’ ’<IDENTIFIER>’ R
       -> R ;
*/
static void Ap() {
    R();

    size_t iter = 0;
    Vertex* at;
    Vertex* temp;
    while (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "@", 1) == 0
    ) {
        at = create_vertex("@");

        add_left_child(at, queue->head->vertex);
        temp = dequeue(queue);

        curr = curr->next;

        if (
            curr != NULL &&
            curr->token->type == IDENTIFIER
        ) {
            char* data = (char*) malloc(sizeof(char));
            sprintf(data, "<ID:%s>", curr->token->value);

            Vertex* identifier = create_vertex(data);

            add_right_sibling(temp, identifier);
            temp = identifier;

            curr = curr->next;
            R();

            add_right_sibling(temp, queue->head->vertex);
            temp = dequeue(queue);

            enqueue(queue, at);
        } else {
            perror("Ap: '<IDENTIFIER>' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

/*
    R -> R Rn
      -> Rn ;
*/
static void R() {
    Rn();

    size_t iter = 0;
    Vertex* gamma;
    Vertex* temp;
    while (
        curr != NULL && (
            curr->token->type == IDENTIFIER ||
            curr->token->type == INTEGER ||
            curr->token->type == STRING ||
            curr->token->type == KEYWORD && (
                strncmp(curr->token->value, "true", 4) == 0 ||
                strncmp(curr->token->value, "false", 5) == 0 ||
                strncmp(curr->token->value, "nil", 3) == 0 ||
                strncmp(curr->token->value, "dummy", 5) == 0
            ) ||
            curr->token->type == PUNCTUATION && strncmp(curr->token->value, "(", 1) == 0
        )
    ) {
        gamma = create_vertex("gamma");

        add_left_child(gamma, queue->head->vertex);
        temp = dequeue(queue);

        Rn();

        add_right_sibling(temp, queue->head->vertex);
        temp = dequeue(queue);

        enqueue(queue, gamma);
    }
}

/*
    Rn -> ’<ID>’
        -> ’<INT>’
        -> ’<STR>’
        -> ’true’
        -> ’false’
        -> ’nil’
        -> ’(’ E ’)’
        -> ’dummy’
*/
static void Rn() {
    if (curr != NULL && (curr->token->type == IDENTIFIER)) {
        char* data = (char*) malloc(sizeof(char));
        sprintf(data, "<ID:%s>", curr->token->value);

        Vertex* identifier = create_vertex(data);

        enqueue(queue, identifier);

        curr = curr->next;
    } else if (curr != NULL && (curr->token->type == INTEGER)) {
        char* data = (char*) malloc(sizeof(char));
        sprintf(data, "<INT:%s>", curr->token->value);

        Vertex* integer = create_vertex(data);

        enqueue(queue, integer);

        curr = curr->next;
    } else if (curr != NULL && (curr->token->type == STRING)) {
        char* data = (char*) malloc(sizeof(char));
        sprintf(data, "<STR:%s>", curr->token->value);

        Vertex* string= create_vertex(data);

        enqueue(queue, string);

        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "true", 4) == 0
    ) {
        Vertex* _true = create_vertex("true");

        enqueue(queue, _true);

        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "false", 5) == 0
    ) {
        Vertex* _false = create_vertex("false");

        enqueue(queue, _false);

        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "nil", 3) == 0
    ) {
        Vertex* nil = create_vertex("nil");

        enqueue(queue, nil);

        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value, "(", 1) == 0
    ) {
        curr = curr->next;
        E();

        if (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value, ")", 1) == 0
        ) {
            curr = curr->next;
        } else {
            perror("Rn: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "dummy", 5) == 0
    ) {
        Vertex* dummy = create_vertex("dummy");

        enqueue(queue, dummy);

        curr = curr->next;
    }
}

/*
    D -> Da ’within’ D => ’within’
      -> Da ;
*/
static void D() {
    Da();

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "within", 6) == 0
    ) {
        Vertex* within = create_vertex("within");

        add_left_child(within, queue->head->vertex);
        dequeue(queue);

        curr = curr->next;
        Da();

        add_right_sibling(get_left_child(within), queue->head->vertex);
        dequeue(queue);

        enqueue(queue, within);
    }
}

/*
    Da -> Dr ( ’and’ Dr )+
       -> Dr ;
*/
static void Da() {
    Dr();

    size_t iter = 0;
    Vertex* and;
    Vertex* temp;
    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "and", 3) == 0
    ) {
        if (iter == 0) {
            and = create_vertex("and");

            add_left_child(and, queue->head->vertex);
            temp = dequeue(queue);
        }

        curr = curr->next;
        Dr();

        add_right_sibling(temp, queue->head->vertex);
        temp = dequeue(queue);
    }
    if (iter > 0) {
        enqueue(queue, and);
    }
}

/*
    Dr -> ’rec’ Db
        -> Db ;
*/
static void Dr() {
    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "rec", 3) == 0
    ) {
        Vertex* rec = create_vertex("rec");

        curr = curr->next;
        Db();

        add_left_child(rec, queue->head->vertex);
        dequeue(queue);

        enqueue(queue, rec);
    } else {
        Db();
    }
}

/*
   Db -> Vl ’=’ E
      -> ’<IDENTIFIER>’ Vb+ ’=’ E
      -> ’(’ D ’)’ ;
*/
void Db() {
    if (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value, "(", 1) == 0
    ) {
        curr = curr->next;
        D();

        if (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value, ")", 1) == 0
        ) {
            curr = curr->next;
        } else {
            perror("Db: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        char* data = (char*) malloc(sizeof(char));
        sprintf(data, "<ID:%s>", curr->token->value);

        Vertex* identifier = create_vertex(data);

        curr = curr->next;

        if (
            curr != NULL && (
                curr->token->type == PUNCTUATION && strncmp(curr->token->value, ",", 1) == 0 ||
                curr->token->type == OPERATOR && strncmp(curr->token->value, "=", 1) == 0
            )
        ) {
            size_t iter = 0;
            Vertex* comma;
            Vertex* temp;
            while (
                curr != NULL &&
                curr->token->type == PUNCTUATION &&
                strncmp(curr->token->value, ",", 1) == 0
            ) {
                if (iter == 0) {
                    comma = create_vertex(",");

                    add_left_child(comma, identifier);
                    temp = identifier;
                }

                curr = curr->next;

                if (
                    curr != NULL &&
                    curr->token->type == IDENTIFIER
                ) {
                    char* data = (char*) malloc(sizeof(char));
                    sprintf(data, "<ID:%s>", curr->token->value);

                    identifier = create_vertex(data);

                    add_right_sibling(temp, identifier);
                    temp = identifier;

                    curr = curr->next;

                    iter++;
                } else {
                    perror("Db: '<IDENTIFIER>' expected.\n");
                    exit(EXIT_FAILURE);
                }
            }
            if (iter > 0) {
                enqueue(queue, comma);
            }

            if (
                curr != NULL &&
                curr->token->type == OPERATOR &&
                strncmp(curr->token->value, "=", 1) == 0
            ) {
                Vertex* equals = create_vertex("=");

                add_left_child(equals, queue->head->vertex);
                dequeue(queue);

                curr = curr->next;
                E();

                add_right_sibling(get_left_child(equals), queue->head->vertex);
                dequeue(queue);

                enqueue(queue, equals);
            } else {
                perror("Db: '=' expected.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            Vertex* fcn_form = create_vertex("fcn_form");

            add_left_child(fcn_form, identifier);

            size_t iter = 0;
            Vertex* temp = identifier;
            while (
                curr != NULL && (
                    curr->token->type == IDENTIFIER ||
                    curr->token->type == PUNCTUATION && strncmp(curr->token->value, "(", 1) == 0
                )
            ) {
                Vb();

                add_right_sibling(temp, queue->head->vertex);
                temp = dequeue(queue);

                iter++;
            }
            if (iter == 0) {
                perror("Db: At least one 'Vb' expected.\n");
                exit(EXIT_FAILURE);
            }

            if (
                curr != NULL &&
                curr->token->type == OPERATOR &&
                strncmp(curr->token->value, "=", 1) == 0
            ) {
                curr = curr->next;
                E();

                add_right_sibling(temp, queue->head->vertex);
                dequeue(queue);

                enqueue(queue, fcn_form);
            }  else {
                perror("Db: '=' expected.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/*
    Vb -> ’<ID>’
        -> ’(’ Vl ’)’
        -> ’(’ ’)’
*/
static void Vb() {
    if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        char* data = (char*) malloc(sizeof(char));
        sprintf(data, "<ID:%s>", curr->token->value);

        Vertex* identifier = create_vertex(data);

        enqueue(queue, identifier);

        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value, "(", 1) == 0
    ) {
        curr = curr->next;

        if (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value, ")", 1) == 0
        ) {
            Vertex* bracket = create_vertex("()");

            enqueue(queue, bracket);

            curr = curr->next;
        }
        else {
            Vl();

            if (
                curr != NULL &&
                curr->token->type == PUNCTUATION &&
                strncmp(curr->token->value, ")", 1) == 0
            ) {
                curr = curr->next;
            } else {
                perror("Vb: ')' expected.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/*
    Vl -> ’<IDENTIFIER>’ list ’,’
*/
static void Vl() {
    if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        Vertex* comma = create_vertex(",");

        char* data = (char*) malloc(sizeof(char));
        sprintf(data, "<ID:%s>", curr->token->value);

        Vertex* identifier = create_vertex(data);

        add_left_child(comma, identifier);

        curr = curr->next;

        size_t iter = 0;
        Vertex* temp = identifier;
        while (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value, ",", 1) == 0
        ) {
            curr = curr->next;

            if (
                curr != NULL &&
                curr->token->type == IDENTIFIER
            ) {
                char* data = (char*) malloc(sizeof(char));
                sprintf(data, "<ID:%s>", curr->token->value);

                Vertex* identifier = create_vertex(data);

                add_right_sibling(temp, identifier);
                temp = identifier;

                curr = curr->next;

                iter++;
            } else {
                perror("Vl: <Identifier> expected.\n");
                exit(EXIT_FAILURE);
            }
        }
        if (iter > 0) {
            enqueue(queue, comma);
        } else {
            free(comma);
            enqueue(queue, identifier);
        }
    }
    else {
        perror("Vl: <Identifier> expected.\n");
        exit(EXIT_FAILURE);
    }
}
