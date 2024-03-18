#include <stdio.h>
#include <regex.h>

// Function to match identifier tokens
void matchIdentifier(char *ptr) {
    regex_t identifier_regex;
    regcomp(&identifier_regex, "[a-zA-Z_][a-zA-Z0-9_]*", REG_EXTENDED);

    regmatch_t match[1];
    if (regexec(&identifier_regex, ptr, 1, match, 0) == 0 && match[0].rm_so == 0) {
        printf("<IDENTIFIER> ");
    }

    regfree(&identifier_regex);
}

// Function to match integer tokens
void matchInteger(char *ptr) {
    regex_t integer_regex;
    regcomp(&integer_regex, "[0-9]+", REG_EXTENDED);

    regmatch_t match[1];
    if (regexec(&integer_regex, ptr, 1, match, 0) == 0 && match[0].rm_so == 0) {
        printf("<INTEGER> ");
    }

    regfree(&integer_regex);
}

// Function to match operator tokens
void matchOperator(char *ptr) {
    regex_t operator_regex;
    regcomp(&operator_regex, "\\+|-|\\*|<|>|&|\\.|@|/|:|=", REG_EXTENDED);

    regmatch_t match[1];
    if (regexec(&operator_regex, ptr, 1, match, 0) == 0 && match[0].rm_so == 0) {
        printf("<OPERATOR> ");
    }

    regfree(&operator_regex);
}

// Function to match string tokens
void matchString(char *ptr) {
    regex_t string_regex;
    regcomp(&string_regex, "\'([^\']|\\\\t|\\\\n|\\\\\\\\|\\\\\')*\'", REG_EXTENDED);

    regmatch_t match[1];
    if (regexec(&string_regex, ptr, 1, match, 0) == 0 && match[0].rm_so == 0) {
        printf("<STRING> ");
    }

    regfree(&string_regex);
}

int main() {
    char input[] = "int main() {\n    printf(\"Hello, world!\");\n    return 0;\n}";
    char* ptr = input;

    while (*ptr != '\0') {
        // Skip spaces
        if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
            ptr++;
            continue;
        }

        // Handle comments
        if (*ptr == '/' && *(ptr + 1) == '/') {
            while (*ptr != '\n') {
                ptr++;
            }
            continue;
        }

        // Match tokens
        matchIdentifier(ptr);
        matchInteger(ptr);
        matchOperator(ptr);
        matchString(ptr);

        ptr++;
    }

    return 0;
}
