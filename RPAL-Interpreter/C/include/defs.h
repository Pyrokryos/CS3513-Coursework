#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define NONE 40
#define YSTAR 41

typedef struct Match
{
  const char *const value;
  const size_t start;
  const size_t end;
} Match;

typedef enum TokenType
{
  IDENTIFIER = 0,
  INTEGER,
  KEYWORD,
  OPERATOR,
  PUNCTUATION,
  STRING
} TokenType;

typedef struct Token
{
  TokenType type;
  union
  {
    int i;
    const char *const s;
  } value;
} Token;

typedef struct Node
{
  Token *token;
  struct Node *next;
} Node;

typedef struct LinkedList
{
  Node *head;
  Node *tail;
  size_t size;
} LinkedList;

// Linked list of tokens
typedef LinkedList TokenStream;

typedef enum PhraseType
{
  E_LET = 6,
  E_LAMBDA,
  E_WHERE,

  T_TAU = 9,
  T_AUG,
  T_COND,

  B_OR = 12,
  B_AND,
  B_NOT,
  B_GR,
  B_GE,
  B_LS,
  B_LE,
  B_EQ,
  B_NE,

  A_ADD = 21,
  A_SUB,
  A_NEG,
  A_MUL,
  A_DIV,
  A_EXP,
  A_AT,

  R_GAMMA = 28,
  R_TRUE,
  R_FALSE,
  R_NIL,
  R_DUMMY,

  D_WITHIN = 33,
  D_AND,
  D_REC,
  D_EQ,
  D_FCN,

  V_BRACKET = 38,
  V_COMMA
} PhraseType;

typedef struct Vertex
{
  size_t type;
  Token *token;
  struct Vertex *left_child;
  struct Vertex *right_sibling;
} Vertex;

const char *token_type_to_string(size_t type);
const char *phrase_type_to_string(size_t type);
