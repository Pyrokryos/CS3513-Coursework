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
    size_t elem_cnt;
    char **elems;
} Tau;

typedef struct Lambda {
    Tau *params;
    CtrlCell *cell;
} Lambda;

typedef enum CellType {
    CELL_ENV,
    CELL_DELTA,
    CELL_BETA,
    CELL_TAU,
    CELL_LAMBDA,
    CELL_OTHER
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

CtrlCell *current_env;

void initCSEMachine();

CtrlCell *generateCtrlStructs(Vertex *vertex);
