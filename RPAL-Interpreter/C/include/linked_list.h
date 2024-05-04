#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stdlib.h>

#include "defs.h"

LinkedList *create_linked_list();

void insert_at_end(LinkedList *list, Token *token);

void display_list(LinkedList *list);

#endif // LINKED_LIST_H
