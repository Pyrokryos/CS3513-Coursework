#pragma once

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "hash_table.h"
#include "tree.h"

#define ENV 42
#define DELTA 43
#define BETA 44
#define TAU 45
#define LAMBDA 46
#define DOUBLE 47

#define add_content(ctrl_cell, content)   \
  _Generic((content),                     \
      Env *: add_env_content,       \
      Lambda *: add_lambda_content, \
      Delta *: add_delta_content,   \
      Tau *: add_tau_content,       \
      char *: add_string_content,   \
      int: add_int_content,               \
      double: add_double_content)(ctrl_cell, content)

#define calc(a, b, op)          \
  _Generic((a),                      \
      int: _Generic((b),             \
      int: calc_int_int,        \
      double: calc_int_double), \
      double: _Generic((b),          \
      int: calc_double_int,     \
      double: calc_double_double))(a, b, op)

typedef struct CtrlCell CtrlCell;

typedef struct Env
{
  const size_t id;
  Dict *const bindings;
  const CtrlCell *const prev;
} Env;

typedef struct Delta
{
  const CtrlCell *const cell;
} Delta;

typedef struct Tau
{
  const size_t expr_cnt;
  CtrlCell *const *const expressions;
} Tau;

typedef struct Lambda
{
  const size_t param_cnt;
  const char *const *const params;
  CtrlCell *const body;
} Lambda;

typedef struct CtrlCell
{
  const size_t type;
  const union
  {
    const char *s;
    int i;
    const Env *env;
    const Delta *delta;
    const Tau *tau;
    const Lambda *lambda;
    double d;
  } content;
  CtrlCell *prev;
  CtrlCell *next;
} CtrlCell;

void init_cse_machine(Vertex *vertex);

static CtrlCell *gen_cond(const Vertex *const vert);
static CtrlCell *gen_tau(const Vertex *const vert);
static CtrlCell *gen_lambda(const Vertex *const vert);
static CtrlCell *gen_ctrl_structs(const Vertex *const vertex, const bool selfish);

static void Rule_I(void);
static void Rule_II(void);
static void Rule_III(void);
static void Rule_IV(void);
static void Rule_V(void);
static void Rule_VI(void);
static void Rule_VII(void);
static void Rule_VIII(void);
static void Rule_IX(void);
static void Rule_X(void);
static void Rule_XI(void);
void eval_cse_machine(void);

/* String Operations */
static void Stem(CtrlCell *cell);
static void Stern(CtrlCell *cell);
static void Conc(CtrlCell *c1, CtrlCell *c2);

/* Tuple Operations */
static void Order(CtrlCell *cell);
static void Null(CtrlCell *cell);

/* Other Operations */
static void Print(CtrlCell *cell);

static void add_string_content(CtrlCell *const cell, const char *const s);
static void add_int_content(CtrlCell *const cell, const int i);
static void add_env_content(CtrlCell *const cell, const Env *const env);
static void add_delta_content(CtrlCell *const cell, const Delta *const delta);
static void add_tau_content(CtrlCell *const cell, const Tau *const tau);
static void add_lambda_content(CtrlCell *const cell, const Lambda *const lambda);
static void add_double_content(CtrlCell *const cell, const double d);

static int calc_int_int(const int a, const int b, const char *const op);
static double calc_int_double(const int a, const double b, const char *const op);
static double calc_double_int(const double a, const int b, const char *const op);
static double calc_double_double(const double a, const double b, const char *const op);

static CtrlCell *alloc_ctrl_cell(size_t type);

Tau *dupl_tau(const Tau *const tau);
Lambda *dupl_lambda(const Lambda *const lambda);
static CtrlCell *dupl_ctrl_cell(const CtrlCell *const cell, const bool preserve_links);
static CtrlCell *dupl_ctrl_structs(const CtrlCell *const cell);

void free_tau(Tau *const tau);
void free_lambda(Lambda *const lambda);
static void free_ctrl_cell(CtrlCell *const cell);
static void free_ctrl_structs(CtrlCell *const cell);
