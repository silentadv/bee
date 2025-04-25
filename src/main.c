#include <stdio.h>
#include <stdlib.h>

#include "common/stmt.h"
#include "frontend/checker.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"

#include "backend/generator.h"

#include "common/stringview.h"
#include "common/token.h"
#include "common/vector.h"

int main(int argc, char **argv) {
  if (argc <= 1) {
    perror("Error: Invalid bee usage ...\n Usage: bee <input.b>\n");
    return EXIT_FAILURE;
  }

  lexer_t *lexer = lexer_read(argv[1]);
  vector_t *tokens = lexer_lex(lexer);
  lexer_destroy(lexer);

  for (size_t i = 0; i < tokens->buf_size; i++) {
    token_t *tok = (token_t *)vector_at(tokens, i);
    printf("token -> k: %d, lexeme: " SV_FMT "\n", tok->kind,
           SV_ARG(tok->lexeme));
  }

  parser_t *parser = parser_new(tokens);
  prog_t *prog = parser_parse(parser);
  vector_t *body = prog->body;
  parser_destroy(parser);

  for (size_t i = 0; i < body->buf_size; i++) {
    stmt_t *stmt = (stmt_t *)vector_at(body, i);
    printf("stmt -> k: %d\n", stmt->kind);
  }

  checker_t *checker = checker_new(prog);
  hashmap_t *symbols = checker_check(checker);

  generator_t *generator = generator_new(prog, symbols);
  generator_gen(generator);
  generator_destroy(generator);

  return 0;
}
