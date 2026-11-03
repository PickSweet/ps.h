/*
MIT License

Copyright (c) 2026 David J. Meagher

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef _H_PS_COLLECTIONS_H_
#define _H_PS_COLLECTIONS_H_

#include "pscore.h"
#include <stdio.h>
#include <stdarg.h>

typedef ptrdiff_t isize_t;

#define _ps_array_size(array) (sizeof(array) / sizeof((array)[0]))

typedef struct ps_slice_t {
    void *items;
    size_t size;
} ps_slice_t;

#define _ps_slice_size(slice_ptr) (((size_t*)(slice_ptr))[1])
#define _ps_slice_items(slice_ptr) (*(slice_ptr))

#define _ps_get_slice(items, size)\
    (ps_slice_t){\
        (void*)(items),\
        size,\
    }

#define _ps_slice_skip(slice_ptr,n)\
    _ps_blk(\
        size_t _n = (n);\
        _ps_assert((_n <= _ps_slice_size(slice_ptr)) && "Out of bounds!");\
        _ps_slice_items(slice_ptr) += _n;\
        _ps_slice_size(slice_ptr) -= n;\
    )

#define _ps_slice_shrink(slice_ptr,n)\
    _ps_blk(\
        size_t _n = (n);\
        _ps_assert((_n <= _ps_slice_size(slice_ptr)) && "Out of bounds!");\
        _ps_slice_size(slice_ptr) -= _n;\
    )


#define _ps_slice_first(slice_ptr) (_ps_slice_items(slice_ptr)[_ps_assert(_ps_slice_size(slice_ptr) > 0), 0])
#define _ps_slice_last(slice_ptr) (_ps_slice_items(slice_ptr)[_ps_assert(_ps_slice_size(slice_ptr) > 0), _ps_slice_size(slice_ptr) - 1])
#define _ps_slice_get(slice_ptr, index) (_ps_slice_items(slice_ptr)[_ps_assert((index) < _ps_slice_size(slice_ptr) && "Index out of bounds!"), (index)])
#define _ps_slice_set(slice_ptr, index, value) (_ps_slice_get(slice_ptr,index) = (value))

#define _ps_slice_sub(slice_ptr, start, end)\
    (ps_slice_t){\
        (void*)(_ps_slice_items(slice_ptr) + (start)),\
        (end) - (start),\
    }

#define _ps_slice_foreach(slice_ptr, iter)\
    for (\
        _ps_typeof(_ps_slice_items(slice_ptr)) iter = _ps_slice_items(slice_ptr);\
        iter < _ps_slice_items(slice_ptr) + _ps_slice_size(slice_ptr);\
        ++iter\
    )

typedef struct ps_list_t {
    void *items;
    size_t capacity;
    size_t size;
} ps_list_t;

#define _ps_list_capacity(list_p) (((size_t*)(list_p))[1])
#define _ps_list_size(list_p) (((size_t*)(list_p))[2])
#define _ps_list_items(list_p) (*(list_p))

#define _ps_list_free(list_p)\
    _ps_blk(\
        if ((list_p) != NULL && _ps_list_items(list_p) != NULL) {\
            _ps_free(_ps_list_items(list_p));\
            _ps_list_items(list_p) = NULL;\
            _ps_list_capacity(list_p) = 0;\
            _ps_list_size(list_p) = 0;\
        }\
    )


#define _ps_list_reserve_precise(list_p, new_capacity)\
    _ps_blk(\
        if (_ps_list_capacity(list_p) < (new_capacity)){\
            void *_new_ptr = _ps_realloc(_ps_list_items(list_p), (new_capacity) * sizeof(*_ps_list_items(list_p)));\
            _ps_assert(_new_ptr != NULL && "Failed to reallocate memory for list!");\
            _ps_list_items(list_p) = (_ps_typeof(_ps_list_items(list_p)))(_new_ptr);\
            _ps_list_capacity(list_p) = (new_capacity);\
        }\
    )

#define _ps_list_reserve(list_p, new_capacity)\
    _ps_blk(\
        size_t *list_capacity_ptr = &_ps_list_capacity(list_p);\
        if (*list_capacity_ptr < (new_capacity)){\
            size_t target_capacity;\
            if (*list_capacity_ptr == 0){\
                target_capacity = 1;\
            } else {\
                target_capacity = *list_capacity_ptr;\
            }\
            while (new_capacity > target_capacity){\
                target_capacity *= 2;\
            }\
            _ps_list_reserve_precise(list_p, target_capacity);\
        }\
    )

#define _ps_list_resize(list_p, new_size)\
    _ps_blk(\
        _ps_list_reserve(list_p, new_size);\
        _ps_list_size(list_p) = (new_size);\
    )

#define _ps_list_append_one(list_p, value)\
    _ps_blk(\
        _ps_list_reserve(list_p, _ps_list_size(list_p) + 1);\
        _ps_list_items(list_p)[_ps_list_size(list_p)] = (value);\
        _ps_list_size(list_p)++;\
    )
#define _ps_list_append_many(list_p, values, count)\
    _ps_blk(\
        _ps_assert(sizeof((values)[0]) == sizeof(*_ps_list_items(list_p)) && "Appending values to list of incompatible type.");\
        _ps_list_reserve(list_p,_ps_list_size(list_p) + (count));\
        _ps_memcpy(_ps_list_items(list_p) + _ps_list_size(list_p), values, (count) * sizeof(*_ps_list_items(list_p)));\
        _ps_list_size(list_p)+=(count);\
    )

#define _ps_list_append_array(list_p,array)\
    _ps_list_append_many(list_p, array, _ps_array_size(array))

#define _ps_list_append_list(list_a_ptr,list_b_ptr)\
    _ps_list_append_many(list_a_ptr, _ps_list_items(list_b_ptr), _ps_list_size(list_b_ptr))

#define _ps_list_append(list_p, ...)\
    _ps_blk(\
        _ps_typeof(*_ps_list_items(list_p)) values[] = {__VA_ARGS__};\
        _ps_list_append_array(list_p, values);\
    )

#define _ps_list_pop(list_p) (_ps_list_items(list_p)[_ps_assert(_ps_list_size(list_p) > 0), --_ps_list_size(list_p)])
#define _ps_list_first(list_p) (_ps_list_items(list_p)[_ps_assert(_ps_list_size(list_p) > 0), 0])
#define _ps_list_last(list_p) (_ps_list_items(list_p)[_ps_assert(_ps_list_size(list_p) > 0), _ps_list_size(list_p) - 1])
#define _ps_list_get(list_p, index) (_ps_list_items(list_p)[_ps_assert((index) < _ps_list_size(list_p) && "Index out of bounds!"), (index)])
#define _ps_list_set(list_p, index, value) (_ps_list_get(list_p,index) = (value))

#define _ps_list_swap_and_pop(list_p, index, out)\
    _ps_blk(\
        (out) = _ps_list_get(list_p, index);\
        _ps_list_get(list_p,index) = _ps_list_get(_ps_list_size(list_p) - 1);\
        _ps_list_size(list_p)--;\
    )

#define _ps_list_unordered_remove(list_p, index)\
    _ps_blk(\
        _ps_typeof(*_ps_list_items(list_p)) out;\
        _ps_list_swap_and_pop(list_p, index, out);\
    )

#define _ps_list_clear(list_p) _ps_list_size(list_p) = 0
#define _ps_list_is_empty(list_p) (_ps_list_size(list_p) == 0)

#define _ps_list_foreach(list_p, iter)\
    for (\
        _ps_typeof(_ps_list_items(list_p)) iter = _ps_list_items(list_p);\
        iter < _ps_list_items(list_p) + _ps_list_size(list_p);\
        ++iter\
    )


PS_DEF void ps_str_append_fmt( char **str, const char *fmt, ...) _ps_printf_format(2, 3);
PS_DEF isize_t ps_str_find_last(char **str, const char *delim);

#ifdef PS_COLLECTIONS_IMP
PS_DEF void ps_str_append_fmt(char** str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (len > 0) {
        size_t old_size = _ps_list_size(str);
        if (old_size > 0 && _ps_list_items(str)[old_size - 1] == '\0') {
            old_size--;
        }
        _ps_list_reserve(str, old_size + len + 1);
        vsnprintf(_ps_list_items(str) + old_size, len + 1, fmt, args);
        _ps_list_size(str) = old_size + len + 1;
    }

    va_end(args);
}

PS_DEF isize_t ps_str_find_last(char **str, const char *delim){
    size_t delim_size = strlen(delim);
    isize_t start = _ps_list_size(str) - delim_size;
    char* check = (char *)_ps_realloc(NULL, delim_size + 1);
    check[delim_size] = 0;
    do {
        _ps_memcpy(check, _ps_list_items(str) + start, delim_size);
        if (!strcmp(check, delim)){
            return start;
        }
    } while (--start);
    _ps_free(check);
    return start;
}

#endif // PS_COLLECTIONS_IMP
#endif // _H_PS_COLLECTIONS_H_