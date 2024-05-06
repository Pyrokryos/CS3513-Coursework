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

  CtrlCell *ctrl_structs = generate_ctrl_structs(vertex);

  c1->next = ctrl_structs;
  c1->prev = ctrl_structs->prev;

  ctrl_structs->prev->next = c1;
  ctrl_structs->prev = c1;

  current_cell = c1->prev;
}

void eval_cse_machine(void)
{
  if (current_cell->type == IDENTIFIER)
  {
    HashTableEntry *entry = search(current_env->content.env->rename_rules, current_cell->content.s);

    if (entry == NULL)
    {
      current_cell = current_cell->prev;
    }
    else
    {
      CtrlCell *cell = alloc_ctrl_cell_with_type(entry->type);

      if (entry->type == INTEGER)
      {
        add_content(cell, entry->val.i);
      }
      else if (entry->type == STRING)
      {
        add_content(cell, strdup(entry->val.s));
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
  else if (current_cell->type == R_GAMMA)
  {

    if (current_cell->next->type == IDENTIFIER)
    {
      CtrlCell *cell = current_cell->next;

      if (strncmp(cell->content.s, "Print", 5) == 0)
      {
        if (cell->next->type == TAU)
        {
          Tau *tau = cell->next->content.tau;
          for (size_t i = 0; i < tau->expr_cnt; i++)
          {
            if (tau->expressions[i]->type == INTEGER)
            {
              printf("%d ", tau->expressions[i]->content.i);
            }
            else if (tau->expressions[i]->type == STRING)
            {
              printf("%s\n", tau->expressions[i]->content.s);
            }
            else if (tau->expressions[i]->type == DOUBLE)
            {
              printf("%lf\n", tau->expressions[i]->content.d);
            }
          }
          printf("\n");
        }
        else if (cell->next->type == INTEGER)
        {
          printf("%d\n", cell->next->content.i);
        }
        else if (cell->next->type == STRING)
        {
          printf("%s\n", cell->next->content.s);
        }
        else if (cell->next->type == DOUBLE)
        {
          printf("%lf\n", cell->next->content.d);
        }

        current_cell = current_cell->prev;

        current_cell->next = cell->next->next;
        cell->next->next->prev = current_cell;
      }
    }
    else if (current_cell->next->type == LAMBDA)
    {
      Lambda *lambda = current_cell->next->content.lambda;

      CtrlCell *c1 = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (c1 == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      CtrlCell *c2 = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (c2 == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      Env *e1 = (Env *)malloc(sizeof(Env));
      if (e1 == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      Env *e2 = (Env *)malloc(sizeof(Env));
      if (e2 == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      e1->id = env_cnt++;
      e2->id = e1->id;

      size_t param_count = sizeof(lambda->params) / sizeof(lambda->params[0]);
      if (param_count == 1)
      {
        e1->rename_rules = init_hash_table(param_count);

        HashTableEntry *entry = (HashTableEntry *)malloc(sizeof(HashTableEntry));
        if (entry == NULL)
        {
          free(current_cell);
          current_cell = NULL;

          free(current_env);
          current_env = NULL;

          free(c1);
          c1 = NULL;

          free(c2);
          c2 = NULL;

          free(e1);
          e1 = NULL;

          free(e2);
          e2 = NULL;

          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }

        entry->key = lambda->params[0];
        entry->type = current_cell->next->next->type;
        if (entry->type == INTEGER)
        {
          entry->val.i = current_cell->next->next->content.i;
        }
        else if (entry->type == IDENTIFIER || entry->type == STRING)
        {
          entry->val.s = strdup(current_cell->next->next->content.s);
        }
        else if (entry->type == DOUBLE)
        {
          entry->val.d = current_cell->next->next->content.d;
        }

        insert(e1->rename_rules, entry);
      }
      else
      {
        e1->rename_rules = init_hash_table(2 * param_count);

        HashTableEntry *entry;
        for (int i = 0; i < param_count; ++i)
        {
          entry = (HashTableEntry *)malloc(sizeof(HashTableEntry));
          if (entry == NULL)
          {
            free(current_cell);
            current_cell = NULL;

            free(current_env);
            current_env = NULL;

            free(c1);
            c1 = NULL;

            free(c2);
            c2 = NULL;

            free(e1);
            e1 = NULL;

            free(e2);
            e2 = NULL;

            perror("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
          }

          entry->key = lambda->params[0];
          entry->type = current_cell->next->next->type;
          if (entry->type == INTEGER)
          {
            entry->val.i = current_cell->next->next->content.i;
          }
          else if (entry->type == IDENTIFIER || entry->type == STRING)
          {
            entry->val.s = strdup(current_cell->next->next->content.s);
          }
          else if (entry->type == DOUBLE)
          {
            entry->val.d = current_cell->next->next->content.d;
          }

          insert(e1->rename_rules, entry);
        }
      }

      e1->prev = current_env->content.env;

      c1->type = ENV;
      c1->content.env = e1;

      c2->type = ENV;
      c2->content.env = e2;

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
      current_cell->type == ENV &&
      current_cell->content.env->id != 0)
  {
    CtrlCell *temp;
    if (current_cell->next->type == ENV)
    {
      current_cell->next->next->prev = current_cell->prev;
      current_cell->prev->next = current_cell->next->next;
    }
    else
    {
      temp = current_cell->next->next;

      temp->next->prev = temp->prev;
      temp->prev->next = temp->next;

      current_cell->next->prev = current_cell->prev;
      current_cell->prev->next = current_cell->next;
    }

    current_cell = current_cell->prev;
    current_env = current_env->prev;
  }
  else if (current_cell->type == TAU)
  {
    CtrlCell *temp;

    Tau *tau = current_cell->content.tau;

    temp = current_cell->next;
    for (size_t i = 0; i < tau->expr_cnt; i++)
    {
      tau->expressions[i] = temp;
      temp = temp->next;
    }

    current_cell->next = temp;
    temp->prev = current_cell;

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

static CtrlCell *generate_ctrl_structs(Vertex *vertex)
{
  if (vertex == NULL)
  {
    return NULL;
  }
  else
  {
    if (vertex->type == T_COND)
    {
      CtrlCell *B = generate_ctrl_structs(get_left_child(vertex));
      CtrlCell *T = generate_ctrl_structs(get_right_sibling(get_left_child(vertex)));
      CtrlCell *E = generate_ctrl_structs(get_right_sibling(get_right_sibling(get_left_child(vertex))));

      CtrlCell *_then = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (_then == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }
      _then->type = DELTA;
      _then->content.delta = (Delta *)malloc(sizeof(Delta));
      if (_then->content.delta == NULL)
      {
        free(_then);
        _then = NULL;

        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      CtrlCell *_else = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (_else == NULL)
      {
        free(_then->content.delta);
        _then->content.delta = NULL;

        free(_then);
        _then = NULL;

        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }
      _else->type = DELTA;
      _else->content.delta = (Delta *)malloc(sizeof(Delta));
      if (_else->content.delta == NULL)
      {
        free(_else);
        _else = NULL;

        free(_then->content.delta);
        _then->content.delta = NULL;

        free(_then);
        _then = NULL;

        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      CtrlCell *beta = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (beta == NULL)
      {
        free(_else->content.delta);
        _else->content.delta = NULL;

        free(_else);
        _else = NULL;

        free(_then->content.delta);
        _then->content.delta = NULL;

        free(_then);
        _then = NULL;

        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }
      beta->type = BETA;
      beta->content.delta = NULL;

      _then->content.delta->cell = T;
      _else->content.delta->cell = E;

      _then->next = _else;
      _else->prev = _then;

      _else->next = beta;
      beta->prev = _else;

      beta->next = B;
      _then->prev = B->prev;
      B->prev->next = _then;
      B->prev = beta;

      CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex));
      CtrlCell *temp = _then->prev;
      if (right_sibling != NULL)
      {
        _then->prev = right_sibling->prev;
        right_sibling->prev->next = _then;
        right_sibling->prev = temp;
        temp->next = right_sibling;
      }

      return _then;
    }
    else if (vertex->type == T_TAU)
    {
      CtrlCell *t1, *t2;
      Vertex *v1;

      CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (cell == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      Tau *tau = (Tau *)malloc(sizeof(Tau));
      if (tau == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      cell->type = TAU;
      cell->content.tau = tau;

      size_t expr_cnt = 1;
      v1 = get_right_sibling(get_left_child(vertex));
      while (v1 != NULL)
      {
        expr_cnt++;
        v1 = get_right_sibling(v1);
      }

      CtrlCell **expressions = (CtrlCell **)malloc(expr_cnt * sizeof(CtrlCell *));
      if (expressions == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      tau->expr_cnt = expr_cnt;
      tau->expressions = expressions;

      t1 = generate_ctrl_structs(get_left_child(vertex));

      cell->prev = t1->prev;
      t1->prev->next = cell;
      t1->prev = cell;
      cell->next = t1;

      t1 = generate_ctrl_structs(get_right_sibling(vertex));
      if (t1 != NULL)
      {
        t2 = cell->prev;

        cell->prev = t1->prev;
        t1->prev->next = cell;
        t1->prev = t2;
        t2->next = t1;
      }

      return cell;
    }
    else if (vertex->type == E_LAMBDA)
    {
      CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (cell == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      Lambda *lambda = (Lambda *)malloc(sizeof(Lambda));
      if (lambda == NULL)
      {
        free(cell);
        cell = NULL;

        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      char **params;
      Vertex *temp = get_left_child(vertex);
      if (temp->type == V_COMMA)
      {
        size_t param_cnt = 1;
        temp = get_right_sibling(get_left_child(get_left_child(vertex)));
        while (temp != NULL)
        {
          param_cnt++;
          temp = temp->right_sibling;
        }

        params = (char **)malloc(param_cnt * sizeof(char *));
        if (params == NULL)
        {
          free(lambda);
          lambda = NULL;

          free(cell);
          cell = NULL;

          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }

        temp = get_left_child(get_left_child(vertex));
        for (int i = 0; i < param_cnt; ++i)
        {
          params[i] = strdup(temp->token->value.s);
          temp = temp->right_sibling;
        }
      }
      else
      {
        params = (char **)malloc(sizeof(char *));
        if (params == NULL)
        {
          free(lambda);
          lambda = NULL;

          free(cell);
          cell = NULL;

          perror("Memory allocation failed.\n");
          exit(EXIT_FAILURE);
        }
        params[0] = temp->token->value.s;
      }

      lambda->params = params;
      lambda->body = generate_ctrl_structs(get_right_sibling(get_left_child(vertex)));

      cell->type = LAMBDA;
      cell->content.lambda = lambda;
      cell->prev = cell;
      cell->next = cell;

      CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex));
      if (right_sibling != NULL)
      {
        cell->next = right_sibling;
        right_sibling->prev->next = cell;
        cell->prev = right_sibling->prev;
        right_sibling->prev = cell;
      }

      return cell;
    }
    else
    {
      CtrlCell *cell = (CtrlCell *)malloc(sizeof(CtrlCell));
      if (cell == NULL)
      {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
      }

      if (vertex->type == NONE)
      {
        if (vertex->token->type == INTEGER)
        {
          cell->content.i = vertex->token->value.i;
        }
        else if (vertex->token->type == IDENTIFIER || vertex->token->type == STRING)
        {
          cell->content.s = strdup(vertex->token->value.s);
        }
        cell->type = vertex->token->type;
      }
      else
      {
        cell->type = vertex->type;
      }

      CtrlCell *left_child = generate_ctrl_structs(get_left_child(vertex));
      CtrlCell *right_sibling = generate_ctrl_structs(get_right_sibling(vertex));

      if (left_child != NULL)
      {
        left_child->prev->next = cell;
        cell->prev = left_child->prev;
        left_child->prev = cell;
        cell->next = left_child;

        if (right_sibling != NULL)
        {
          right_sibling->prev->next = cell;
          cell->prev->next = right_sibling;
          right_sibling->prev = cell->prev;
          cell->prev = right_sibling->prev;
        }
      }
      else
      {
        if (right_sibling != NULL)
        {
          right_sibling->prev->next = cell;
          cell->prev = right_sibling->prev;
          right_sibling->prev = cell;
          cell->next = right_sibling;
        }
        else
        {
          cell->prev = cell;
          cell->next = cell;
        }
      }

      return cell;
    }
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
  else if (type == LAMBDA)
  {
    cell->content.lambda = (Lambda *)malloc(sizeof(Lambda));
    if (cell->content.lambda == NULL)
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

  return cell;
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

static void free_ctrl_cell(CtrlCell *cell)
{
  if (cell->type == ENV)
  {
    free_hash_table(cell->content.env->rename_rules);
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
    free(cell->content.delta);
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
