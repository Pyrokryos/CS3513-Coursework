#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct Vertex Vertex;

typedef struct Item {
    Vertex* vertex;
    struct Item* next;
} Item;

typedef struct Queue {
    Item* head;
    Item* tail;
} Queue;

Queue* create_empty_queue();

size_t get_size(Queue* queue);
bool is_empty(Queue* queue);

void enqueue(Queue* queue, Vertex* vertex);
Vertex* dequeue(Queue* queue);

void free_queue(Queue* queue);

#endif
