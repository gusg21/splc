#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#define SCANNER_KEYWORD_MAX_LENGTH 16

struct scanner_keyword_type_pair {
    const char*     keyword;
    enum token_type token_type;
};

static struct scanner_keyword_type_pair keyword_map[] = {
    {"and", TOKEN_AND},
    {"class", TOKEN_CLASS},
    {"else", TOKEN_ELSE},
    {"false", TOKEN_FALSE},
    {"for", TOKEN_FOR},
    {"fun", TOKEN_FUN},
    {"if", TOKEN_IF},
    {"nil", TOKEN_NIL},
    {"or", TOKEN_OR},
    {"print", TOKEN_PRINT},
    {"return", TOKEN_RETURN},
    {"super", TOKEN_SUPER},
    {"this", TOKEN_THIS},
    {"true", TOKEN_TRUE},
    {"var", TOKEN_VAR},
    {"while", TOKEN_WHILE},
};
static size_t keyword_map_count = sizeof(keyword_map) / sizeof(struct scanner_keyword_type_pair);

static bool scanner_is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static bool scanner_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static bool scanner_is_alphanumeric(char c)
{
    return scanner_is_alpha(c) || scanner_is_digit(c);
}

static enum token_type scanner_get_token_type_of_keyword(char* keyword, size_t keyword_length) {
    for (size_t pair_index = 0; pair_index < keyword_map_count; pair_index++) {
        struct scanner_keyword_type_pair* pair = &keyword_map[pair_index];
        if (strncmp(pair->keyword, keyword, keyword_length) == 0) {
            return pair->token_type;
        }
    }
    return -1; // A little gross, but I don't think we should add a TOKEN_INVALID just for this case.
}

void scanner_init(struct scanner* scanner)
{
    scanner->source        = NULL;
    scanner->source_length = 0;
    scanner->current       = NULL;
    scanner->lexeme_start  = NULL;
    scanner->line          = 1;
}

void scanner_set_source(struct scanner* scanner, char* source, size_t source_length)
{
    scanner->source        = source;
    scanner->source_length = source_length;
    scanner->current       = source;
    scanner->lexeme_start  = source;
    scanner->line          = 1;
}

int scanner_scan_tokens(struct scanner* scanner, struct token_container* tokens)
{
    if (!scanner->source || scanner->source_length <= 0) {
        log_splc_error(0, "Either no source was set or the set source was empty!");
        return ERR_NO_SOURCE;
    }

    scanner->tokens = tokens;

    while (!scanner_is_at_end(scanner)) {
        // We are now at a new lexeme. Every call to scanner_scan_token() will "consume" the whole lexeme.
        scanner->lexeme_start = scanner->current;

        scanner_scan_token(scanner);
    }

    // Add an EOF at the end of the tokens.
    token_container_add_token(tokens,
        (struct token) { .type = TOKEN_EOF,
            .line              = scanner->line,
            .string_literal    = NULL,
            .decimal_literal   = 0.0,
            .lexeme            = NULL });

    return OK;
}

bool scanner_is_at_end(struct scanner* scanner)
{
    return scanner->current - scanner->source >= scanner->source_length || *scanner->current == '\0';
}

