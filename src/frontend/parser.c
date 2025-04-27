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

    if (tok->kind == TOK_KW_LET) {
      var_decl_stmt_t var_decl_stmt = parser_parse_var_decl_stmt(parser);
      stmt_t stmt = {.kind = STMT_VAR_DECL,
                     .body = {.var_decl_stmt = var_decl_stmt}};
      vector_push(prog->body, &stmt);
      continue;
    }

    expr_stmt_t expr_stmt = parser_parse_expr_stmt(parser);
    stmt_t stmt = {.kind = STMT_EXPR, .body = {.expr_stmt = expr_stmt}};
    vector_push(prog->body, &stmt);
  }

  return prog;
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

  parser_expect(parser, TOK_ASSIGN);
  expr_t value = parser_parse_expr(parser); // parsing variable value (expr);
  parser_expect(parser, TOK_SEMICOL);       // expects end of line

  var_decl_stmt_t var_decl_stmt = {.type_kw = type_kw,
                                   .identifier = ident_expr,
                                   .value = value,
                                   .var_kw = *var_kw};

  return var_decl_stmt;
}

expr_stmt_t parser_parse_expr_stmt(parser_t *parser) {
  expr_t expr = parser_parse_expr(parser);
  parser_expect(parser, TOK_SEMICOL);

  expr_stmt_t expr_stmt = {.expr = expr};
  return expr_stmt;
}

expr_t parser_parse_expr(parser_t *parser) {
  token_t *curr = parser_peek(parser);
  if (curr->kind == TOK_IDENT) {
    ident_expr_t ident_expr = parser_parse_ident_expr(parser);
    expr_t expr = {.kind = EXPR_IDENT, .body = {.ident_expr = ident_expr}};
    if (parser_peek(parser)->kind != TOK_BANG)
      return expr;

    call_expr_t call_expr = parser_parse_call_expr(parser, ident_expr);
    expr = (expr_t){.kind = EXPR_CALL, .body = {.call_expr = call_expr}};
    return expr;
  }

  lit_expr_t lit_expr = parser_parse_lit_expr(parser);
  expr_t expr = {.kind = EXPR_LIT, .body = {.lit_expr = lit_expr}};
  return expr;
}

lit_expr_t parser_parse_lit_expr(parser_t *parser) {
  token_t *lit = parser_parse_lit_tok(parser);
  lit_expr_t lit_expr = {.token = *lit};
  return lit_expr;
}

ident_expr_t parser_parse_ident_expr(parser_t *parser) {
  token_t *ident = parser_expect(parser, TOK_IDENT);
  ident_expr_t ident_expr = {.token = *ident};
  return ident_expr;
}

call_expr_t parser_parse_call_expr(parser_t *parser, ident_expr_t identifier) {
  parser_expect(parser, TOK_BANG);
  parser_expect(parser, TOK_LPAREN);

  vector_t *args = vector_new(25, sizeof(expr_t));

  do {
    if (parser_peek(parser)->kind == TOK_COMMA)
      parser_consume(parser);

    expr_t arg = parser_parse_expr(parser);
    vector_push(args, &arg);
  } while (parser_peek(parser)->kind == TOK_COMMA);

  parser_expect(parser, TOK_RPAREN);

  call_expr_t call_expr = {
      .identifier = identifier, .builtin = true, .args = args};
  return call_expr;
}

token_t *parser_parse_lit_tok(parser_t *parser) {
  token_t *tok = parser_consume(parser);
  switch (tok->kind) {
  case TOK_NUM:
  case TOK_STR:
    return tok;
  default:
    fprintf(stderr,
            "parser -> Invalid lit token received during parsing: (%d, " SV_FMT
            ")",
            tok->kind, SV_ARG(tok->lexeme));
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
    fprintf(stderr,
            "parser -> Invalid type token received during parsing: (%d, " SV_FMT
            ")",
            tok->kind, SV_ARG(tok->lexeme));
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
    fprintf(stderr, "parser -> Expected k: %d, but received null.\n", kind);
    exit(EXIT_FAILURE);
  }

  token_t *tok = parser_consume(parser);
  if (tok->kind == kind)
    return tok;

  if (tok->kind == TOK_EOF) {
    fprintf(stderr, "parser -> Expected k: %d, but received eof.\n", kind);
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "parser -> Expected k: %d, but received: %d.\n", kind,
          tok->kind);
  exit(EXIT_FAILURE);
}
