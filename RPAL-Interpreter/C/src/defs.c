#include "../include/defs.h"

const char *token_type_to_string(TokenType type) {
    switch (type) {
        case IDENTIFIER: return "IDENTIFIER";
        case INTEGER: return "INTEGER";
        case KEYWORD: return "KEYWORD";
        case OPERATOR: return "OPERATOR";
        case PUNCTUATION: return "PUNCTUATION";
        case STRING: return "STRING";
        default:
            fprintf(stderr, "Unknown token type encountered: %d\n", type);
            exit(EXIT_FAILURE);
    }
}

const char *phrase_type_to_string(PhraseType type) {
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

        case Y_STAR: return "Y*";

        case NONE: return "NONE";

        default:
            fprintf(stderr, "Unknown phrase type encountered: %d\n", type);
            exit(EXIT_FAILURE);
    }
}
