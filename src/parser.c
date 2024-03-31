#include "../include/parser.h"

static Node* curr;

void parse(TokenStream* stream) {
    curr = stream->head;

    if (curr == NULL) {
        printf("Parsing complete!");
    } else {
        E();
        printf("Parsing complete!");
    }
}

static void E() {
    printf("E()\n");

    if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "let") == 0) {
        curr = curr->next;
        D();

        if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "in") == 0) {
            curr = curr->next;
            E();
        } else {
            perror("E: 'in' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "fn") == 0) {
        curr = curr->next;

        int iter = 0;
        while (curr->token->type == IDENTIFIER || curr->token->type == PUNCTUATION && strcmp(curr->token->value, "(") == 0) {
            Vb();

            iter++;
        }
        if (iter == 0) {
            perror("E: At least one 'Vb' expected.\n");
            exit(EXIT_FAILURE);
        }

        if (curr->token->type == OPERATOR && strcmp(curr->token->value, ".") == 0) {
            curr = curr->next;
            E();
        } else {
            perror("E: '.' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        Ew();
    }
}

static void Ew() {
    printf("Ew()\n");

    T();

    if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "where") == 0) {
        curr = curr->next;
        Dr();
    }
}

static void T() {
    printf("T()\n");

    Ta();

    while (curr->token->type == PUNCTUATION && strcmp(curr->token->value, ",")) {
        curr = curr->next;
        Ta();
    }
}

static void Ta() {
    printf("Ta()\n");

    Tc();

    while (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "aug") == 0) {
        curr = curr->next;
        Tc();
    }
}

static void Tc() {
    printf("Tc()\n");

    B();

    if (curr->token->type == OPERATOR && strcmp(curr->token->value, "-") == 0) {
        curr = curr->next;

        if (curr->token->type == OPERATOR && strcmp(curr->token->value, ">") == 0) {
            curr = curr->next;
            Tc();

            if (curr->token->type == OPERATOR && strcmp(curr->token->value, "|") == 0) {
                curr = curr->next;
                Tc();
            } else {
                perror("Tc: '|' expected.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("Tc: '->' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void B() {
    printf("B()\n");

    Bt();

    while (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "or") == 0) {
        curr = curr->next;
        Bt();
    }
}

static void Bt() {
    printf("Bt()\n");

    Bs();

    while (curr->token->type == OPERATOR && strcmp(curr->token->value, "&") == 0) {
        curr = curr->next;
        Bs();
    }
}

static void Bs() {
    printf("Bs()\n");

    if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "not") == 0) {
        curr = curr->next;
        Bp();
    } else {
        Bp();
    }
}

static void Bp() {
    printf("Bp()\n");

    A();

    if (
        curr->token->type == IDENTIFIER && strcmp(curr->token->value, "gr") == 0 ||
        curr->token->type == IDENTIFIER && strcmp(curr->token->value, "ge") == 0 ||
        curr->token->type == OPERATOR && strcmp(curr->token->value, ">") == 0
    ) {
        curr = curr->next;
        if (curr->token->type == OPERATOR && strcmp(curr->token->value, "=") == 0) {
            curr = curr->next;
        }
        A();
    } else if (
        curr->token->type == IDENTIFIER && strcmp(curr->token->value, "ls") == 0 ||
        curr->token->type == IDENTIFIER && strcmp(curr->token->value, "le") == 0 ||
        curr->token->type == OPERATOR && strcmp(curr->token->value, "<") == 0
    ) {
        curr = curr->next;
        if (curr->token->type == OPERATOR && strcmp(curr->token->value, "=") == 0) {
            curr = curr->next;
        }
        A();
    } else if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "eq") == 0) {
        A();
    } else if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "ne") == 0) {
        A();
    }
}

static void A() {
    printf("A()\n");

    int iter = 0;
    while (
        curr->token->type == OPERATOR && strcmp(curr->token->value, "+") == 0 ||
        curr->token->type == OPERATOR && strcmp(curr->token->value, "-") == 0
    ) {
        curr = curr->next;
        At();
    }
    if (iter == 0) {
        perror("A: '+' or '-' expected.\n");
        exit(EXIT_FAILURE);
    }
}

static void At() {
    printf("At()\n");

    Af();

    while (
        curr->token->type == OPERATOR && strcmp(curr->token->value, "*") == 0 ||
        curr->token->type == OPERATOR && strcmp(curr->token->value, "/") == 0
    ) {
        curr = curr->next;
        Af();
    }
}

