#ifndef OPTIONALS_H
#define OPTIONALS_H

#include <stdbool.h>

typedef struct {
    bool has_value;
    char value;
} opt_char_t;

typedef struct {
    bool has_value;
    void *value;
} opt_any_t; 

static inline opt_char_t opt_char_some(char c) {
    return (opt_char_t){ .has_value = true, .value= c };
} 

static inline opt_char_t opt_char_none() {
    return (opt_char_t){ .has_value = false };
} 

static inline opt_any_t opt_any_some(void* val) {
    return (opt_any_t){ .has_value = true, .value = val };
} 

static inline opt_any_t opt_any_none() {
    return (opt_any_t){ .has_value= false, .value = NULL };
} 


#endif