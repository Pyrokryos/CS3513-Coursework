#include "../include/tree.h"

void AST(Vertex *vertex)
{
	depth_first_left_to_right_traversal(vertex, 0);
	printf("\n");
}

void ST(Vertex *vertex)
{
	depth_first_left_to_right_traversal(standardize(vertex), 0);
	printf("\n");
}

// Function to create a new vertex.
Vertex *create_vertex(size_t type, Token *token)
{
	Vertex *vertex = (Vertex *)malloc(sizeof(Vertex));
	if (vertex == NULL)
	{
		perror("Failed to allocate memory for vertex.\n");
		exit(EXIT_FAILURE);
	}

	if (type != NONE)
	{
		vertex->type = type;
		vertex->token = NULL;
	}
	else
	{
		vertex->type = type;
		vertex->token = token;
	}

	vertex->left_child = NULL;
	vertex->right_sibling = NULL;

	return vertex;
}

void add_left_child(Vertex *parent, Vertex *left_child)
{
	parent->left_child = left_child;
}

Vertex *get_left_child(Vertex *parent)
{
	return (struct Vertex *)parent->left_child;
}

void add_right_sibling(Vertex *vertex, Vertex *right_sibling)
{
	vertex->right_sibling = right_sibling;
}

Vertex *get_right_sibling(Vertex *vertex)
{
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

void depth_first_left_to_right_traversal(Vertex *vertex, size_t depth)
{
	if (vertex != NULL)
	{
		for (int i = 0; i < depth; i++)
		{
			printf(".");
		}

		if (vertex->type != YSTAR && vertex->type != NONE)
		{
			printf("%s\n", phrase_type_to_string(vertex->type));
		}
		else if (vertex->type == YSTAR)
		{
			printf("Y*\n");
		}
		else
		{
			if (vertex->token->type == IDENTIFIER)
			{
				printf("<ID:%s>\n", vertex->token->value.s);
			}
			else if (vertex->token->type == INTEGER)
			{
				printf("<INT:%d>\n", vertex->token->value.i);
			}
			else if (vertex->token->type == STRING)
			{
				printf("<STR:%s>\n", vertex->token->value.s);
			}
		}

		depth_first_left_to_right_traversal(vertex->left_child, depth + 1);
		depth_first_left_to_right_traversal(vertex->right_sibling, depth);
	}
}

static Vertex *clone_vertex(Vertex *vertex)
{
	if (vertex == NULL)
	{
		return NULL;
	}
	else
	{
		if (vertex->type != NONE)
		{
			return create_vertex(vertex->type, NULL);
		}
		else
		{
			Token *token = (Token *)malloc(sizeof(Token));
			if (token == NULL)
			{
				perror("Failed to allocate memory for token.\n");
				exit(EXIT_FAILURE);
			}

			if (vertex->token->type == INTEGER)
			{
				Token temp = {.type = INTEGER, .value.i = vertex->token->value.i};

				memcpy(token, &temp, sizeof(Token));
			}
			else
			{
				Token temp = {.type = vertex->token->type, .value.s = strdup(vertex->token->value.s)};

				memcpy(token, &temp, sizeof(Token));
			}

			return create_vertex(NONE, token);
		}
	}
}

static Vertex *clone_subtree(Vertex *root)
{
	if (root == NULL)
	{
		return NULL;
	}
	else
	{
		Vertex *cloned_root = clone_vertex(root);

		cloned_root->left_child = clone_subtree(root->left_child);

		if (cloned_root->left_child != NULL)
		{
			Vertex *t1 = root->left_child->right_sibling;
			Vertex *t2 = cloned_root->left_child;
			while (t1 != NULL)
			{
				t2->right_sibling = clone_subtree(t1);

				t2 = t2->right_sibling;
				t1 = t1->right_sibling;
			}
		}
		return cloned_root;
	}
}

Vertex *standardize(Vertex *vertex)
{
	if (vertex == NULL)
	{
		return NULL;
	}

	if (vertex->left_child != NULL)
	{
		Vertex *t1 = get_right_sibling(get_left_child(vertex));

		add_left_child(vertex, standardize(vertex->left_child));

		Vertex *t2 = get_left_child(vertex);
		Vertex *t3 = t1;
		while (t3 != NULL)
		{
			t1 = get_right_sibling(t1);

			add_right_sibling(t2, standardize(t3));

			t2 = get_right_sibling(t2);
			t3 = t1;
		}
	}

	Vertex *gamma, *lambda;
	if (
			vertex->type == E_LET &&
			vertex->left_child->type == D_EQ)
	{
		gamma = create_vertex(R_GAMMA, NULL);
		lambda = create_vertex(E_LAMBDA, NULL);

		Vertex *X = get_left_child(get_left_child(vertex));
		Vertex *E = get_right_sibling(get_left_child(get_left_child(vertex)));
		Vertex *P = get_right_sibling(get_left_child(vertex));

		add_right_sibling(X, P);

		add_left_child(lambda, X);
		add_right_sibling(lambda, E);

		add_left_child(gamma, lambda);

		return gamma;
	}
	else if (
			vertex->type == E_WHERE &&
			vertex->left_child->right_sibling->type == D_EQ)
	{
		gamma = create_vertex(R_GAMMA, NULL);
		lambda = create_vertex(E_LAMBDA, NULL);

		Vertex *X = get_left_child(get_right_sibling(get_left_child(vertex)));
		Vertex *E = get_right_sibling(get_left_child(get_right_sibling(get_left_child(vertex))));
		Vertex *P = get_left_child(vertex);

		add_right_sibling(X, P);
		add_right_sibling(P, NULL);

		add_left_child(lambda, X);
		add_right_sibling(lambda, E);

		add_left_child(gamma, lambda);

		return gamma;
	}
	else if (
			vertex->type == D_WITHIN &&
			vertex->left_child->type == D_EQ &&
			vertex->left_child->right_sibling->type == D_EQ)
	{
		gamma = create_vertex(R_GAMMA, NULL);
		lambda = create_vertex(E_LAMBDA, NULL);

		Vertex *eq = create_vertex(D_EQ, NULL);

		Vertex *X1 = get_left_child(get_left_child(vertex));
		Vertex *E1 = get_right_sibling(get_left_child(get_left_child(vertex)));
		Vertex *X2 = get_left_child(get_right_sibling(get_left_child(vertex)));
		Vertex *E2 = get_right_sibling(get_left_child(get_right_sibling(get_left_child(vertex))));

		add_right_sibling(X1, E2);
		add_right_sibling(X2, gamma);

		add_left_child(lambda, X1);
		add_right_sibling(lambda, E1);

		add_left_child(gamma, lambda);

		add_left_child(eq, X2);

		return eq;
	}
	else if (
			vertex->type == D_REC &&
			vertex->left_child->type == D_EQ)
	{
		gamma = create_vertex(R_GAMMA, NULL);
		lambda = create_vertex(E_LAMBDA, NULL);

		Vertex *eq = create_vertex(D_EQ, NULL);
		Vertex *yStar = create_vertex(YSTAR, NULL);
		Vertex *_X = clone_subtree(get_left_child(get_left_child(vertex)));

		Vertex *X = get_left_child(get_left_child(vertex));
		Vertex *E = get_right_sibling(get_left_child(get_left_child(vertex)));

		add_left_child(lambda, X);

		add_right_sibling(yStar, lambda);

		add_left_child(gamma, yStar);

		add_right_sibling(_X, gamma);

		add_left_child(eq, _X);

		return eq;
	}
	else if (vertex->type == D_FCN)
	{
		lambda = create_vertex(E_LAMBDA, NULL);

		Vertex *eq = create_vertex(D_EQ, NULL);
		Vertex *comma = create_vertex(V_COMMA, NULL);
		Vertex *P = get_left_child(vertex);

		Vertex *t1 = get_right_sibling(get_left_child(vertex));
		Vertex *t2 = NULL;
		do
		{
			if (t1->type == V_COMMA)
			{
				if (t2 == NULL)
				{
					add_left_child(comma, get_left_child(t1));
				}
				else
				{
					add_right_sibling(t2, get_left_child(t1));
				}

				t2 = get_left_child(t1);
				while (get_right_sibling(t2) != NULL)
				{
					t2 = get_right_sibling(t2);
				}
			}
			else if (t1->type == NONE && t1->token->type == IDENTIFIER)
			{
				if (t2 == NULL)
				{
					add_left_child(comma, clone_vertex(t1));

					t2 = get_left_child(comma);
				}
				else
				{
					add_right_sibling(t2, clone_vertex(t1));

					t2 = get_right_sibling(t2);
				}
			}

			t1 = get_right_sibling(t1);
		} while (get_right_sibling(t1) != NULL);

		Vertex *E = t1;

		if (get_right_sibling(get_left_child(comma)) == NULL)
		{
			comma = get_left_child(comma);
		}

		add_right_sibling(comma, E);
		add_left_child(lambda, comma);
		add_right_sibling(P, lambda);
		add_left_child(eq, P);

		return eq;
	}
	else if (
			vertex->type == E_LAMBDA &&
			get_right_sibling(get_right_sibling(get_left_child(vertex))) != NULL)
	{
		Vertex *comma = create_vertex(V_COMMA, NULL);

		Vertex *t1 = get_left_child(vertex);
		Vertex *t2 = NULL;
		do
		{
			if (t1->type == V_COMMA)
			{
				if (t2 == NULL)
				{
					add_left_child(comma, get_left_child(t1));
				}
				else
				{
					add_right_sibling(t2, get_left_child(t1));
				}

				t2 = get_left_child(t1);
				while (get_right_sibling(t2) != NULL)
				{
					t2 = get_right_sibling(t2);
				}
			}
			else if (t1->type == NONE && t1->token->type == IDENTIFIER)
			{
				if (t2 == NULL)
				{
					add_left_child(comma, clone_vertex(t1));

					t2 = get_left_child(comma);
				}
				else
				{
					add_right_sibling(t2, clone_vertex(t1));

					t2 = get_right_sibling(t2);
				}
			}

			t1 = get_right_sibling(t1);
		} while (get_right_sibling(t1) != NULL);

		Vertex *E = t1;

		add_right_sibling(comma, E);
		add_left_child(vertex, comma);

		return vertex;
	}
	else if (vertex->type == D_AND)
	{
		Vertex *eq = create_vertex(D_EQ, NULL);
		Vertex *comma = create_vertex(V_COMMA, NULL);
		Vertex *tau = create_vertex(T_TAU, NULL);

		add_left_child(eq, comma);
		add_right_sibling(comma, tau);

		Vertex *t1 = get_left_child(vertex), *t2, *t3;
		while (t1 != NULL)
		{
			if (get_left_child(comma) == NULL)
			{
				add_left_child(comma, get_left_child(t1));
				add_left_child(tau, get_right_sibling(get_left_child(t1)));

				t2 = get_left_child(comma);
				t3 = get_left_child(tau);
			}
			else
			{
				add_right_sibling(t2, get_left_child(t1));
				add_right_sibling(t3, get_right_sibling(get_left_child(t1)));

				t2 = get_right_sibling(t2);
				add_right_sibling(t2, NULL);
				t3 = get_right_sibling(t3);
				add_right_sibling(t3, NULL);
			}

			t1 = get_right_sibling(t1);
		}

		return eq;
	}
	else if (vertex->type == A_AT)
	{
		Vertex *g1 = create_vertex(R_GAMMA, NULL);
		Vertex *g2 = create_vertex(R_GAMMA, NULL);

		Vertex *E1 = get_left_child(vertex);
		Vertex *N = get_right_sibling(get_left_child(vertex));
		Vertex *E2 = get_right_sibling(get_right_sibling(get_left_child(vertex)));

		add_right_sibling(E1, NULL);

		add_right_sibling(N, E1);

		add_left_child(g2, N);

		add_right_sibling(g2, E2);

		add_left_child(g1, g2);

		return g1;
	}
	else
	{
		return vertex;
	}
}
