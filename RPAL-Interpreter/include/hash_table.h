#pragma once

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOUBLE 47
#define HASH_BASE 257
#define INTEGER 1
#define LAMBDA 46
#define STRING 5
#define TAU 45

#define insert(dict, key, val) \
  _Generic((val),                \
      char *: insert_str,        \
      int: insert_int,           \
      Tau *: insert_tau,         \
      Lambda *: insert_lambda,   \
      double: insert_dbl)(dict, key, val)

#define dupl_val(val) \
  _Generic((val),         \
      char *: strdup,     \
      Tau *: dupl_tau,    \
      Lambda *: dupl_lambda)(val)

#define free_val(val) \
  _Generic((val),         \
      char *: free_str,   \
      Tau *: free_tau,    \
      Lambda *: free_lambda)(val)

typedef struct Tau Tau;
typedef struct Lambda Lambda;

/**
 * @struct Binding
 * @brief Represents a binding stored in a dictionary.
 *
 * This structure stores a name binding in the form of a key-val pair.
 * The key is a constant string that uniquely identifies the binding stored in the dictionary.
 * The val can be of various types, as indicated by the type field.
 * The val union stores the actual val, which can be a string, an integer, a double,
 * a pointer to a Tau structure, or a pointer to a Lambda structure.
 *
 * The type field is not constant because RPAL does not enforce strong typing,
 * allowing the type of the val to change dynamically.
 *
 * The contents of the val union are modifiable, allowing the val to be updated.
 * However, the contents pointed to by Tau and Lambda pointers are considered constant,
 * meaning that while you can change which Tau or Lambda a Binding points to,
 * you cannot modify the internal state of the Tau or Lambda structures through these pointers.
 */
typedef struct Binding
{
  const char *const key;
  size_t type;
  union
  {
    const char *s;
    int i;
    const Tau *tau;
    const Lambda *lambda;
    double d;
  } val;
} Binding;

/**
 * @struct Dict
 * @brief Represents a dictionary.
 *
 * This structure defines a dictionary, which stores a collection of entries in the form of key-val pairs.
 * The size field represents the number of buckets in the dictionary and is constant because the size is
 * determined at the time of creation and does not change.
 *
 * The entries field is a constant pointer to an array of pointers to Binding structures.
 * While the array itself cannot be reallocated or moved, the contents of the array (i.e., the pointers to
 * Binding structures) can be modified, allowing for dynamic updates to the dictionary entries.
 */
typedef struct Dict
{
  const size_t size;
  Binding *const *const bindings;
} Dict;

Dict *init_dict(const size_t size);

static size_t hash_by_div(const char *const key, const size_t dict_size);

size_t insert_str(const Dict *const dict, const char *const key, const char *const val);
size_t insert_int(const Dict *const dict, const char *const key, const int val);
size_t insert_tau(const Dict *const dict, const char *const key, const Tau *const val);
size_t insert_lambda(const Dict *const dict, const char *const key, const Lambda *const val);
size_t insert_dbl(const Dict *const dict, const char *const key, const double val);

Binding *search(const Dict *const dict, const char *const key);

Dict *merge_dicts(const Dict *const dict1, const Dict *const dict2);

Tau *dupl_tau(const Tau *const tau);
Lambda *dupl_lambda(const Lambda *const lambda);
Binding *dupl_binding(const Binding *const binding);
Dict *dupl_dict(const Dict *const dict);

void free_str(char *const s);
void free_tau(Tau *const tau);
void free_lambda(Lambda *const lambda);
void free_binding(Binding *const entry);
void free_dict(Dict *const dict);
