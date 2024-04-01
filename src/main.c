#include <stdbool.h>

#include "../include/lexer.h"
#include "../include/parser.h"

#define MAX_FILE_SIZE 100000

int main()
{
    // Open the file for reading.
    FILE *file = fopen("tests/1.rpal", "r");
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

    TokenStream *stream = lex(content);
    display_list(stream);

    parse(stream);
    free(stream);
    return 0;
}
