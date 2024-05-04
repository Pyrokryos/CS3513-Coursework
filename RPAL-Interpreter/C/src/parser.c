#include "../include/parser.h"

static Node* curr;

Vertex* parse(TokenStream *stream) {
    curr = stream->head;

    return E();
}

const char *phrase_type_to_string(size_t type) {
    assert(type >= 6 && type <= 39);
    switch (type) {
        case E_LET: return "let";
        case E_LAMBDA: return "lambda";
        case E_WHERE: return "where";

        case T_TAU: return "tau";
        case T_AUG: return "aug";
        case T_COND: return "->";

        case B_OR: return "or";
        case B_AND: return "&";
        case B_NOT: return "not";
        case B_GR: return "gr";
        case B_GE: return "ge";
        case B_LS: return "ls";
        case B_LE: return "le";
        case B_EQ: return "eq";
        case B_NE: return "ne";

        case A_ADD: return "+";
        case A_SUB: return "-";
        case A_NEG: return "neg";
        case A_MUL: return "*";
        case A_DIV: return "/";
        case A_EXP: return "**";
        case A_AT: return "@";

        case R_GAMMA: return "gamma";
        case R_TRUE: return "true";
        case R_FALSE: return "false";
        case R_NIL: return "nil";
        case R_DUMMY: return "dummy";

        case D_WITHIN: return "within";
        case D_AND: return "and";
        case D_REC: return "rec";
        case D_EQ: return "=";
        case D_FCN: return "fcn_form";

        case V_BRACKET: return "()";
        case V_COMMA: return ",";
    }
}

// E -> ’let’ D ’in’ E | ’fn’ Vb+ ’.’ E | Ew
static Vertex *E() {
    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "let", 3) == 0
    ) {
        Vertex *let = create_vertex(E_LET, NULL);

        curr = curr->next;

        add_left_child(let, D());

        if (
            curr != NULL &&
            curr->token->type == KEYWORD &&
            strncmp(curr->token->value.s, "in", 2) == 0
        ) {
            curr = curr->next;
            
            add_right_sibling(get_left_child(let), E());

            return let;
        } else {
            perror("E: 'in' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "fn", 2) == 0
    ) {
        Vertex *lambda = create_vertex(E_LAMBDA, NULL);

        curr = curr->next;

        size_t iter = 0;
        Vertex *temp = lambda;
        while (
            curr != NULL &&
            curr->token->type == IDENTIFIER ||
            curr->token->type == PUNCTUATION && strncmp(curr->token->value.s, "(", 1) == 0
        ) {
            if (iter == 0) {
                add_left_child(temp, Vb());
                temp = get_left_child(temp);
            } else {
                add_right_sibling(temp, Vb());
                temp = get_right_sibling(temp);
            }

            iter++;
        }
        if (iter == 0) {
            perror("E: At least one 'Vb' expected.\n");
            exit(EXIT_FAILURE);
        }

        if (
            curr != NULL &&
            curr->token->type == OPERATOR &&
            strncmp(curr->token->value.s, ".", 1) == 0
        ) {
            curr = curr->next;

            add_right_sibling(temp, E());
            return lambda;
        } else {
            perror("E: '.' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        return Ew();
    }
}

// Ew ->  T ’where’ Dr | T
static Vertex *Ew() {
    Vertex *temp = T();

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "where", 5) == 0
    ) {
        Vertex* where = create_vertex(E_WHERE, NULL);

        curr = curr->next;

        add_left_child(where, temp);
        add_right_sibling(get_left_child(where), Dr());

        return where;
    } else {
        return temp;
    }
}

// T -> Ta ( ’,’ Ta )+ | Ta
static Vertex *T() {
    size_t iter = 0;
    Vertex *tau;
    Vertex *temp = Ta();
    while (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value.s, ",", 1) == 0
    ) {
        if (iter == 0) {
            tau = create_vertex(T_TAU, NULL);

            add_left_child(tau, temp);
            temp = get_left_child(tau);
        }

        curr = curr->next;

        add_right_sibling(temp, Ta());
        temp = get_right_sibling(temp);

        iter++;
    }
    if (iter > 0) {
        return tau;
    } else {
        return temp;
    }
}

