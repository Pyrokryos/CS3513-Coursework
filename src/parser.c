#include "../include/parser.h"

static Node* curr = NULL;

void parse(TokenStream* stream) {
    curr = stream->head;

    E();
    printf("Parsing complete!\n");
}

// E -> ’let’ D ’in’ E | ’fn’ Vb+ ’.’ E | Ew
static void E() {
    printf("E()\n");

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "let", 3) == 0
    ) {
        curr = curr->next;
        D();

        if (
            curr != NULL &&
            curr->token->type == KEYWORD &&
            strncmp(curr->token->value, "in", 2) == 0
        ) {
            curr = curr->next;
            E();
        } else {
            printf("E: 'in' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "fn", 2) == 0
    ) {
        curr = curr->next;

        int iter = 0;
        while (
            curr != NULL &&
            curr->token->type == IDENTIFIER ||
            curr->token->type == PUNCTUATION && strncmp(curr->token->value, "(", 1) == 0
        ) {
            Vb();

            iter++;
        }
        if (iter == 0) {
            printf("E: At least one 'Vb' expected.\n");
            exit(EXIT_FAILURE);
        }

        if (
            curr != NULL &&
            curr->token->type == OPERATOR &&
            strncmp(curr->token->value, ".", 1) == 0
        ) {
            curr = curr->next;
            E();
        } else {
            printf("E: '.' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        Ew();
    }
}

// Ew ->  T ’where’ Dr | T
static void Ew() {
    printf("Ew()\n");

    T();

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "where", 5) == 0
    ) {
        curr = curr->next;
        Dr();
    }
}

// T -> Ta ( ’,’ Ta )+ | Ta
static void T() {
    printf("T()\n");

    Ta();

    while (
        curr != NULL &&
        curr->token->type == PUNCTUATION &&
        strncmp(curr->token->value, ",", 1) == 0
    ) {
        curr = curr->next;
        Ta();
    }
}

// Ta -> Ta ’aug’ Tc | Tc
static void Ta() {
    printf("Ta()\n");

    Tc();

    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "aug", 3) == 0
    ) {
        curr = curr->next;
        Tc();
    }
}

// Tc -> B ’->’ Tc ’|’ Tc | B
static void Tc() {
    printf("Tc()\n");

    B();

    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "->", 2) == 0
    ) {
        curr = curr->next;
        Tc();

        if (
            curr != NULL &&
            curr->token->type == OPERATOR &&
            strncmp(curr->token->value, "|", 1) == 0
        ) {
            curr = curr->next;
            Tc();
        } else {
            printf("Tc: '|' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

// B -> B ’or’ Bt | Bt
static void B() {
    printf("B()\n");

    Bt();

    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "or", 2) == 0
    ) {
        curr = curr->next;
        Bt();
    }
}

// Bt -> Bt ’&’ Bs | Bs
static void Bt() {
    printf("Bt()\n");

    Bs();

    while (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "&", 1) == 0
    ) {
        curr = curr->next;
        Bs();
    }
}

// Bs -> ’not’ Bp | Bp
static void Bs() {
    printf("Bs()\n");

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "not", 3) == 0
    ) {
        curr = curr->next;
        Bp();
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
    printf("Bp()\n");

    A();

    if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "gr", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, ">", 1) == 0
        )
    ) {
        curr = curr->next;
        A();
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "ge", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, ">=", 2) == 0
        )
    ) {
        curr = curr->next;
        A();
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "ls", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "<", 1) == 0
        )
    ) {
        curr = curr->next;
        A();
    } else if (
        curr != NULL && (
            curr->token->type == KEYWORD && strncmp(curr->token->value, "le", 2) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "<=", 2) == 0
        )
    ) {
        curr = curr->next;
        A();
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "eq", 2) == 0
    ) {
        curr = curr->next;
        A();
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "ne", 2) == 0
    ) {
        curr = curr->next;
        A();
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
    printf("A()\n");

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
        curr = curr->next;
        At();
    } else {
        At();
    }

    while (
        curr != NULL && (
            curr->token->type == OPERATOR && strncmp(curr->token->value, "+", 1) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "-", 1) == 0 && strncmp(curr->token->value, "->", 2) != 0
        )
    ) {
        curr = curr->next;
        At();
    }
}

/*
    At  -> At ’*’ Af
        -> At ’/’  Af
        -> Af ;
*/
static void At() {
    printf("At()\n");

    Af();

    while (
        curr != NULL && (
            curr->token->type == OPERATOR && strncmp(curr->token->value, "*", 1) == 0 ||
            curr->token->type == OPERATOR && strncmp(curr->token->value, "/", 1) == 0
        )
    ) {
        curr = curr->next;
        Af();
    }
}

