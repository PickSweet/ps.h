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

#ifdef PS_STRIP_PREFIX
#ifdef COLLECTIONS_IMP
#define PS_COLLECTIONS_IMP
#endif /* COLLECTIONS_IMP */
#endif /* PS_STRIP_PREFIX */

#ifdef PS_COLLECTIONS_IMP
#define PS_CORE_IMP
#define PS_LOG_IMP
#endif /* PS_COLLECTIONS_IMP */

#include "ps_core.h"
#include "ps_log.h"
#include <stdio.h>
#include <stdarg.h>


#define _ps_array_size(array) (sizeof(array) / sizeof((array)[0]))

typedef struct ps_slice_t {
    void *items;
    ps_half_t size;
} ps_slice_t;

typedef struct ps_str_view_t {
    char *items;
    ps_half_t size;
} ps_str_view_t;

#define _ps_slice_size(slice_p) (((ps_half_t*)(((ps_size_t*)(slice_p)) + 1))[0])
#define _ps_slice_item_size(slice_p) (sizeof(*(*(slice_p))))
#define _ps_slice_items(slice_p) (*(slice_p))

#define _ps_get_slice(items, size)\
    (ps_slice_t){\
        (void*)(items),\
        size,\
    }

#define _ps_slice_skip(slice_p,n)\
    _ps_blk(\
        ps_half_t _n = (n);\
        _ps_do_assert((_n <= _ps_slice_size(slice_p)) && "Out of bounds!");\
        _ps_slice_items(slice_p) += _n;\
        _ps_slice_size(slice_p) -= _n;\
    )

#define _ps_slice_shrink(slice_p,n)\
    _ps_blk(\
        ps_half_t _n = (n);\
        _ps_do_assert((_n <= _ps_slice_size(slice_p)) && "Out of bounds!");\
        _ps_slice_size(slice_p) -= _n;\
    )


#define _ps_slice_first(slice_p) (_ps_slice_items(slice_p)[_ps_do_assert(_ps_slice_size(slice_p) > 0), 0])
#define _ps_slice_last(slice_p) (_ps_slice_items(slice_p)[_ps_do_assert(_ps_slice_size(slice_p) > 0), _ps_slice_size(slice_p) - 1])
#define _ps_slice_get(slice_p, index) (_ps_slice_items(slice_p)[_ps_do_assert((index) < _ps_slice_size(slice_p) && "Index out of bounds!"), (index)])
#define _ps_slice_set(slice_p, index, value) (_ps_slice_get(slice_p,index) = (value))

#define _ps_slice_sub(slice_p, start, end)\
    (ps_slice_t){\
        (void*)(_ps_slice_items(slice_p) + (start)),\
        (end) - (start),\
    }

#define _ps_slice_foreach(slice_p, iter)\
    for (\
        _ps_typeof(_ps_slice_items(slice_p)) iter = _ps_slice_items(slice_p);\
        iter < _ps_slice_items(slice_p) + _ps_slice_size(slice_p);\
        ++iter\
    )


#define _ps_slice_to_unterminated(slice_p) (_ps_slice_items(slice_p))
#define _ps_slice_to_terminated(slice_p, terminal) (_ps_typeof(_ps_slice_items(slice_p)))__i_ps_to_terminated(_ps_slice_items(slice_p), &(terminal),_ps_slice_item_size(slice_p), _ps_slice_size(slice_p))
typedef struct ps_list_t {
    void *items;
    ps_half_t capacity;
    ps_half_t size;
} ps_list_t;

typedef struct ps_str_t {
    char *items;
    ps_half_t capacity;
    ps_half_t size;
} ps_str_t;
typedef struct ps_list_list_t {
    ps_list_t *items;
    ps_half_t capacity;
    ps_half_t size;
} ps_list_list_t;

typedef struct ps_list_slice_t {
    ps_slice_t *items;
    ps_half_t capacity;
    ps_half_t size;
} ps_list_slice_t;

#define _ps_list_capacity(list_p) (((ps_half_t*)(((ps_size_t*)(list_p)) + 1))[0])
#define _ps_list_size(list_p) (((ps_half_t*)(((ps_size_t*)(list_p)) + 1))[1])
#define _ps_list_item_size(list_p) (sizeof(*(*(list_p))))
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
            void *_new_ptr = _ps_realloc(_ps_list_items(list_p), (new_capacity) * _ps_list_item_size(list_p));\
            _ps_do_assert(_new_ptr != NULL && "Failed to reallocate memory for list!");\
            _ps_list_items(list_p) = (_ps_typeof(_ps_list_items(list_p)))(_new_ptr);\
            _ps_list_capacity(list_p) = (new_capacity);\
        }\
    )

