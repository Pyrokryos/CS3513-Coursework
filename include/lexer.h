#ifndef LEXER_H
#define LEXER_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

// Define the regex patterns as constants
#define DIGIT_REGEX "[0-9]"
#define LETTER_REGEX "[a-zA-Z]"
#define PUNCTUATION_REGEX "[();,]"
#define WHITESPACE_REGEX "[ \t\n]+"
#define COMMENT_REGEX "//[]'();,\\ \ta-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\n"
#define IDENTIFIER_REGEX "[a-zA-Z][a-zA-Z0-9_]*"
#define NUMBER_REGEX "[0-9]+"
#define OPERATOR_REGEX "[]+-\*<>&.@/:=~|$!#%^_[{}\"`?]"
#define STRING_REGEX "\"[][.\t.][.\n.][.\\.][.\".]();, a-zA-Z0-9+-\\*<>&.@/:=~|$!#%^_[{}\"`?]*\""

char* match_comments(char *input);

#endif /* LEXER_H */
