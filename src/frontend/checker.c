#include "checker.h"
#include <stdio.h>
#include <stdlib.h>

static const type_t type_anottations[] = {
    [TOK_KW_STR] = TYPE_STR, [TOK_KW_INT] = TYPE_INT};

checker_t *checker_new(prog_t *program, hashmap_t *runtime_env) {
  checker_t *checker = malloc(sizeof(checker_t));
  size_t symbols_capacity = 50;
  checker->program = program;
  checker->symbols = hashmap_new(symbols_capacity);
  checker->runtime_env = runtime_env;
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
      checker_expect(type_anottations[var_decl_stmt.type_kw.value.kind],
                     received_type);

    symbol_t symbol = {.ident_expr = var_decl_stmt.identifier,
                       .value = var_decl_stmt.value,
                       .type = received_type};

    string_view_t lexeme = var_decl_stmt.identifier.token.lexeme;
    hashmap_insert(checker->symbols, lexeme, &symbol, sizeof(symbol));
    break;
  }
  case STMT_EXPR: {
    expr_stmt_t expr_stmt = stmt->body.expr_stmt;
    checker_check_expr(checker, &expr_stmt.expr);
    break;
  }
  default:
    fprintf(stderr,
            "checker -> Invalid statement found during semantic checking -> k: "
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
  case EXPR_CALL:
    return checker_check_call_expr(checker, expr);
  default:
    fprintf(stderr,
            "checker -> Invalid expression found during checking -> k: %d \n",
            expr->kind);
    exit(EXIT_FAILURE);
  }
}

type_t checker_check_ident_expr(checker_t *checker, expr_t *expr) {
  ident_expr_t ident_expr = expr->body.ident_expr;
  string_view_t lexeme = ident_expr.token.lexeme;
  hashmap_bucket_t *bucket = hashmap_get(checker->symbols, lexeme);

  if (bucket == NULL) {
    fprintf(stderr, "checker -> " SV_FMT " is not defined \n", SV_ARG(lexeme));
    exit(EXIT_FAILURE);
  }

  symbol_t *symbol = (symbol_t *)bucket->buf;
  return symbol->type;
}

type_t checker_check_lit_expr(checker_t *checker, expr_t *expr) {
  lit_expr_t lit_expr = expr->body.lit_expr;
  switch (lit_expr.token.kind) {
  case TOK_NUM:
    return TYPE_INT;
  case TOK_STR:
    return TYPE_STR;
  default:
    fprintf(stderr,
            "checker -> Invalid literal found during checking -> k: %d \n",
            lit_expr.token.kind);
    exit(EXIT_FAILURE);
  }
}

type_t checker_check_call_expr(checker_t *checker, expr_t *expr) {
  call_expr_t call_expr = expr->body.call_expr;
  string_view_t identifier = call_expr.identifier.token.lexeme;
  hashmap_bucket_t *bucket = hashmap_get(checker->runtime_env, identifier);
  if (bucket == NULL) {
    fprintf(stderr,
            "checker -> function " SV_FMT " is not a builtin function\n",
            SV_ARG(identifier));
    exit(EXIT_FAILURE);
  }

  runtime_fun_t *fun = (runtime_fun_t *)bucket->buf;
  vector_t *args = call_expr.args;

  if (args->buf_size < fun->input_count) {
    fprintf(stderr,
            "checker -> function " SV_FMT
            " expected %zu arguments, but received only %zu\n",
            SV_ARG(identifier), fun->input_count, args->buf_size);
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < fun->input_count; i++) {
    expr_t *current_arg = vector_at(args, i);
    runtime_reg_t current_param = fun->inputs[i];

    type_t received = checker_check_expr(checker, current_arg);
    type_t expected = current_param.type;
    printf(SV_FMT "%d , %d\n", SV_ARG(current_param.name), received, expected);

    checker_expect(expected, received);
  }

  if (fun->output_count)
    return fun->outputs[0].type;

  return TYPE_VOID;
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

void checker_expect(type_t expects, type_t received) {
  if (expects == received)
    return;

  fprintf(stderr, "checker -> Expects type %d, but received type: %d\n",
          expects, received);
  exit(EXIT_FAILURE);
}
