#pragma once

/**
  * @file lexer.h
  * @brief Header file for the lexer module
  *
  * This file contains the declarations of functions and data structures
  * used for lexical analysis in the RPAL interpreter.
  *
  * The lexer module provides functions for tokenizing input strings and
  * identifying different types of tokens such as identifiers, integers,
  * keywords, operators, punctuation, and strings.
  */

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

//Regular expressions
#define DIGIT_REGEX "[0-9]"
#define LETTER_REGEX "[a-zA-Z]"
#define OPERATOR_REGEX "\\+|-|\\*|\\*\\*|>|>=|<|<=|&|\\.|@|/|=|\\||->"
#define PUNCTUATION_REGEX "[();,]"

#define WHITESPACE_REGEX "[ \t\n]+"
#define COMMENT_REGEX "//[]'();,\\ \ta-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\n"

#define IDENTIFIER_REGEX "[a-zA-Z][a-zA-Z0-9_]*"
#define KEYWORD_REGEX "and|aug|dummy|eq|false|fn|ge|gr|in|let|le|ls|ne|nil|not|or|rec|true|where|within"
#define INTEGER_REGEX "[0-9]+"
#define STRING_REGEX "\'[][.\t.][.\n.][.\\.][.\".]();, a-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\'"

// Structure representing a match, containing the matched value and its start and end positions
typedef struct Match {
    char *value;
    size_t start;
    size_t end;
} Match;

// Enumeration of different token types
typedef enum TokenType {
    IDENTIFIER,
    INTEGER,
    KEYWORD,
    OPERATOR,
    PUNCTUATION,
    STRING
} TokenType;

// Structure representing a token, containing its value and type
typedef struct Token {
    char *value;
    TokenType type;
} Token;

// Linked list of tokens
typedef LinkedList TokenStream;

// Lexical analysis function, takes an input string and returns a token stream
TokenStream *lex(char *input);

// Function to convert a token type to a string representation
const char *token_type_to_string(TokenType type);

// Helper functions for regular expression matching
static regex_t *compile_regex(const char *pattern);
static bool does_match(const regex_t *regex, const char *input);
static Match *match_regex(const regex_t *regex, const char *input);

// Helper functions for identifying different types of characters
static bool is_digit(char *input);
static bool is_letter(char *input);
static bool is_operator(char *input);
static bool is_punctuation(char *input);
static bool is_whitespace(char *input);

// Helper functions for ignoring whitespace and comments
static char *ignore_whitespace(char *input);
static char *ignore_comment(char *input);

// Helper function for determining the length of a keyword
static size_t keyword_length(char *input);

// Helper functions for identifying specific token types
static char *identify_identifier(char *input, TokenStream *stream);
static char *identify_integer(char *input, TokenStream *stream);
static char *identify_operator(char *input, TokenStream *stream);
static char *identify_punctuation(char *input, TokenStream *stream);
static char *identify_string(char *input, TokenStream *stream);

// Helper functions for freeing resources
static void free_regex();
