#include "../include/lexer.h"

// Declare global variables to hold compiled regex patterns.
regex_t *digit_regex = NULL;
regex_t *letter_regex = NULL;
regex_t *punctuation_regex = NULL;
regex_t *whitespace_regex = NULL;
regex_t *comment_regex = NULL;
regex_t *identifier_regex = NULL;
regex_t *number_regex = NULL;
regex_t *operator_regex = NULL;
regex_t *string_regex = NULL;

// Matches comments in the input string and returns the location of the comment.
char* match_comments(char *input)
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
		return input + match.rm_eo; // If there is a match, return the location of the comment.
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
