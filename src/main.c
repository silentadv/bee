#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/hashmap.h"
#include "common/stmt.h"
#include "frontend/checker.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"

#include "backend/generator.h"

#include "common/runtime.h"
#include "common/stringview.h"
#include "common/token.h"
#include "common/vector.h"

hashmap_t *create_runtime_env() {
  hashmap_t *hm = hashmap_new(10);

  runtime_reg_t *print_input = malloc(sizeof(runtime_reg_t));
  print_input->name = SV("rdi");
  print_input->type = TYPE_STR;

  runtime_reg_t *exit_input = malloc(sizeof(runtime_reg_t));
  exit_input->name = SV("rdi");
  exit_input->type = TYPE_INT;

  runtime_reg_t *strlen_input = malloc(sizeof(runtime_reg_t));
  strlen_input->name = SV("rdi");
  strlen_input->type = TYPE_STR;

  runtime_reg_t *strlen_output = malloc(sizeof(runtime_reg_t));
  strlen_output->name = SV("rax");
  strlen_output->type = TYPE_INT;

  runtime_fun_t print_fun = {.name = SV("print"),
                             .inputs = print_input,
                             .input_count = 1,
                             .outputs = NULL,
                             .output_count = 0};

  runtime_fun_t exit_fun = {.name = SV("exit"),
                            .inputs = exit_input,
                            .input_count = 1,
                            .outputs = NULL,
                            .output_count = 0};

  runtime_fun_t strlen_fun = {.name = SV("strlen"),
                              .inputs = strlen_input,
                              .input_count = 1,
                              .outputs = strlen_output,
                              .output_count = 1};

  hashmap_insert(hm, SV("print"), &print_fun, sizeof(runtime_fun_t));
  hashmap_insert(hm, SV("exit"), &exit_fun, sizeof(runtime_fun_t));
  hashmap_insert(hm, SV("strlen"), &strlen_fun, sizeof(runtime_fun_t));

  return hm;
}

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

  hashmap_t *runtime_env = create_runtime_env();
  checker_t *checker = checker_new(prog, runtime_env);
  hashmap_t *symbols = checker_check(checker);

  generator_t *generator = generator_new(prog, symbols, runtime_env);
  generator_gen(generator);
  generator_destroy(generator);
  return 0;
}
