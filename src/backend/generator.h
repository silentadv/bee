#ifndef GENERATOR_H
#define GENERATOR_H

#include "../common/hashmap.h"
#include "../common/rodata.h"
#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include <stddef.h>
#include <stdio.h>

typedef struct {
  prog_t *prog;
  hashmap_t *symbols;
  rodata_t *rodata;
  FILE *output;
  size_t cursor, stack_size;
} generator_t;

generator_t *generator_new(prog_t *prog, hashmap_t *symbols);
void generator_destroy(generator_t *generator);

void generator_gen(generator_t *generator);
void generator_gen_stmt(generator_t *generator);
void generator_gen_exit_stmt(generator_t *generator);
void generator_gen_var_decl_stmt(generator_t *generator);
void generator_gen_write_stmt(generator_t *generator);

void generator_gen_expr(generator_t *generator, expr_t *expr);
void generator_gen_lit_expr(generator_t *generator, lit_expr_t *expr);
void generator_gen_ident_expr(generator_t *generator, ident_expr_t *expr);

stmt_t *generator_peek(generator_t *generator);
stmt_t *generator_consume(generator_t *generator);
stmt_t *generator_expect(generator_t *generator, stmt_kind_t stmt_kind);

#endif