void scanner_scan_token(struct scanner* scanner)
{
    char c = scanner_advance(scanner);
    switch (c) {
    case '(': scanner_add_simple_token(scanner, TOKEN_LEFT_PAREN); break;
    case ')': scanner_add_simple_token(scanner, TOKEN_RIGHT_PAREN); break;
    case '{': scanner_add_simple_token(scanner, TOKEN_LEFT_BRACE); break;
    case '}': scanner_add_simple_token(scanner, TOKEN_RIGHT_BRACE); break;
    case ',': scanner_add_simple_token(scanner, TOKEN_COMMA); break;
    case '.': scanner_add_simple_token(scanner, TOKEN_DOT); break;
    case '-': scanner_add_simple_token(scanner, TOKEN_MINUS); break;
    case '+': scanner_add_simple_token(scanner, TOKEN_PLUS); break;
    case ';': scanner_add_simple_token(scanner, TOKEN_SEMICOLON); break;
    case '*': scanner_add_simple_token(scanner, TOKEN_STAR); break;
    case '!': {
        scanner_add_simple_token(scanner, scanner_match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        break;
    }
    case '=': {
        scanner_add_simple_token(scanner, scanner_match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        break;
    }
    case '<': {
        scanner_add_simple_token(scanner, scanner_match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        break;
    }
    case '>': {
        scanner_add_simple_token(scanner, scanner_match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        break;
    }
    case '/': {
        if (scanner_match(scanner, '/')) {
            while (true) {
                char current_char = scanner_peek(scanner);
                bool ended        = scanner_is_at_end(scanner);
                if (current_char != '\n' && !ended) {
                    scanner_advance(scanner);
                } else {
                    break;
                }
            }
        } else {
            scanner_add_simple_token(scanner, TOKEN_SLASH);
        }
        break;
    }

    // Ignore whitespace.
    case ' ':
    case '\t':
    case '\r': break;

    case '\n': scanner->line++; break;

    case '"': {
        scanner_consume_string(scanner);
        break;
    }

    // case '\0': printf("*"); break;
    default: {
        if (scanner_is_digit(c)) {
            scanner_consume_number(scanner);
        } else if (scanner_is_alpha(c)) {
            scanner_consume_identifier(scanner);
        } else {
            fprintf(stderr, "ERROR: Bad character '%c'!\n", c);
        }
        break;
    }
    }
}

/**
 * @brief Consumes and returns the current character.
 *
 * @param scanner
 * @return char
 */
char scanner_advance(struct scanner* scanner)
{
    char c = *scanner->current;
    scanner->current++;
    return c;
}

void scanner_add_simple_token(struct scanner* scanner, enum token_type type)
{
    token_container_add_token(scanner->tokens,
        (struct token) { .type = type,
            .lexeme            = scanner->lexeme_start,
            .line              = scanner->line,
            .decimal_literal   = 0.0,
            .string_literal    = NULL });
}

/**
 * @brief Checks if the current character matches a given character, and consumes it if so.
 *
 * @param scanner
 * @param c
 * @return true
 * @return false
 */
bool scanner_match(struct scanner* scanner, char c)
{
    if (scanner_is_at_end(scanner)) {
        return false;
    }

    if (*scanner->current != c) return false;

    scanner->current++;
    return true;
}

char scanner_peek(struct scanner* scanner)
{
    if (scanner_is_at_end(scanner)) {
        return '\0';
    }
    return *scanner->current;
}

char scanner_peek_next(struct scanner* scanner)
{
    if (scanner->current - scanner->source + 1 >= scanner->source_length || *(scanner->current + 1) == '\0') {
        return '\0';
    }
    return *(scanner->current + 1);
}

void scanner_consume_string(struct scanner* scanner)
{
    while (scanner_peek(scanner) != '"' && !scanner_is_at_end(scanner)) {
        if (scanner_peek(scanner) == '\n') scanner->line++;
        scanner_advance(scanner);
    }

    if (scanner_is_at_end(scanner)) {
        log_splc_error(scanner->line, "Unterminated string!");
    }

    scanner_advance(scanner);

    char*  string        = scanner->lexeme_start + 1;
    size_t string_length = scanner->current - string - 1;
    scanner_add_token_with_string(scanner, TOKEN_STRING, string, string_length);
}

void scanner_consume_number(struct scanner* scanner)
{
    while (scanner_is_digit(scanner_peek(scanner))) {
        scanner_advance(scanner);
    }

    if (scanner_peek(scanner) == '.' && scanner_is_digit(scanner_peek_next(scanner))) {
        scanner_advance(scanner);
        while (scanner_is_digit(scanner_peek(scanner))) {
            scanner_advance(scanner);
        }
    }

    // Convert appropriate substring to floating number.
    size_t number_str_length = scanner->current - scanner->lexeme_start;
    char*  number_str        = malloc(number_str_length * sizeof(char));
    memcpy(number_str, scanner->lexeme_start, number_str_length);
    double number = atof(number_str);
    scanner_add_token_with_number(scanner, TOKEN_NUMBER, number);
    free(number_str);
}

void scanner_consume_identifier(struct scanner* scanner)
{
    while (scanner_is_alphanumeric(scanner_peek(scanner))) scanner_advance(scanner);

    size_t identifier_length = scanner->current - scanner->lexeme_start;
    char* identifier_str = malloc(identifier_length + 1);
    memset(identifier_str, 0, identifier_length);
    memcpy(identifier_str, scanner->lexeme_start, identifier_length);
    enum token_type type = scanner_get_token_type_of_keyword(identifier_str, identifier_length);
    if (type == -1) type = TOKEN_IDENTIFIER;
    free(identifier_str);

    scanner_add_simple_token(scanner, type);
}

void scanner_add_token_with_string(struct scanner* scanner, enum token_type type, char* string, size_t string_length)
{
    token_container_add_token(scanner->tokens,
        (struct token) { .type     = type,
            .lexeme                = scanner->lexeme_start,
            .line                  = scanner->line,
            .decimal_literal       = 0.0,
            .string_literal        = string,
            .string_literal_length = string_length });
}

void scanner_add_token_with_number(struct scanner* scanner, enum token_type type, double number)
{
    token_container_add_token(scanner->tokens,
        (struct token) { .type     = type,
            .lexeme                = scanner->lexeme_start,
            .line                  = scanner->line,
            .decimal_literal       = number,
            .string_literal        = NULL,
            .string_literal_length = 0 });
}
