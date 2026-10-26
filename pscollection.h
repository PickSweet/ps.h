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

#ifndef PSCOLLECTION_H
#define PSCOLLECTION_H

#include "pscore.h"

#define PS_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

typedef struct ps_slice_info_t {
    size_t size;
    void *ptr;
} ps_slice_info_t;

#define ps_slice_t(T) T**

#define ps_slice_size(slice) (((size_t*)(slice))[-1])
#define ps_slice_ptr(slice) (*(slice))

#define ps_slice_init(data, size)\
    (PS_TYPEOF((data))*)(&(ps_slice_info_t){\
        size,\
        (void*)(data)\
    }.ptr)

static PS_INLINE ps_slice_info_t *ps_slice_info_new(void){
    ps_slice_info_t *info = (ps_slice_info_t*)PS_REALLOC(NULL, sizeof(ps_slice_info_t));
    PS_ASSERT(info != NULL && "Failed to allocate memory for slice info");
    info->size = 0;
    info->ptr = 0;
    return info;
}

#define ps_slice_new(T) ((T**)&(ps_slice_info_new()->ptr))

#define ps_slice_free(slice)\
    PS_BLK(\
        if ((slice) != NULL) {\
            PS_FREE(((size_t*)(slice)) - 1);\
            *(slice) = NULL;\
        }\
    )

#define ps_slice_skip(slice,n)\
    PS_BLK(\
        size_t _n = (n);\
        PS_ASSERT((_n <= ps_slice_size(slice)) && "Out of bounds!");\
        ps_slice_ptr(slice) += _n;\
        ps_slice_size(slice) -= n;\
    )

#define ps_slice_shrink(slice,n)\
    PS_BLK(\
        size_t _n = (n);\
        PS_ASSERT((_n <= ps_slice_size(slice)) && "Out of bounds!");\
        ps_slice_size(slice) -= _n;\
    )


#define ps_slice_first(slice) (ps_slice_ptr(slice)[PS_ASSERT(ps_slice_size(slice) > 0), 0])
#define ps_slice_last(slice) (ps_slice_ptr(slice)[PS_ASSERT(ps_slice_size(slice) > 0), ps_slice_size(slice) - 1])
#define ps_slice_get(slice, index) (ps_slice_ptr(slice)[PS_ASSERT((index) < ps_slice_size(slice) && "Index out of bounds!"), (index)])
#define ps_slice_set(slice, index, value) (ps_slice_get(slice,index) = (value))

#define ps_slice_sub(slice, start, end)\
    (void*)(&(ps_slice_info_t){\
        (end) - (start),\
        (void*)(ps_slice_ptr(slice) + (start)),\
    }.ptr)

#define ps_slice_foreach(slice, iter)\
    for (\
        PS_TYPEOF(ps_slice_ptr(slice)) iter = ps_slice_ptr(slice);\
        iter < ps_slice_ptr(slice) + ps_slice_size(slice);\
        ++iter\
    )

typedef struct ps_list_info_t {
    size_t capacity;
    size_t size;
    void *ptr;
} ps_list_info_t;

#define ps_list_t(T) T**

#define ps_list_capacity(list) (((size_t*)(list))[-2])
#define ps_list_size(list) (((size_t*)(list))[-1])
#define ps_list_ptr(list) (*(list))

#define ps_list_init(T)\
    (T**)(&(ps_list_info_t){0}.ptr)

#define ps_list_info_free(list)\
    PS_BLK(\
        if ((list) != NULL && ps_list_ptr(list) != NULL) {\
            PS_FREE(ps_list_ptr(list));\
            ps_list_ptr(list) = NULL;\
            ps_list_capacity(list) = 0;\
            ps_list_size(list) = 0;\
        }\
    )

#define ps_list_free(list)\
    PS_BLK(\
        if ((list) != NULL) {\
            ps_list_info_free(list);\
            PS_FREE(((size_t*)(list)) - 2);\
            *(list) = NULL;\
        }\
    )

#define ps_list_reserve_precise(list, new_capacity)\
    PS_BLK(\
        if (ps_list_capacity(list) < (new_capacity)){\
            void *_new_ptr = PS_REALLOC(ps_list_ptr(list), (new_capacity) * sizeof(*ps_list_ptr(list)));\
            PS_ASSERT(_new_ptr != NULL && "Failed to reallocate memory for list!");\
            ps_list_ptr(list) = PS_DECLTYPE_CAST(ps_list_ptr(list))(_new_ptr);\
            ps_list_capacity(list) = (new_capacity);\
        }\
    )