static void Af() {
    printf("Af()\n");

    Ap();

    if (curr->token->type == OPERATOR && strcmp(curr->token->value, "*") == 0) {
        curr = curr->next;

        if (curr->token->type == OPERATOR && strcmp(curr->token->value, "*") == 0) {
            curr = curr->next;
            Af();
        } else {
            perror("Af: '**' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void Ap() {
    printf("Ap()\n");

    R();

    while (curr->token->type == OPERATOR && strcmp(curr->token->value, "@") == 0) {
        curr = curr->next;

        if (curr->token->type == IDENTIFIER) {
            curr = curr->next;
            R();
        } else {
            perror("Ap: '<IDENTIFIER>' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void R() {
    printf("R()\n");

    Rn();

    while (
        curr->token->type == IDENTIFIER ||
        curr->token->type == INTEGER ||
        curr->token->type == STRING ||
        curr->token->type == PUNCTUATION && strcmp(curr->token->value, "(") == 0
    ) {
        Rn();
    }
}

static void Rn() {
    printf("procRN\n");

    if (
        curr->token->type == IDENTIFIER ||
        curr->token->type == INTEGER ||
        curr->token->type == STRING
    ) {
        curr = curr->next;
    } else if (curr->token->type == PUNCTUATION && strcmp(curr->token->value, "(") == 0) {
        curr = curr->next;
        E();

        if (curr->token->type == PUNCTUATION && strcmp(curr->token->value, ")") == 0) {
            curr = curr->next;
        } else {
            perror("Rn: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void D() {
    printf("D()\n");

    Da();

    if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "within") == 0) {
        curr = curr->next;
        D();
    }
}

static void Da() {
    printf("Da()\n");

    Dr();

    while (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "and") == 0) {
        curr = curr->next;
        Dr();
    }
}

static void Dr() {
    printf("Dr()\n");

    if (curr->token->type == IDENTIFIER && strcmp(curr->token->value, "rec") == 0) {
        curr = curr->next;
    }
    Db();
}

void Db() {
    printf("Db()\n");

    if (curr->token->type == PUNCTUATION && strcmp(curr->token->value, "(") == 0) {
        curr = curr->next;
        D();

        if (curr->token->type == PUNCTUATION && strcmp(curr->token->value, ")") == 0) {
            curr = curr->next;
        } else {
            perror("Db: ')' expected.\n");
            exit(EXIT_FAILURE);
        }
    } else if (curr->token->type == IDENTIFIER) {
        curr = curr->next;

        if (curr->token->type == OPERATOR && strcmp(curr->token->value, ",") == 0) {
            curr = curr->next;
            Vb();

            if (curr->token->type == OPERATOR && strcmp(curr->token->value, "=") == 0) {
                curr = curr->next;
                E();
            } else {
                perror("Db: '=' expected.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            if (curr->token->type == OPERATOR && strcmp(curr->token->value, "=") == 0) {
                curr = curr->next;
                E();
            }
            else {
                int iter = 0;
                while (
                    curr->token->type == IDENTIFIER ||
                    curr->token->type == PUNCTUATION && strcmp(curr->token->value, "(") == 0
                ) {
                    Vb();
                    iter++;
                }
                if (iter == 0) {
                    perror("Db: At least one 'Vb' expected.\n");
                    exit(EXIT_FAILURE);
                }

                if (curr->token->type == OPERATOR && strcmp(curr->token->value, "=") == 0) {
                    curr = curr->next;
                    E();
                }  else {
                    perror("Db: '=' expected.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

static void Vb() {
    printf("Vb()\n");

    if (curr->token->type == IDENTIFIER) {
        curr = curr->next;
    } else if (curr->token->type == PUNCTUATION && strcmp(curr->token->value, "(") == 0) {
        curr = curr->next;

        if (curr->token->type == PUNCTUATION && strcmp(curr->token->value, ")") == 0) {
            curr = curr->next;
        }
        else {
            Vl();

            if (curr->token->type == PUNCTUATION && strcmp(curr->token->value, ")") == 0) {
                curr = curr->next;
            } else {
                perror("Vb: ')' expected.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

static void Vl() {
    printf("Vl()\n");

    if (curr->token->type == IDENTIFIER){
        curr = curr->next;

        while (curr->token->type == PUNCTUATION && strcmp(curr->token->value, ",") == 0) {
            curr = curr->next;

            if (curr->token->type == IDENTIFIER) {
                curr = curr->next;
            } else {
                perror("Vl: <Identifier> expected.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        perror("Vl: <Identifier> expected.\n");
        exit(EXIT_FAILURE);
    }
}
