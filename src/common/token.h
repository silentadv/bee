#ifndef TOKEN_H
#define TOKEN_H

#include "stringview.h"

typedef enum {
  TOK_EOF = 0, // end of file

  // Keywords
  TOK_KW_INT, // int type keyword
  TOK_KW_STR, // str type keyword
  TOK_KW_LET, // let keyword

  // Literals
  TOK_IDENT, // identifier
  TOK_NUM,   // integer number
  TOK_STR,   // string

  // Punctuactors
  TOK_COMMA,   // ,
  TOK_SEMICOL, // ;
  TOK_COLON,   // :
  TOK_ASSIGN,  // =
  TOK_LPAREN,  // (
  TOK_RPAREN,  // )
  TOK_BANG,    // !
} token_kind_t;

typedef struct {
  token_kind_t kind;
  string_view_t lexeme;
} token_t;

#endif
