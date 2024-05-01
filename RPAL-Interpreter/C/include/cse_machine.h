#pragma once

#include "hashtable.h"

typedef struct Env {
    HashTable *const rename_rules;
    const Env *const prev;
} Env;

typedef struct Tuple {
    const size_t elem_cnt;
    const char **const elems;
} Tuple;

typedef struct Lambda {
    const struct Tuple *const params;
    const struct CtrlCell *const cell;
} Lambda;
t
typedef enum CellType {
    T_Env,
    T_Tuple,
    T_Lambda,
    T_Gamma,
    T_Other
} CellType;

typedef struct CtrlCell {
    const CellType type;
    union {
        const struct Env *const env;
        const struct Tuple *const tuple;
        const struct Lambda *const lambda;
        const char *const other;
    } content;
} CtrlCell;
