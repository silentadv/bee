#include "../frontend/checker.h"
#include "../frontend/parser.h"

#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

generator_t *generator_new(prog_t *prog, hashmap_t *symbols) {
  generator_t *instance = malloc(sizeof(generator_t));
  instance->prog = prog;
  instance->symbols = symbols;
  instance->rodata = rodata_new(64);
  instance->cursor = 0;
  instance->stack_size = 0;
  instance->output = NULL;

  return instance;
}

void generator_destroy(generator_t *generator) {
  vector_destroy(generator->prog->body);
  hashmap_destroy(generator->rodata->hm);
  free(generator->rodata);
  free(generator);
}

void generator_gen(generator_t *generator) {
  generator->cursor = 0;
  generator->output = fopen("./build/out.asm", "w+");

  FILE *output = generator->output;
  if (!output) {
    printf("generator -> Unexpected error ocurred during file opening.");
    exit(EXIT_FAILURE);
  }

  fprintf(output, "section .text\n");
  fprintf(output, "   global _start\n");
  fprintf(output, "_start:\n");

  while (generator_peek(generator) != NULL)
    generator_gen_stmt(generator);

  rodata_emit(generator->rodata, output);
  fclose(output);
}

void generator_gen_stmt(generator_t *generator) {
  stmt_t *stmt = generator_peek(generator);

  switch (stmt->kind) {
  case STMT_EXIT:
    generator_gen_exit_stmt(generator);
    break;
  case STMT_VAR_DECL:
    generator_gen_var_decl_stmt(generator);
    break;
  case STMT_WRITE:
    generator_gen_write_stmt(generator);
    break;
  default:
    printf("generator -> Unexpected statement found during generation, "
           "received: %d.\n",
           stmt->kind);
    exit(EXIT_FAILURE);
  }
}

void generator_gen_exit_stmt(generator_t *generator) {
  stmt_t *stmt = generator_expect(generator, STMT_EXIT);
  exit_stmt_t exit_stmt = stmt->body.exit_stmt;

  generator_gen_expr(generator, &exit_stmt.exit_code_expr);

  FILE *output = generator->output;
  fprintf(output, "   mov rdi, rax\n");
  fprintf(output, "   mov rax, 60\n");
  fprintf(output, "   syscall\n");
}

void generator_gen_write_stmt(generator_t *generator) {
  stmt_t *stmt = generator_expect(generator, STMT_WRITE);
  write_stmt_t write_stmt = stmt->body.write_stmt;
  generator_gen_expr(generator, &write_stmt.message_expr);

  FILE *output = generator->output;
  fprintf(output, "   mov rsi, rax\n");
  fprintf(output, "   mov rax, 1\n");
  fprintf(output, "   mov rdi, 1\n");
  fprintf(output, "   syscall\n");
}

void generator_gen_var_decl_stmt(generator_t *generator) {
  stmt_t *stmt = generator_expect(generator, STMT_VAR_DECL);
  var_decl_stmt_t var_decl_stmt = stmt->body.var_decl_stmt;

  generator_gen_expr(generator, &var_decl_stmt.value);

  string_view_t ident = var_decl_stmt.identifier.identifier.lexeme;
  hashmap_bucket_t *bucket = hashmap_get(generator->symbols, ident);
  symbol_t *symbol = (symbol_t *)bucket->buf;
  symbol->stack_pos = generator->stack_size++;

  FILE *output = generator->output;
  fprintf(output, "   push rax\n");
}

void generator_gen_expr(generator_t *generator, expr_t *expr) {
  switch (expr->kind) {
  case EXPR_LIT:
    generator_gen_lit_expr(generator, &expr->body.lit_expr);
    break;
  case EXPR_IDENT:
    generator_gen_ident_expr(generator, &expr->body.ident_expr);
    break;
  default:
    printf("generator -> Unexpected expression found during generation, "
           "received: %d.\n",
           expr->kind);
    exit(EXIT_FAILURE);
  }
}

void generator_gen_lit_expr(generator_t *generator, lit_expr_t *expr) {
  FILE *output = generator->output;
  switch (expr->literal.kind) {
  case TOK_NUM:
    fprintf(output, "   mov rax, " SV_FMT "\n", SV_ARG(expr->literal.lexeme));
    break;
  case TOK_STR: {
    rodata_set(generator->rodata, expr->literal.lexeme);
    size_t id = generator->rodata->len - 1;
    fprintf(output, "   lea rax, [rel .LC%zu]\n", id);
    fprintf(output, "   mov rdx, .LC%zu_len\n", id);
    break;
  }
  default:
    fprintf(stderr,
            "generator -> unexpected tok lit found during generation: %d.",
            expr->literal.kind);
    exit(EXIT_FAILURE);
  }
}

void generator_gen_ident_expr(generator_t *generator, ident_expr_t *expr) {
  string_view_t ident = expr->identifier.lexeme;
  hashmap_bucket_t *bucket = hashmap_get(generator->symbols, ident);

  if (bucket == NULL) {
    printf("generator (unexpected!) -> " SV_FMT " is not defined",
           SV_ARG(ident));
    exit(EXIT_FAILURE);
  }

  symbol_t *symbol = (symbol_t *)bucket->buf;

  FILE *output = generator->output;
  fprintf(output, "   mov rax, [rsp + %lu]\n",
          (generator->stack_size - symbol->stack_pos - 1) * 8);
}

stmt_t *generator_peek(generator_t *generator) {
  vector_t *stmts = generator->prog->body;

  if (generator->cursor >= stmts->buf_size)
    return NULL;

  stmt_t *stmt = (stmt_t *)vector_at(stmts, generator->cursor);
  return stmt;
}

stmt_t *generator_consume(generator_t *generator) {
  vector_t *stmts = generator->prog->body;

  if (generator->cursor >= stmts->buf_size)
    return NULL;

  stmt_t *stmt = (stmt_t *)vector_at(stmts, generator->cursor);
  generator->cursor++;

  return stmt;
}

stmt_t *generator_expect(generator_t *generator, stmt_kind_t kind) {
  if (!generator_peek(generator)) {
    printf("generator -> Expected k: %d, but received null.\n", kind);
    exit(EXIT_FAILURE);
  }

  stmt_t *stmt = generator_consume(generator);
  if (stmt->kind == kind)
    return stmt;

  printf("generator -> Expected k: %d, but received: %d.\n", kind, stmt->kind);
  exit(EXIT_FAILURE);
}
