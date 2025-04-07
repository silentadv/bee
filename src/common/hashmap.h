#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct hashmap_bucket_t {
    char *key;
    void *buf;
    size_t buf_size;
    struct hashmap_bucket_t *next;
} hashmap_bucket_t;

typedef struct {
    hashmap_bucket_t **buckets;
    size_t capacity;
} hashmap_t;

hashmap_t* hashmap_new(size_t capacity);
void hashmap_destroy(hashmap_t *hm);
void hashmap_bucket_destroy(hashmap_bucket_t *bucket);

void hashmap_insert(hashmap_t *hm, const char *key, void *buf, size_t buf_size);
hashmap_bucket_t* hashmap_get(hashmap_t *hm, const char *key);

// Caller must free bucket->key, bucket->buf, and the bucket itself
hashmap_bucket_t* hashmap_remove(hashmap_t *hm, const char *key);

// FNV-1a hashing function
size_t hashmap_hash(const char *str); 



#endif