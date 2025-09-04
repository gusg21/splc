#if !defined(TOKEN_CONTAINER_H)
#define TOKEN_CONTAINER_H

#include <stddef.h>
#include <stdint.h>

#include "token.h"

struct token_container {
    struct token* tokens;
    uint32_t      token_count;
    size_t        max_token_count;
    uint32_t      next_token_index;
};

void token_container_allocate(struct token_container* container, size_t count);
void token_container_add_token(struct token_container* container, struct token token);
void token_container_clear(struct token_container* container);

#endif  // TOKEN_CONTAINER_H