// Ta -> Ta ’aug’ Tc | Tc
static Vertex *Ta() {
    size_t iter = 0;
    Vertex *aug;
    Vertex *temp = Tc();
    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "aug", 3) == 0
    ) {
        aug = create_vertex(T_AUG, NULL);

        curr = curr->next;

        add_left_child(aug, temp);
        add_right_sibling(get_left_child(aug), Tc());

        temp = aug;

        iter++;
    }

    return temp;
}

// Tc -> B ’->’ Tc ’|’ Tc | B
static Vertex *Tc() {
    Vertex *temp = B();

    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value.s, "->", 2) == 0
    ) {
        Vertex* arrow = create_vertex(T_COND, NULL);

        curr = curr->next;

        add_left_child(arrow, temp);
        add_right_sibling(get_left_child(arrow), Tc());

        if (
            curr != NULL &&
            curr->token->type == OPERATOR &&
            strncmp(curr->token->value.s, "|", 1) == 0
        ) {
            curr = curr->next;
            
            add_right_sibling(get_right_sibling(get_left_child(arrow)), Tc());
            
            return arrow;
        } else {
            perror("Tc: '|' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        return temp;
    }
}

// B -> B ’or’ Bt | Bt
static Vertex *B() {
    size_t iter = 0;
    Vertex *or;
    Vertex *temp = Bt();
    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "or", 2) == 0
    ) {
        or = create_vertex(B_OR, NULL);

        curr = curr->next;

        add_left_child(or, temp);
        add_right_sibling(get_left_child(or), Bt());
        
        temp = or;

        iter++;
    }

    return temp;
}

// Bt -> Bt ’&’ Bs | Bs
static Vertex *Bt() {
    int iter = 0;
    Vertex *and;
    Vertex *temp = Bs();
    while (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value.s, "&", 1) == 0
    ) {
        and = create_vertex(B_AND, NULL);

        curr = curr->next;

        add_left_child(and, temp);
        add_right_sibling(get_left_child(and), Bs());
        
        temp = and;

        iter++;
    }

    return temp;
}

// Bs -> ’not’ Bp | Bp
static Vertex *Bs() {
    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "not", 3) == 0
    ) {
        Vertex *not = create_vertex(B_NOT, NULL);

        curr = curr->next;

        add_left_child(not, Bp());

        return not;
    } else {
        return Bp();
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
static Vertex *Bp() {
    Vertex *temp = A();

    if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value.s, "gr", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value.s, ">", 1) == 0
        )
    ) {
        Vertex *gr = create_vertex(B_GR, NULL);
        curr = curr->next;

        add_left_child(gr, temp);
        add_right_sibling(get_left_child(gr), A());
        
        return gr;
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value.s, "ge", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value.s, ">=", 2) == 0
        )
    ) {
        Vertex *ge = create_vertex(B_GE, NULL);
        curr = curr->next;

        add_left_child(ge, temp);
        add_right_sibling(get_left_child(ge), A());
        
        return ge;
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value.s, "ls", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value.s, "<", 1) == 0
        )
    ) {
        Vertex *ls = create_vertex(B_LS, NULL);
        curr = curr->next;

        add_left_child(ls, temp);
        add_right_sibling(get_left_child(ls), A());
        
        return ls;
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value.s, "le", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value.s, "<=", 2) == 0
        )
    ) {
        Vertex *le = create_vertex(B_LE, NULL);
        curr = curr->next;

        add_left_child(le, temp);
        add_right_sibling(get_left_child(le), A());
        
        return le;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "eq", 2) == 0
    ) {
        Vertex *eq = create_vertex(B_EQ, NULL);
        curr = curr->next;

        add_left_child(eq, temp);
        add_right_sibling(get_left_child(eq), A());
        
        return eq;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "ne", 2) == 0
    ) {
        Vertex *ne = create_vertex(B_NE, NULL);
        curr = curr->next;

        add_left_child(ne, temp);
        add_right_sibling(get_left_child(ne), A());
        
        return ne;
    } else {
        return temp;
    }
}

