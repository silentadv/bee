#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "backend/generator.h"
#include "common/vector.h"


int main(int argc, char** argv) {
    if (argc <= 1) {
        perror("Error: Invalid bee usage ...\n Usage: bee <input.b>\n");
        return EXIT_FAILURE;
    }

    lexer_t *lexer = lexer_read(argv[1]);
    vector_t *tokens = lexer_lex(lexer);
    lexer_destroy(lexer);

    parser_t *parser = parser_new(tokens);
    prog_t *prog = parser_parse(parser);
    parser_destroy(parser);

    generator_t *generator = generator_new(prog);
    generator_gen(generator);
    generator_destroy(generator);

    return 0;
}
