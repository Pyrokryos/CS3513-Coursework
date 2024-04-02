#include "../include/queue.h"

// Function to create a new queue.
Queue* create_empty_queue() {
    Queue* queue = (Queue*) malloc(sizeof(Queue));

    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

// Function to get the size of the queue.
size_t get_size(Queue* queue) {
    size_t size = 0;
    Item* item = queue->head;

    while (item != NULL) {
        size++;
        item = item->next;
    }

    return size;
}

// Function to check if the queue is empty.
bool is_empty(Queue* queue) {
    return queue->head == NULL;
}

// Function to add a new item to the queue.
void enqueue(Queue* queue, Vertex* vertex) {
    Item* item = (Item*) malloc(sizeof(Item));

    item->vertex = vertex;
    item->next = NULL;

    if (queue->head == NULL) {
        queue->head = item;
        queue->tail = item;
    } else {
        queue->tail->next = item;
        queue->tail = item;
    }

}

// Function to remove an item from the queue.
Vertex* dequeue(Queue* queue) {
    if (queue->head == NULL) {
        return NULL;
    }

    Item* item = queue->head;
    Vertex* vertex = item->vertex;

    queue->head = item->next;

    free(item);

    return vertex;
}

// Function to free the memory held by queue and its items.
void free_queue(Queue* queue) {
    Item* item = queue->head;

    while (item != NULL) {
        Item* next = item->next;
        free(item);
        item = next;
    }

    free(queue);
}
