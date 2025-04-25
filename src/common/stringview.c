#include "stringview.h"
#include <stdlib.h>
#include <string.h>

string_view_t sv_create(const char *buf, size_t len) {
  return (string_view_t){.data = buf, .len = len};
}

bool sv_equals(string_view_t a, string_view_t b) {
  if (a.len != b.len)
    return false;
  return memcmp(a.data, b.data, a.len) == 0;
}

bool sv_starts_with(string_view_t source, string_view_t prefix) {
  if (source.len < prefix.len)
    return false;
  string_view_t temp = sv_create(source.data, prefix.len);
  return sv_equals(temp, prefix);
}

string_view_t sv_shift(string_view_t *source, size_t amount) {
  if (source->len < amount)
    source->len = amount;

  string_view_t substr = sv_create(source->data, amount);
  source->data += amount;
  source->len -= amount;

  return substr;
}

string_view_t sv_pop(string_view_t *source, size_t amount) {
  if (source->len < amount)
    source->len = amount;

  string_view_t substr = sv_create(source->data + source->len - amount, amount);
  source->len -= amount;

  return substr;
}

string_view_t sv_range(string_view_t source, size_t start, size_t end) {
  if (end > source.len || start > end)
    return SV_LIT("");

  return sv_create(source.data + start, end - start);
}

string_view_t sv_copy_with_predicate(string_view_t source,
                                     string_view_predicate_t predicate) {
  size_t cursor = 0;
  while (cursor < source.len && predicate(source.data[cursor]))
    cursor++;

  return sv_create(source.data, cursor);
}
