#include <stdio.h>
#include <stdlib.h>

#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "common/vector.h"

int main(int argc, char** argv) {
    if (argc <= 1) {
        perror("Error: Invalid bee usage ...\n Usage: bee <input.b>\n");
        return EXIT_FAILURE;
    }

    lexer_t *lexer = lexer_read(argv[1]);
    vector_t *tokens = lexer_lex(lexer);
    parser_t *parser = parser_new(tokens);
    prog_t *prog = parser_parse(parser);
    size_t stmt_count = prog->body->buf_size;

    for (size_t i = 0; i < stmt_count; i++) {
        stmt_t *stmt = (stmt_t*) vector_at(prog->body, i);
        printf("k: %d, code: %s\n", 
            stmt->kind, stmt->body.exit_stmt.exit_token.lexeme);
    }

    return 0;
}
