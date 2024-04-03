#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/lexer.h"
#include "../include/parser.h"

#define MAX_FILE_SIZE 100000

int main(int argc, char *argv[]) {
    char *filePath = (argc > 2) ? argv[2] : "tests/1.rpal";

    // Open the file for reading.
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }

    // Allocate memory to store the file content.
    char *content = (char *)malloc(MAX_FILE_SIZE * sizeof(char));
    if (content == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Read the content of the file into the allocated memory.
    size_t bytesRead = fread(content, sizeof(char), MAX_FILE_SIZE, file);
    content[bytesRead] = '\0'; // Null-terminate the string.

    // Close the file.
    fclose(file);

    if (strcmp(argv[1], "-lex") == 0) {
        TokenStream *stream = lex(content);
        display_list(stream);
        free(stream);
    } else if (strcmp(argv[1], "-ast") == 0) {
        TokenStream *stream = lex(content);
        AST(parse(stream));
        free(stream);
    } else {
        printf("Invalid argument.\n");
    }

    free(content);
    return 0;
}
