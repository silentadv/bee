#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "../common/optionals.h"
#include "../common/vector.h"
#include "../common/token.h"

typedef struct {
    size_t col, row, cursor;
    char *source;
} lexer_t;

lexer_t* lexer_read(const char *filename);
lexer_t* lexer_new(const char *source);
void lexer_destroy(lexer_t *lexer);
vector_t* lexer_lex(lexer_t *lexer);

opt_char_t lexer_peek(lexer_t *lexer);
opt_char_t lexer_consume(lexer_t *lexer);

#endif