#define ps_list_reserve(list, new_capacity)\
    PS_BLK(\
        size_t *list_capacity_ptr = &ps_list_capacity(list);\
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
            ps_list_reserve_precise(list, target_capacity);\
        }\
    )

#define ps_list_resize(list, new_size)\
    PS_BLK(\
        ps_list_reserve(list, new_size);\
        ps_list_size(list) = (new_size);\
    )

#define ps_list_append_one(list, value)\
    PS_BLK(\
        ps_list_reserve(list, ps_list_size(list) + 1);\
        ps_list_ptr(list)[ps_list_size(list)] = (value);\
        ps_list_size(list)++;\
    )
#define ps_list_append_many(list, values, count)\
    PS_BLK(\
        PS_ASSERT(sizeof((values)[0]) == sizeof(*ps_list_ptr(list)) && "Appending values to list of incompatible type.");\
        ps_list_reserve(list,ps_list_size(list) + (count));\
        PS_MEMCPY(ps_list_ptr(list) + ps_list_size(list), values, (count) * sizeof(*ps_list_ptr(list)));\
        ps_list_size(list)+=(count);\
    )

#define ps_list_append_array(list,array)\
    ps_list_append_many(list, array, PS_ARRAY_SIZE(array))

#define ps_list_append_list(list_a,list_b)\
    ps_list_append_many(list_a, ps_list_ptr(list_b), ps_list_size(list_b))

#define ps_list_append(list, ...)\
    PS_BLK(\
        PS_TYPEOF(*ps_list_ptr(list)) values[] = {__VA_ARGS__};\
        ps_list_append_array(list, values);\
    )

#define ps_list_pop(list) (ps_list_ptr(list)[PS_ASSERT(ps_list_size(list) > 0), --ps_list_size(list)])
#define ps_list_first(list) (ps_list_ptr(list)[PS_ASSERT(ps_list_size(list) > 0), 0])
#define ps_list_last(list) (ps_list_ptr(list)[PS_ASSERT(ps_list_size(list) > 0), ps_list_size(list) - 1])
#define ps_list_get(list, index) (ps_list_ptr(list)[PS_ASSERT((index) < ps_list_size(list) && "Index out of bounds!"), (index)])
#define ps_list_set(list, index, value) (ps_list_get(list,index) = (value))

#define ps_list_swap_and_pop(list, index, out)\
    PS_BLK(\
        (out) = ps_list_get(list, index);\
        ps_list_get(list,index) = ps_list_get(ps_list_size(list) - 1);\
        ps_list_size(list)--;\
    )

#define ps_list_unordered_remove(list, index)\
    PS_BLK(\
        PS_TYPEOF(*ps_list_ptr(list)) out;\
        ps_list_swap_and_pop(list, index, out);\
    )

#define ps_list_clear(list) ps_list_size(list) = 0
#define ps_list_is_empty(list) (ps_list_size(list) == 0)

#define ps_list_foreach(list, iter)\
    for (\
        PS_TYPEOF(ps_list_ptr(list)) iter = ps_list_ptr(list);\
        iter < ps_list_ptr(list) + ps_list_size(list);\
        ++iter\
    )

static PS_INLINE ps_list_info_t *ps_list_info_new(const void *data, size_t size){
    ps_list_info_t *info = (ps_list_info_t*)PS_REALLOC(NULL, sizeof(ps_list_info_t));
    PS_ASSERT(info != NULL && "Failed to allocate memory for list info");
    info->capacity = 0;
    info->size = 0;
    info->ptr = NULL;
    if (data != NULL && size > 0){
        ps_list_append_many(&info->ptr, data, size);
    }
    return info;
}

#define ps_list_new(T) \
    ((T**)&(ps_list_info_new(NULL, 0)->ptr))

#define ps_list_new_with_data(T, ...) \
    ((T**)&(ps_list_info_new(\
        (T[]){__VA_ARGS__}, \
        sizeof((T[]){__VA_ARGS__}) / sizeof(T)\
    )->ptr))

#endif // PSCOLLECTION_H