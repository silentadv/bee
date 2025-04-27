#ifndef STMT_H
#define STMT_H

#include "expr.h"
#include "optionals.h"
#include "token.h"
#include <stddef.h>

typedef enum {
  STMT_VAR_DECL,
  STMT_EXPR,
} stmt_kind_t;

typedef struct stmt_t stmt_t;
typedef struct var_decl_stmt_t var_decl_stmt_t;
typedef struct expr_stmt_t expr_stmt_t;

struct var_decl_stmt_t {
  ident_expr_t identifier;
  expr_t value;
  token_t var_kw;
  opt_token_t type_kw;
};

struct expr_stmt_t {
  expr_t expr;
};

struct stmt_t {
  stmt_kind_t kind;
  union {
    var_decl_stmt_t var_decl_stmt;
    expr_stmt_t expr_stmt;
  } body;
};

#endif
