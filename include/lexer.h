#ifndef LEXER_H
#define LEXER_H

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/linked_list.h"

#define DIGIT_REGEX "[0-9]"
#define LETTER_REGEX "[a-zA-Z]"
#define PUNCTUATION_REGEX "[();,]"
#define WHITESPACE_REGEX "[ \t\n]+"
#define COMMENT_REGEX "//[]'();,\\ \ta-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\n"
#define IDENTIFIER_REGEX "[a-zA-Z][a-zA-Z0-9_]*"
#define NUMBER_REGEX "[0-9]+"
#define OPERATOR_REGEX "[]+-\*<>&.@/:=~|$!#%^_[{}\"`?]"
#define STRING_REGEX "\"[][.\t.][.\n.][.\\.][.\".]();, a-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\""

typedef struct LinkedList TokenStream;

enum TokenType {
    DELETE,
    IDENTIFIER,
    INTEGER,
    OPERATOR,
    STRING,
    PUNCTUATION
};

struct Token {
    char *value;
    enum TokenType type;
};

const char* token_type_to_string(enum TokenType type);

bool is_digit(char* input);
bool is_letter(char* input);
bool is_punctuation(char* input);
bool is_whitespace(char* input);

char* ignore_whitespace(char *input);
char* ignore_comment(char *input);

#endif /* LEXER_H */
