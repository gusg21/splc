#include "token_container.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

void token_container_allocate(struct token_container* container, size_t count)
{
    container->tokens = malloc(count * sizeof(struct token));
    memset(container->tokens, 0, count * sizeof(struct token));
    container->max_token_count = count;
    container->next_token_index = 0;
    container->token_count = 0;
}

void token_container_add_token(struct token_container* container, struct token token) {
    container->tokens[container->next_token_index] = token;
    container->next_token_index++;
    container->token_count++;

    assert(container->next_token_index < container->max_token_count && "Not enough token space allocated in container!");
}

void token_container_clear(struct token_container* container)
{
    memset(container->tokens, 0, container->max_token_count * sizeof(struct token));
    container->next_token_index = 0;
    container->token_count = 0;
}
