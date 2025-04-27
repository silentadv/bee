#ifndef RUNTIME_H
#define RUNTIME_H

#include "stringview.h"

typedef enum { TYPE_INT, TYPE_STR, TYPE_VOID } type_t;

typedef struct {
  string_view_t name;
  type_t type;
} runtime_reg_t;

typedef struct {
  string_view_t name;
  size_t input_count;
  size_t output_count;
  runtime_reg_t *inputs;
  runtime_reg_t *outputs;
} runtime_fun_t;

#endif