/*
    A   -> A ’+’ At
        -> A ’-’ At
        -> ’+’ At
        -> ’-’ At
        -> At ;
*/
static Vertex *A() {
    Vertex *temp;
    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value.s, "+", 1) == 0
    ) {
        curr = curr->next;
        temp = At();
    } else if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value.s, "-", 1) == 0 &&
        strncmp(curr->token->value.s, "->", 2) != 0
    ) {
        Vertex *neg = create_vertex(A_NEG, NULL);

        curr = curr->next;

        add_left_child(neg, At());
        
        temp = neg;
    } else {
        temp = At();
    }

    size_t iter = 0;
    Vertex *add_sub;
    while (
        curr != NULL && (
            curr->token->type == OPERATOR && strncmp(curr->token->value.s, "+", 1) == 0 || (
                curr->token->type == OPERATOR && strncmp(curr->token->value.s, "-", 1) == 0 &&
                strncmp(curr->token->value.s, "->", 2) != 0
            )
        )
    ) {
        if (strncmp(curr->token->value.s, "+", 1) == 0) {
            add_sub = create_vertex(A_ADD, NULL);
        } else {
            add_sub = create_vertex(A_SUB, NULL);
        }

        curr = curr->next;

        add_left_child(add_sub, temp);
        add_right_sibling(get_left_child(add_sub), At());
        
        temp = add_sub;

        iter++;
    }

    return temp;
}

/*
    At  -> At ’*’ Af
        -> At ’/’  Af
        -> Af ;
*/
static Vertex *At() {
    size_t iter = 0;
    Vertex *mul_div;
    Vertex *temp = Af();
    while (
        curr != NULL && (
            curr->token->type == OPERATOR && strncmp(curr->token->value.s, "*", 1) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value.s, "/", 1) == 0
        )
    ) {
        if (strncmp(curr->token->value.s, "*", 1) == 0) {
            mul_div = create_vertex(A_MUL, NULL);
        } else {
            mul_div = create_vertex(A_DIV, NULL);
        }

        curr = curr->next;

        add_left_child(mul_div, temp);
        add_right_sibling(get_left_child(mul_div), Af());
        
        temp = mul_div;
    }

    return temp;
}

/*
    Af -> Ap ’**’ Af
       -> Ap ;
*/
static Vertex *Af() {
    Vertex *temp = Ap();

    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value.s, "**", 2) == 0
    ) {
        Vertex *exp = create_vertex(A_EXP, NULL);

        curr = curr->next;

        add_left_child(exp, temp);
        add_right_sibling(get_left_child(exp), Af());
        
        return exp;
    }
    return temp;
}

/*
    Ap -> Ap ’@’ ’<IDENTIFIER>’ R
       -> R ;
*/
static Vertex *Ap() {
    size_t iter = 0;
    Vertex *at;
    Vertex *temp = R();
    while (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value.s, "@", 1) == 0
    ) {
        at = create_vertex(A_AT, NULL);

        curr = curr->next;

        add_left_child(at, temp);        

        if (
            curr != NULL &&
            curr->token->type == IDENTIFIER
        ) {
            Vertex* identifier = create_vertex(NONE, curr->token);

            curr = curr->next;

            add_right_sibling(get_left_child(at), identifier);

            add_right_sibling(get_right_sibling(get_left_child(at)), R());
            
            temp = at;
        } else {
            perror("Ap: '<IDENTIFIER>' expected.\n");
            exit(EXIT_FAILURE);
        }
    }

    return temp;
}

