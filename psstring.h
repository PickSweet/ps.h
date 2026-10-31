#ifndef PSSTRING_H
#define PSSTRING_H

#include "pscore.h"
#include "pscollection.h"

typedef ps_list_t(char) ps_string_t;
typedef ps_slice_t(char) ps_string_view_t;

#define ps_str_init() ps_list_init(char)
#define ps_str_new() ps_list_new(char)
#define ps_str_info_free(str) ps_list_info_free(str)
#define ps_str_free(str) ps_list_free(str)

#define ps_str_size(str) ps_list_size(str)
#define ps_str_raw(str) ps_list_ptr(str)
#define ps_str_info(str) ps_list_info(str)
#define ps_str_concat(str1,str2) ps_list_append_list(str1,str2)
#define ps_str_concat_c_str(str, cstr) ps_list_append_many(str,cstr,strlen(cstr))

#define ps_str_to_view(str, start, size) ps_slice_init(ps_list_ptr(str) + (start), (size))
#define ps_str_to_new_view(str, start, size) ps_slice_new(char, ps_list_ptr(str) + (start), (size))

static PS_INLINE void _ps_string_append_fmt_internal(ps_string_t str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (len > 0) {
        size_t old_size = ps_list_size(str);
        if (old_size > 0 && ps_list_ptr(str)[old_size - 1] == '\0') {
            old_size--;
        }
        ps_list_reserve(str, old_size + len + 1);
        vsnprintf(ps_list_ptr(str) + old_size, len + 1, fmt, args);
        ps_list_size(str) = old_size + len + 1;
    }

    va_end(args);
}

#define ps_str_append_fmt(str, ...)\
    _ps_string_append_fmt_internal(str, __VA_ARGS__)

#endif // PSSTRING_H