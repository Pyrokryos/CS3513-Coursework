#include "../include/lexer.h"
#include "../include/linked_list.h"

// Create a new linked list.
LinkedList* create_linked_list() {
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (list == NULL)
    {
        perror("Failed to allocate memory for linked list.");
        exit(EXIT_FAILURE);
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return list;
}

// Insert a token at the end of the linked list.
void insert_at_end(LinkedList* list, struct Token *token) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL)
    {
        perror("Failed to allocate memory for new node.");
        exit(EXIT_FAILURE);
    }

    node->token = token;
    node->next = NULL;

    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
}

// Display the contents of the linked list.
void display_list(LinkedList* list) {
    Node *current = list->head;

    while (current != NULL) {
        if (current->token->type == INTEGER) {
            printf("<%s %d>\n", token_type_to_string(current->token->type), current->token->value.i);
        } else {
            printf("<%s %s>\n", token_type_to_string(current->token->type), current->token->value.s);
        }

        current = current->next;
    }
}