#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "linked_list.h"
#include "queue.h"
#include "tree.h"

static Node* curr;
static Queue* queue;

Vertex* parse(TokenStream* stream);
void AST(Vertex* vertex);

/* Expressions */
static void E();
static void Ew();

/* Tuple Expressions */
static void T();
static void Ta();
static void Tc();

/* Boolean Expressions */
static void B();
static void Bt();
static void Bs();
static void Bp();

/* Arithmetic Expressions */
static void A();
static void At();
static void Af();
static void Ap();

/* Rators And Rands */
static void R();
static void Rn();

/* Definitions */
static void D();
static void Da();
static void Dr();
static void Db();

/* Variables */
static void Vb();
static void Vl();

#endif
