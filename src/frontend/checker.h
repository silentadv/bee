#ifndef CHECKER_H
#define CHECKER_H

#include "../common/hashmap.h"
#include "parser.h"
#include <stddef.h>

typedef enum { TYPE_INT, TYPE_STR } type_t;

typedef struct {
  ident_expr_t ident_expr;
  expr_t value;
  type_t type;
  size_t stack_pos;
} symbol_t;

typedef struct {
  size_t cursor;
  prog_t *program;
  hashmap_t *symbols;
} checker_t;

checker_t *checker_new(prog_t *program);
hashmap_t *checker_check(checker_t *checker);
void checker_check_stmt(checker_t *checker);

type_t checker_check_expr(checker_t *checker, expr_t *expr);
type_t checker_check_ident_expr(checker_t *checker, expr_t *expr);
type_t checker_check_lit_expr(checker_t *checker, expr_t *expr);

void checker_expect(type_t expects, type_t received);

stmt_t *checker_peek(checker_t *checker);
stmt_t *checker_consume(checker_t *checker);

#endif