/*
    R -> R Rn
      -> Rn ;
*/
static Vertex *R() {
    size_t iter = 0;
    Vertex *gamma;
    Vertex *temp = Rn();
    while (
        curr != NULL && (
            curr->token->type == IDENTIFIER ||
            curr->token->type == INTEGER ||
            curr->token->type == STRING || (
                curr->token->type == KEYWORD && (
                    strncmp(curr->token->value.s, "true", 4) == 0 ||
                    strncmp(curr->token->value.s, "false", 5) == 0 ||
                    strncmp(curr->token->value.s, "nil", 3) == 0 ||
                    strncmp(curr->token->value.s, "dummy", 5) == 0
                )
            ) ||
            curr->token->type == PUNCTUATION && strncmp(curr->token->value.s, "(", 1) == 0
        )
    ) {
        gamma = create_vertex(R_GAMMA, NULL);

        add_left_child(gamma, temp);
        add_right_sibling(get_left_child(gamma), Rn());
        
        temp = gamma;
    }

    return temp;
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
static Vertex *Rn() {
    if (curr != NULL && (curr->token->type == IDENTIFIER)) {
        Vertex *identifier = create_vertex(NONE, curr->token);

        curr = curr->next;

        return identifier;
    } else if (curr != NULL && (curr->token->type == INTEGER)) {
        Vertex *integer = create_vertex(NONE, curr->token);

        curr = curr->next;

        return integer;
    } else if (curr != NULL && (curr->token->type == STRING)) {
        Vertex *string = create_vertex(NONE, curr->token);

        curr = curr->next;

        return string;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "true", 4) == 0
    ) {
        Vertex *_true = create_vertex(R_TRUE, NULL);

        curr = curr->next;

        return _true;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "false", 5) == 0
    ) {
        Vertex *_false = create_vertex(R_FALSE, NULL);

        curr = curr->next;

        return _false;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "nil", 3) == 0
    ) {
        Vertex *nil = create_vertex(R_NIL, NULL);

        curr = curr->next;

        return nil;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "dummy", 5) == 0
    ) {
        Vertex *dummy = create_vertex(R_DUMMY, NULL);

        curr = curr->next;

        return dummy;
    } else if (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value.s, "(", 1) == 0
    ) {
        curr = curr->next;
        
        Vertex *temp = E();

        if (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value.s, ")", 1) == 0
        ) {
            curr = curr->next;

            return temp;
        } else {
            perror("Rn: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    } 
}

/*
    D -> Da ’within’ D => ’within’
      -> Da ;
*/
static Vertex *D() {
    Vertex *temp = Da();

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "within", 6) == 0
    ) {
        Vertex *within = create_vertex(D_WITHIN, NULL);

        curr = curr->next;

        add_left_child(within, temp);
        add_right_sibling(get_left_child(within), D());
        
        temp = within;
    }

    return temp;
}

/*
    Da -> Dr ( ’and’ Dr )+
       -> Dr ;
*/
static Vertex *Da() {
    size_t iter = 0;
    Vertex *and;
    Vertex *temp =  Dr();
    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "and", 3) == 0
    ) {
        if (iter == 0) {
            and = create_vertex(D_AND, NULL);

            add_left_child(and, temp);
            temp = get_left_child(and);
        }

        curr = curr->next;

        add_right_sibling(temp, Dr());
        temp = get_right_sibling(temp);

        iter++;
    }
    if (iter > 0) {
        return and;
    } else {
        return temp;
    }
}

/*
    Dr -> ’rec’ Db
        -> Db ;
*/
static Vertex *Dr() {
    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value.s, "rec", 3) == 0
    ) {
        Vertex* rec = create_vertex(D_REC, NULL);

        curr = curr->next;
        
        add_left_child(rec, Db());
        
        return rec;
    } else {
        return Db();
    }
}