#define _ps_list_reserve(list_p, new_capacity)\
    _ps_blk(\
        ps_half_t list_capacity = _ps_list_capacity(list_p);\
        if (list_capacity < (new_capacity)){\
            ps_half_t target_capacity;\
            if (list_capacity == 0){\
                target_capacity = 1;\
            } else {\
                target_capacity = list_capacity;\
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
        _ps_do_assert(sizeof((values)[0]) == _ps_list_item_size(list_p) && "Appending values to list of incompatible type.");\
        _ps_list_reserve(list_p,_ps_list_size(list_p) + (count));\
        _ps_memcpy(_ps_list_items(list_p) + _ps_list_size(list_p), values, (count) * _ps_list_item_size(list_p));\
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

#define _ps_list_pop(list_p) (_ps_list_items(list_p)[_ps_do_assert(_ps_list_size(list_p) > 0), --_ps_list_size(list_p)])
#define _ps_list_first(list_p) (_ps_list_items(list_p)[_ps_do_assert(_ps_list_size(list_p) > 0), 0])
#define _ps_list_last(list_p) (_ps_list_items(list_p)[_ps_do_assert(_ps_list_size(list_p) > 0), _ps_list_size(list_p) - 1])
#define _ps_list_get(list_p, index) (_ps_list_items(list_p)[_ps_do_assert((index) < _ps_list_size(list_p) && "Index out of bounds!"), (index)])
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

#define _ps_list_to_unterminated(list_p) (_ps_list_items(list_p))
#define _ps_list_to_terminated(list_p, terminal) __i_ps_to_terminated(_ps_list_items(list_p), &terminal, _ps_list_item_size(list_p), _ps_list_size(list_p))
#define _ps_list_clone(list_p) (_ps_typeof(list_p))__i_ps_list_clone((void**)(list_p), _ps_list_item_size(list_p))
#define _ps_list_split(list_p, delimiter) __i_ps_list_split((void**)(list_p), _ps_list_item_size(list_p), (void*)(delimiter))
#define _ps_list_find(list_p, value) __i_ps_list_find((void**)(list_p), _ps_list_item_size(list_p), (void*)(value), 1)
#define _ps_list_find_last(list_p, value) __i_ps_list_find((void**)(list_p), _ps_list_item_size(list_p), (void*)(value), -1)

#define _ps_str_concat_one(str_p, str) _ps_list_append_many(str_p, str, strlen(str))

#define _ps_str_concat(str_p, ...)\
    _ps_blk(\
        char *strs[] = {__VA_ARGS__, NULL};\
        char **strs_p = strs;\
        do {\
            _ps_str_concat_one(str_p, *strs_p);\
        } while (*(++strs_p));\
    )

#define _ps_str_concatz(str_p, ...)\
    _ps_blk(\
        char *strs[] = {__VA_ARGS__, NULL};\
        char **strs_p = strs;\
        do {\
            _ps_str_concat_one(str_p, *strs_p);\
        } while (*(++strs_p));\
        _ps_list_append_one(str_p, '\0');\
    )

PS_DEF void ps_str_append_fmt( char **str, const char *fmt, ...) _ps_printf_format(2, 3);
PS_DEF void* __i_ps_to_terminated(void *ptr, void* terminal, ps_half_t item_size, ps_half_t size);
PS_DEF ps_list_t __i_ps_list_clone( void **list, ps_half_t item_size);
PS_DEF ps_list_slice_t __i_ps_list_split(void **list, ps_half_t item_size, void* delimiter);
PS_DEF ps_ihalf_t __i_ps_list_find(void **list, ps_half_t item_size, void* value, ps_ihalf_t direction);


#ifdef PS_STRIP_PREFIX
#define _array_size _ps_array_size
#define slice_t ps_slice_t
#define str_view_t ps_str_view_t
#define _slice_size _ps_slice_size
#define _slice_item_size _ps_slice_item_size
#define _slice_items _ps_slice_items
#define _get_slice _ps_get_slice
#define _slice_skip _ps_slice_skip
#define _slice_shrink _ps_slice_shrink
#define _slice_first _ps_slice_first
#define _slice_last _ps_slice_last
#define _slice_get _ps_slice_get
#define _slice_set _ps_slice_set
#define _slice_sub _ps_slice_sub
#define _slice_foreach _ps_slice_foreach
#define _slice_to_unterminated _ps_slice_to_unterminated
#define _slice_to_terminated _ps_slice_to_terminated

#define list_t ps_list_t
#define str_t ps_str_t
#define list_list_t ps_list_list_t
#define list_slice_t ps_list_slice_t
#define _list_capacity _ps_list_capacity
#define _list_size _ps_list_size
#define _list_item_size _ps_list_item_size
#define _list_items _ps_list_items
#define _list_free _ps_list_free
#define _list_reserve_precise _ps_list_reserve_precise
#define _list_reserve _ps_list_reserve
#define _list_resize _ps_list_resize
#define _list_append_one _ps_list_append_one
#define _list_append_many _ps_list_append_many
#define _list_append_array _ps_list_append_array
#define _list_append_list _ps_list_append_list
#define _list_append _ps_list_append
#define _list_pop _ps_list_pop
#define _list_first _ps_list_first
#define _list_last _ps_list_last
#define _list_get _ps_list_get
#define _list_set _ps_list_set
#define _list_swap_and_pop _ps_list_swap_and_pop
#define _list_unordered_remove _ps_list_unordered_remove
#define _list_clear _ps_list_clear
#define _list_is_empty _ps_list_is_empty
#define _list_foreach _ps_list_foreach
#define _list_to_unterminated _ps_list_to_unterminated
#define _list_to_terminated _ps_list_to_terminated
#define _list_clone _ps_list_clone
#define _list_split _ps_list_split
#define _list_find _ps_list_find
#define _list_find_last _ps_list_find_last

#define _str_concat_one _ps_str_concat_one
#define _str_concat _ps_str_concat
#define _str_concatz _ps_str_concatz
#define str_append_fmt ps_str_append_fmt
#endif /* PS_STRIP_PREFIX */

#ifdef PS_COLLECTIONS_IMP
PS_DEF void ps_str_append_fmt(char** str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (len > 0) {
        ps_half_t old_size = _ps_list_size(str);
        if (old_size > 0 && _ps_list_items(str)[old_size - 1] == '\0') {
            old_size--;
        }
        _ps_list_reserve(str, old_size + len + 1);
        vsnprintf(_ps_list_items(str) + old_size, len + 1, fmt, args);
        _ps_list_size(str) = old_size + len + 1;
    }

    va_end(args);
}

PS_DEF void* __i_ps_to_terminated(void *ptr, void* terminal, ps_half_t item_size, ps_half_t size){
    void* result = _ps_realloc(NULL, size * item_size);
    if (result) {
        _ps_memcpy(result, ptr, size * item_size);
        _ps_memcpy((char*)result + size * item_size, terminal, item_size);
    }
    return result;
}

PS_DEF ps_list_t __i_ps_list_clone( void **list, ps_half_t item_size){
    ps_list_t result = {0};
    if (list && _ps_list_size(list) > 0) {
        result.items = _ps_realloc(result.items, _ps_list_size(list) * item_size);
        _ps_memcpy(result.items, _ps_list_items(list), _ps_list_size(list) * item_size);
        _ps_list_size(&result) = _ps_list_size(list);
    }
    return result;
}

PS_DEF ps_list_slice_t __i_ps_list_split(void **list, ps_half_t item_size, void* delimiter) {
    ps_list_slice_t result = {0};
    ps_slice_t **r_ptr = (ps_slice_t **)&result;
    ps_half_t list_size = _ps_list_size(list);
    ps_half_t i = 0;
    if (list_size == 0) return result;
    ps_slice_t current = {0};
    current.items = *list;
    char *list_bytes = (char *)_ps_list_items(list);
    while (i < list_size * item_size) {
        if (ps_mem_eql(list_bytes + i, delimiter, item_size)) {
            /* Handle split logic here */
            _ps_list_append_one(r_ptr, current);
            current = (ps_slice_t){0};
            i += item_size;
            current.items = list_bytes + i;
            continue;
        }
        i += item_size;
        current.size += 1;
    }
    if (current.size > 0) {
        _ps_list_append_one(r_ptr, current);
    }
    return result;
}

PS_DEF ps_ihalf_t __i_ps_list_find(void **list, ps_half_t item_size, void* value, ps_ihalf_t direction){
    ps_ihalf_t i = direction > 0 ? 0 : _ps_list_size(list) - 1;
    while (i >= 0 && i < (ps_ihalf_t)_ps_list_size(list)) {
        if (ps_mem_eql((char *)_ps_list_items(list) + (i * item_size), value, item_size)) {
            return i;
        }
        i += direction;
    }
    return -1;
}

#endif /* PS_COLLECTIONS_IMP */
#endif /* _H_PS_COLLECTIONS_H_ */