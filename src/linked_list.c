#include "../include/lexer.h"
#include "../include/linked_list.h"

// Create a new linked list.
struct LinkedList* create_linked_list()
{
    struct LinkedList *list = (struct LinkedList *)malloc(sizeof(struct LinkedList));
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
void insert_at_end(struct LinkedList* list, struct Token *token)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
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

void display_list(struct LinkedList* list)
{
    struct Node *current = list->head;
    while (current != NULL)
    {
        printf("<%s %s>\n", token_type_to_string(current->token->type), current->token->value);
        current = current->next;
    }
}
