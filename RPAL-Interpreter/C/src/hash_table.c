#include "../include/hash_table.h"

Dict *init_dictionary(size_t size)
{
  Dict *dict = (Dict *)malloc(sizeof(Dict));
  if (dict == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }

  *(size_t *)&dict->size = size;
  dict->bindings = (Binding **)calloc(dict->size, sizeof(Binding *));
  if (dict->bindings == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }

  return dict;
}

static size_t hash_by_div(const char *const key, const size_t dict_size)
{
  if (key == NULL)
  {
    fprintf(stderr, "%s. : Cannot hash a null string.\n", strerror(ENOTSUP));
    exit(EXIT_FAILURE);
  }

  size_t sum = 0;
  for (size_t i = 0; key[i] != '\0'; ++i)
  {

    sum += (size_t)key[i] * (size_t)pow(HASH_BASE, i);
  }

  return sum % dict_size;
}

static size_t insert_str(const Dict *const dict, const char *const key, const char *const val)
{
  assert(dict != NULL && key != NULL && val != NULL);

  size_t idx = hash_by_div(key, dict->size);
  const size_t orig_idx = idx;
  while (dict->bindings[idx] != NULL)
  {
    if (strncmp(dict->bindings[idx]->key, key, strlen(key)) == 0)
    {
      assert(dict->bindings[idx]->type != NULL);

      if (dict->bindings[idx]->type == STRING)
      {
        free_val(dict->bindings[idx]->val.s);
      }
      else if (dict->bindings[idx]->type == TAU)
      {
        free_val(dict->bindings[idx]->val.tau);
      }
      else if (dict->bindings[idx]->type == LAMBDA)
      {
        free_val(dict->bindings[idx]->val.lambda);
      }
      memset(&dict->bindings[idx]->val, 0, sizeof(dict->bindings[idx]->val));

      dict->bindings[idx]->type = STRING;
      dict->bindings[idx]->val.s = dupl_val(val);
      return EXIT_SUCCESS;
    }

    idx = (idx + 1) % dict->size;

    if (idx == orig_idx)
    {
      fprintf(stderr, "%s. : Cannot insert to an already full dictionary.\n", strerror(ECANCELED));
      exit(EXIT_FAILURE);
    }
  }

  Binding *binding = (Binding *)malloc(sizeof(Binding));
  if (binding == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  *(char **)&binding->key = strdup(key);
  binding->type = STRING;
  binding->val.s = dupl_val(val);

  *(Binding **)&dict->bindings[idx] = binding;

  return EXIT_SUCCESS;
}
static size_t insert_int(const Dict *const dict, const char *const key, const int val)
{
  assert(dict != NULL && key != NULL && val != NULL);

  size_t idx = hash_by_div(key, dict->size);
  const size_t orig_idx = idx;
  while (dict->bindings[idx] != NULL)
  {
    if (strncmp(dict->bindings[idx]->key, key, strlen(key)) == 0)
    {
      assert(dict->bindings[idx]->type != NULL);

      if (dict->bindings[idx]->type == STRING)
      {
        free_val(dict->bindings[idx]->val.s);
      }
      else if (dict->bindings[idx]->type == TAU)
      {
        free_val(dict->bindings[idx]->val.tau);
      }
      else if (dict->bindings[idx]->type == LAMBDA)
      {
        free_val(dict->bindings[idx]->val.lambda);
      }
      memset(&dict->bindings[idx]->val, 0, sizeof(dict->bindings[idx]->val));

      dict->bindings[idx]->type = INTEGER;
      dict->bindings[idx]->val.i = val;
      return EXIT_SUCCESS;
    }

    idx = (idx + 1) % dict->size;

    if (idx == orig_idx)
    {
      fprintf(stderr, "%s. : Cannot insert to an already full dictionary.\n", strerror(ECANCELED));
      exit(EXIT_FAILURE);
    }
  }

  Binding *binding = (Binding *)malloc(sizeof(Binding));
  if (binding == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  *(char **)&binding->key = strdup(key);
  binding->type = INTEGER;
  binding->val.i = val;

  *(Binding **)&dict->bindings[idx] = binding;

  return EXIT_SUCCESS;
}
static size_t insert_tau(const Dict *const dict, const char *const key, const Tau *const val)
{
  assert(dict != NULL && key != NULL && val != NULL);

  size_t idx = hash_by_div(key, dict->size);
  const size_t orig_idx = idx;
  while (dict->bindings[idx] != NULL)
  {
    if (strncmp(dict->bindings[idx]->key, key, strlen(key)) == 0)
    {
      assert(dict->bindings[idx]->type != NULL);

      if (dict->bindings[idx]->type == STRING)
      {
        free_val(dict->bindings[idx]->val.s);
      }
      else if (dict->bindings[idx]->type == TAU)
      {
        free_val(dict->bindings[idx]->val.tau);
      }
      else if (dict->bindings[idx]->type == LAMBDA)
      {
        free_val(dict->bindings[idx]->val.lambda);
      }
      memset(&dict->bindings[idx]->val, 0, sizeof(dict->bindings[idx]->val));

      dict->bindings[idx]->type = TAU;
      dict->bindings[idx]->val.tau = dupl_val(val);
      return EXIT_SUCCESS;
    }

    idx = (idx + 1) % dict->size;

    if (idx == orig_idx)
    {
      fprintf(stderr, "%s. : Cannot insert to an already full dictionary.\n", strerror(ECANCELED));
      exit(EXIT_FAILURE);
    }
  }

  Binding *binding = (Binding *)malloc(sizeof(Binding));
  if (binding == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  *(char **)&binding->key = strdup(key);
  binding->type = TAU;
  binding->val.tau = dupl_val(val);

  *(Binding **)&dict->bindings[idx] = binding;

  return EXIT_SUCCESS;
}
static size_t insert_lambda(const Dict *const dict, const char *const key, const Lambda *const val)
{
  assert(dict != NULL && key != NULL && val != NULL);

  size_t idx = hash_by_div(key, dict->size);
  const size_t orig_idx = idx;
  while (dict->bindings[idx] != NULL)
  {
    if (strncmp(dict->bindings[idx]->key, key, strlen(key)) == 0)
    {
      assert(dict->bindings[idx]->type != NULL);

      if (dict->bindings[idx]->type == STRING)
      {
        free_val(dict->bindings[idx]->val.s);
      }
      else if (dict->bindings[idx]->type == TAU)
      {
        free_val(dict->bindings[idx]->val.tau);
      }
      else if (dict->bindings[idx]->type == LAMBDA)
      {
        free_val(dict->bindings[idx]->val.lambda);
      }
      memset(&dict->bindings[idx]->val, 0, sizeof(dict->bindings[idx]->val));

      dict->bindings[idx]->type = LAMBDA;
      dict->bindings[idx]->val.lambda = dupl_val(val);
      return EXIT_SUCCESS;
    }

    idx = (idx + 1) % dict->size;

    if (idx == orig_idx)
    {
      fprintf(stderr, "%s. : Cannot insert to an already full dictionary.\n", strerror(ECANCELED));
      exit(EXIT_FAILURE);
    }
  }

  Binding *binding = (Binding *)malloc(sizeof(Binding));
  if (binding == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  *(char **)&binding->key = strdup(key);
  binding->type = LAMBDA;
  binding->val.lambda = dupl_val(val);

  *(Binding **)&dict->bindings[idx] = binding;

  return EXIT_SUCCESS;
}
static size_t insert_dbl(const Dict *const dict, const char *const key, const double val)
{
  assert(dict != NULL && key != NULL && val != NULL);

  size_t idx = hash_by_div(key, dict->size);
  const size_t orig_idx = idx;
  while (dict->bindings[idx] != NULL)
  {
    if (strncmp(dict->bindings[idx]->key, key, strlen(key)) == 0)
    {
      assert(dict->bindings[idx]->type != NULL);

      if (dict->bindings[idx]->type == STRING)
      {
        free_val(dict->bindings[idx]->val.s);
      }
      else if (dict->bindings[idx]->type == TAU)
      {
        free_val(dict->bindings[idx]->val.tau);
      }
      else if (dict->bindings[idx]->type == LAMBDA)
      {
        free_val(dict->bindings[idx]->val.lambda);
      }
      memset(&dict->bindings[idx]->val, 0, sizeof(dict->bindings[idx]->val));

      dict->bindings[idx]->type = DOUBLE;
      dict->bindings[idx]->val.d = val;
      return EXIT_SUCCESS;
    }

    idx = (idx + 1) % dict->size;

    if (idx == orig_idx)
    {
      fprintf(stderr, "%s. : Cannot insert to an already full dictionary.\n", strerror(ECANCELED));
      exit(EXIT_FAILURE);
    }
  }

  Binding *binding = (Binding *)malloc(sizeof(Binding));
  if (binding == NULL)
  {
    fprintf(stderr, "%s.\n", strerror(ENOMEM));
    exit(EXIT_FAILURE);
  }
  *(char **)&binding->key = strdup(key);
  binding->type = DOUBLE;
  binding->val.d = val;

  *(Binding **)&dict->bindings[idx] = binding;

  return EXIT_SUCCESS;
}

Binding *search(const Dict *const dict, const char *const key)
{
  assert(dict != NULL && key != NULL);

  size_t idx = hash_by_div(key, dict->size);
  const size_t orig_idx = idx;
  while (dict->bindings[idx] != NULL)
  {
    if (strncmp(dict->bindings[idx]->key, key, strlen(key)) == 0)
    {
      return dupl_binding(dict->bindings[idx]);
    }

    idx = (idx + 1) % dict->size;

    if (idx == orig_idx)
    {
      break;
    }
  }

  return NULL;
}

Dict *merge_dicts(const Dict *const dict1, const Dict *const dict2)
{
  if (dict1 == NULL && dict2 == NULL)
  {
    return NULL;
  }
  else if (dict1 == NULL)
  {
    return dupl_dict(dict2);
  }
  else if (dict2 == NULL)
  {
    return dupl_dict(dict1);
  }
  else
  {
    Dict *merged = init_dictionary(dict1->size + dict2->size);

    for (size_t i = 0; i < dict1->size; ++i)
    {
      if (dict1->bindings[i] != NULL)
      {
        insert(merged, dict1->bindings[i]->key, dict1->bindings[i]->val);
      }
    }

    for (size_t i = 0; i < dict2->size; ++i)
    {
      if (dict2->bindings[i] != NULL)
      {
        insert(merged, dict2->bindings[i]->key, dict2->bindings[i]->val);
      }
    }

    return merged;
  }
}

Binding *dupl_binding(const Binding *const binding)
{
  if (binding == NULL)
  {
    return NULL;
  }
  else
  {
    assert(binding->key != NULL && binding->val != NULL);

    Binding *dupl = (Binding *)malloc(sizeof(Binding));
    if (dupl == NULL)
    {
      fprintf(stderr, "%s.\n", strerror(ENOMEM));
      exit(EXIT_FAILURE);
    }

    *(char **)&dupl->key = strdup(binding->key);
    dupl->type = binding->type;

    if (binding->type == STRING)
    {
      dupl->val.s = dupl_val(binding->val.s);
    }
    else if (binding->type == INTEGER)
    {
      dupl->val.i = binding->val.i;
    }
    else if (binding->type == TAU)
    {
      dupl->val.tau = dupl_val(binding->val.tau);
    }
    else if (binding->type == LAMBDA)
    {
      dupl->val.lambda = dupl_val(binding->val.lambda);
    }
    else if (binding->type == DOUBLE)
    {
      dupl->val.d = binding->val.d;
    }

    return dupl;
  }
}
Dict *dupl_dict(const Dict *const dict)
{
  if (dict == NULL)
  {
    return NULL;
  }
  else
  {
    assert(dict->bindings != NULL);
    
    Dict *dupl = init_dictionary(dict->size);

    for (size_t i = 0; i < dict->size; ++i)
    {
      if (dict->bindings[i] != NULL)
      {
        insert(dupl, dict->bindings[i]->key, dict->bindings[i]->val);
      }
    }

    return dupl;
  }
}

static void free_str(char *const s)
{
  free((void *)s);
}
static void free_binding(Binding *const binding)
{
  if (binding == NULL)
  {
    return;
  }
  else
  {
    assert(binding->key != NULL && binding->val != NULL);

    free((void *)binding->key);
    memset(&binding->key, 0, sizeof(binding->key));

    if (binding->type == STRING)
    {
      free_val(binding->val.s);
    }
    else if (binding->type == TAU)
    {
      free_val(binding->val.tau);
    }
    else if (binding->type == LAMBDA)
    {
      free_val(binding->val.lambda);
    }
    memset(&binding->val, 0, sizeof(binding->val));

    binding->type = 0;

    free((void *)binding);
  }
}
void free_dict(Dict *const dict)
{
  if (dict == NULL)
  {
    return;
  }
  else
  {
    assert(dict->bindings != NULL);

    for (size_t i = 0; i < dict->size; ++i)
    {
      free_binding((void *)dict->bindings[i]);
      memset(&dict->bindings[i], 0, sizeof(dict->bindings[i]));
    }

    free((void *)dict->bindings);
    memset(&dict->bindings, 0, sizeof(dict->bindings));

    free(dict);
  }
}
