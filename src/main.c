#include <stdio.h>
#include <stdlib.h>

#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "backend/generator.h"
#include "common/vector.h"
#include "common/token.h"

int main(int argc, char** argv) {
    if (argc <= 1) {
        perror("Error: Invalid bee usage ...\n Usage: bee <input.b>\n");
        return EXIT_FAILURE;
    }

    lexer_t *lexer = lexer_read(argv[1]);
    vector_t *tokens = lexer_lex(lexer);
    lexer_destroy(lexer);

    for (size_t i = 0; i < tokens->buf_size; i++) {
        token_t *tok = (token_t*) vector_at(tokens, i);
        printf("token -> k: %d, lexeme: %s\n", tok->kind, tok->lexeme); 
    }

    
    parser_t *parser = parser_new(tokens);
    prog_t *prog = parser_parse(parser);
    vector_t *body = prog->body;
    parser_destroy(parser);

    for (size_t i = 0; i < body->buf_size; i++) {
       stmt_t *stmt = (stmt_t*) vector_at(body, i);
       printf("stmt -> k: %d\n", stmt->kind);
    }

    /*
    generator_t *generator = generator_new(prog);
    generator_gen(generator);
    generator_destroy(generator); 
    */

    return 0;
}
