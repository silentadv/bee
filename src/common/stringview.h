#ifndef STRINGVIEW_H
#define STRINGVIEW_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  const char *data;
  size_t len;
} string_view_t;

typedef bool (*string_view_predicate_t)(const char c);

#define SV(__lit__) sv_create(__lit__, strlen(__lit__))
#define SV_LIT(__lit__)                                                        \
  (string_view_t) { .data = (__lit__), .len = strlen(__lit__) }

#define SV_FMT "%.*s"
#define SV_ARG(__sv__) (int)(__sv__).len, (__sv__).data
#define sv_slice(source, amount) sv_range((source), (amount), (source).len)
#define sv_rslice(source, amount) sv_range((source), 0, (amount))

string_view_t sv_create(const char *buf, size_t len);
bool sv_equals(string_view_t a, string_view_t b);
bool sv_starts_with(string_view_t source, string_view_t prefix);
string_view_t sv_shift(string_view_t *source, size_t amount);
string_view_t sv_pop(string_view_t *source, size_t amount);
string_view_t sv_range(string_view_t source, size_t start, size_t end);
string_view_t sv_copy_with_predicate(string_view_t source,
                                     string_view_predicate_t predicate);
char *sv_cstr(string_view_t source);
#endif
