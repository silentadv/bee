#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct {
    void *buf;
    size_t buf_size, element_size;
    size_t capacity;
} vector_t;

vector_t* vector_new(size_t capacity, size_t element_size);
void vector_destroy(vector_t *vector);

void vector_push(vector_t *vector, void *value);
void* vector_pop(vector_t *vector);
void* vector_at(vector_t *vector, size_t idx);

#endif