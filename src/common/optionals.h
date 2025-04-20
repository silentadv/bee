#ifndef OPTIONALS_H
#define OPTIONALS_H

#include <stdbool.h>
#include "token.h"

typedef struct {
    bool has_value;
    char value;
} opt_char_t;

typedef struct {
    bool has_value;
    token_t value;
} opt_token_t;

static inline opt_char_t opt_char_some(char c) {
    return (opt_char_t){ .has_value = true, .value= c };
} 

static inline opt_char_t opt_char_none() {
    return (opt_char_t){ .has_value = false };
} 

static inline opt_token_t opt_token_some(token_t tok) {
    return (opt_token_t){ .has_value = true, .value = tok };
}

static inline opt_token_t opt_token_none() {
    return (opt_token_t){ .has_value = false };
}

#endif
