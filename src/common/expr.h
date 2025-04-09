#ifndef EXPR_H
#define EXPR_H

#include "token.h"

typedef enum {
  EXPR_LIT,
  EXPR_IDENT,
} expr_kind_t;

typedef struct {
  token_t literal;
} lit_expr_t;

typedef struct {
  token_t identifier;
} ident_expr_t;

typedef struct {
  expr_kind_t kind;
  union {
      lit_expr_t lit_expr;
      ident_expr_t ident_expr;
  } body;
} expr_t;

#endif
