#include "../include/lexer.h"

// Declare global variables to hold compiled regex patterns.
static regex_t *digit_regex = NULL;
static regex_t *letter_regex = NULL;
static regex_t *punctuation_regex = NULL;
static regex_t *whitespace_regex = NULL;
static regex_t *comment_regex = NULL;
static regex_t *identifier_regex = NULL;
static regex_t *number_regex = NULL;
static regex_t *operator_regex = NULL;
static regex_t *string_regex = NULL;

const char* token_type_to_string(enum TokenType type)
{
    switch (type)
    {
        case DELETE: return "DELETE";
        case IDENTIFIER: return "IDENTIFIER";
        case INTEGER: return "INTEGER";
        case OPERATOR: return "OPERATOR";
        case STRING: return "STRING";
        case PUNCTUATION: return "PUNCTUATION";
        default:
            fprintf(stderr, "Unknown token type encountered: %d\n", type);
            exit(EXIT_FAILURE);
    }
}

bool is_digit(char* input)
{
    int reti;
    if (digit_regex == NULL) {
        digit_regex = (regex_t *)malloc(sizeof(regex_t));
        if (digit_regex == NULL) {
            perror("Failed to allocate memory for regex object");
            exit(EXIT_FAILURE);
        }

        reti = regcomp(digit_regex, DIGIT_REGEX, REG_EXTENDED); // Compile the regex pattern.
        if (reti != _REG_NOERROR)
        {
            // If there is an error in compiling the regex pattern, handle the error.
            size_t length = regerror(reti, digit_regex, NULL, 0);
            char *buffer = (char *)malloc(length);
            (void)regerror(reti, digit_regex, buffer, length);

            fprintf(stderr, "%s\n", buffer);
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    regmatch_t match;
    reti = regexec(digit_regex, input, 1, &match, 0); // Execute the regex pattern matching.
    regfree(digit_regex); // Free the regex object.

    if (reti == _REG_NOERROR && match.rm_so == 0)
    {
        return true; // If there is a match, return true.
    }
    else if (reti == REG_NOMATCH || match.rm_so != 0)
    {
        return false; // If there is no match, return false.
    }
    else
    {
        // If there is an error in executing the regex pattern matching, handle the error.
        size_t length = regerror(reti, digit_regex, NULL, 0);
        char *buffer = (char *)malloc(length);
        (void)regerror(reti, digit_regex, buffer, length);

        fprintf(stderr, buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }
}

bool is_letter(char* input) {
    int reti;
    if (letter_regex == NULL) {
        letter_regex = (regex_t *)malloc(sizeof(regex_t));
        if (letter_regex == NULL) {
            perror("Failed to allocate memory for regex object");
            exit(EXIT_FAILURE);
        }

        reti = regcomp(letter_regex, LETTER_REGEX, REG_EXTENDED); // Compile the regex pattern.
        if (reti != _REG_NOERROR)
        {
            // If there is an error in compiling the regex pattern, handle the error.
            size_t length = regerror(reti, letter_regex, NULL, 0);
            char *buffer = (char *)malloc(length);
            (void)regerror(reti, letter_regex, buffer, length);

            fprintf(stderr, "%s\n", buffer);
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    regmatch_t match;
    reti = regexec(letter_regex, input, 1, &match, 0); // Execute the regex pattern matching.
    regfree(letter_regex); // Free the regex object.

    if (reti == _REG_NOERROR && match.rm_so == 0)
    {
        return true; // If there is a match, return true.
    }
    else if (reti == REG_NOMATCH || match.rm_so != 0)
    {
        return false; // If there is no match, return false.
    }
    else
    {
        // If there is an error in executing the regex pattern matching, handle the error.
        size_t length = regerror(reti, letter_regex, NULL, 0);
        char *buffer = (char *)malloc(length);
        (void)regerror(reti, letter_regex, buffer, length);

        fprintf(stderr, buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }
}

bool is_punctuation(char *input) {
    int reti;
    if (punctuation_regex == NULL) {
        punctuation_regex = (regex_t *)malloc(sizeof(regex_t));
        if (punctuation_regex == NULL) {
            perror("Failed to allocate memory for regex object");
            exit(EXIT_FAILURE);
        }

        reti = regcomp(punctuation_regex, PUNCTUATION_REGEX, REG_EXTENDED); // Compile the regex pattern.
        if (reti != _REG_NOERROR)
        {
            // If there is an error in compiling the regex pattern, handle the error.
            size_t length = regerror(reti, punctuation_regex, NULL, 0);
            char *buffer = (char *)malloc(length);
            (void)regerror(reti, punctuation_regex, buffer, length);

            fprintf(stderr, "%s\n", buffer);
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    regmatch_t match;
    reti = regexec(punctuation_regex, input, 1, &match, 0); // Execute the regex pattern matching.
    regfree(punctuation_regex); // Free the regex object.

    if (reti == _REG_NOERROR && match.rm_so == 0)
    {
        return true; // If there is a match, return true.
    }
    else if (reti == REG_NOMATCH || match.rm_so != 0)
    {
        return false; // If there is no match, return false.
    }
    else
    {
        // If there is an error in executing the regex pattern matching, handle the error.
        size_t length = regerror(reti, punctuation_regex, NULL, 0);
        char *buffer = (char *)malloc(length);
        (void)regerror(reti, punctuation_regex, buffer, length);

        fprintf(stderr, buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }
}

bool is_whitespace(char *input) {
    int reti;
    if (whitespace_regex == NULL) {
        whitespace_regex = (regex_t *)malloc(sizeof(regex_t));
        if (whitespace_regex == NULL) {
            perror("Failed to allocate memory for regex object");
            exit(EXIT_FAILURE);
        }

        reti = regcomp(whitespace_regex, WHITESPACE_REGEX, REG_EXTENDED); // Compile the regex pattern.
        if (reti != _REG_NOERROR)
        {
            // If there is an error in compiling the regex pattern, handle the error.
            size_t length = regerror(reti, whitespace_regex, NULL, 0);
            char *buffer = (char *)malloc(length);
            (void)regerror(reti, whitespace_regex, buffer, length);

            fprintf(stderr, "%s\n", buffer);
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    regmatch_t match;
    reti = regexec(whitespace_regex, input, 1, &match, 0); // Execute the regex pattern matching.
    regfree(whitespace_regex); // Free the regex object.

    if (reti == _REG_NOERROR && match.rm_so == 0)
    {
        return true; // If there is a match, return true.
    }
    else if (reti == REG_NOMATCH || match.rm_so != 0)
    {
        return false; // If there is no match, return false.
    }
    else
    {
        // If there is an error in executing the regex pattern matching, handle the error.
        size_t length = regerror(reti, whitespace_regex, NULL, 0);
        char *buffer = (char *)malloc(length);
        (void)regerror(reti, whitespace_regex, buffer, length);

        fprintf(stderr, buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }

}

char* ignore_whitespace(char *input) {
    int reti;
    if (whitespace_regex == NULL) {
        whitespace_regex = (regex_t *)malloc(sizeof(regex_t));
        if (whitespace_regex == NULL) {
            perror("Failed to allocate memory for regex object");
            exit(EXIT_FAILURE);
        }

        reti = regcomp(whitespace_regex, WHITESPACE_REGEX, REG_EXTENDED); // Compile the regex pattern.
        if (reti != _REG_NOERROR)
        {
            // If there is an error in compiling the regex pattern, handle the error.
            size_t length = regerror(reti, whitespace_regex, NULL, 0);
            char *buffer = (char *)malloc(length);
            (void)regerror(reti, whitespace_regex, buffer, length);

            fprintf(stderr, "%s\n", buffer);
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    regmatch_t match;
    reti = regexec(whitespace_regex, input, 1, &match, 0); // Execute the regex pattern matching.
    regfree(whitespace_regex); // Free the regex object.

    if (reti == _REG_NOERROR && match.rm_so == 0)
    {
        return input + match.rm_eo; // If there is a match, return the location of the whitespace.
    }
    else if (reti == REG_NOMATCH || match.rm_so != 0)
    {
        return NULL; // If there is no match, return NULL.
    }
    else
    {
        // If there is an error in executing the regex pattern matching, handle the error.
        size_t length = regerror(reti, whitespace_regex, NULL, 0);
        char *buffer = (char *)malloc(length);
        (void)regerror(reti, whitespace_regex, buffer, length);

        fprintf(stderr, buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }
}

char* ignore_comment(char *input)
{
    int reti;
    if (comment_regex == NULL) {
        comment_regex = (regex_t *)malloc(sizeof(regex_t));
        if (comment_regex == NULL) {
            perror("Failed to allocate memory for regex object");
            exit(EXIT_FAILURE);
        }

        reti = regcomp(comment_regex, COMMENT_REGEX, REG_EXTENDED); // Compile the regex pattern.
        if (reti != _REG_NOERROR)
	    {
            // If there is an error in compiling the regex pattern, handle the error.
            size_t length = regerror(reti, comment_regex, NULL, 0);
            char *buffer = (char *)malloc(length);
            (void)regerror(reti, comment_regex, buffer, length);

            fprintf(stderr, "%s\n", buffer);
            free(buffer);
            exit(EXIT_FAILURE);
	    }
    }

	regmatch_t match;
	reti = regexec(comment_regex, input, 1, &match, 0); // Execute the regex pattern matching.
	regfree(comment_regex); // Free the regex object.

	if (reti == _REG_NOERROR)
	{
		return input + match.rm_eo - 1; // If there is a match, return the location of the comment.
	}
	else if (reti == REG_NOMATCH)
	{
		return NULL; // If there is no match, return NULL.
	}
	else
	{
		// If there is an error in executing the regex pattern matching, handle the error.
		size_t length = regerror(reti, comment_regex, NULL, 0);
		char *buffer = (char *)malloc(length);
		(void)regerror(reti, comment_regex, buffer, length);

		fprintf(stderr, buffer);
		free(buffer);
		exit(EXIT_FAILURE);
	}
}
