#include "../include/tree.h"

// Function to create a new vertex.
Vertex* create_vertex(char* data) {
    Vertex* vertex = (Vertex*) malloc(sizeof(Vertex));

    vertex->data = data;
    vertex->left_child = NULL;
    vertex->right_sibling = NULL;

    return vertex;
}

void add_left_child(Vertex* parent, Vertex* left_child) {
    parent->left_child = left_child;
}

Vertex* get_left_child(Vertex* parent) {
    return parent->left_child;
}

void add_right_sibling(Vertex* vertex, Vertex* right_sibling) {
    vertex->right_sibling = right_sibling;
}

Vertex* get_right_sibling(Vertex* vertex) {
    return vertex->right_sibling;
}
