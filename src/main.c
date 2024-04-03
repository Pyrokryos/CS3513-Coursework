/**
 * @file main.c
 * @brief Entry point of the program.
 *
 * This file contains the main function, which serves as the entry point of the program.
 *
 * It parses command line arguments, opens a file, reads its content, performs lexical analysis
 * and/or abstract syntax tree generation based on the command line arguments, and frees allocated memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/lexer.h"
#include "../include/parser.h"

#define TEST_FILE "tests/1.rpal"
#define MAX_FILE_SIZE 100000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE 100000

/**
 * @brief Main function of the program.
 *
 * This function is the entry point of the program. It takes command line arguments, parses them,
 * opens a file, reads its content, performs lexical analysis and/or abstract syntax tree generation
 * based on the command line arguments, and frees allocated memory.
 *
 * @param argc The number of command line arguments
 * @param argv An array of strings containing the command line arguments
 * @return The exit status of the program.
 */
int main(int argc, char *argv[]) {
    // Array of valid command line switches
    const char *switches[] = {"-ast", "-lex"};

    // Array to keep track of found switches.
    bool found_switch[sizeof(switches) / sizeof(switches[0])] = {false};

    // Iterate through command line arguments.
    for (int i = 1; i <= sizeof(switches) / sizeof(switches[0]); i++) {
        bool valid_arg = false;

        // Check if the argument matches any of the valid switches.
        for (int j = 0; j < sizeof(switches) / sizeof(switches[0]); j++) {
            if (strcmp(argv[i], switches[j]) == 0) {
                // Check if the switch has already been found.
                if (found_switch[j]) {
                    fprintf(stderr, "Duplicate switch: '%s'\n", argv[i]);
                    return EXIT_FAILURE;
                }

                found_switch[j] = true;
                valid_arg = true;
                break;
            }
        }

        // If the argument is not a valid switch, print an error message and exit.
        if (!valid_arg) {
            fprintf(stderr, "Invalid switch or unexpected text: '%s'\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    // Count the number of switches found.
    int switch_count = 0;
    for (int i = 0; i < sizeof(found_switch) / sizeof(found_switch[0]); i++) {
        if (found_switch[i]) {
            switch_count++;
        }
    }

    // Check if there are too many arguments.
    if (argc > switch_count + 2) {
        fprintf(stderr, "Too many arguments.\n");
        return EXIT_FAILURE;
    }

    // Get the file path as a command line argument or use a default file path.
    char *file_path = (argc == switch_count + 2) ? argv[switch_count + 1] : TEST_FILE;

    // Open the file for reading.
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return EXIT_FAILURE;
    }

    // Allocate memory to store the file content.
    char *content = (char *)malloc(MAX_FILE_SIZE * sizeof(char));
    if (content == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Read the content of the file into the allocated memory.
    size_t bytesRead = fread(content, sizeof(char), MAX_FILE_SIZE, file);
    content[bytesRead] = '\0'; // Null-terminate the string

    // Close the file.
    fclose(file);

    // Perform lexical analysis and/or abstract syntax tree generation based on command line argument.
    TokenStream *stream = lex(content);
    if (found_switch[0]) {
        display_list(stream);
        printf("\n");
    }

    if (found_switch[1]) {
        AST(parse(stream));
    }

    // Free the allocated memory.
    free(stream);
    free(content);

    return EXIT_SUCCESS;
}
