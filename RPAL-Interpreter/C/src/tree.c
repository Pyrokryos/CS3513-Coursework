#include "../include/tree.h"

// Function to create a new vertex.
Vertex* create_vertex(PhraseType type, Token *token) {
    Vertex* vertex = (Vertex *) malloc(sizeof(Vertex));
    if (vertex == NULL) {
        perror("Failed to allocate memory for vertex.\n");
        exit(EXIT_FAILURE);
    }

    if (type != NONE) {
        vertex->type = type;
        vertex->token = NULL;
    } else {
        vertex->type = NONE;
        vertex->token = token;
    }

    vertex->left_child = NULL;
    vertex->right_sibling = NULL;

    return vertex;
}

void add_left_child(Vertex* parent, Vertex* left_child) {
    parent->left_child = left_child;
}

Vertex *get_left_child(Vertex *parent) {
    return (struct Vertex *)parent->left_child;
}

void add_right_sibling(Vertex *vertex, Vertex *right_sibling) {
    vertex->right_sibling = right_sibling;
}

Vertex* get_right_sibling(Vertex* vertex) {
    return vertex->right_sibling;
}

// static void free_vertex(Vertex *vertex) {
//     if (vertex != NULL) {
//         if (vertex->data != NULL) {
//             free(vertex->data);
//         }
//         free(vertex);
//     }
// }

// static void free_subtree(Vertex *vertex) {
//     if (vertex != NULL) {
//         if (vertex->left_child != NULL) {
//             free_subtree(vertex->left_child);
//             free_subtree(vertex->left_child->right_sibling);
//         }
//         free_vertex(vertex);
//     }
// }

void depth_first_left_to_right_traversal(Vertex* vertex, size_t depth) {
    if (vertex != NULL) {
        for (int i = 0; i < depth; i++) {
            printf(".");
        }
        
        if (vertex->type != NONE) {
            printf("%s\n", phrase_type_to_string(vertex->type));
        } else {
            if (vertex->token->type == IDENTIFIER) {
                printf("<ID:%s>\n", vertex->token->value);
            } else if (vertex->token->type == INTEGER) {
                printf("<INT:%s>\n", vertex->token->value);
            } else if (vertex->token->type == STRING) {
                printf("<STR:%s>\n", vertex->token->value);
            }
        }

        depth_first_left_to_right_traversal(vertex->left_child, depth + 1);
        depth_first_left_to_right_traversal(vertex->right_sibling, depth);
    }
}

// static Vertex *clone_vertex(Vertex *vertex) {
//     if (vertex == NULL) {
//         return NULL;
//     } else {
//         char *data = strdup(vertex->data);
//         return create_vertex(data);
//     }
// }

// static Vertex *clone_subtree(Vertex *root) {
//     if (root == NULL) {
//         return NULL;
//     } else {
//         char *data = strdup(root->data);
//         Vertex *cloned_root = create_vertex(data);

//         cloned_root->left_child = clone_subtree(root->left_child);

//         if (cloned_root->left_child != NULL) {
//             Vertex *t1 = root->left_child->right_sibling;
//             Vertex *t2 = cloned_root->left_child;
//             while (t1 != NULL) {
//                 t2->right_sibling = clone_subtree(t1);

//                 t2 = t2->right_sibling;
//                 t1 = t1->right_sibling;
//             }
//         }
//         return cloned_root;
//     }
// }

// Vertex *standardize(Vertex *vertex) {
//     Vertex *duplicate = clone_vertex(vertex);

//     if (vertex->left_child != NULL) {
//         duplicate->left_child = standardize(vertex->left_child);

//         Vertex *t1 = vertex->left_child->right_sibling;
//         Vertex *t2 = duplicate->left_child;
//         while (t1 != NULL) {
//             t2->right_sibling = standardize(t1);

//             t2 = t2->right_sibling;
//             t1 = t1->right_sibling;
//         }
//     }

//     Vertex *gamma, *lambda;
//     if (
//         strncmp(duplicate->data, "let", 3) == 0 &&
//         strncmp(duplicate->left_child->data, "=", 1) == 0
//     ) {
//         gamma = create_vertex("gamma");
//         lambda = create_vertex("lambda");

