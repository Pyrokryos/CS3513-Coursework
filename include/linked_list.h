#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Token;

struct Node {
    struct Token *token;
    struct Node *next;
};

struct LinkedList {
    struct Node *head;
    struct Node *tail;
    size_t size;
};

struct LinkedList* create_linked_list();

void insert_at_end(struct LinkedList* list, struct Token *token);

void display_list(struct LinkedList* list);

#endif // LINKED_LIST_H
