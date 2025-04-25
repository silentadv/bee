#include "hashmap.h"
#include <stdlib.h>
#include <string.h>

// FNV-1a hashing function
size_t hashmap_hash(string_view_t str) {
  size_t hash = 14695981039346656037UL;

  for (size_t i = 0; i < str.len; i++) {
    hash ^= (unsigned char)(str.data[i]);
    hash *= 1099511628211UL;
  }

  return hash;
}

hashmap_t *hashmap_new(size_t capacity) {
  hashmap_t *instance = malloc(sizeof(hashmap_t));
  instance->capacity = capacity;
  instance->buckets = calloc(capacity, sizeof(hashmap_bucket_t *));

  return instance;
}

void hashmap_destroy(hashmap_t *hm) {
  for (size_t i = 0; i < hm->capacity; i++) {
    hashmap_bucket_t *bucket = hm->buckets[i];
    hashmap_bucket_destroy(bucket);
  }

  free(hm->buckets);
  free(hm);
}

void hashmap_bucket_destroy(hashmap_bucket_t *bucket) {
  while (bucket != NULL) {
    hashmap_bucket_t *next = bucket->next;
    free(bucket->buf);
    free(bucket);
    bucket = next;
  }
}

hashmap_bucket_t *hashmap_get(hashmap_t *hm, string_view_t key) {
  size_t idx = hashmap_hash(key) % hm->capacity;
  hashmap_bucket_t *bucket = hm->buckets[idx];

  while (bucket != NULL) {
    if (bucket->key.len > 0 && sv_equals(bucket->key, key))
      return bucket;
    bucket = bucket->next;
  }

  return NULL;
}

void hashmap_insert(hashmap_t *hm, string_view_t key, void *buf,
                    size_t buf_size) {
  size_t idx = hashmap_hash(key) % hm->capacity;
  hashmap_bucket_t *bucket = hm->buckets[idx];

  if (bucket == NULL) {
    bucket = calloc(1, sizeof(hashmap_bucket_t));
    bucket->buf = malloc(buf_size);
    memcpy(bucket->buf, buf, buf_size);
    bucket->buf_size = buf_size;
    bucket->key = key;
    hm->buckets[idx] = bucket;
    return;
  }

  while (bucket->next != NULL)
    bucket = bucket->next;

  hashmap_bucket_t *new_bucket = calloc(1, sizeof(hashmap_bucket_t));
  new_bucket->buf = malloc(buf_size);
  memcpy(new_bucket->buf, buf, buf_size);
  new_bucket->buf_size = buf_size;
  new_bucket->key = key;

  bucket->next = new_bucket;
}

// Caller must free bucket->key, bucket->buf, and the bucket itself
hashmap_bucket_t *hashmap_remove(hashmap_t *hm, string_view_t key) {
  size_t idx = hashmap_hash(key) % hm->capacity;
  hashmap_bucket_t *bucket = hm->buckets[idx];
  hashmap_bucket_t *prev = NULL;

  while (bucket != NULL) {
    if (sv_equals(key, bucket->key)) {
      if (prev == NULL) {
        hm->buckets[idx] = bucket->next;
      } else {
        prev->next = bucket->next;
      }

      bucket->next = NULL;
      return bucket;
    }

    prev = bucket;
    bucket = bucket->next;
  }

  return NULL;
}
