#ifndef STMT_H
#define STMT_H

#include "expr.h"
#include "optionals.h"
#include "token.h"
#include <stddef.h>

typedef enum {
  STMT_EXIT,
  STMT_VAR_DECL,
  STMT_WRITE,
} stmt_kind_t;

typedef struct {
  expr_t exit_code_expr;
} exit_stmt_t;

typedef struct {
  expr_t message_expr;
} write_stmt_t;

typedef struct {
  ident_expr_t identifier;
  expr_t value;
  token_t var_kw;
  opt_token_t type_kw;
} var_decl_stmt_t;

typedef struct {
  stmt_kind_t kind;
  union {
    exit_stmt_t exit_stmt;
    var_decl_stmt_t var_decl_stmt;
    write_stmt_t write_stmt;
  } body;
} stmt_t;

#endif
