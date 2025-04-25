#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vector_t *vector_new(size_t capacity, size_t element_size) {
  vector_t *instance = malloc(sizeof(vector_t));
  instance->buf = malloc(capacity * element_size);
  instance->buf_size = 0;
  instance->element_size = element_size;
  instance->capacity = capacity;

  return instance;
}

void vector_push(vector_t *vector, void *value) {
  if (vector->buf_size >= vector->capacity) {
    void *new_buf =
        realloc(vector->buf, vector->capacity * 2 * vector->element_size);
    if (new_buf == NULL) {
      perror("vector -> realloc error");
      return;
    }
    vector->buf = new_buf;
    vector->capacity *= 2;
  }

  size_t offset = vector->element_size * vector->buf_size;
  memmove(vector->buf + offset, value, vector->element_size);
  vector->buf_size++;
}

void *vector_pop(vector_t *vector) {
  if (vector->buf_size <= 0)
    return NULL;

  size_t offset = vector->element_size * (vector->buf_size - 1);
  void *value = vector->buf + offset;

  vector->buf_size--;
  return value;
}

void *vector_at(vector_t *vector, size_t idx) {
  if (vector->buf_size <= 0 || idx >= vector->buf_size)
    return NULL;

  size_t offset = vector->element_size * idx;
  void *value = vector->buf + offset;

  return value;
}

void vector_destroy(vector_t *vector) {
  free(vector->buf);
  free(vector);
}
