#include "../include/lexer.h"

static regex_t* digit_regex;
static regex_t* letter_regex;
static regex_t* punctuation_regex;
static regex_t* whitespace_regex;
static regex_t* comment_regex;
static regex_t* keyword_regex;
static regex_t* identifier_regex;
static regex_t* integer_regex;
static regex_t* operator_regex;
static regex_t* string_regex;

TokenStream *lex(char *input) {
    TokenStream *stream = create_linked_list();

    while (input != NULL && *input != '\0') {
        if (is_whitespace(input)) {
            input = ignore_whitespace(input);
        } else if (strncmp(input, "//", 2) == 0) {
            input = ignore_comment(input);
        } else if (is_digit(input)) {
            input = identify_integer(input, stream);
        } else if (is_letter(input)) {
            input = identify_identifier(input, stream);

            if (keyword_length(stream->tail->token->value.s) == strlen(stream->tail->token->value.s)) {
                stream->tail->token->type = KEYWORD;
            }
        } else if (is_operator(input)) {
            input = identify_operator(input, stream);
        } else if (is_punctuation(input)) {
            input = identify_punctuation(input, stream);
        } else if (*input == '\'') {
            input = identify_string(input, stream);
        } else {
            fprintf(stderr, "Unrecognised token: %c\n", *input);
            exit(EXIT_FAILURE);
        }
    }

    free_regex();
    return stream;
}

const char *token_type_to_string(size_t type) {
    assert(type >= 0 && type <= 5);
    switch (type) {
        case IDENTIFIER: return "IDENTIFIER";
        case INTEGER: return "INTEGER";
        case KEYWORD: return "KEYWORD";
        case OPERATOR: return "OPERATOR";
        case PUNCTUATION: return "PUNCTUATION";
        case STRING: return "STRING";
    }
}

