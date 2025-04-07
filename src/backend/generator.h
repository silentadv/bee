#ifndef GENERATOR_H
#define GENERATOR_H

#include <stddef.h>
#include <stdio.h>
#include "../frontend/lexer.h"
#include "../frontend/parser.h"

typedef struct {
    prog_t *prog;
    FILE *output;
    size_t cursor;
} generator_t ;

generator_t* generator_new(prog_t *prog);
void generator_destroy(generator_t *generator);

void generator_gen(generator_t *generator);
void generator_gen_stmt(generator_t *generator);
void generator_gen_exit_stmt(generator_t *generator);

stmt_t* generator_peek(generator_t *generator);
stmt_t* generator_consume(generator_t *generator);
stmt_t* generator_expect(generator_t *generator, stmt_kind_t stmt_kind);


#endif