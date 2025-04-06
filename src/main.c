#include <stdio.h>
#include <stdlib.h>

#include "frontend/lexer.h"
#include "common/vector.h"

int main(int argc, char** argv) {
    if (argc <= 1) {
        perror("Error: Invalid bee usage ...\n Usage: bee <input.b>\n");
        return EXIT_FAILURE;
    }

    lexer_t *lexer = lexer_read(argv[1]);
    vector_t *tokens = lexer_lex(lexer);
    size_t token_count = tokens->buf_size;

    for (size_t i = 0; i < token_count; i++) {
        token_t *token = (token_t*) vector_at(tokens, i);
        printf("%s\n", token->lexeme);
    }

    return 0;
}
