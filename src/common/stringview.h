#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    const char *data;
    size_t len;
} string_view_t;

#define SV(__lit__) sv_create(__lit__, sizeof(__lit__) - 1)
#define SV_LIT(__lit__) (string_view_t) { .data = (__lit__), .len = sizeof(__lit__) - 1 }

#define SV_FMT "%.*s"
#define SV_ARG(__sv__) (int) (__sv__).len, (__sv__).data

string_view_t sv_create(const char *buf, size_t len);
bool sv_equals(string_view_t a, string_view_t b);
bool sv_starts_with(string_view_t source, string_view_t prefix);

#endif