/*
   Db -> Vl ’=’ E
      -> ’<IDENTIFIER>’ Vb+ ’=’ E
      -> ’(’ D ’)’ ;
*/
static Vertex *Db() {
    Vertex *temp;

    if (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value.s, "(", 1) == 0
    ) {
        curr = curr->next;
        
        temp = D();

        if (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value.s, ")", 1) == 0
        ) {
            curr = curr->next;

            return temp;
        } else {
            perror("Db: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        Vertex* identifier = create_vertex(NONE, curr->token);

        curr = curr->next;

        if (
            curr != NULL && (
                curr->token->type == PUNCTUATION && strncmp(curr->token->value.s, ",", 1) == 0 ||
                curr->token->type == OPERATOR && strncmp(curr->token->value.s, "=", 1) == 0
            )
        ) {
            size_t iter = 0;
            Vertex* comma;
            Vertex* temp = identifier;
            while (
                curr != NULL &&
                curr->token->type == PUNCTUATION &&
                strncmp(curr->token->value.s, ",", 1) == 0
            ) {
                comma = create_vertex(V_COMMA, NULL);

                if (iter == 0) {
                    add_left_child(comma, identifier);
                } else {
                    if (
                    curr != NULL &&
                    curr->token->type == IDENTIFIER
                    ) {
                        identifier = create_vertex(NONE, curr->token);

                        curr = curr->next;

                        add_right_sibling(temp, identifier);
                        temp = get_right_sibling(temp);

                        iter++;
                    } else {
                        perror("Db: '<IDENTIFIER>' expected.\n");
                        exit(EXIT_FAILURE);
                    }
                }

                curr = curr->next;
            }
            if (iter > 0) {
                temp = comma;
            }

            if (
                curr != NULL &&
                curr->token->type == OPERATOR &&
                strncmp(curr->token->value.s, "=", 1) == 0
            ) {
                Vertex *eq = create_vertex(D_EQ, NULL);

                add_left_child(eq, temp);
                
                curr = curr->next;
                
                add_right_sibling(get_left_child(eq), E());
                
                return eq;
            } else {
                perror("Db: '=' expected.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            Vertex *fcn_form = create_vertex(D_FCN, NULL);

            add_left_child(fcn_form, identifier);

            size_t iter = 0;
            Vertex *temp = identifier;
            while (
                curr != NULL && (
                    curr->token->type == IDENTIFIER ||
                    curr->token->type == PUNCTUATION && strncmp(curr->token->value.s, "(", 1) == 0
                )
            ) {
                add_right_sibling(temp, Vb());
                temp = get_right_sibling(temp);

                iter++;
            }
            if (iter == 0) {
                perror("Db: At least one 'Vb' expected.\n");
                exit(EXIT_FAILURE);
            }

            if (
                curr != NULL &&
                curr->token->type == OPERATOR &&
                strncmp(curr->token->value.s, "=", 1) == 0
            ) {
                curr = curr->next;

                add_right_sibling(temp, E());
                
                return fcn_form;
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
static Vertex *Vb() {
    if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        Vertex *identifier = create_vertex(NONE, curr->token);

        curr = curr->next;

        return identifier;
    } else if (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value.s, "(", 1) == 0
    ) {
        curr = curr->next;

        if (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value.s, ")", 1) == 0
        ) {
            Vertex *bracket = create_vertex(V_BRACKET, NULL);

            curr = curr->next;

            return bracket;
        }
        else {
            Vertex *temp = Vl();

            if (
                curr != NULL &&
                curr->token->type == PUNCTUATION &&
                strncmp(curr->token->value.s, ")", 1) == 0
            ) {
                curr = curr->next;

                return temp;
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
static Vertex *Vl() {
    if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        Vertex *comma = create_vertex(V_COMMA, NULL);
        Vertex *identifier = create_vertex(NONE, curr->token);

        curr = curr->next;

        add_left_child(comma, identifier);

        size_t iter = 0;
        Vertex *temp = identifier;
        while (
            curr != NULL &&
            curr->token->type == PUNCTUATION &&
            strncmp(curr->token->value.s, ",", 1) == 0
        ) {
            curr = curr->next;

            if (
                curr != NULL &&
                curr->token->type == IDENTIFIER
            ) {
                identifier = create_vertex(NONE, curr->token);

                add_right_sibling(temp, identifier);
                temp = get_right_sibling(temp);

                curr = curr->next;

                iter++;
            } else {
                perror("Vl: <Identifier> expected.\n");
                exit(EXIT_FAILURE);
            }
        }
        if (iter > 0) {
            return comma;
        } else {
            return identifier;
        }
    }
    else {
        perror("Vl: <Identifier> expected.\n");
        exit(EXIT_FAILURE);
    }
}
