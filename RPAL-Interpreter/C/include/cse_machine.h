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

#define add_content(ctrl_cell, content) _Generic((content), \
    Env *: add_env_content,                                 \
    Lambda *: add_lambda_content,                           \
    Delta *: add_delta_content,                             \
    Tau *: add_tau_content,                                 \
    char *: add_string_content,                             \
    int: add_int_content,                                   \
    double: add_double_content)(ctrl_cell, content)

#define calculate(a, b, op) _Generic((a), \
    int: _Generic((b),                    \
    int: calculate_int_int,               \
    double: calculate_int_double),        \
    double: _Generic((b),                 \
    int: calculate_double_int,            \
    double: calculate_double_double))(a, b, op)

void init_cse_machine(Vertex *vertex);
void eval_cse_machine(void);

static CtrlCell *generate_ctrl_structs(Vertex *vertex, bool selfish);

static CtrlCell *alloc_ctrl_cell(void);
static CtrlCell *alloc_ctrl_cell_with_type(size_t type);

static Tau *dupl_tau(Tau *tau);
static Lambda *dupl_lambda(Lambda *lambda);
static CtrlCell *dupl_ctrl_structs(CtrlCell *cell);
static CtrlCell *dupl_ctrl_cell(CtrlCell *cell, bool preserve_links);

static void free_ctrl_cell(CtrlCell *cell);

static void add_env_content(CtrlCell *cell, Env *env);
static void add_lambda_content(CtrlCell *cell, Lambda *lambda);
static void add_delta_content(CtrlCell *cell, Delta *delta);
static void add_tau_content(CtrlCell *cell, Tau *tau);
static void add_string_content(CtrlCell *cell, char *s);
static void add_int_content(CtrlCell *cell, int i);
static void add_double_content(CtrlCell *cell, double d);

static int calculate_int_int(int a, int b, char *op);
static double calculate_int_double(int a, double b, char *op);
static double calculate_double_int(double a, int b, char *op);
static double calculate_double_double(double a, double b, char *op);

static void Order(CtrlCell *cell);
static void Print(CtrlCell *cell);
