#include "rodata.h"
#include "hashmap.h"
#include "stringview.h"

rodata_t *rodata_new(size_t capacity) {
  rodata_t *instance = malloc(sizeof(rodata_t));
  instance->hm = hashmap_new(capacity);
  instance->len = 0;
  return instance;
}

void rodata_set(rodata_t *rod, string_view_t val) {
  hashmap_bucket_t *bucket = hashmap_get(rod->hm, val);
  if (bucket != NULL)
    return;

  rodata_value_t entry = {.str = val, .id = rod->len++};
  hashmap_insert(rod->hm, val, &entry, sizeof(entry));
}

rodata_value_t *rodata_get(rodata_t *rod, string_view_t key) {
  hashmap_bucket_t *bucket = hashmap_get(rod->hm, key);
  if (bucket == NULL)
    return NULL;
  return (rodata_value_t *)bucket->buf;
}

void rodata_emit(rodata_t *rod, FILE *output) {
  hashmap_t *hm = rod->hm;
  fprintf(output, "section .rodata\n");

  for (size_t i = 0; i < hm->capacity; i++) {
    hashmap_bucket_t *bucket = hm->buckets[i];

    while (bucket != NULL) {
      rodata_value_t *value = (rodata_value_t *)bucket->buf;
      fprintf(output, " .LC%zu: db \"" SV_FMT "\"\n", value->id,
              SV_ARG(value->str));
      fprintf(output, " .LC%zu_len equ $ - .LC%zu\n", value->id, value->id);
      bucket = bucket->next;
    }
  }
}
