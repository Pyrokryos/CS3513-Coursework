#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct Token Token;

typedef struct Node {
    struct Token *token;
    struct Node *next;
} Node;

typedef struct LinkedList {
    Node *head;
    Node *tail;
    size_t size;
} LinkedList;

LinkedList* create_linked_list();

void insert_at_end(LinkedList* list, Token *token);

void display_list(LinkedList* list);

#endif // LINKED_LIST_H
