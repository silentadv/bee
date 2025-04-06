#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "vector.h"

int main() {
    lexer_t *lexer = lexer_new("exit0 0a;");
    vector_t *tokens = lexer_lex(lexer);
    size_t token_count = tokens->buf_size;

    for (size_t i = 0; i < token_count; i++) {
        token_t *token = (token_t*) vector_at(tokens, i);
        printf("%s\n", token->lexeme);
    }

    return 0;
}
