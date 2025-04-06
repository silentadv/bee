#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "../common/optionals.h"
#include "../common/vector.h"

typedef enum {
    TOK_EOF = 0,
    TOK_EXIT,
    TOK_IDENT,
    TOK_NUM,
    TOK_SEMICOL,
} token_kind_t;

typedef struct {
    token_kind_t kind;
    char *lexeme;
} token_t;

typedef struct {
    size_t col, row, cursor;
    char *source;
} lexer_t;

lexer_t* lexer_read(const char *filename);
lexer_t* lexer_new(const char *source);
vector_t* lexer_lex(lexer_t *lexer);

opt_char_t lexer_peek(lexer_t *lexer);
opt_char_t lexer_consume(lexer_t *lexer);

#endif