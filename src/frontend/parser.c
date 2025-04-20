#include <stdio.h>
#include <stdlib.h>

#include "../common/token.h"
#include "../common/vector.h"
#include "parser.h"

parser_t *parser_new(vector_t *tokens) {
  parser_t *instance = malloc(sizeof(parser_t));
  instance->cursor = 0;
  instance->tokens = tokens;

  return instance;
}

void parser_destroy(parser_t *parser) {
  vector_destroy(parser->tokens);
  free(parser);
}

prog_t *parser_parse(parser_t *parser) {
  prog_t *prog = malloc(sizeof(prog_t));
  prog->body = vector_new(20, sizeof(stmt_t));

  while (parser_peek(parser) != NULL && parser_peek(parser)->kind != TOK_EOF) {
    token_t *tok = parser_peek(parser);

    if (tok->kind == TOK_KW_EXIT) {
      exit_stmt_t exit_stmt = parser_parse_exit_stmt(parser);
      stmt_t stmt = {.kind = STMT_EXIT, .body = {.exit_stmt = exit_stmt}};
      vector_push(prog->body, &stmt);
      continue;
    }

    if (tok->kind == TOK_KW_LET) {
      var_decl_stmt_t var_decl_stmt = parser_parse_var_decl_stmt(parser);
      stmt_t stmt = {.kind = STMT_VAR_DECL,
                     .body = {.var_decl_stmt = var_decl_stmt}};
      vector_push(prog->body, &stmt);
      continue;
    }

    printf("Invalid token found during parsing: k: %d -> '%s' \n", tok->kind,
           tok->lexeme);
    exit(EXIT_FAILURE);
  }

  return prog;
}

exit_stmt_t parser_parse_exit_stmt(parser_t *parser) {
  parser_expect(parser, TOK_KW_EXIT); // eating exit keyword;

  expr_t exit_code_expr = parser_parse_expr(parser);
  exit_stmt_t exit_stmt = {.exit_code_expr = exit_code_expr};

  parser_expect(parser, TOK_SEMICOL);
  return exit_stmt;
}

var_decl_stmt_t parser_parse_var_decl_stmt(parser_t *parser) {
  token_t *var_kw = parser_expect(parser, TOK_KW_LET); // eating let keyword
  ident_expr_t ident_expr =
      parser_parse_ident_expr(parser); // parsing variable identifier

  token_t *curr = parser_peek(parser);
  opt_token_t type_kw = opt_token_none();

  if (curr && curr->kind == TOK_COLON) {
    parser_consume(parser); // consuming colon -> :
    token_t *type_tok = parser_parse_type_tok(parser);
    type_kw = opt_token_some(*type_tok);
  }

  token_t *assign_op = parser_expect(parser, TOK_ASSIGN);
  expr_t value = parser_parse_expr(parser); // parsing variable value (expr);
  parser_expect(parser, TOK_SEMICOL);       // expects end of line

  var_decl_stmt_t var_decl_stmt = {.type_kw = type_kw,
                                   .identifier = ident_expr,
                                   .value = value,
                                   .var_kw = *var_kw};

  return var_decl_stmt;
}

expr_t parser_parse_expr(parser_t *parser) {
  token_t *curr = parser_peek(parser);
  if (curr->kind == TOK_IDENT) {
    ident_expr_t ident_expr = parser_parse_ident_expr(parser);
    expr_t expr = {.kind = EXPR_IDENT, .body = {.ident_expr = ident_expr}};
    return expr;
  }

  lit_expr_t lit_expr = parser_parse_lit_expr(parser);
  expr_t expr = {.kind = EXPR_LIT, .body = {.lit_expr = lit_expr}};
  return expr;
}

lit_expr_t parser_parse_lit_expr(parser_t *parser) {
  token_t *lit = parser_parse_lit_tok(parser);
  lit_expr_t lit_expr = {.literal = *lit};
  return lit_expr;
}

ident_expr_t parser_parse_ident_expr(parser_t *parser) {
  token_t *ident = parser_expect(parser, TOK_IDENT);
  ident_expr_t ident_expr = {.identifier = *ident};
  return ident_expr;
}

token_t *parser_parse_lit_tok(parser_t *parser) {
  token_t *tok = parser_consume(parser);
  switch (tok->kind) {
  case TOK_NUM:
  case TOK_STR:
    return tok;
  default:
    printf("parser -> Invalid lit token received during parsing: (%d, %s)",
           tok->kind, tok->lexeme);
    exit(EXIT_FAILURE);
  }
}

token_t *parser_parse_type_tok(parser_t *parser) {
  token_t *tok = parser_consume(parser);
  switch (tok->kind) {
  case TOK_KW_INT:
  case TOK_KW_STR:
    return tok;
  default:
    printf("parser -> Invalid type token received during parsing: (%d, %s)",
           tok->kind, tok->lexeme);
    exit(EXIT_FAILURE);
  }
}

token_t *parser_peek(parser_t *parser) {
  if (parser->cursor >= parser->tokens->buf_size)
    return NULL;

  token_t *tok = (token_t *)vector_at(parser->tokens, parser->cursor);
  return tok;
}

token_t *parser_consume(parser_t *parser) {
  if (parser->cursor >= parser->tokens->buf_size)
    return NULL;

  token_t *tok = (token_t *)vector_at(parser->tokens, parser->cursor);
  parser->cursor++;

  return tok;
}

token_t *parser_expect(parser_t *parser, token_kind_t kind) {
  if (!parser_peek(parser)) {
    printf("parser -> Expected k: %d, but received null.\n", kind);
    exit(EXIT_FAILURE);
  }

  token_t *tok = parser_consume(parser);
  if (tok->kind == kind)
    return tok;

  if (tok->kind == TOK_EOF) {
    printf("parser -> Expected k: %d, but received eof.\n", kind);
    exit(EXIT_FAILURE);
  }

  printf("parser -> Expected k: %d, but received: %d.\n", kind, tok->kind);
  exit(EXIT_FAILURE);
}
