#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vertex {
    char *data;
    struct Vertex *left_child;
    struct Vertex *right_sibling;
} Vertex;

Vertex *create_vertex(char *data);

void add_left_child(Vertex *parent, Vertex *left_child);
Vertex *get_left_child(Vertex *parent);

void add_right_sibling(Vertex *vertex, Vertex *right_sibling);
Vertex *get_right_sibling(Vertex *vertex);

static void free_vertex(Vertex *vertex);
static void free_subtree(Vertex *vertex);

void depth_first_left_to_right_traversal(Vertex *vertex, size_t depth);

static Vertex *clone_vertex(Vertex *vertex);
static Vertex *clone_subtree(Vertex *root);
Vertex *standardize(Vertex *vertex);
