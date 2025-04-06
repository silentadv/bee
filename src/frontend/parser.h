#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "../common/vector.h"
#include "../frontend/lexer.h"

typedef enum {
    STMT_EXIT,
} stmt_kind_t;

typedef struct {
    token_t exit_token; // exit code token
} exit_stmt_t;

typedef struct {
    stmt_kind_t kind;
    union {
        exit_stmt_t exit_stmt;
    } body;
} stmt_t;

typedef struct {
    vector_t *body;
} prog_t;

typedef struct {
    vector_t *tokens;
    size_t cursor;
} parser_t;

parser_t* parser_new(vector_t *tokens);
prog_t* parser_parse(parser_t *parser);

token_t* parser_peek(parser_t *parser);
token_t* parser_consume(parser_t *parser);
token_t* parser_expect(parser_t *parser, token_kind_t kind);

#endif