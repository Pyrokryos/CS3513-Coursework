#ifndef LEXER_H
#define LEXER_H

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

#define DIGIT_REGEX "[0-9]"
#define LETTER_REGEX "[a-zA-Z]"
#define PUNCTUATION_REGEX "[();,]"
#define WHITESPACE_REGEX "[ \t\n]+"
#define COMMENT_REGEX "//[]'();,\\ \ta-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\n"
#define KEYWORD_REGEX "(and |aug |dummy |eq |false |fn |ge |gt |in |let |le |ls |ne |nil |not |or |rec |true |where |within )"
#define IDENTIFIER_REGEX "[a-zA-Z][a-zA-Z0-9_]*"
#define INTEGER_REGEX "[0-9]+"
#define OPERATOR_REGEX "(.|(->)|&|>|(>=)|<|(<=)|\\+|-|\\*|/|(\\*\\*)|@|=)"
#define STRING_REGEX "\"[][.\t.][.\n.][.\\.][.\".]();, a-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\""

typedef LinkedList TokenStream;

typedef enum TokenType {
    IDENTIFIER,
    INTEGER,
    KEYWORD,
    OPERATOR,
    PUNCTUATION,
    STRING
} TokenType;

typedef struct Token {
    char* value;
    TokenType type;
} Token;

TokenStream* lex(char *input);

const char* token_type_to_string(TokenType type);

static bool is_digit(char* input);
static bool is_letter(char* input);
static bool is_punctuation(char* input);
static bool is_whitespace(char* input);

static char* ignore_comment(char* input);
static char* ignore_whitespace(char* input);

static char* identify_keyword(char* input, TokenStream* stream);
static char* identify_identifier(char* input, TokenStream* stream);
static char* identify_integer(char* input, TokenStream* stream);
static char* identify_operator(char* input, TokenStream* stream);
static char* identify_punctuation(char* input, TokenStream* stream);
static char* identify_string(char* input, TokenStream* stream);

static void free_regex();

#endif /* LEXER_H */
