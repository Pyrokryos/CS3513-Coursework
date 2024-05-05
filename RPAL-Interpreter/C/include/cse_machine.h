#pragma once

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "hash_table.h"
#include "tree.h"

typedef struct CtrlCell CtrlCell;

typedef struct Env
{
  size_t id;
  HashTable *rename_rules;
  struct Env *prev;
} Env;

typedef struct Delta
{
  CtrlCell *cell;
} Delta;

typedef struct Tau
{
  size_t expr_cnt;
  CtrlCell **expressions;
} Tau;

typedef struct Lambda
{
  size_t param_cnt;
  char **params;
  CtrlCell *body;
} Lambda;

typedef enum CellType
{
  ENV = 42,
  DELTA,
  BETA,
  TAU,
  LAMBDA,

  DOUBLE
} CellType;

typedef struct CtrlCell
{
  size_t type;
  union
  {
    Env *env;
    Delta *delta;
    Tau *tau;
    Lambda *lambda;
    char *s;
    int i;
    double d;
  } content;
  struct CtrlCell *prev;
  struct CtrlCell *next;
} CtrlCell;

void init_cse_machine(Vertex *vertex);
void eval_cse_machine();

static CtrlCell *generate_ctrl_structs(Vertex *vertex);
