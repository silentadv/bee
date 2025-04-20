#include "checker.h"
#include <stdio.h>
#include <stdlib.h>

static const type_t type_anottations[] = {
    [TOK_KW_STR] = TYPE_STR,
    [TOK_KW_INT] = TYPE_INT
};

checker_t *checker_new(prog_t *program) {
  checker_t *checker = malloc(sizeof(checker_t));
  size_t symbols_capacity = 50;
  checker->program = program;
  checker->symbols = hashmap_new(symbols_capacity);
  checker->cursor = 0;

  return checker;
}

hashmap_t *checker_check(checker_t *checker) {
  while (checker_peek(checker) != NULL)
    checker_check_stmt(checker);

  return checker->symbols;
}

void checker_check_stmt(checker_t *checker) {
  stmt_t *stmt = checker_consume(checker);

  switch (stmt->kind) {
  case STMT_VAR_DECL: {
    var_decl_stmt_t var_decl_stmt = stmt->body.var_decl_stmt;
    type_t received_type = checker_check_expr(checker, &var_decl_stmt.value);
    
    if (var_decl_stmt.type_kw.has_value)
        checker_expect(type_anottations[var_decl_stmt.type_kw.value.kind], received_type);  

    symbol_t symbol = {.ident_expr = var_decl_stmt.identifier,
                       .value = var_decl_stmt.value,
                       .type = received_type};

    hashmap_insert(checker->symbols, var_decl_stmt.identifier.identifier.lexeme,
                   &symbol, sizeof(symbol));
    break;
  }
  case STMT_EXIT: {
    exit_stmt_t exit_stmt = stmt->body.exit_stmt;
    type_t code_type = checker_check_expr(checker, &exit_stmt.exit_code_expr);
    checker_expect(TYPE_INT, code_type);
    break;
  }
  default:
    printf("checker -> Invalid statement found during semantic checking -> k: "
           "%d \n",
           stmt->kind);
    exit(EXIT_FAILURE);
  }
}

type_t checker_check_expr(checker_t *checker, expr_t *expr) {
  switch (expr->kind) {
  case EXPR_IDENT:
    return checker_check_ident_expr(checker, expr);
  case EXPR_LIT:
    return checker_check_lit_expr(checker, expr);
  default:
    printf("checker -> Invalid expression found during checking -> k: %d \n",
           expr->kind);
    exit(EXIT_FAILURE);
  }
}

type_t checker_check_ident_expr(checker_t *checker, expr_t *expr) {
  ident_expr_t ident_expr = expr->body.ident_expr;
  hashmap_bucket_t *bucket =
      hashmap_get(checker->symbols, ident_expr.identifier.lexeme);

  if (bucket == NULL) {
    printf("checker - %s is not defined \n", ident_expr.identifier.lexeme);
    exit(EXIT_FAILURE);
  }

  symbol_t *symbol = (symbol_t *)bucket->buf;
  return symbol->type;
}

type_t checker_check_lit_expr(checker_t *checker, expr_t *expr) {
  lit_expr_t lit_expr = expr->body.lit_expr;
  switch (lit_expr.literal.kind) {
  case TOK_NUM:
    return TYPE_INT;
  case TOK_STR:
    return TYPE_STR;
  default:
    printf("checker -> Invalid literal found during checking -> k: %d \n",
           lit_expr.literal.kind);
    exit(EXIT_FAILURE);
  }
}

stmt_t *checker_peek(checker_t *checker) {
  vector_t *stmts = checker->program->body;

  if (checker->cursor >= stmts->buf_size)
    return NULL;

  stmt_t *stmt = (stmt_t *)vector_at(stmts, checker->cursor);
  return stmt;
}

stmt_t *checker_consume(checker_t *checker) {
  vector_t *stmts = checker->program->body;

  if (checker->cursor >= stmts->buf_size)
    return NULL;

  stmt_t *stmt = (stmt_t *)vector_at(stmts, checker->cursor);
  checker->cursor++;

  return stmt;
}

void checker_expect(type_t expects, type_t received){
  if (expects == received)
    return;

  printf("checker -> Expects type %d, but received type: %d\n", expects,
         received);
  exit(EXIT_FAILURE);
}
