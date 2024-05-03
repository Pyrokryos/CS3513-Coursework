#pragma once

#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "hash_table.h"
#include "tree.h"

typedef struct CtrlCell CtrlCell;

typedef struct Env {
    HashTable *rename_rules;
    struct Env *prev;
} Env;

typedef struct Delta {
    CtrlCell *cell;
} Delta;

typedef struct Tau {
    CtrlCell **expressions;
} Tau;

typedef struct Lambda {
    char **params;
    CtrlCell *body;
} Lambda;

typedef enum CellType {
    CELL_ENV,
    CELL_DELTA,
    CELL_BETA,
    CELL_TAU,
    CELL_LAMBDA,
    
    CELL_AUG,

    CELL_OR,
    CELL_AND,
    CELL_NOT,
    CELL_GR,
    CELL_GE,
    CELL_LS,
    CELL_LE,
    CELL_EQ,
    CELL_NE,

    CELL_ADD,
    CELL_SUB,
    CELL_NEG,
    CELL_MUL,
    CELL_DIV,
    CELL_EXP,

    CELL_GAMMA,
    CELL_TRUE,
    CELL_FALSE,
    CELL_NIL,
    CELL_DUMMY,

    CELL_EQUIV,

    CELL_YSTAR,

    CELL_ID,
    CELL_INT,
    CELL_STR
} CellType;

typedef struct CtrlCell {
    CellType type;
    union {
        Env *env;
        Delta *delta;
        Tau *tau;
        Lambda *lambda;
        char *other;
    } content;
    struct CtrlCell *prev;
    struct CtrlCell *next;
} CtrlCell;

CtrlCell *init_cse_machine();

CtrlCell *generate_ctrl_structs(Vertex *vertex);
