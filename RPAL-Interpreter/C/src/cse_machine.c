#include "../include/cse_machine.h"

size_t env_cnt = 0;

static CtrlCell *current_cell = NULL;
static CtrlCell *current_env = NULL;

void init_cse_machine(Vertex *vertex)
{
  current_cell = alloc_ctrl_cell();
  current_env = alloc_ctrl_cell();

  CtrlCell *c1 = alloc_ctrl_cell_with_type(ENV);

  c1->content.env->id = env_cnt++;
  c1->content.env->rename_rules = NULL;
  c1->content.env->prev = NULL;

  current_env = c1;

  CtrlCell *ctrl_structs = generate_ctrl_structs(vertex, true);

  c1->next = ctrl_structs;
  c1->prev = ctrl_structs->prev;

  ctrl_structs->prev->next = c1;
  ctrl_structs->prev = c1;

  current_cell = c1->prev;
}

static CtrlCell *generate_ctrl_structs(Vertex *vertex, bool selfish)
{
  if (vertex == NULL)
  {
    return NULL;
  }
  else
  {
    if (vertex->type == T_AUG)
    {
      CtrlCell *cell = NULL, *left_child = NULL, *right_sibling = NULL, *tmp = NULL;

      left_child = generate_ctrl_structs(get_left_child(vertex), true);
      right_sibling = generate_ctrl_structs(get_right_sibling(get_left_child(vertex)), true);

      if (left_child->type == R_NIL)
      {
        cell = alloc_ctrl_cell_with_type(TAU);

        cell->content.tau->expr_cnt = 1;
        cell->content.tau->expressions = (CtrlCell **)malloc(sizeof(CtrlCell *));
        if (cell->content.tau->expressions == NULL)
        {
          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }

        cell->next = right_sibling;
        cell->prev = right_sibling->prev;

        right_sibling->prev->next = cell;
        right_sibling->prev = cell;
      }
      else if (left_child->type == TAU)
      {
        cell = left_child;

        cell->content.tau->expr_cnt++;
        cell->content.tau->expressions = (CtrlCell **)realloc(cell->content.tau->expressions, cell->content.tau->expr_cnt * sizeof(CtrlCell *));

        tmp = cell->next;

        cell->prev = right_sibling->prev;
        right_sibling->prev->next = cell;

        tmp->next = right_sibling;
        right_sibling->prev = tmp;
      }
      else
      {
        cell = alloc_ctrl_cell_with_type(TAU);

        cell->content.tau->expr_cnt = 2;
        cell->content.tau->expressions = (CtrlCell **)malloc(2 * sizeof(CtrlCell *));
        if (cell->content.tau->expressions == NULL)
        {
          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }

        cell->next = left_child;
        left_child->prev->next = right_sibling;
        right_sibling->prev->next = cell;

        cell->prev = right_sibling->prev;
        right_sibling->prev = left_child->prev;
        left_child->prev = cell;
      }

      if (!selfish)
      {
        right_sibling = generate_ctrl_structs(get_right_sibling(vertex), false);
        if (right_sibling != NULL)
        {
          cell->prev = right_sibling->prev;
          right_sibling->prev->next = cell;
          cell->next = right_sibling;
          right_sibling->prev = cell;
        }
      }

      return cell;
    }
    else if (vertex->type == T_COND)
    {
      CtrlCell *_then = alloc_ctrl_cell_with_type(DELTA);
      CtrlCell *_else = alloc_ctrl_cell_with_type(DELTA);
      CtrlCell *beta = alloc_ctrl_cell_with_type(BETA);

      CtrlCell *B = generate_ctrl_structs(get_left_child(vertex), true);
      CtrlCell *T = generate_ctrl_structs(get_right_sibling(get_left_child(vertex)), true);
      CtrlCell *E = generate_ctrl_structs(get_right_sibling(get_right_sibling(get_left_child(vertex))), true);

      _then->content.delta->cell = T;
      _else->content.delta->cell = E;
      beta->content.delta = NULL;

      _then->next = _else;
      _else->prev = _then;

      _else->next = beta;
      beta->prev = _else;

      beta->next = B;
      _then->prev = B->prev;
      B->prev->next = _then;
      B->prev = beta;

      if (!selfish)
      {
        CtrlCell *temp = _then->prev;
        CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex), false);
        if (right_sibling != NULL)
        {
          _then->prev = right_sibling->prev;
          right_sibling->prev->next = _then;
          right_sibling->prev = temp;
          temp->next = right_sibling;
        }
      }

      return _then;
    }
    else if (vertex->type == T_TAU)
    {
      CtrlCell *cell = alloc_ctrl_cell_with_type(TAU), *t1, *t2;
      Vertex *v1;

      size_t expr_cnt = 1;
      v1 = get_right_sibling(get_left_child(vertex));
      while (v1 != NULL)
      {
        expr_cnt++;
        v1 = get_right_sibling(v1);
      }
      cell->content.tau->expr_cnt = expr_cnt;

      CtrlCell **expressions = (CtrlCell **)malloc(expr_cnt * sizeof(CtrlCell *));
      if (expressions == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }
      for (size_t i = 0; i < expr_cnt; ++i)
      {
        expressions[i] = NULL;
      }
      cell->content.tau->expressions = expressions;

      cell->prev = cell;
      cell->next = cell;

      v1 = get_left_child(vertex);
      while (v1 != NULL)
      {
        t1 = cell->prev;
        t2 = generate_ctrl_structs(v1, true);

        cell->prev = t2->prev;
        t2->prev->next = cell;
        t2->prev = t1;
        t1->next = t2;

        v1 = get_right_sibling(v1);
      }

      if (!selfish)
      {
        t1 = cell->prev;
        CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex), false);
        if (right_sibling != NULL)
        {
          cell->prev = right_sibling->prev;
          right_sibling->prev->next = cell;
          right_sibling->prev = t1;
          t1->next = right_sibling;
        }
      }

      return cell;
    }
    else if (vertex->type == E_LAMBDA)
    {
      CtrlCell *cell = alloc_ctrl_cell_with_type(LAMBDA);
      Vertex *temp;

      size_t param_cnt = 1;
      char **params;
      temp = get_left_child(vertex);
      if (temp->type == V_COMMA)
      {
        temp = get_right_sibling(get_left_child(temp));
        while (temp != NULL)
        {
          param_cnt++;
          temp = temp->right_sibling;
        }

        params = (char **)malloc(param_cnt * sizeof(char *));
        if (params == NULL)
        {
          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }

        temp = get_left_child(get_left_child(vertex));
        for (int i = 0; i < param_cnt; ++i)
        {
          params[i] = strdup(temp->token->val.s);
          temp = get_right_sibling(temp);
        }
      }
      else
      {
        params = (char **)malloc(sizeof(char *));
        if (params == NULL)
        {
          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }

        params[0] = strdup(get_left_child(vertex)->token->val.s);
      }

      cell->content.lambda->param_cnt = param_cnt;
      cell->content.lambda->params = params;
      cell->content.lambda->body = generate_ctrl_structs(get_right_sibling(get_left_child(vertex)), true);

      cell->prev = cell;
      cell->next = cell;

      if (!selfish)
      {
        CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex), false);
        if (right_sibling != NULL)
        {
          cell->prev = right_sibling->prev;
          right_sibling->prev->next = cell;
          cell->next = right_sibling;
          right_sibling->prev = cell;
        }
      }

      return cell;
    }
    else
    {
      CtrlCell *cell = alloc_ctrl_cell();

      if (vertex->type == NONE)
      {
        if (vertex->token->type == INTEGER)
        {
          cell->content.i = vertex->token->val.i;
        }
        else if (vertex->token->type == IDENTIFIER || vertex->token->type == STRING)
        {
          cell->content.s = strdup(vertex->token->val.s);
        }
        cell->type = vertex->token->type;
      }
      else
      {
        cell->type = vertex->type;
      }

      cell->prev = cell;
      cell->next = cell;

      CtrlCell *left_child = generate_ctrl_structs(get_left_child(vertex), false);
      if (left_child != NULL)
      {
        cell->prev = left_child->prev;
        left_child->prev->next = cell;
        cell->next = left_child;
        left_child->prev = cell;
      }

      if (!selfish)
      {
        CtrlCell *t1 = cell->prev;
        CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex), false);
        if (right_sibling != NULL)
        {
          cell->prev = right_sibling->prev;
          right_sibling->prev->next = cell;
          right_sibling->prev = t1;
          t1->next = right_sibling;
        }
      }

      return cell;
    }
  }
}

