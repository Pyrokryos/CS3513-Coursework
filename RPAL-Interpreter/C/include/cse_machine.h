#pragma once

#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "parser.h"

typedef struct Env {
    HashTable *rename_rules;
    struct Env *prev;
} Env;

typedef struct Tuple {
    size_t elem_cnt;
    char **elems;
} Tuple;

typedef struct Lambda {
    struct Tuple *params;
    struct CtrlCell *cell;
} Lambda;

typedef enum CellType {
    T_Env,
    T_Tuple,
    T_Lambda,
    T_Gamma,
    T_Other
} CellType;

typedef struct CtrlCell {
    CellType type;
    union {
        struct Env *env;
        struct Tuple *tuple;
        struct Lambda *lambda;
        char *other;
    } content;
    struct CtrlCell *prev;
    struct CtrlCell *next;
} CtrlCell;

CtrlCell *currentEnv;
CtrlCell *currentCell;

void initCSEMachine();

void generateCtrlStructs(Vertex *vertex);
