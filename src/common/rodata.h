#ifndef RODATA_H
#define RODATA_H

#ifndef __RODATA_LABEL_SIZE__
#endif

#include "expr.h"
#include "hashmap.h"
#include "stmt.h"
#include "stringview.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  string_view_t str;
  size_t id;
} rodata_value_t;

typedef struct {
  hashmap_t *hm;
  size_t len;
} rodata_t;

rodata_t *rodata_new(size_t capacity);
void rodata_set(rodata_t *rod, string_view_t val);
void rodata_emit(rodata_t *rod, FILE *output);
rodata_value_t *rodata_get(rodata_t *rod, string_view_t key);

#endif