/*
    Af -> Ap ’**’ Af
       -> Ap ;
*/
static void Af() {
    printf("Af()\n");

    Ap();

    if (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "**", 2) == 0
    ) {
        curr = curr->next;
        Af();
    }
}

/*
    Ap -> Ap ’@’ ’<IDENTIFIER>’ R
       -> R ;
*/
static void Ap() {
    printf("Ap()\n");

    R();

    while (
        curr != NULL &&
        curr->token->type == OPERATOR &&
        strncmp(curr->token->value, "@", 1) == 0
    ) {
        curr = curr->next;

        if (
            curr != NULL &&
            curr->token->type == IDENTIFIER
        ) {
            curr = curr->next;
            R();
        } else {
            printf("Ap: '<IDENTIFIER>' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

/*
    R -> R Rn
      -> Rn ;
*/
static void R() {
    printf("R()\n");

    Rn();

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
        Rn();
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
    printf("Rn()\n");

    if (
        curr != NULL && (
            curr->token->type == IDENTIFIER ||
            curr->token->type == INTEGER ||
            curr->token->type == STRING
        )
    ) {
        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "true", 4) == 0
    ) {
        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "false", 5) == 0
    ) {
        curr = curr->next;
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "nil", 3) == 0
    ) {
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
            printf("Rn: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "dummy", 5) == 0
    ) {
        curr = curr->next;
    }
}

/*
    D -> Da ’within’ D => ’within’
      -> Da ;
*/
static void D() {
    printf("D()\n");

    Da();

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "within", 6) == 0
    ) {
        curr = curr->next;
        Da();
    }
}

/*
    Da -> Dr ( ’and’ Dr )+
       -> Dr ;
*/
static void Da() {
    printf("Da()\n");

    Dr();

    while (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "and", 3) == 0
    ) {
        curr = curr->next;
        Dr();
    }
}

/*
    Dr -> ’rec’ Db
        -> Db ;
*/
static void Dr() {
    printf("Dr()\n");

    if (
        curr != NULL &&
        curr->token->type == KEYWORD &&
        strncmp(curr->token->value, "rec", 3) == 0
    ) {
        curr = curr->next;
    }
    Db();
}

/*
   Db -> Vl ’=’ E
      -> ’<IDENTIFIER>’ Vb+ ’=’ E
      -> ’(’ D ’)’ ;
*/
void Db() {
    printf("Db()\n");

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
            printf("Db: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        curr = curr->next;

        if (
            curr != NULL && (
                curr->token->type == PUNCTUATION &&
                strncmp(curr->token->value, ",", 1) == 0 ||
                curr->token->type == OPERATOR && strncmp(curr->token->value, "=", 1) == 0
            )
        ) {
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
                    curr = curr->next;
                } else {
                    printf("Db: '<IDENTIFIER>' expected.\n");
                    exit(EXIT_FAILURE);
                }
            }

            if (
                curr != NULL &&
                curr->token->type == OPERATOR &&
                strncmp(curr->token->value, "=", 1) == 0
            ) {
                curr = curr->next;
                E();
            } else {
                printf("Db: '=' expected.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            int iter = 0;
            while (
                curr != NULL && (
                    curr->token->type == IDENTIFIER ||
                    curr->token->type == PUNCTUATION && strncmp(curr->token->value, "(", 1) == 0
                )
            ) {
                Vb();
                iter++;
            }
            if (iter == 0) {
                printf("Db: At least one 'Vb' expected.\n");
                exit(EXIT_FAILURE);
            }

            if (
                curr != NULL &&
                curr->token->type == OPERATOR &&
                strncmp(curr->token->value, "=", 1) == 0
            ) {
                curr = curr->next;
                E();
            }  else {
                printf("Db: '=' expected.\n");
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
    printf("Vb()\n");

    if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
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
                printf("Vb: ')' expected.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/*
    Vl -> ’<IDENTIFIER>’ list ’,’
*/
static void Vl() {
    printf("Vl()\n");

    if (
        curr != NULL &&
        curr->token->type == IDENTIFIER
    ) {
        curr = curr->next;

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
                curr = curr->next;
            } else {
                printf("Vl: <Identifier> expected.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        printf("Vl: <Identifier> expected.\n");
        exit(EXIT_FAILURE);
    }
}
