#ifndef EXPR_H
#define EXPR_H

#include "token.h"
#include "vector.h"

typedef enum {
  EXPR_LIT,
  EXPR_IDENT,
  EXPR_CALL,
} expr_kind_t;

typedef struct expr_t expr_t;
typedef struct lit_expr_t lit_expr_t;
typedef struct ident_expr_t ident_expr_t;
typedef struct call_expr_t call_expr_t;

struct lit_expr_t {
  token_t token;
};

struct ident_expr_t {
  token_t token;
};

struct call_expr_t {
  ident_expr_t identifier;
  bool builtin;
  vector_t *args;
};

struct expr_t {
  expr_kind_t kind;
  union {
    lit_expr_t lit_expr;
    ident_expr_t ident_expr;
    call_expr_t call_expr;
  } body;
};

#endif
