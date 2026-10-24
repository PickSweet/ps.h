#ifndef PSCOLLECTION_H
#define PSCOLLECTION_H

#include "pscore.h"

#define PS_LIST_T(T) \
    struct { \
        T* items; \
        size_t size; \
        size_t capacity; \
    }

#define PS_SLICE_T(T)\
    struct {\
        T* items;\
        size_t size;\
    }

#define PS_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ps_list_reserve_precise(list, new_capacity) \
    PS_BLK( \
        if ((list)->capacity < (new_capacity)) { \
            (list)->items = PS_DECLTYPE_CAST((list)->items)(PS_REALLOC((list)->items, (new_capacity) * sizeof(*(list)->items))); \
            (list)->capacity = (new_capacity); \
            PS_ASSERT((list)->items != NULL && "Failed to reallocate memory for list"); \
        } \
    )

#define ps_list_reserve(list, new_capacity) \
    PS_BLK( \
        if ((list)->capacity < (new_capacity)) { \
            int target_capacity;\
            if((list)->capacity == 0) { \
                target_capacity = 1;\
            } else {\
                target_capacity = (list)->capacity;\
            }\
            while (new_capacity > target_capacity) { \
                target_capacity *= 2; \
            } \
            ps_list_reserve_precise(list, target_capacity); \
        } \
    )

#define ps_list_free(list) \
    PS_BLK( \
        PS_FREE((list)->items); \
        (list)->items = NULL; \
        (list)->size = 0; \
        (list)->capacity = 0; \
    )

#define ps_list_resize(list, new_size) \
    PS_BLK( \
        ps_list_reserve(list, (new_size)); \
        (list)->size = (new_size); \
    )

#define ps_list_append(list, value) \
    PS_BLK( \
        ps_list_reserve(list, (list)->size + 1); \
        (list)->items[(list)->size++] = (value); \
    )

#define ps_list_append_many(list, values, count) \
    PS_BLK( \
        ps_list_reserve(list, (list)->size + (count)); \
        PS_MEMCPY(&(list)->items[(list)->size], (values), (count) * sizeof(*(list)->items)); \
        (list)->size += (count); \
    )

#define ps_list_pop(list) (list)->items[PS_ASSERT((list)->size > 0), --(list)->size]
#define ps_slice_pop(slice) ps_list_pop(slice)
#define ps_list_first(list) (list)->items[PS_ASSERT((list)->size > 0), 0]
#define ps_slice_first(slice) ps_list_first(slice)
#define ps_list_last(list) (list)->items[PS_ASSERT((list)->size > 0), (list)->size - 1]
#define ps_slice_last(slice) ps_list_last(slice)
#define ps_list_get(list, index) (list)->items[PS_ASSERT((index) < (list)->size && "Index out of bounds."), (index)]
#define ps_slice_get(slice, index) ps_list_get(slice,index)
#define ps_list_set(list, index, value) \
    PS_BLK( \
        PS_ASSERT((index) < (list)->size && "Index out of bounds"); \
        (list)->items[(index)] = (value); \
    )
#define ps_slice_set(slice, index, value) ps_list_set(slice, index, value)
#define ps_list_unordered_remove(list, index) \
    PS_BLK( \
        PS_ASSERT((index) < (list)->size && "Index out of bounds"); \
        (list)->items[(index)] = (list)->items[--(list)->size]; \
    )

#define ps_list_clear(list) \
    PS_BLK( \
        (list)->size = 0; \
    )

#define ps_list_is_empty(list) ((list)->size == 0)
#define ps_slice_is_empty(slice) ps_list_is_empty(slice)

#define ps_list_foreach(T, it, list) \
    for (T* it = (list)->items; it < (list)->items + (list)->size; ++it)

#define ps_slice_foreach(T, it, slice) ps_list_foreach(T, it, slice)

#define ps_list_init_with_size(list,size)\
    ps_list_resize(list,size);\
    PS_MEMSET((list)->items,0,sizeof(*(list)->items) * (size))

#define ps_list_slice(list, start, len) \
    ( \
        PS_ASSERT((start) + (len) <= (list)->size && "Out of bounds."), \
        (PS_SLICE_T(typeof(*(list)->items))){ (list)->items + (start), (len) } \
    )

#define ps_array_slice(arr, start, len)\
    (PS_SLICE_T(typeof(*(arr)))){arr + (start), (len)}

#define ps_slice_slice(slice, start, len) ps_list_slice(slice, start, len)


#define ps_slice_skip(slice, n)\
    PS_BLK(\
        size_t N = n;\
        if (N > (slice)->size) N = (slice)->size;\
        (slice)->items += N;\
        (slice)->size -= N;\
    )

#define ps_slice_shrink(slice, n)\
    PS_BLK(\
        size_t N = n;\
        if( N > (slice)->size) N = (slice)->size;\
        (slice)->size -= N;\
    )

#endif // PSCOLLECTION_H
