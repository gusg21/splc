#if !defined(SCANNER_H)
#define SCANNER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "token.h"
#include "token_container.h"

enum scanner_result {
    OK,
    ERR_NO_SOURCE,
};

struct scanner {
    const char*             source;
    size_t                  source_length;
    char*                   current;
    char*                   lexeme_start;
    uint32_t                line;
    struct token_container* tokens;
};

void scanner_init(struct scanner* scanner);
void scanner_set_source(struct scanner* scanner, char* source, size_t source_length);
int  scanner_scan_tokens(struct scanner* scanner, struct token_container* tokens);
bool scanner_is_at_end(struct scanner* scanner);
void scanner_scan_token(struct scanner* scanner);
char scanner_advance(struct scanner* scanner);
bool scanner_match(struct scanner* scanner, char c);
char scanner_peek(struct scanner* scanner);
char scanner_peek_next(struct scanner* scanner);
void scanner_consume_string(struct scanner* scanner);
void scanner_consume_number(struct scanner* scanner);
void scanner_consume_identifier(struct scanner* scanner);
void scanner_add_simple_token(struct scanner* scanner, enum token_type type);
void scanner_add_token_with_string(struct scanner* scanner, enum token_type type, char* string, size_t string_length);
void scanner_add_token_with_number(struct scanner* scanner, enum token_type type, double number);

#endif  // SCANNER_H
