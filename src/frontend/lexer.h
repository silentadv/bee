#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "../common/optionals.h"
#include "../common/stringview.h"
#include "../common/token.h"
#include "../common/vector.h"

typedef struct {
  size_t col, row, cursor;
  string_view_t source;
} lexer_t;

lexer_t *lexer_read(const char *filename);
lexer_t *lexer_new(string_view_t source);
void lexer_destroy(lexer_t *lexer);
vector_t *lexer_lex(lexer_t *lexer);
token_t lexer_tokenize(lexer_t *lexer);

#endif
