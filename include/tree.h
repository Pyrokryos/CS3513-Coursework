#ifndef TREE_H
#define TREE_H

#include <stdlib.h>

typedef struct Vertex {
    char* data;
    struct Vertex *left_child;
    struct Vertex *right_sibling;
} Vertex;

Vertex* create_vertex(char* data);

void add_left_child(Vertex* parent, Vertex* left_child);
Vertex* get_left_child(Vertex* parent);

void add_right_sibling(Vertex* vertex, Vertex* right_sibling);
Vertex* get_right_sibling(Vertex* vertex);

#endif
