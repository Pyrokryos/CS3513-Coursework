#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Match {
    char *value;
    size_t start;
    size_t end;
} Match;

typedef enum TokenType {
    IDENTIFIER,
    INTEGER,
    KEYWORD,
    OPERATOR,
    PUNCTUATION,
    STRING
} TokenType;

typedef struct Token {
    const char *const value;
    TokenType type;
} Token;

typedef struct Node {
    Token *token;
    struct Node *next;
} Node;

typedef struct LinkedList {
    Node *head;
    Node *tail;
    size_t size;
} LinkedList;

typedef LinkedList TokenStream;

typedef enum PhraseType {
    E_LET,
    E_LAMBDA,
    E_WHERE,
    
    T_TAU,
    T_AUG,
    T_COND,

    B_OR,
    B_AND,
    B_NOT,
    B_GR,
    B_GE,
    B_LS,
    B_LE,
    B_EQ,
    B_NE,

    A_ADD,
    A_SUB,
    A_NEG,
    A_MUL,
    A_DIV,
    A_EXP,
    A_AT,

    R_GAMMA,
    R_TRUE,
    R_FALSE,
    R_NIL,
    R_DUMMY,

    D_WITHIN,
    D_AND,
    D_REC,
    D_EQ,
    D_FCN,

    V_BRACKET,
    V_COMMA,

    Y_STAR,

    NONE
} PhraseType;

typedef struct Vertex {
    PhraseType type;
    Token *token;
    struct Vertex *left_child;
    struct Vertex *right_sibling;
} Vertex;

typedef struct Item {
    Vertex* vertex;
    struct Item* next;
} Item;

typedef struct Queue {
    Item* head;
    Item* tail;
} Queue;

const char *token_type_to_string(TokenType type);
const char *phrase_type_to_string(PhraseType type);