static regex_t *compile_regex(const char *pattern) {
    regex_t *regex = (regex_t *)malloc(sizeof(regex_t));

    if (regex == NULL) {
        perror("Failed to allocate memory for regex");
        exit(EXIT_FAILURE);
    } else {
        int reti = regcomp(regex, pattern, REG_EXTENDED);

        if (reti != _REG_NOERROR) {
            size_t length = regerror(reti, regex, NULL, 0);
            char *buffer = (char *)malloc(length);
            (void)regerror(reti, regex, buffer, length);

            fprintf(stderr, "%s\n", buffer);
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    return regex;
}

static bool does_match(const regex_t *regex, const char *input) {
    regmatch_t match;
    int reti = regexec(regex, input, 1, &match, 0);

    return reti == _REG_NOERROR && match.rm_so == 0;
}

static Match *match_regex(const regex_t *regex, const char *input) {
    regmatch_t match;
    int reti = regexec(regex, input, 1, &match, 0);

    if (reti == _REG_NOERROR && match.rm_so == 0) {
        Match *result = (Match *)malloc(sizeof(Match));

        if (result == NULL) {
            perror("Failed to allocate memory for match");
            exit(EXIT_FAILURE);
        } else {
            char *value = (char *)malloc(match.rm_eo - match.rm_so + 1);

            if (value == NULL) {
                perror("Failed to allocate memory for value");
                exit(EXIT_FAILURE);
            } else {
                strncpy(value, input, match.rm_eo - match.rm_so);
                value[match.rm_eo - match.rm_so] = '\0';

                Match temp = { .value=value, .start=match.rm_so, .end=match.rm_eo - 1 };

                memcpy(result, &temp, sizeof(Match));

                return result;
            }
        }
    } else if (reti == REG_NOMATCH || match.rm_so != 0) {
        return NULL;
    } else {
        size_t length = regerror(reti, whitespace_regex, NULL, 0);
        char *buffer = (char *)malloc(length);
        (void)regerror(reti, whitespace_regex, buffer, length);

        fprintf(stderr, "%s\n", buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }
}

static bool is_digit(char *input) {
    if (digit_regex == NULL) {
        digit_regex = compile_regex(DIGIT_REGEX);
    }

    return does_match(digit_regex, &input[0]);
}

static bool is_letter(char *input) {
    if (letter_regex == NULL) {
        letter_regex = compile_regex(LETTER_REGEX);
    }

    return does_match(letter_regex, &input[0]);
}

static bool is_operator(char *input) {
    if (operator_regex == NULL) {
        operator_regex = compile_regex(OPERATOR_REGEX);
    }

    return does_match(operator_regex, &input[0]);
}

static bool is_punctuation(char *input) {
    if (punctuation_regex == NULL) {
        punctuation_regex = compile_regex(PUNCTUATION_REGEX);
    }

    return does_match(punctuation_regex, &input[0]);
}

static bool is_whitespace(char *input) {
    if (whitespace_regex == NULL) {
        whitespace_regex = compile_regex(WHITESPACE_REGEX);
    }

    return does_match(whitespace_regex, &input[0]);
}

static char *ignore_whitespace(char *input) {
    if (whitespace_regex == NULL) {
        whitespace_regex = compile_regex(WHITESPACE_REGEX);
    }
    Match *match = match_regex(whitespace_regex, input);

    return (match != NULL) ? input + match->end + 1 : input;
}

static char *ignore_comment(char *input) {
    if (comment_regex == NULL) {
        comment_regex = compile_regex(COMMENT_REGEX);
    }
    Match *match = match_regex(comment_regex, input);

	return (match != NULL) ? input + match->end + 1 : input;
}

static size_t keyword_length(char *input) {
    if (keyword_regex == NULL) {
        keyword_regex = compile_regex(KEYWORD_REGEX);
    }
    Match *match = match_regex(keyword_regex, input);

    return (match != NULL) ? match->end - match->start + 1 : 0;
}

static char* identify_identifier(char* input, TokenStream* stream) {
    if (identifier_regex == NULL) {
        identifier_regex = compile_regex(IDENTIFIER_REGEX);
    }
    Match *match = match_regex(identifier_regex, input);

    if (match == NULL) {
        free(match->value);
        free(match);
        return input;
    } else if (match != NULL) {
        Token *token = (Token *)malloc(sizeof(Token));

        if (token == NULL) {
            perror("Failed to allocate memory for token");
            exit(EXIT_FAILURE);
        } else {
            Token temp = { .type=IDENTIFIER, .value.s=match->value };
            memcpy(token, &temp, sizeof(Token));

            insert_at_end(stream, token);

            free(match);
            return input + match->end + 1;
        }
    }
}

static char* identify_integer(char* input, TokenStream* stream) {
    if (integer_regex == NULL) {
        integer_regex = compile_regex(INTEGER_REGEX);
    }
    Match *match = match_regex(integer_regex, input);

    if (match == NULL) {
        free(match->value);
        free(match);
        return input;
    } else if (match != NULL) {
        Token *token = (Token *)malloc(sizeof(Token));

        if (token == NULL) {
            perror("Failed to allocate memory for token");
            exit(EXIT_FAILURE);
        } else {
            Token temp = { .type=INTEGER, .value.i=atoi((const char *) (match->value)) };
            memcpy(token, &temp, sizeof(Token));

            insert_at_end(stream, token);

            free(match->value);
            free(match);
            return input + match->end + 1;
        }
    }
}

static char* identify_operator(char* input, TokenStream* stream) {
    if (operator_regex == NULL) {
        operator_regex = compile_regex(OPERATOR_REGEX);
    }
    Match *match = match_regex(operator_regex, input);

    if (match != NULL) {
        Token *token = (Token *)malloc(sizeof(Token));

        if (token == NULL) {
            perror("Failed to allocate memory for token");
            exit(EXIT_FAILURE);
        } else {
            Token temp = { .type=OPERATOR, .value.s=match->value };
            memcpy(token, &temp, sizeof(Token));

            insert_at_end(stream, token);

            free(match);
            return input + match->end + 1;
        }
    }
    return input;
}

static char* identify_punctuation(char* input, TokenStream* stream) {
    if (punctuation_regex == NULL) {
        punctuation_regex = compile_regex(PUNCTUATION_REGEX);
    }
    Match *match = match_regex(punctuation_regex, input);

    if (match == NULL) {
        free(match->value);
        free(match);
        return input;
    } else if (match != NULL) {
        Token *token = (Token *)malloc(sizeof(Token));

        if (token == NULL) {
            perror("Failed to allocate memory for token");
            exit(EXIT_FAILURE);
        } else {
            Token temp = { .type=PUNCTUATION, .value.s=match->value };
            memcpy(token, &temp, sizeof(Token));

            insert_at_end(stream, token);

            free(match);
            return input + match->end + 1;
        }
    }
}

static char* identify_string(char* input, TokenStream* stream) {
    if (string_regex == NULL) {
        string_regex = compile_regex(STRING_REGEX);
    }
    Match *match = match_regex(string_regex, input);

    if (match == NULL) {
        free(match->value);
        free(match);
        return input;
    } else if (match != NULL) {
        Token *token = (Token *)malloc(sizeof(Token));

        if (token == NULL) {
            perror("Failed to allocate memory for token");
            exit(EXIT_FAILURE);
        } else {
            Token temp = { .type=STRING, .value.s=match->value };
            memcpy(token, &temp, sizeof(Token));

            insert_at_end(stream, token);

            free(match);
            return input + match->end + 1;
        }
    }
}

static void free_regex() {
    if (digit_regex != NULL) {
        regfree(digit_regex);
    }

    if (letter_regex != NULL) {
        regfree(letter_regex);
    }

    if (punctuation_regex != NULL) {
        regfree(punctuation_regex);
    }

    if (whitespace_regex != NULL) {
        regfree(whitespace_regex);
    }

    if (comment_regex != NULL) {
        regfree(comment_regex);
    }

    if (identifier_regex != NULL) {
        regfree(identifier_regex);
    }

    if (integer_regex != NULL) {
        regfree(integer_regex);
    }

    if (operator_regex != NULL) {
        regfree(operator_regex);
    }

    if (string_regex != NULL) {
        regfree(string_regex);
    }
};
