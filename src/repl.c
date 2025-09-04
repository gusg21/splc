#include "repl.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "scanner.h"
#include "token.h"

#define STRINGIFY(x) "\"" #x "\""
#define REPL_MAX_INPUT_LENGTH 2048
#define REPL_MAX_TOKEN_COUNT 2048

int repl_run()
{
    struct scanner scanner = { 0 };
    scanner_init(&scanner);

    char repl_input[REPL_MAX_INPUT_LENGTH] = { 0 };

    struct token_container tokens;
    token_container_allocate(&tokens, REPL_MAX_TOKEN_COUNT);

    while (true) {
        memset(repl_input, 0, REPL_MAX_INPUT_LENGTH * sizeof(char));
        token_container_clear(&tokens);

        printf("> ");
        fgets(repl_input, REPL_MAX_INPUT_LENGTH, stdin);
        repl_input[strcspn(repl_input, "\n")] = '\0';

        scanner_set_source(&scanner, repl_input, REPL_MAX_INPUT_LENGTH);
        scanner_scan_tokens(&scanner, &tokens);

        for (size_t token_index = 0; token_index < tokens.token_count; token_index++) {
            struct token* token = &tokens.tokens[token_index];

            if (token->type == TOKEN_STRING) {
                printf("Token (Type %d) '%.*s'\n", token->type, (int)token->string_literal_length, token->string_literal);
            } else if(token->type == TOKEN_NUMBER) {
                printf("Token (Type %d) %g\n", token->type, token->decimal_literal);
            } else {
                printf("Token (Type %d)\n", token->type);
            }
        }

        break;  // For debug.
    }

    return 0;
}