#include "../include/cse_machine.h"

size_t env_cnt = 0;

static CtrlCell *current_cell = NULL;
static CtrlCell *current_env = NULL;

void init_cse_machine(Vertex *vert)
{
  CtrlCell *c1 = alloc_ctrl_cell(ENV);

  *(size_t *)&c1->content.env->id = env_cnt++;
  memset(&c1->content.env->bindings, 0, sizeof(c1->content.env->bindings));
  memset(&c1->content.env->prev, 0, sizeof(c1->content.env->prev));

  current_env = c1;

  CtrlCell *ctrl_structs = gen_ctrl_structs(vert, true);

  c1->next = ctrl_structs;
  c1->prev = ctrl_structs->prev;

  ctrl_structs->prev->next = c1;
  ctrl_structs->prev = c1;

  current_cell = c1->prev;
}

static CtrlCell *gen_cond(const Vertex *const vert)
{
  assert(vert != NULL && vert->type == T_COND);

  CtrlCell *_then = alloc_ctrl_cell(DELTA);
  CtrlCell *_else = alloc_ctrl_cell(DELTA);
  CtrlCell *beta = alloc_ctrl_cell(BETA);

  CtrlCell *B = gen_ctrl_structs(get_left_child(vert), true);
  CtrlCell *T = gen_ctrl_structs(get_right_sibling(get_left_child(vert)), true);
  CtrlCell *E = gen_ctrl_structs(get_right_sibling(get_right_sibling(get_left_child(vert))), true);

  *(CtrlCell **)&_then->content.delta->cell = T;
  *(CtrlCell **)&_else->content.delta->cell = E;

  _then->next = _else;
  _else->prev = _then;

  _else->next = beta;
  beta->prev = _else;

  beta->next = B;
  _then->prev = B->prev;
  B->prev->next = _then;
  B->prev = beta;

  return _then;
}
static CtrlCell *gen_tau(const Vertex *const vert)
{
  assert(vert != NULL && vert->type == T_TAU);

  CtrlCell *cell = alloc_ctrl_cell(TAU), *t1, *t2;
  Vertex *v1;

  size_t expr_cnt = 1;
  v1 = get_right_sibling(get_left_child(vert));
  while (v1 != NULL)
  {
    expr_cnt++;
    v1 = get_right_sibling(v1);
  }
  *(size_t *)&cell->content.tau->expr_cnt = expr_cnt;

  CtrlCell **expressions = (CtrlCell **)calloc(expr_cnt, sizeof(CtrlCell *));
  if (expressions == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  *(CtrlCell ***)&cell->content.tau->expressions = expressions;

  cell->prev = cell;
  cell->next = cell;

  v1 = get_left_child(vert);
  while (v1 != NULL)
  {
    t1 = cell->prev;
    t2 = gen_ctrl_structs(v1, true);

    cell->prev = t2->prev;
    t2->prev->next = cell;
    t2->prev = t1;
    t1->next = t2;

    v1 = get_right_sibling(v1);
  }

  return cell;
}
static CtrlCell *gen_lambda(const Vertex *const vert)
{
  assert(vert != NULL && vert->type == E_LAMBDA);

  CtrlCell *cell = alloc_ctrl_cell(LAMBDA);
  Vertex *tmp;

  size_t param_cnt = 1;
  char **params;
  tmp = get_left_child(vert);
  if (tmp->type == V_COMMA)
  {
    tmp = get_right_sibling(get_left_child(tmp));
    while (tmp != NULL)
    {
      param_cnt++;
      tmp = tmp->right_sibling;
    }

    params = (char **)calloc(param_cnt, sizeof(char *));
    if (params == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    tmp = get_left_child(get_left_child(vert));
    for (int i = 0; i < param_cnt; ++i)
    {
      params[i] = strdup(tmp->token->value.s);
      tmp = get_right_sibling(tmp);
    }
  }
  else
  {
    params = (char **)malloc(sizeof(char *));
    if (params == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    params[0] = strdup(tmp->token->value.s);
  }

  *(size_t *)&cell->content.lambda->param_cnt = param_cnt;
  *(char ***)&cell->content.lambda->params = params;
  *(CtrlCell **)&cell->content.lambda->body = gen_ctrl_structs(get_right_sibling(get_left_child(vert)), true);

  cell->prev = cell;
  cell->next = cell;

  return cell;
}
static CtrlCell *gen_ctrl_structs(const Vertex *const vert, const bool selfish)
{
  if (vert == NULL)
  {
    return NULL;
  }
  else
  {
    CtrlCell *cell;
    if (vert->type == T_COND)
    {
      cell = gen_cond(vert);
    }
    else if (vert->type == T_TAU)
    {
      cell = gen_tau(vert);
    }
    else if (vert->type == E_LAMBDA)
    {
      cell = gen_lambda(vert);
    }
    else
    {
      if (vert->type == NONE)
      {
        cell = alloc_ctrl_cell(vert->token->type);

        if (vert->token->type == INTEGER)
        {
          *(int *)&cell->content.i = vert->token->value.i;
        }
        else if (vert->token->type == IDENTIFIER || vert->token->type == STRING)
        {
          *(char **)&cell->content.s = strdup(vert->token->value.s);
        }
      }
      else
      {
        cell = alloc_ctrl_cell(vert->type);
      }

      cell->prev = cell;
      cell->next = cell;

      CtrlCell *left_child = gen_ctrl_structs(get_left_child(vert), false);
      if (left_child != NULL)
      {
        cell->prev = left_child->prev;
        left_child->prev->next = cell;
        cell->next = left_child;
        left_child->prev = cell;
      }
    }

    if (!selfish)
    {
      CtrlCell *tmp = cell->prev;
      CtrlCell *right_sibling = gen_ctrl_structs(get_right_sibling(vert), false);
      if (right_sibling != NULL)
      {
        cell->prev = right_sibling->prev;
        right_sibling->prev->next = cell;
        right_sibling->prev = tmp;
        tmp->next = right_sibling;
      }
    }

    return cell;
  }
}

static void Rule_I(void)
{
  assert(
      current_cell != NULL &&
      current_cell->type == IDENTIFIER &&
      current_cell->content.s != NULL);
  assert(current_env != NULL && current_env->content.env != NULL);

  // Search for the identifier in the path of environments from the current environment to the global environment.
  CtrlCell *env = current_env;
  Binding *binding = NULL;

  while (binding == NULL && env != NULL)
  {
    if (env->content.env->bindings != NULL)
    {
      binding = search(env->content.env->bindings, current_cell->content.s);
      env = env->content.env->prev;
    }
    else
    {
      break;
    }
  }

  /**
   * If the identifier is found, it is replaced with the val it is bound to.
   * Otherwise, proceed to the next cell.
   */
  if (binding == NULL)
  {
    current_cell = current_cell->prev;
  }
  else
  {
    CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
    if (cell == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }
    *(size_t *)&cell->type = binding->type;

    // Identifiers may be bound to STRINGs, INTEGERs, TRUTHs, TUPLEs, FUNCTIONs, or DOUBLEs.

    if (binding->type == STRING)
    {
      add_content(cell, (char *)binding->val.s);
    }
    else if (binding->type == INTEGER)
    {
      add_content(cell, binding->val.i);
    }
    else if (binding->type == TAU)
    {
      add_content(cell, (Tau *)binding->val.tau);
    }
    else if (binding->type == LAMBDA)
    {
      add_content(cell, (Lambda *)binding->val.lambda);
    }
    else if (binding->type == DOUBLE)
    {
      add_content(cell, binding->val.d);
    }
    memset(&binding->val.s, 0, sizeof(binding->val));
    free_binding(binding);

    cell->prev = current_cell->prev;
    cell->next = current_cell->next;

    free_ctrl_cell(current_cell);

    cell->prev->next = cell;
    cell->next->prev = cell;

    current_cell = cell->prev;
  }
}
static void Rule_II(void)
{
  assert(current_cell != NULL && current_cell->type == LAMBDA);

  current_cell = current_cell->prev;
}
static void Rule_III(void)
{
  assert(
      current_cell != NULL &&
      current_cell->next != NULL &&
      current_cell->type == R_GAMMA &&
      current_cell->next->type == IDENTIFIER);

  CtrlCell *op = current_cell->next, *cell;

  // String Operations
  if (strncmp(op->content.s, "Stem", 4) == 0)
  {
    assert(op->next != NULL && op->next->type == STRING);

    op->prev->next = op->next;
    op->next->prev = op->prev;
    free_ctrl_cell(op);

    Stem(current_cell->next);
  }
  else if (strncmp(op->content.s, "Stern", 5) == 0)
  {
    assert(op->next != NULL && op->next->type == STRING);

    op->prev->next = op->next;
    op->next->prev = op->prev;
    free_ctrl_cell(op);

    Stern(current_cell->next);
  }
  else if (strncmp(op->content.s, "Conc", 4) == 0)
  {
    assert(
        op->next != NULL &&
        op->next->next != NULL &&
        op->next->type == STRING &&
        op->next->next->type == STRING);

    op->prev->next = op->next;
    op->next->prev = op->prev;
    free_ctrl_cell(op);

    Conc(current_cell->next, current_cell->next->next);
  }
  // Tuple Operations
  else if (strncmp(op->content.s, "Order", 5) == 0)
  {
    assert(
        op->next != NULL &&
        (op->next->type == R_NIL || op->next->type == TAU));

    op->prev->next = op->next;
    op->next->prev = op->prev;
    free_ctrl_cell(op);

    Order(current_cell->next);
  }
  else if (strncmp(op->content.s, "Null", 5) == 0)
  {
    assert(
        op->next != NULL &&
        (op->next->type == R_NIL || op->next->type == TAU));

    op->prev->next = op->next;
    op->next->prev = op->prev;
    free_ctrl_cell(op);

    Null(current_cell->next);
  }
  // Other Operations
  else if (strncmp(op->content.s, "Print", 5) == 0)
  {
    assert(
        op->next != NULL &&
        (op->next->type == INTEGER ||
         op->next->type == STRING ||
         op->next->type == R_TRUE ||
         op->next->type == R_FALSE ||
         op->next->type == TAU ||
         op->next->type == DOUBLE));

    op->prev->next = op->next;
    op->next->prev = op->prev;
    free_ctrl_cell(op);

    Print(current_cell->next);
  }

  cell = current_cell;

  current_cell = current_cell->prev;
  current_cell->next = current_cell->next->next;
  current_cell->next->prev = current_cell;

  free_ctrl_cell(cell);
}
static void Rule_IV(void)
{
  assert(
      current_cell != NULL &&
      current_cell->next != NULL &&
      current_cell->type == R_GAMMA &&
      (current_cell->next->type == LAMBDA &&
       current_cell->next->content.lambda != NULL &&
       current_cell->next->content.lambda->param_cnt == 1));

  Lambda *lambda = current_cell->next->content.lambda;
  CtrlCell *c1 = alloc_ctrl_cell(ENV), *c2 = alloc_ctrl_cell(ENV), *c3;

  *(size_t *)&c1->content.env->id = env_cnt++;
  *(size_t *)&c2->content.env->id = c1->content.env->id;

  *(Dict **)&c1->content.env->bindings = init_dict(1);
  if (current_cell->next->next->type == INTEGER)
  {
    insert(c1->content.env->bindings, lambda->params[0], current_cell->next->next->content.i);
  }
  else if (
      current_cell->next->next->type == IDENTIFIER ||
      current_cell->next->next->type == STRING)
  {
    insert(c1->content.env->bindings, lambda->params[0], (char *)current_cell->next->next->content.s);
  }
  else if (current_cell->next->next->type == TAU)
  {
    insert(c1->content.env->bindings, lambda->params[0], (Tau *)current_cell->next->next->content.tau);
  }
  else if (current_cell->next->next->type == LAMBDA)
  {
    insert(c1->content.env->bindings, lambda->params[0], (Lambda *)current_cell->next->next->content.lambda);
  }
  else if (current_cell->next->next->type == DOUBLE)
  {
    insert(c1->content.env->bindings, lambda->params[0], current_cell->next->next->content.d);
  }

  *(CtrlCell **)&c1->content.env->prev = current_env;

  c1->prev = current_cell->prev;
  c1->next = current_cell->next->content.lambda->body;

  c2->prev = current_cell->next->content.lambda->body->prev;
  c2->next = current_cell->next->next->next;

  current_cell->next->content.lambda->body->prev->next = c2;
  current_cell->next->content.lambda->body->prev = c1;

  c1->prev->next = c1;
  c2->next->prev = c2;

  c3 = current_cell;
  free_ctrl_cell(c3->next->next);
  memset(&c3->next->content.s, 0, sizeof(c3->next->content));
  free_ctrl_cell(c3->next);
  free_ctrl_cell(c3);

  current_cell = c2->prev;
  current_env = c1;
}
static void Rule_V(void)
{
  assert(
      current_cell != NULL &&
      current_cell->next != NULL &&
      current_cell->next->next != NULL &&
      current_cell->type == ENV &&
      current_cell->next->next->type == ENV &&
      current_cell->content.env->id == current_cell->next->next->content.env->id &&
      current_cell->content.env->id == current_env->content.env->id &&
      current_cell->content.env->id != 0);

  CtrlCell *c1 = current_cell, *c2 = current_cell->next->next;
  current_cell = current_cell->prev;

  c1->prev->next = c1->next;
  c1->next->prev = c1->prev;

  c2->prev->next = c2->next;
  c2->next->prev = c2->prev;

  if (current_cell->next->type == LAMBDA)
  {
    Dict *dict = merge_dicts(current_env->content.env->prev->content.env->bindings, current_env->content.env->bindings);

    free_dict(current_env->content.env->prev->content.env->bindings);
    *(Dict **)&current_env->content.env->prev->content.env->bindings = dict;
  }

  current_env = current_env->content.env->prev;

  free_ctrl_cell(c1);
  free_ctrl_cell(c2);
}
static void Rule_VI(void)
{
  assert(
      current_cell != NULL &&
      current_cell->next != NULL &&
      current_cell->next->next != NULL);
  assert(
      current_cell->type == T_AUG ||
      current_cell->type == B_OR ||
      current_cell->type == B_AND ||
      current_cell->type == B_GR ||
      current_cell->type == B_GE ||
      current_cell->type == B_LS ||
      current_cell->type == B_LE ||
      current_cell->type == B_EQ ||
      current_cell->type == B_NE ||
      current_cell->type == A_ADD ||
      current_cell->type == A_SUB ||
      current_cell->type == A_MUL ||
      current_cell->type == A_DIV ||
      current_cell->type == A_EXP);

  CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
  if (cell == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  memset(&cell->content.s, 0, sizeof(cell->content));

  if (current_cell->type == T_AUG)
  {
    size_t expr_cnt = 0;
    if (current_cell->next->type == TAU)
    {
      expr_cnt += current_cell->next->content.tau->expr_cnt;
    }
    else if (current_cell->next->type == INTEGER || current_cell->next->type == STRING || current_cell->next->type == DOUBLE)
    {
      expr_cnt++;
    }

    if (current_cell->next->next->type == TAU)
    {
      expr_cnt += current_cell->next->next->content.tau->expr_cnt;
    }
    else if (current_cell->next->next->type == INTEGER || current_cell->next->next->type == STRING || current_cell->next->next->type == DOUBLE)
    {
      expr_cnt++;
    }

    Tau *tau = (Tau *)malloc(sizeof(Tau));
    if (tau == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }
    *(size_t *)&tau->expr_cnt = expr_cnt;
    *(CtrlCell ***)&tau->expressions = (CtrlCell **)malloc(expr_cnt * sizeof(CtrlCell *));

    size_t idx = 0;
    if (current_cell->next->type == TAU)
    {
      for (size_t i = 0; i < current_cell->next->content.tau->expr_cnt; ++i)
      {
        *(CtrlCell **)&tau->expressions[idx++] = dupl_ctrl_cell(current_cell->next->content.tau->expressions[i], false);
      }
    }
    else if (current_cell->next->type == INTEGER || current_cell->next->type == STRING || current_cell->next->type == DOUBLE)
    {
      *(CtrlCell **)&tau->expressions[idx++] = dupl_ctrl_cell(current_cell->next, false);
    }

    if (current_cell->next->next->type == TAU)
    {
      for (size_t i = 0; i < current_cell->next->next->content.tau->expr_cnt; ++i)
      {
        *(CtrlCell **)&tau->expressions[idx++] = dupl_ctrl_cell(current_cell->next->next->content.tau->expressions[i], false);
      }
    }
    else if (current_cell->next->next->type == INTEGER || current_cell->next->next->type == STRING || current_cell->next->next->type == DOUBLE)
    {
      *(CtrlCell **)&tau->expressions[idx++] = dupl_ctrl_cell(current_cell->next->next, false);
    }

    *(size_t *)&cell->type = TAU;
    add_content(cell, tau);
  }
  else if (current_cell->type == B_OR || current_cell->type == B_AND)
  {
    assert(current_cell->next->type == R_TRUE || current_cell->next->type == R_FALSE);
    assert(current_cell->next->next->type == R_TRUE || current_cell->next->next->type == R_FALSE);

    if (current_cell->type == B_OR)
    {
      if (current_cell->next->type == R_FALSE || current_cell->next->next->type == R_FALSE)
      {
        *(size_t *)&cell->type = R_FALSE;
      }
      else
      {
        *(size_t *)&cell->type = R_TRUE;
      }
    }
    else
    {
      if (current_cell->next->type == R_TRUE && current_cell->next->next->type == R_TRUE)
      {
        *(size_t *)&cell->type = R_TRUE;
      }
      else
      {
        *(size_t *)&cell->type = R_FALSE;
      }
    }
  }
  else if (
      current_cell->type == B_GR ||
      current_cell->type == B_GE ||
      current_cell->type == B_LS ||
      current_cell->type == B_LE)
  {
    assert((current_cell->next->type == INTEGER || current_cell->next->type == DOUBLE) && (current_cell->next->next->type == INTEGER || current_cell->next->next->type == DOUBLE));

    double a, b;

    if (current_cell->next->type == INTEGER)
    {
      a = (double)current_cell->next->content.i;
    }
    else if (current_cell->next->type == DOUBLE)
    {
      a = current_cell->next->content.d;
    }

    if (current_cell->next->next->type == INTEGER)
    {
      b = (double)current_cell->next->next->content.i;
    }
    else if (current_cell->next->next->type == DOUBLE)
    {
      b = current_cell->next->next->content.d;
    }

    if (current_cell->type == B_GR)
    {
      if (a > b)
      {
        *(size_t *)&cell->type = R_TRUE;
      }
      else
      {
        *(size_t *)&cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_GE)
    {
      if (a >= b)
      {
        *(size_t *)&cell->type = R_TRUE;
      }
      else
      {
        *(size_t *)&cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_LS)
    {
      if (a < b)
      {
        *(size_t *)&cell->type = R_TRUE;
      }
      else
      {
        *(size_t *)&cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_LE)
    {
      if (a <= b)
      {
        *(size_t *)&cell->type = R_TRUE;
      }
      else
      {
        *(size_t *)&cell->type = R_FALSE;
      }
    }
  }
  else if (current_cell->type == B_EQ || current_cell->type == B_NE)
  {
    assert(
        ((current_cell->next->type == INTEGER || current_cell->next->type == DOUBLE) && (current_cell->next->next->type == INTEGER || current_cell->next->next->type == DOUBLE)) ||
        (current_cell->next->type == STRING && current_cell->next->next->type == STRING));

    if (current_cell->next->type == INTEGER || current_cell->next->type == DOUBLE)
    {
      double a, b;

      if (current_cell->next->type == INTEGER)
      {
        a = (double)current_cell->next->content.i;
      }
      else if (current_cell->next->type == DOUBLE)
      {
        a = current_cell->next->content.d;
      }

      if (current_cell->next->next->type == INTEGER)
      {
        b = (double)current_cell->next->next->content.i;
      }
      else if (current_cell->next->next->type == DOUBLE)
      {
        b = current_cell->next->next->content.d;
      }

      if (current_cell->type == B_EQ)
      {
        if (a == b)
        {
          *(size_t *)&cell->type = R_TRUE;
        }
        else
        {
          *(size_t *)&cell->type = R_FALSE;
        }
      }
      else if (current_cell->type == B_NE)
      {
        if (a != b)
        {
          *(size_t *)&cell->type = R_TRUE;
        }
        else
        {
          *(size_t *)&cell->type = R_FALSE;
        }
      }
    }
    else
    {
      assert(current_cell->next->content.s != NULL && current_cell->next->next->content.s != NULL);

      if (current_cell->type == B_EQ)
      {
        if (strcmp(current_cell->next->content.s, current_cell->next->next->content.s) == 0)
        {
          *(size_t *)&cell->type = R_TRUE;
        }
        else
        {
          *(size_t *)&cell->type = R_FALSE;
        }
      }
      else if (current_cell->type == B_NE)
      {
        if (strcmp(current_cell->next->content.s, current_cell->next->next->content.s) != 0)
        {
          *(size_t *)&cell->type = R_TRUE;
        }
        else
        {
          *(size_t *)&cell->type = R_FALSE;
        }
      }
    }
  }
  else if (
      current_cell->type == A_ADD ||
      current_cell->type == A_SUB ||
      current_cell->type == A_MUL ||
      current_cell->type == A_DIV ||
      current_cell->type == A_EXP)
  {
    assert((current_cell->next->type == INTEGER || current_cell->next->type == DOUBLE) && (current_cell->next->next->type == INTEGER || current_cell->next->next->type == DOUBLE));

    if (current_cell->next->type == INTEGER && current_cell->next->next->type == INTEGER)
    {
      *(size_t *)&cell->type = INTEGER;
      add_content(cell, calc(current_cell->next->content.i, current_cell->next->next->content.i, phrase_type_to_string(current_cell->type)));
    }
    else if (current_cell->next->type == INTEGER && current_cell->next->next->type == DOUBLE)
    {
      *(size_t *)&cell->type = DOUBLE;
      add_content(cell, calc(current_cell->next->content.i, current_cell->next->next->content.d, phrase_type_to_string(current_cell->type)));
    }
    else if (current_cell->next->type == DOUBLE && current_cell->next->next->type == INTEGER)
    {
      *(size_t *)&cell->type = DOUBLE;
      add_content(cell, calc(current_cell->next->content.d, current_cell->next->next->content.i, phrase_type_to_string(current_cell->type)));
    }
    else if (current_cell->next->type == DOUBLE && current_cell->next->next->type == DOUBLE)
    {
      *(size_t *)&cell->type = DOUBLE;
      add_content(cell, calc(current_cell->next->content.d, current_cell->next->next->content.d, phrase_type_to_string(current_cell->type)));
    }
  }

  cell->prev = current_cell->prev;
  cell->next = current_cell->next->next->next;
  cell->prev->next = cell;
  cell->next->prev = cell;

  free_ctrl_cell(current_cell->next->next);
  free_ctrl_cell(current_cell->next);
  free_ctrl_cell(current_cell);

  current_cell = cell->prev;
}
static void Rule_VII(void)
{
  assert(current_cell != NULL && current_cell->next != NULL);
  assert(current_cell->type == B_NOT || current_cell->type == A_NEG);

  CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
  if (cell == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  memset(&cell->content.s, 0, sizeof(cell->content));

  if (current_cell->type == B_NOT)
  {
    assert(current_cell->next->type == R_TRUE || current_cell->next->type == R_FALSE);

    if (current_cell->next->type == R_TRUE)
    {
      *(size_t *)&cell->type = R_FALSE;
    }
    else
    {
      *(size_t *)&cell->type = R_TRUE;
    }
  }
  else if (current_cell->type == A_NEG)
  {
    assert(current_cell->next->type == INTEGER || current_cell->next->type == DOUBLE);

    if (current_cell->next->type == INTEGER)
    {
      *(size_t *)&cell->type = INTEGER;
      add_content(cell, -current_cell->next->content.i);
    }
    else if (current_cell->next->type == DOUBLE)
    {
      *(size_t *)&cell->type = DOUBLE;
      add_content(cell, -current_cell->next->content.d);
    }
  }

  cell->prev = current_cell->prev;
  cell->next = current_cell->next->next;
  cell->prev->next = cell;
  cell->next->prev = cell;

  free_ctrl_cell(current_cell->next);
  free_ctrl_cell(current_cell);

  current_cell = cell->prev;
}
static void Rule_VIII(void)
{
  CtrlCell *tmp;
  if (current_cell->next->type == R_TRUE)
  {
    current_cell = current_cell->prev->prev;

    tmp = current_cell->next;

    current_cell->next->next->next->next->prev = current_cell;
    current_cell->next = current_cell->next->next->next->next;

    free_ctrl_cell(tmp->next->next);
    free_ctrl_cell(tmp->next);
    free_ctrl_cell(tmp);
  }
  else
  {
    current_cell = current_cell->prev;

    tmp = current_cell->prev;

    current_cell->prev->prev->next = current_cell;
    current_cell->prev = current_cell->prev->prev;

    free_ctrl_cell(tmp);

    tmp = current_cell->next;

    current_cell->next->next->next->prev = current_cell;
    current_cell->next = current_cell->next->next->next;

    free_ctrl_cell(tmp->next);
    free_ctrl_cell(tmp);
  }

  CtrlCell *cell = current_cell->content.delta->cell;
  tmp = cell->prev;

  current_cell->prev->next = cell;
  cell->prev = current_cell->prev;

  current_cell->next->prev = tmp;
  tmp->next = current_cell->next;

  *(CtrlCell **)&current_cell->content.delta->cell = NULL;
  free_ctrl_cell(current_cell);

  current_cell = tmp;
}
static void Rule_IX(void)
{
  Tau *tau = current_cell->content.tau;

  CtrlCell *tmp = current_cell->next;
  for (size_t i = 0; i < tau->expr_cnt; i++)
  {
    *(CtrlCell **)&tau->expressions[i] = tmp;
    tmp = tmp->next;
  }

  current_cell->next = tmp;
  tmp->prev = current_cell;

  for (size_t i = 0; i < tau->expr_cnt; i++)
  {
    tau->expressions[i]->prev = tau->expressions[i];
    tau->expressions[i]->next = tau->expressions[i];
  }

  current_cell = current_cell->prev;
}
static void Rule_X(void)
{
  CtrlCell *cell = dupl_ctrl_cell(current_cell->next->content.tau->expressions[current_cell->next->next->content.i - 1], false);

  current_cell->prev->next = cell;
  cell->prev = current_cell->prev;

  current_cell->next->next->next->prev = cell;
  cell->next = current_cell->next->next->next;

  current_cell = cell->prev;
}
static void Rule_XI(void)
{
  assert(
      (current_cell != NULL && current_cell->next != NULL && current_cell->next->next != NULL) &&
      (current_cell->type == R_GAMMA && current_cell->next->type == LAMBDA) &&
      (current_cell->next->content.lambda != NULL && current_cell->next->content.lambda->param_cnt > 1 && current_cell->next->content.lambda->body != NULL));

  Lambda *lambda = current_cell->next->content.lambda;
  CtrlCell *c1 = alloc_ctrl_cell(ENV), *c2 = alloc_ctrl_cell(ENV), *c3;

  *(size_t *)&c1->content.env->id = env_cnt++;
  *(size_t *)&c2->content.env->id = c1->content.env->id;

  size_t provided_param_cnt = 0;
  if (current_cell->next->next->type == TAU)
  {
    provided_param_cnt += current_cell->next->next->content.tau->expr_cnt;
  }
  else if (current_cell->next->next->type == INTEGER || current_cell->next->next->type == STRING || current_cell->next->next->type == DOUBLE)
  {
    provided_param_cnt++;
  }

  if (provided_param_cnt == 1)
  {
    *(Dict **)&c1->content.env->bindings = init_dict(1);
    if (current_cell->next->next->type == INTEGER)
    {
      insert(c1->content.env->bindings, lambda->params[0], current_cell->next->next->content.i);
    }
    else if (
        current_cell->next->next->type == IDENTIFIER ||
        current_cell->next->next->type == STRING)
    {
      insert(c1->content.env->bindings, lambda->params[0], (char *)current_cell->next->next->content.s);
    }
    else if (current_cell->next->next->type == TAU)
    {
      insert(c1->content.env->bindings, lambda->params[0], (Tau *)current_cell->next->next->content.tau);
    }
    else if (current_cell->next->next->type == LAMBDA)
    {
      insert(c1->content.env->bindings, lambda->params[0], (Lambda *)current_cell->next->next->content.lambda);
    }
    else if (current_cell->next->next->type == DOUBLE)
    {
      insert(c1->content.env->bindings, lambda->params[0], current_cell->next->next->content.d);
    }
  }
  else
  {
    *(Dict **)&c1->content.env->bindings = init_dict(2 * provided_param_cnt);
    for (int i = 0; i < provided_param_cnt; ++i)
    {
      if (current_cell->next->next->content.tau->expressions[i]->type == INTEGER)
      {
        insert(c1->content.env->bindings, lambda->params[i], current_cell->next->next->content.tau->expressions[i]->content.i);
      }
      else if (current_cell->next->next->content.tau->expressions[i]->type == IDENTIFIER || current_cell->next->next->content.tau->expressions[i]->type == STRING)
      {
        insert(c1->content.env->bindings, lambda->params[i], (char *)current_cell->next->next->content.tau->expressions[i]->content.s);
      }
      else if (current_cell->next->next->content.tau->expressions[i]->type == TAU)
      {
        insert(c1->content.env->bindings, lambda->params[i], (Tau *)current_cell->next->next->content.tau->expressions[i]->content.tau);
      }
      else if (current_cell->next->next->content.tau->expressions[i]->type == LAMBDA)
      {
        insert(c1->content.env->bindings, lambda->params[i], (Lambda *)current_cell->next->next->content.tau->expressions[i]->content.lambda);
      }
      else if (current_cell->next->next->content.tau->expressions[i]->type == DOUBLE)
      {
        insert(c1->content.env->bindings, lambda->params[i], current_cell->next->next->content.tau->expressions[i]->content.d);
      }
    }
  }

  *(CtrlCell **)&c1->content.env->prev = current_env;

  if (provided_param_cnt == lambda->param_cnt)
  {
    c1->prev = current_cell->prev;
    c1->next = current_cell->next->content.lambda->body;

    c2->prev = current_cell->next->content.lambda->body->prev;
    c2->next = current_cell->next->next->next;

    current_cell->next->content.lambda->body->prev->next = c2;
    current_cell->next->content.lambda->body->prev = c1;

    c1->prev->next = c1;
    c2->next->prev = c2;

    c3 = current_cell;
    free_ctrl_cell(c3->next->next);
    memset(&c3->next->content.s, 0, sizeof(c3->next->content));
    free_ctrl_cell(c3->next);
    free_ctrl_cell(c3);

    current_cell = c2->prev;
    current_env = c1;
  }
  else
  {
    char **params = (char **)malloc((lambda->param_cnt - provided_param_cnt) * sizeof(char *));
    if (params == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < lambda->param_cnt - provided_param_cnt; ++i)
    {
      params[i] = strdup(lambda->params[i + provided_param_cnt]);
      if (params[i] == NULL)
      {
        fprintf(stderr, "%s.\n", strerror(ENOMEM));
        exit(EXIT_FAILURE);
      }
    }

    for (size_t i = 0; i < lambda->param_cnt + provided_param_cnt; ++i)
    {
      free(lambda->params[i]);
    }
    free(lambda->params);

    *(size_t *)&lambda->param_cnt = lambda->param_cnt - provided_param_cnt;
    *(char ***)&lambda->params = params;

    c1->prev = current_cell->prev;
    c1->next = current_cell->next;

    c2->prev = current_cell->next;
    c2->next = current_cell->next->next->next;

    c1->prev->next = c1;
    c2->next->prev = c2;

    c3 = current_cell->next->next;

    current_cell->next->prev = c1;
    current_cell->next->next = c2;

    free_ctrl_cell(c3);
    free_ctrl_cell(current_cell);

    current_cell = c1;
    current_env = c1;
  }
}
void eval_cse_machine(void)
{
  assert(current_cell != NULL);

  switch (current_cell->type)
  {
  case IDENTIFIER:
    Rule_I();
    break;
  case INTEGER:
    current_cell = current_cell->prev;
    break;
  case STRING:
    current_cell = current_cell->prev;
    break;
  case DOUBLE:
    current_cell = current_cell->prev;
    break;
  case T_AUG:
  case B_OR:
  case B_AND:
  case B_GR:
  case B_GE:
  case B_LS:
  case B_LE:
  case B_EQ:
  case B_NE:
  case A_ADD:
  case A_SUB:
  case A_MUL:
  case A_DIV:
  case A_EXP:
    Rule_VI();
    break;
  case B_NOT:
  case A_NEG:
    Rule_VII();
    break;
  case R_GAMMA:
    assert(
        current_cell->next != NULL && current_cell->next->next != NULL &&
        (current_cell->next->type == IDENTIFIER || current_cell->next->type == TAU || current_cell->next->type == LAMBDA));

    if (current_cell->next->type == IDENTIFIER)
    {
      Rule_III();
    }
    else if (current_cell->next->type == TAU)
    {
      Rule_X();
    }
    else if (current_cell->next->type == LAMBDA)
    {
      if (current_cell->next->content.lambda->param_cnt == 1)
      {
        Rule_IV();
      }
      else
      {
        Rule_XI();
      }
    }
    break;
  case R_TRUE:
  case R_FALSE:
  case R_NIL:
  case R_DUMMY:
    current_cell = current_cell->prev;
    break;
  case YSTAR:
    CtrlCell *cell = current_cell->next->content.lambda->body;

    current_cell->prev->prev->next = cell;
    cell->prev = current_cell->prev->prev;

    current_cell->next->next->prev = cell;
    cell->next = current_cell->next->next;

    current_cell = cell->prev;
    break;
  case ENV:
    if (current_cell->content.env->id != 0)
    {
      Rule_V();
    }
    break;
  case BETA:
    Rule_VIII();
    break;
  case TAU:
    Rule_IX();
    break;
  case LAMBDA:
    Rule_II();
    break;
  default:
    fprintf(stderr, "Cannot identify the control cell.\n");
    exit(EXIT_FAILURE);
  }

  if (current_cell->type != ENV || current_cell->content.env->id != 0)
  {
    eval_cse_machine();
  }
}

static void Stem(CtrlCell *cell)
{
  assert(
      cell != NULL &&
      cell->type == STRING &&
      cell->content.s != NULL);

  char *ref_str = cell->content.s, *new_str;
  size_t len = strlen(ref_str) + 1;

  if (len == 0)
  {
    new_str = strdup("");
  }
  else
  {
    new_str = (char *)malloc(len);
    if (new_str == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    strncpy(new_str, ref_str, len - 1);
    new_str[len - 1] = '\0';
  }

  CtrlCell *new_cell = alloc_ctrl_cell(STRING);
  add_content(new_cell, new_str);

  new_cell->prev = cell->prev;
  new_cell->next = cell->next;
  new_cell->prev->next = new_cell;
  new_cell->next->prev = new_cell;

  free_ctrl_cell(cell);
}
static void Stern(CtrlCell *cell)
{
  assert(
      cell != NULL &&
      cell->type == STRING &&
      cell->content.s != NULL);

  char *ref_str = cell->content.s, *new_str;
  size_t len = strlen(ref_str) + 1;

  if (len == 0)
  {
    new_str = strdup("");
  }
  else
  {
    new_str = (char *)malloc(len);
    if (new_str == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    strncpy(new_str, ref_str + 1, len - 1);
    new_str[len - 1] = '\0';
  }

  CtrlCell *new_cell = alloc_ctrl_cell(STRING);
  add_content(new_cell, new_str);

  new_cell->prev = cell->prev;
  new_cell->next = cell->next;
  new_cell->prev->next = new_cell;
  new_cell->next->prev = new_cell;

  free_ctrl_cell(cell);
}
static void Conc(CtrlCell *c1, CtrlCell *c2)
{
  assert(
      c1 != NULL &&
      c2 != NULL &&
      c1->type == STRING &&
      c2->type == STRING &&
      c1->content.s != NULL &&
      c2->content.s != NULL);

  char *ref_str1 = c1->content.s, *ref_str2 = c2->content.s, *new_str;
  size_t len1 = strlen(ref_str1), len2 = strlen(ref_str2), len = len1 + len2 + 1;

  if (len == 0)
  {
    new_str = strdup("");
  }
  else
  {
    new_str = (char *)malloc(len);
    if (new_str == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    strncpy(new_str, ref_str1, len1);
    strncpy(new_str + len1, ref_str2, len2);
    new_str[len - 1] = '\0';
  }

  CtrlCell *new_cell = alloc_ctrl_cell(STRING);
  add_content(new_cell, new_str);

  new_cell->prev = c1->prev;
  new_cell->next = c2->next;
  new_cell->prev->next = new_cell;
  new_cell->next->prev = new_cell;

  free_ctrl_cell(c1);
  free_ctrl_cell(c2);
}

static void Order(CtrlCell *cell)
{
  assert(
      cell != NULL &&
      (cell->type == R_NIL ||
       (cell->type == TAU && cell->content.tau != NULL)));

  CtrlCell *new_cell = alloc_ctrl_cell(INTEGER);
  add_content(new_cell, cell->type == R_NIL ? 0 : (int)cell->content.tau->expr_cnt);

  new_cell->prev = cell->prev;
  new_cell->next = cell->next;
  new_cell->prev->next = new_cell;
  new_cell->next->prev = new_cell;

  free_ctrl_cell(cell);
}
static void Null(CtrlCell *cell)
{
  assert(
      cell != NULL &&
      (cell->type == R_NIL ||
       (cell->type == TAU && cell->content.tau != NULL)));

  CtrlCell *new_cell;

  if (cell->type == R_NIL)
  {
    new_cell = alloc_ctrl_cell(R_TRUE);
  }
  else
  {
    new_cell = alloc_ctrl_cell(cell->content.tau->expr_cnt == 0 ? R_TRUE : R_FALSE);
  }

  new_cell->prev = cell->prev;
  new_cell->next = cell->next;
  new_cell->prev->next = new_cell;
  new_cell->next->prev = new_cell;

  free_ctrl_cell(cell);
}

static void Print(CtrlCell *cell)
{
  assert(
      cell != NULL &&
      (cell->type == INTEGER ||
       (cell->type == STRING && cell->content.s != NULL) ||
       cell->type == R_TRUE ||
       cell->type == R_FALSE ||
       (cell->type == TAU && cell->content.tau != NULL) ||
       cell->type == DOUBLE));

  if (cell->type == TAU)
  {
    Tau *tau = cell->content.tau;
    for (size_t i = 0; i < tau->expr_cnt; i++)
    {
      Print(tau->expressions[i]);
    }
  }
  else if (cell->type == INTEGER)
  {
    printf("%d ", cell->content.i);
  }
  else if (cell->type == STRING)
  {
    printf("%s ", cell->content.s);
  }
  else if (cell->type == DOUBLE)
  {
    printf("%lf ", cell->content.d);
  }
  else if (cell->type == R_TRUE)
  {
    printf("true ");
  }
  else if (cell->type == R_FALSE)
  {
    printf("false ");
  }
}

static void add_string_content(CtrlCell *const cell, const char *const s)
{
  assert(cell != NULL && s != NULL && (cell->type == IDENTIFIER || cell->type == STRING));

  *(char **)&cell->content.s = s;
}
static void add_int_content(CtrlCell *const cell, const int i)
{
  assert(cell != NULL && cell->type == INTEGER);

  *(int *)&cell->content.i = i;
}
static void add_env_content(CtrlCell *const cell, const Env *const env)
{
  assert(cell != NULL && env != NULL && cell->type == ENV);

  *(Env **)&cell->content.env = env;
}
static void add_delta_content(CtrlCell *const cell, const Delta *const delta)
{
  assert(cell != NULL && delta != NULL && cell->type == DELTA);

  *(Delta **)&cell->content.delta = delta;
}
static void add_tau_content(CtrlCell *const cell, const Tau *const tau)
{
  assert(cell != NULL && tau != NULL && cell->type == TAU);

  *(Tau **)&cell->content.tau = tau;
}
static void add_lambda_content(CtrlCell *const cell, const Lambda *const lambda)
{
  assert(cell != NULL && lambda != NULL && cell->type == LAMBDA);

  *(Lambda **)&cell->content.lambda = lambda;
}
static void add_double_content(CtrlCell *const cell, const double d)
{
  assert(cell != NULL && cell->type == DOUBLE);

  *(double *)&cell->content.d = d;
}

static int calc_int_int(const int a, const int b, const char *const op)
{
  assert(op != NULL);

  switch (op[0])
  {
  case '+':
    return a + b;
  case '-':
    return a - b;
  case '*':
    if (op[1] == '*')
    {
      return (int)pow((double)a, (double)b);
    }
    else
    {
      return a * b;
    }
  case '/':
    if (b != 0)
    {
      return a / b;
    }
    else
    {
      fprintf(stderr, "%s. : Cannot divide by zero.\n", strerror(EINVAL));
      exit(EXIT_FAILURE);
    }
  default:
    fprintf(stderr, "%s. : Invalid operator.\n", strerror(EINVAL));
    exit(EXIT_FAILURE);
  }
}
double calc_int_double(const int a, const double b, const char *const op)
{
  return calc_double_double((double)a, b, op);
}
double calc_double_int(const double a, const int b, const char *const op)
{
  return calc_double_double(a, (double)b, op);
}
double calc_double_double(const double a, const double b, const char *const op)
{
  assert(op != NULL);

  switch (op[0])
  {
  case '+':
    return a + b;
  case '-':
    return a - b;
  case '*':
    if (op[1] == '*')
    {
      return pow(a, b);
    }
    else
    {
      return a * b;
    }
  case '/':
    if (b != 0)
    {
      return a / b;
    }
    else
    {
      fprintf(stderr, "%s. : Cannot divide by zero.\n", strerror(EINVAL));
      exit(EXIT_FAILURE);
    }
  default:
    fprintf(stderr, "%s. : Invalid operator.\n", strerror(EINVAL));
    exit(EXIT_FAILURE);
  }
}

static CtrlCell *alloc_ctrl_cell(size_t type)
{
  CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
  if (cell == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  *(size_t *)&cell->type = type;

  memset(&cell->content.s, 0, sizeof(cell->content));
  if (type == ENV)
  {
    *(Env **)&cell->content.env = (Env *)malloc(sizeof(Env));
    if (cell->content.env == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }
  }
  else if (type == DELTA)
  {
    *(Delta **)&cell->content.delta = (Delta *)malloc(sizeof(Delta));
    if (cell->content.delta == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }
  }
  else if (type == TAU)
  {
    *(Tau **)&cell->content.tau = (Tau *)malloc(sizeof(Tau));
    if (cell->content.tau == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }
  }
  else if (type == LAMBDA)
  {
    *(Lambda **)&cell->content.lambda = (Lambda *)malloc(sizeof(Lambda));
    if (cell->content.lambda == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }
  }

  return cell;
}

Tau *dupl_tau(const Tau *const tau)
{
  if (tau == NULL)
  {
    return NULL;
  }
  else
  {
    Tau *new_tau = (Tau *)malloc(sizeof(Tau));
    if (new_tau == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    *(size_t *)&new_tau->expr_cnt = tau->expr_cnt;
    *(CtrlCell ***)&new_tau->expressions = (CtrlCell **)malloc(tau->expr_cnt * sizeof(CtrlCell *));
    if (new_tau->expressions == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    if (tau->expressions[0] == NULL)
    {
      *(CtrlCell **)&new_tau->expressions[0] = NULL;

      for (size_t i = 1; i < tau->expr_cnt; ++i)
      {
        assert(tau->expressions[i] == NULL);

        *(CtrlCell **)&new_tau->expressions[i] = NULL;
      }
    }
    else
    {
      *(CtrlCell **)&new_tau->expressions[0] = dupl_ctrl_cell(tau->expressions[0], false);

      for (size_t i = 0; i < tau->expr_cnt; ++i)
      {
        assert(tau->expressions[i] != NULL);

        *(CtrlCell **)&new_tau->expressions[i] = dupl_ctrl_cell(tau->expressions[i], false);
      }
    }

    return new_tau;
  }
}
Lambda *dupl_lambda(const Lambda *const lambda)
{
  if (lambda == NULL)
  {
    return NULL;
  }
  else
  {
    for (size_t i = 0; i < lambda->param_cnt; ++i)
    {
      assert(lambda->params[i] != NULL);
    }

    Lambda *new_lambda = (Lambda *)malloc(sizeof(Lambda));
    if (new_lambda == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    *(size_t *)&new_lambda->param_cnt = lambda->param_cnt;
    *(char ***)&new_lambda->params = (char **)malloc(lambda->param_cnt * sizeof(char *));
    if (new_lambda->params == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < lambda->param_cnt; ++i)
    {
      *(char **)&new_lambda->params[i] = strdup(lambda->params[i]);
    }

    *(CtrlCell **)&new_lambda->body = dupl_ctrl_structs(lambda->body);

    return new_lambda;
  }
}
static CtrlCell *dupl_ctrl_cell(const CtrlCell *const cell, const bool preserve_links)
{
  if (cell == NULL)
  {
    return NULL;
  }
  else
  {
    CtrlCell *new_cell = (CtrlCell *)malloc(sizeof(CtrlCell));
    if (new_cell == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }
    *(size_t *)&new_cell->type = cell->type;

    memset(&new_cell->content.s, 0, sizeof(new_cell->content));
    if (cell->type == INTEGER)
    {
      add_content(new_cell, cell->content.i);
    }
    else if (cell->type == IDENTIFIER || cell->type == STRING)
    {
      add_content(new_cell, strdup(cell->content.s));
    }
    else if (cell->type == ENV)
    {
      *(Env **)&new_cell->content.env = (Env *)malloc(sizeof(Env));
      if (new_cell->content.env == NULL)
      {
        fprintf(stderr, "%s.\n", strerror(ENOMEM));
        exit(EXIT_FAILURE);
      }

      *(size_t *)&new_cell->content.env->id = cell->content.env->id;
      *(Dict **)&new_cell->content.env->bindings = dupl_dict(cell->content.env->bindings);
      *(CtrlCell **)&new_cell->content.env->prev = cell->content.env->prev;
    }
    else if (cell->type == DELTA)
    {
      *(Delta **)&new_cell->content.delta = (Delta *)malloc(sizeof(Delta));
      if (new_cell->content.delta == NULL)
      {
        fprintf(stderr, "%s.\n", strerror(ENOMEM));
        exit(EXIT_FAILURE);
      }

      *(CtrlCell **)&new_cell->content.delta->cell = dupl_ctrl_structs(cell->content.delta->cell);
    }
    else if (cell->type == TAU)
    {
      add_content(new_cell, dupl_tau(cell->content.tau));
    }
    else if (cell->type == LAMBDA)
    {
      add_content(new_cell, dupl_lambda(cell->content.lambda));
    }
    else if (cell->type == DOUBLE)
    {
      add_content(new_cell, cell->content.d);
    }

    if (preserve_links)
    {
      new_cell->prev = cell->prev;
      new_cell->next = cell->next;
    }
    else
    {
      new_cell->prev = new_cell;
      new_cell->next = new_cell;
    }

    return new_cell;
  }
}
static CtrlCell *dupl_ctrl_structs(const CtrlCell *const cell)
{
  if (cell == NULL)
  {
    return NULL;
  }
  else
  {
    size_t cells_to_dupl = 1;
    CtrlCell *c1 = cell, *c2 = cell->next, *c3;
    while (c1 != c2)
    {
      cells_to_dupl++;
      c2 = c2->next;
    }

    c2 = dupl_ctrl_cell(c1, false);
    c3 = c2;
    for (size_t i = 1; i < cells_to_dupl; ++i)
    {
      c3->next = dupl_ctrl_cell(c1->next, false);
      c3->next->prev = c3;
      c3 = c3->next;
      c1 = c1->next;
    }

    c3->next = c2;
    c2->prev = c3;

    return c2;
  }
}

void free_tau(Tau *const tau)
{
  if (tau == NULL)
  {
    return;
  }
  else if (tau->expressions != NULL)
  {
    if (tau->expressions[0] == NULL)
    {
      for (size_t i = 1; i < tau->expr_cnt; ++i)
      {
        assert(tau->expressions[i] == NULL);
      }
    }
    else
    {
      free_ctrl_structs(tau->expressions[0]);

      for (size_t i = 1; i < tau->expr_cnt; ++i)
      {
        assert(tau->expressions[i] != NULL);

        free_ctrl_structs(tau->expressions[i]);
      }
    }

    free((void *)tau->expressions);
  }

  free((void *)tau);
}
void free_lambda(Lambda *const lambda)
{
  if (lambda == NULL)
  {
    return;
  }
  else
  {
    if (lambda->params != NULL)
    {
      for (size_t i = 0; i < lambda->param_cnt; ++i)
      {
        free((void *)lambda->params[i]);
      }

      free((void *)lambda->params);
    }

    free_ctrl_structs(lambda->body);

    free((void *)lambda);
  }
}
static void free_ctrl_cell(CtrlCell *const cell)
{
  if (cell == NULL)
  {
    return;
  }
  else if (cell->type == IDENTIFIER || cell->type == STRING)
  {
    free((void *)cell->content.s);
  }
  else if (cell->type == ENV)
  {
    if (cell->content.env != NULL)
    {
      free_dict(cell->content.env->bindings);
      free((void *)cell->content.env);
    }
  }
  else if (cell->type == LAMBDA)
  {
    if (cell->content.lambda != NULL)
    {
      free_lambda(cell->content.lambda);
    }
  }
  else if (cell->type == DELTA)
  {
    if (cell->content.delta != NULL)
    {
      free_ctrl_structs(cell->content.delta->cell);
      free((void *)cell->content.delta);
    }
  }
  else if (cell->type == TAU)
  {
    if (cell->content.tau != NULL)
    {
      free_tau(cell->content.tau);
    }
  }

  free((void *)cell);
}
static void free_ctrl_structs(CtrlCell *const cell)
{
  if (cell == NULL)
  {
    return;
  }
  else
  {
    size_t cells_to_free = 1;
    CtrlCell *c1 = cell, *c2 = cell->next;
    while (c1 != c2)
    {
      cells_to_free++;
      c2 = c2->next;
    }

    c1 = cell, c2 = cell->next;
    for (size_t i = 0; i < cells_to_free; ++i)
    {
      free_ctrl_cell(c1);
      c1 = c2;
      c2 = c2->next;
    }
  }
}
