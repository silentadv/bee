#include "stringview.h"
#include <stdlib.h>
#include <string.h>

string_view_t sv_create(const char *buf, size_t len) {
    return (string_view_t) { .data = buf, .len = len };
}

bool sv_equals(string_view_t a, string_view_t b) {
    if (a.len != b.len) return false;
    return memcmp(a.data, b.data, a.len) == 0;
}

bool sv_starts_with(string_view_t source, string_view_t prefix) {
    if (source.len < prefix.len) return false;
    string_view_t temp = sv_create(source.data, prefix.len);
    return sv_equals(temp, prefix);
}
