#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

// FNV-1a hashing function
size_t hashmap_hash(const char *str) {
    size_t hash = 14695981039346656037UL;

    while (*str) {
        hash ^= (unsigned char)(*str++);
        hash *= 1099511628211UL;
    }

    return hash;
}

hashmap_t* hashmap_new(size_t capacity) {
    hashmap_t *instance = malloc(sizeof(hashmap_t));
    instance->capacity = capacity;
    instance->buckets = calloc(capacity, sizeof(hashmap_bucket_t*));

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
        free(bucket->key);
        free(bucket);
        bucket = next;
    }
}

hashmap_bucket_t* hashmap_get(hashmap_t *hm, const char *key) {
    size_t idx = hashmap_hash(key) % hm->capacity;
    hashmap_bucket_t *bucket = hm->buckets[idx];

    while (bucket != NULL) {
        if (bucket->key && strcmp(bucket->key, key) == 0)
            return bucket;
        bucket = bucket->next;
    }
        
    return NULL;
}

void hashmap_insert(hashmap_t *hm, const char *key, void *buf, size_t buf_size) {
    size_t idx = hashmap_hash(key) % hm->capacity;
    hashmap_bucket_t *bucket = hm->buckets[idx];

    if (bucket == NULL) {
        bucket = calloc(1, sizeof(hashmap_bucket_t));
        bucket->buf = malloc(buf_size);
        memcpy(bucket->buf, buf, buf_size);
        bucket->buf_size = buf_size;
        bucket->key = strdup(key);
        hm->buckets[idx] = bucket;
        return;
    }

    while (bucket->next != NULL)
        bucket = bucket->next;

    hashmap_bucket_t *new_bucket = calloc(1, sizeof(hashmap_bucket_t));
    new_bucket->buf = malloc(buf_size);
    memcpy(new_bucket->buf, buf, buf_size);
    new_bucket->buf_size = buf_size;
    new_bucket->key = strdup(key);

    bucket->next = new_bucket;
}

// Caller must free bucket->key, bucket->buf, and the bucket itself
hashmap_bucket_t* hashmap_remove(hashmap_t *hm, const char *key) {
    size_t idx = hashmap_hash(key) % hm->capacity;
    hashmap_bucket_t *bucket = hm->buckets[idx];
    hashmap_bucket_t *prev = NULL;

    while (bucket != NULL) {
        if (strcmp(bucket->key, key) == 0) {
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