//         gamma->left_child = lambda;
//         gamma->left_child->right_sibling = clone_subtree(duplicate->left_child->left_child->right_sibling);

//         lambda->left_child = clone_subtree(duplicate->left_child->left_child);
//         lambda->left_child->right_sibling = clone_subtree(duplicate->left_child->right_sibling);

//         free_subtree(duplicate);

//         return gamma;
//     } else if (
//         strncmp(duplicate->data, "where", 5) == 0 &&
//         strncmp(duplicate->left_child->right_sibling->data, "=", 1) == 0
//     ) {
//         gamma = create_vertex("gamma");
//         lambda = create_vertex("lambda");

//         gamma->left_child = lambda;
//         gamma->left_child->right_sibling = clone_subtree(duplicate->left_child->right_sibling->left_child->right_sibling);

//         lambda->left_child = clone_subtree(duplicate->left_child->right_sibling->left_child);
//         lambda->left_child->right_sibling = clone_subtree(duplicate->left_child);

//         free_subtree(duplicate);

//         return gamma;
//     } else if (
//         strncmp(duplicate->data, "within", 6) == 0 &&
//         strncmp(duplicate->left_child->data, "=", 1) == 0 &&
//         strncmp(duplicate->left_child->right_sibling->data, "=", 1) == 0
//     ) {
//         gamma = create_vertex("gamma");
//         lambda = create_vertex("lambda");

//         Vertex *eq = create_vertex("=");
//         eq->left_child = clone_subtree(duplicate->left_child->right_sibling->left_child);
//         eq->left_child->right_sibling = gamma;

//         gamma->left_child = lambda;
//         gamma->left_child->right_sibling = clone_subtree(duplicate->left_child->left_child->right_sibling);

//         lambda->left_child = clone_subtree(duplicate->left_child->left_child);
//         lambda->left_child->right_sibling = clone_subtree(duplicate->left_child->right_sibling->left_child->right_sibling);

//         free_subtree(duplicate);

//         return eq;
//     } else if (
//         strncmp(duplicate->data, "rec", 3) == 0 &&
//         strncmp(duplicate->left_child->data, "=", 1) == 0
//     ) {
//         gamma = create_vertex("gamma");
//         lambda = create_vertex("lambda");

//         Vertex *eq = create_vertex("=");
//         eq->left_child = clone_subtree(duplicate->left_child->left_child);
//         eq->left_child->right_sibling = gamma;

//         gamma->left_child = create_vertex("Ystar");
//         gamma->left_child->right_sibling = lambda;

//         lambda->left_child = clone_subtree(duplicate->left_child->left_child);
//         lambda->left_child->right_sibling = clone_subtree(duplicate->left_child->left_child->right_sibling);

//         free_subtree(duplicate);

//         return eq;
//     } else if (
//         strncmp(duplicate->data, "fcn_form", 8) == 0
//     ) {
//         lambda = create_vertex("lambda");

//         Vertex *eq = create_vertex("=");
//         eq->left_child = clone_subtree(duplicate->left_child);
//         eq->left_child->right_sibling = lambda;

//         lambda->left_child = clone_subtree(duplicate->left_child->right_sibling);

//         Vertex *t1 = duplicate->left_child->right_sibling->right_sibling;
//         Vertex *t2 = lambda->left_child;
//         while (t1 != NULL) {
//             t2->right_sibling = clone_subtree(t1);

//             t1 = t1->right_sibling;
//             t2 = t2->right_sibling;
//         }

//         free_subtree(duplicate);

//         return eq;
//     } else if (strncmp(duplicate->data, "@", 1) == 0) {
//         Vertex *g1 = create_vertex("gamma");
//         Vertex *g2 = create_vertex("gamma");

//         g1->left_child = g2;
//         g1->left_child->right_sibling = clone_subtree(duplicate->left_child->right_sibling->right_sibling);

//         g2->left_child = clone_subtree(duplicate->left_child->right_sibling);
//         g2->left_child->right_sibling = clone_subtree(duplicate->left_child);

//         free_subtree(duplicate);

//         return g1;
//     } else {
//         return duplicate;
//     }
// }