void eval_cse_machine(void)
{
  // CSE Rule 1
  if (current_cell->type == IDENTIFIER)
  {
    // Search for the identifier in the path of environments from the current environment to the global environment.
    CtrlCell *env = current_env;
    Binding *entry = NULL;

    while (entry == NULL && env != NULL)
    {
      if (env->content.env->rename_rules != NULL)
      {
        entry = search(env->content.env->rename_rules, current_cell->content.s);
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
    if (entry == NULL)
    {
      current_cell = current_cell->prev;
    }
    else
    {
      CtrlCell *cell = alloc_ctrl_cell_with_type(entry->type);

      // Identifiers may be bound to INTEGERs, STRINGs, TRUTHs, TUPLEs, FUNCTIONs, or DOUBLEs.
      if (entry->type == INTEGER)
      {
        add_content(cell, entry->val.i);
      }
      else if (entry->type == STRING)
      {
        add_content(cell, strdup(entry->val.s));
      }
      else if (entry->type == TAU)
      {
        add_content(cell, dupl_tau(entry->val.tau));
      }
      else if (entry->type == LAMBDA)
      {
        add_content(cell, dupl_lambda(entry->val.lambda));
      }
      else if (entry->type == DOUBLE)
      {
        add_content(cell, entry->val.d);
      }

      cell->prev = current_cell->prev;
      cell->next = current_cell->next;

      free_ctrl_cell(current_cell);
      current_cell = NULL;

      cell->prev->next = cell;
      cell->next->prev = cell;

      current_cell = cell->prev;
    }
  }
  else if (
      current_cell->type == INTEGER ||
      current_cell->type == STRING ||
      current_cell->type == DOUBLE)
  {
    current_cell = current_cell->prev;
  }
  else if (
      current_cell->type == B_OR ||
      current_cell->type == B_AND ||
      current_cell->type == B_NOT)
  {
    if (current_cell->next->type == R_TRUE)
    {
      if (current_cell->type == B_NOT)
      {
        current_cell->next->type = R_FALSE;
      }
      else if (
          current_cell->type == B_OR ||
          current_cell->next->next->type == R_TRUE)
      {
        current_cell->next->next = current_cell->next->next->next;

        free_ctrl_cell(current_cell->next->next->prev);

        current_cell->next->next->prev = current_cell->next;
      }
      else
      {
        current_cell->next = current_cell->next->next;

        free_ctrl_cell(current_cell->next->prev);

        current_cell->next->prev = current_cell;
      }
    }
    else
    {
      if (current_cell->type == B_NOT)
      {
        current_cell->next->type = R_TRUE;
      }
      else if (
          current_cell->type == B_AND ||
          current_cell->next->next->type == R_FALSE)
      {
        current_cell->next->next = current_cell->next->next->next;

        free_ctrl_cell(current_cell->next->next->prev);

        current_cell->next->next->prev = current_cell->next;
      }
      else
      {
        current_cell->next = current_cell->next->next;

        free_ctrl_cell(current_cell->next->prev);

        current_cell->next->prev = current_cell;
      }
    }

    current_cell->prev->next = current_cell->next;
    current_cell->next->prev = current_cell->prev;

    CtrlCell *temp = current_cell;
    current_cell = current_cell->prev;
    free_ctrl_cell(temp);
  }
  else if (
      current_cell->type == B_GR ||
      current_cell->type == B_GE ||
      current_cell->type == B_LS ||
      current_cell->type == B_LE ||
      current_cell->type == B_EQ ||
      current_cell->type == B_NE)
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

    CtrlCell *cell = alloc_ctrl_cell();
    if (current_cell->type == B_GR)
    {
      if (a > b)
      {
        cell->type = R_TRUE;
      }
      else
      {
        cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_GE)
    {
      if (a >= b)
      {
        cell->type = R_TRUE;
      }
      else
      {
        cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_LS)
    {
      if (a < b)
      {
        cell->type = R_TRUE;
      }
      else
      {
        cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_LE)
    {
      if (a <= b)
      {
        cell->type = R_TRUE;
      }
      else
      {
        cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_EQ)
    {
      if (a == b)
      {
        cell->type = R_TRUE;
      }
      else
      {
        cell->type = R_FALSE;
      }
    }
    else if (current_cell->type == B_NE)
    {
      if (a != b)
      {
        cell->type = R_TRUE;
      }
      else
      {
        cell->type = R_FALSE;
      }
    }

    cell->prev = current_cell->prev;
    cell->next = current_cell->next->next->next;

    current_cell->prev->next = cell;
    current_cell->next->next->next->prev = cell;

    free_ctrl_cell(current_cell->next->next);
    free_ctrl_cell(current_cell->next);
    free_ctrl_cell(current_cell);

    current_cell = cell->prev;
  }
  else if (
      current_cell->type == A_ADD ||
      current_cell->type == A_SUB ||
      current_cell->type == A_MUL ||
      current_cell->type == A_DIV ||
      current_cell->type == A_EXP)
  {
    CtrlCell *cell = alloc_ctrl_cell();

    if (current_cell->next->type == INTEGER && current_cell->next->next->type == INTEGER)
    {
      cell->type = INTEGER;
      add_content(cell, calculate(current_cell->next->content.i, current_cell->next->next->content.i, phrase_type_to_string(current_cell->type)));
    }
    else if (current_cell->next->type == INTEGER && current_cell->next->next->type == DOUBLE)
    {
      cell->type = DOUBLE;
      add_content(cell, calculate(current_cell->next->content.i, current_cell->next->next->content.d, phrase_type_to_string(current_cell->type)));
    }
    else if (current_cell->next->type == DOUBLE && current_cell->next->next->type == INTEGER)
    {
      cell->type = DOUBLE;
      add_content(cell, calculate(current_cell->next->content.d, current_cell->next->next->content.i, phrase_type_to_string(current_cell->type)));
    }
    else if (current_cell->next->type == DOUBLE && current_cell->next->next->type == DOUBLE)
    {
      cell->type = DOUBLE;
      add_content(cell, calculate(current_cell->next->content.d, current_cell->next->next->content.d, phrase_type_to_string(current_cell->type)));
    }

    cell->prev = current_cell->prev;
    cell->next = current_cell->next->next->next;

    current_cell->prev->next = cell;
    current_cell->next->next->next->prev = cell;

    free_ctrl_cell(current_cell->next->next);
    free_ctrl_cell(current_cell->next);
    free_ctrl_cell(current_cell);

    current_cell = cell->prev;
  }
  else if (current_cell->type == A_NEG)
  {
    CtrlCell *cell = current_cell->next;

    if (cell->type == INTEGER)
    {
      add_content(cell, -cell->content.i);
    }
    else if (cell->type == DOUBLE)
    {
      add_content(current_cell->next, -current_cell->next->content.d);
    }

    current_cell->prev->next = current_cell->next;
    current_cell->next->prev = current_cell->prev;

    free_ctrl_cell(current_cell);
    current_cell = NULL;

    current_cell = cell->prev;
  }
  else if (current_cell->type == R_GAMMA)
  {
    if (current_cell->next->type == IDENTIFIER)
    {
      CtrlCell *cell = current_cell->next;

      if (strncmp(cell->content.s, "Order", 5) == 0)
      {
        Order(cell->next);
      }
      else if (strncmp(cell->content.s, "Print", 5) == 0)
      {
        Print(cell->next);

        current_cell = current_cell->prev;

        current_cell->next = cell->next->next;
        cell->next->next->prev = current_cell;
      }
    }
    else if (current_cell->next->type == TAU)
    {
      CtrlCell *cell = dupl_ctrl_cell(current_cell->next->content.tau->expressions[current_cell->next->next->content.i - 1], false);

      current_cell->prev->next = cell;
      cell->prev = current_cell->prev;

      current_cell->next->next->next->prev = cell;
      cell->next = current_cell->next->next->next;

      current_cell = cell->prev;
    }
    else if (current_cell->next->type == LAMBDA)
    {
      Lambda *lambda = current_cell->next->content.lambda;

      CtrlCell *c1 = alloc_ctrl_cell_with_type(ENV);
      CtrlCell *c2 = alloc_ctrl_cell_with_type(ENV);

      c1->content.env->id = env_cnt++;
      c2->content.env->id = c1->content.env->id;

      if (lambda->param_cnt == 1)
      {
        c1->content.env->rename_rules = init_dictionary(1);

        Binding *entry = (Binding *)malloc(sizeof(Binding));
        if (entry == NULL)
        {
          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }

        entry->key = lambda->params[0];
        entry->type = current_cell->next->next->type;
        if (entry->type == INTEGER)
        {
          entry->val.i = current_cell->next->next->content.i;
        }
        else if (
            entry->type == IDENTIFIER ||
            entry->type == STRING)
        {
          entry->val.s = strdup(current_cell->next->next->content.s);
        }
        else if (entry->type == TAU)
        {
          entry->val.tau = current_cell->next->next->content.tau;
        }
        else if (entry->type == LAMBDA)
        {
          entry->val.lambda = current_cell->next->next->content.lambda;
        }
        else if (entry->type == DOUBLE)
        {
          entry->val.d = current_cell->next->next->content.d;
        }

        insert(c1->content.env->rename_rules, entry);
      }
      else
      {
        c1->content.env->rename_rules = init_dictionary(2 * lambda->param_cnt);

        Binding *entry;
        for (int i = 0; i < lambda->param_cnt; ++i)
        {
          entry = (Binding *)malloc(sizeof(Binding));
          if (entry == NULL)
          {
            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
          }

          entry->key = lambda->params[i];
          entry->type = current_cell->next->next->content.tau->expressions[i]->type;
          if (entry->type == INTEGER)
          {
            entry->val.i = current_cell->next->next->content.tau->expressions[i]->content.i;
          }
          else if (entry->type == IDENTIFIER || entry->type == STRING)
          {
            entry->val.s = strdup(current_cell->next->next->content.tau->expressions[i]->content.s);
          }
          else if (entry->type == TAU)
          {
            entry->val.tau = current_cell->next->next->content.tau->expressions[i]->content.tau;
          }
          else if (entry->type == LAMBDA)
          {
            entry->val.lambda = current_cell->next->next->content.lambda;
          }
          else if (entry->type == DOUBLE)
          {
            entry->val.d = current_cell->next->next->content.tau->expressions[i]->content.d;
          }

          insert(c1->content.env->rename_rules, entry);
        }
      }

      c1->content.env->prev = current_env;

      c1->prev = current_cell->prev;
      c1->next = current_cell->next->content.lambda->body;

      c2->prev = current_cell->next->content.lambda->body->prev;
      c2->next = current_cell->next->next->next;

      current_cell->next->content.lambda->body->prev->next = c2;
      current_cell->next->content.lambda->body->prev = c1;

      current_cell->prev->next = c1;
      c2->next->prev = c2;

      current_cell = c2->prev;
      current_env = c1;
    }
  }
  else if (
      current_cell->type == R_TRUE ||
      current_cell->type == R_FALSE)
  {
    current_cell = current_cell->prev;
  }
  else if (current_cell->type == YSTAR)
  {
    CtrlCell *cell = current_cell->next->content.lambda->body;

    current_cell->prev->prev->next = cell;
    cell->prev = current_cell->prev->prev;

    current_cell->next->next->prev = cell;
    cell->next = current_cell->next->next;

    current_cell = cell->prev;
  }
  else if (
      current_cell->type == ENV &&
      current_cell->content.env->id != 0)
  {
    CtrlCell *temp;
    if (current_cell->next->type == ENV)
    {
      current_cell->next->next->prev = current_cell->prev;
      current_cell->prev->next = current_cell->next->next;
    }
    else if (current_cell->next->next->type == ENV)
    {
      temp = current_cell->next->next;

      temp->next->prev = temp->prev;
      temp->prev->next = temp->next;

      current_cell->next->prev = current_cell->prev;
      current_cell->prev->next = current_cell->next;

      if (current_cell->next->type == LAMBDA)
      {
        current_env->content.env->prev->content.env->rename_rules = merge_dicts(current_env->content.env->prev->content.env->rename_rules, current_env->content.env->rename_rules);
      }
    }

    current_cell = current_cell->prev;
    current_env = current_env->content.env->prev;
  }
  else if (current_cell->type == BETA)
  {
    CtrlCell *temp;
    if (current_cell->next->type == R_TRUE)
    {
      current_cell = current_cell->prev->prev;

      temp = current_cell->next;

      current_cell->next->next->next->next->prev = current_cell;
      current_cell->next = current_cell->next->next->next->next;

      free_ctrl_cell(temp->next->next);
      free_ctrl_cell(temp->next);
      free_ctrl_cell(temp);
    }
    else
    {
      current_cell = current_cell->prev;

      temp = current_cell->prev;

      current_cell->prev->prev->next = current_cell;
      current_cell->prev = current_cell->prev->prev;

      free_ctrl_cell(temp);

      temp = current_cell->next;

      current_cell->next->next->next->prev = current_cell;
      current_cell->next = current_cell->next->next->next;

      free_ctrl_cell(temp->next);
      free_ctrl_cell(temp);
    }

    CtrlCell *cell = current_cell->content.delta->cell;
    temp = cell->prev;

    current_cell->prev->next = cell;
    cell->prev = current_cell->prev;

    current_cell->next->prev = temp;
    temp->next = current_cell->next;

    current_cell->content.delta->cell = NULL;
    free_ctrl_cell(current_cell);

    current_cell = temp;
  }
  // CSE Rule 9 (Tuple Formation)
  else if (current_cell->type == TAU)
  {
    Tau *tau = current_cell->content.tau;

    CtrlCell *tmp = current_cell->next;
    for (size_t i = 0; i < tau->expr_cnt; i++)
    {
      tau->expressions[i] = tmp;
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
  else if (current_cell->type == LAMBDA)
  {
    current_cell = current_cell->prev;
  }

  if (
      current_cell->type != ENV ||
      current_cell->content.env->id != 0)
  {
    eval_cse_machine();
  }
}

static CtrlCell *alloc_ctrl_cell(void)
{
  CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
  if (cell == NULL)
  {
    perror("Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  return cell;
}
static CtrlCell *alloc_ctrl_cell_with_type(size_t type)
{
  CtrlCell *cell = alloc_ctrl_cell();

  cell->type = type;

  if (type == ENV)
  {
    cell->content.env = (Env *)malloc(sizeof(Env));
    if (cell->content.env == NULL)
    {
      perror("Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  else if (type == DELTA)
  {
    cell->content.delta = (Delta *)malloc(sizeof(Delta));
    if (cell->content.delta == NULL)
    {
      perror("Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  else if (type == TAU)
  {
    cell->content.tau = (Tau *)malloc(sizeof(Tau));
    if (cell->content.tau == NULL)
    {
      perror("Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  else if (type == LAMBDA)
  {
    cell->content.lambda = (Lambda *)malloc(sizeof(Lambda));
    if (cell->content.lambda == NULL)
    {
      perror("Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }

  return cell;
}

static Tau *dupl_tau(Tau *tau)
{
  Tau *new_tau = (Tau *)malloc(sizeof(Tau));
  if (new_tau == NULL)
  {
    perror("Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  new_tau->expr_cnt = tau->expr_cnt;
  new_tau->expressions = (CtrlCell **)malloc(tau->expr_cnt * sizeof(CtrlCell *));
  if (new_tau->expressions == NULL)
  {
    perror("Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  if (tau->expressions[1] == NULL)
  {
    return new_tau;
  }
  else
  {
    for (size_t i = 0; i < tau->expr_cnt; ++i)
    {
      new_tau->expressions[i] = dupl_ctrl_structs(tau->expressions[i]);
    }

    return new_tau;
  }
}
static Lambda *dupl_lambda(Lambda *lambda)
{
  Lambda *new_lambda = (Lambda *)malloc(sizeof(Lambda));
  if (new_lambda == NULL)
  {
    perror("Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  new_lambda->param_cnt = lambda->param_cnt;
  new_lambda->params = (char **)malloc(lambda->param_cnt * sizeof(char *));
  if (new_lambda->params == NULL)
  {
    perror("Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < lambda->param_cnt; ++i)
  {
    new_lambda->params[i] = strdup(lambda->params[i]);
  }

  new_lambda->body = dupl_ctrl_structs(lambda->body);

  return new_lambda;
}
static CtrlCell *dupl_ctrl_structs(CtrlCell *cell)
{
  if (cell == NULL)
  {
    return NULL;
  }
  else
  {
    CtrlCell *c1 = cell, *c2 = cell, *c3 = NULL, *c4 = NULL, *c5 = NULL;

    do
    {
      if (c3 == NULL)
      {
        c3 = dupl_ctrl_cell(c1, false);

        c4 = c3;
        c5 = c3;
      }
      else
      {
        c5 = dupl_ctrl_cell(c2, false);

        c4->next = c5;
        c5->prev = c4;

        c4 = c5;
      }

      c2 = c2->next;
    } while (c1 != c2);

    if (c3 == c4)
    {
      c3->prev = c3;
      c3->next = c3;
    }
    else
    {
      c3->prev = c4;
      c4->next = c3;
    }

    return c3;
  }
}
static CtrlCell *dupl_ctrl_cell(CtrlCell *cell, bool preserve_links)
{
  CtrlCell *new_cell = alloc_ctrl_cell_with_type(cell->type);

  if (cell->type == ENV)
  {
    new_cell->content.env = (Env *)malloc(sizeof(Env));
    if (new_cell->content.env == NULL)
    {
      perror("Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }

    new_cell->content.env->id = cell->content.env->id;
    new_cell->content.env->rename_rules = cell->content.env->rename_rules;
    new_cell->content.env->prev = cell->content.env->prev;
  }
  else if (cell->type == DELTA)
  {
    new_cell->content.delta = (Delta *)malloc(sizeof(Delta));
    if (new_cell->content.delta == NULL)
    {
      perror("Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }

    new_cell->content.delta->cell = dupl_ctrl_structs(cell->content.delta->cell);
  }
  else if (cell->type == TAU)
  {
    new_cell->content.tau = dupl_tau(cell->content.tau);
  }
  else if (cell->type == LAMBDA)
  {
    new_cell->content.lambda = dupl_lambda(cell->content.lambda);
  }
  else if (cell->type == INTEGER)
  {
    new_cell->content.i = cell->content.i;
  }
  else if (
      cell->type == IDENTIFIER ||
      cell->type == STRING)
  {
    new_cell->content.s = strdup(cell->content.s);
  }
  else if (cell->type == DOUBLE)
  {
    new_cell->content.d = cell->content.d;
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

static void free_ctrl_cell(CtrlCell *cell)
{
  if (cell->type == ENV)
  {
    free_dict(cell->content.env->rename_rules);
    cell->content.env->rename_rules = NULL;

    free(cell->content.env);
    cell->content.env = NULL;
  }
  else if (cell->type == LAMBDA)
  {
    for (size_t i = 0; i < cell->content.lambda->param_cnt; ++i)
    {
      free(cell->content.lambda->params[i]);
    }

    free(cell->content.lambda);
    cell->content.lambda = NULL;
  }
  else if (cell->type == DELTA)
  {
    CtrlCell *t1 = cell->content.delta->cell, *t2;
    if (t1 != NULL)
    {
      t1->prev->next = NULL;
      t1->prev = NULL;
    }

    while (t1 != NULL)
    {
      t2 = t1;
      t1 = t1->next;
      free_ctrl_cell(t2);
    }

    cell->content.delta = NULL;
  }
  else if (cell->type == TAU)
  {
    free(cell->content.tau);
    cell->content.tau = NULL;
  }

  free(cell);
  cell = NULL;
}

static void add_env_content(CtrlCell *cell, Env *env)
{
  cell->content.env = env;
}
static void add_lambda_content(CtrlCell *cell, Lambda *lambda)
{
  cell->content.lambda = lambda;
}
static void add_delta_content(CtrlCell *cell, Delta *delta)
{
  cell->content.delta = delta;
}
static void add_tau_content(CtrlCell *cell, Tau *tau)
{
  cell->content.tau = tau;
}
static void add_string_content(CtrlCell *cell, char *s)
{
  cell->content.s = s;
}
static void add_int_content(CtrlCell *cell, int i)
{
  cell->content.i = i;
}
static void add_double_content(CtrlCell *cell, double d)
{
  cell->content.d = d;
}

static int calculate_int_int(int a, int b, char *op)
{
  if (strncmp(op, "+", 1) == 0)
  {
    return a + b;
  }
  else if (strncmp(op, "-", 1) == 0)
  {
    return a - b;
  }
  else if (strncmp(op, "**", 2) == 0)
  {
    return (int)pow((double)a, (double)b);
  }
  else if (strncmp(op, "*", 1) == 0)
  {
    return a * b;
  }
  else if (strncmp(op, "/", 1) == 0)
  {
    if (b != 0)
    {
      return a / b;
    }
    else
    {
      perror("Error: Division by zero\n");
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    printf("Error: Invalid operator\n");
    exit(EXIT_FAILURE);
  }
}
double calculate_int_double(int a, double b, char *op)
{
  return calculate_double_double((double)a, b, op);
}
double calculate_double_int(double a, int b, char *op)
{
  return calculate_double_double(a, (double)b, op);
}
double calculate_double_double(double a, double b, char *op)
{
  if (strncmp(op, "+", 1) == 0)
  {
    return a + b;
  }
  else if (strncmp(op, "-", 1) == 0)
  {
    return a - b;
  }
  else if (strncmp(op, "**", 2) == 0)
  {
    return pow(a, b);
  }
  else if (strncmp(op, "*", 1) == 0)
  {
    return a * b;
  }
  else if (strncmp(op, "/", 1) == 0)
  {
    if (b != 0)
    {
      return a / b;
    }
    else
    {
      perror("Error: Division by zero\n");
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    printf("Error: Invalid operator\n");
    exit(EXIT_FAILURE);
  }
}

static void Order(CtrlCell *cell)
{
  assert(cell->type == TAU);

  CtrlCell *order = alloc_ctrl_cell();

  order->type = INTEGER;
  order->content.i = cell->content.tau->expr_cnt;

  order->prev = cell->prev->prev->prev;
  order->next = cell->next;

  free_ctrl_cell(cell->prev->prev);
  free_ctrl_cell(cell->prev);
  free_ctrl_cell(cell);

  order->prev->next = order;
  order->next->prev = order;

  current_cell = order->prev;
}
static void Print(CtrlCell *cell)
{
  if (cell->type == TAU)
  {
    Tau *tau = cell->content.tau;
    for (size_t i = 0; i < tau->expr_cnt; i++)
    {
      Print(tau->expressions[i]);
      if (tau->expressions[i]->type != TAU)
      {
        printf(" ");
      }
    }
  }
  else if (cell->type == INTEGER)
  {
    printf("%d", cell->content.i);
  }
  else if (cell->type == STRING)
  {
    printf("%s", cell->content.s);
  }
  else if (cell->type == DOUBLE)
  {
    printf("%lf", cell->content.d);
  }
  else if (cell->type == R_TRUE)
  {
    printf("true");
  }
  else if (cell->type == R_FALSE)
  {
    printf("false");
  }
}
