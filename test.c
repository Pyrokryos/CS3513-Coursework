#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum TokenType {
    IDENTIFIER,
    INTEGER,
    KEYWORD,
    OPERATOR,
    PUNCTUATION,
    STRING
} TokenType;

typedef struct Token {
    const char *const value;
    TokenType type;
} Token;

Token *create_token() {
    Token temp = { strdup("hello"), IDENTIFIER };
    printf("%ld\n", temp.value);

    Token *token = malloc(sizeof(Token));
    memcpy(token, &temp, sizeof(Token));
    printf("%ld\n", token->value);
    // Token *token = malloc(sizeof(Token));
    // token->value = strdup("hello");
    // token->type = IDENTIFIER;
    printf("%ld\n", token);

    return token;
}

int main() {
    Token *token = create_token();

    printf("Token value: %s\n", token->value);

    free(token);
    
    return 0;
}
