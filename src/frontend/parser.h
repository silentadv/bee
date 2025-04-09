#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "../common/token.h"
#include "../common/vector.h"
#include "../common/stmt.h"
#include "../common/expr.h"
#include "../frontend/lexer.h"

typedef struct {
  vector_t *body;
} prog_t;

typedef struct {
  vector_t *tokens;
  size_t cursor;
} parser_t;

parser_t *parser_new(vector_t *tokens);
void parser_destroy(parser_t *parser);

prog_t *parser_parse(parser_t *parser);
exit_stmt_t parser_parse_exit_stmt(parser_t *parser);
var_decl_stmt_t parser_parse_var_decl_stmt(parser_t *parser);

expr_t parser_parse_expr(parser_t *parser);
lit_expr_t parser_parse_lit_expr(parser_t *parser);
ident_expr_t parser_parse_ident_expr(parser_t *parser);

token_t *parser_peek(parser_t *parser);
token_t *parser_consume(parser_t *parser);
token_t *parser_expect(parser_t *parser, token_kind_t kind);

#endif
