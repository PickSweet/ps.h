#ifndef PSARRAY_H
#define PSARRAY_H

#include "pscore.h"

#define PS_DARRAY_T(T) \
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

#define PS_SARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ps_darray_reserve_precise(arr, new_capacity) \
    PS_BLK( \
        if ((arr)->capacity < (new_capacity)) { \
            (arr)->items = PS_DECLTYPE_CAST((arr)->items)(PS_REALLOC((arr)->items, (new_capacity) * sizeof(*(arr)->items))); \
            (arr)->capacity = (new_capacity); \
            PS_ASSERT((arr)->items != NULL && "Failed to reallocate memory for dynamic array"); \
        } \
    )

#define ps_darray_reserve(arr, new_capacity) \
    PS_BLK( \
        if ((arr)->capacity < (new_capacity)) { \
            int target_capacity;\
            if((arr)->capacity == 0) { \
                target_capacity = 1;\
            } else {\
                target_capacity = (arr)->capacity;\
            }\
            while (new_capacity > target_capacity) { \
                target_capacity *= 2; \
            } \
            ps_darray_reserve_precise(arr, target_capacity); \
        } \
    )

#define ps_darray_free(arr) \
    PS_BLK( \
        PS_FREE((arr)->items); \
        (arr)->items = NULL; \
        (arr)->size = 0; \
        (arr)->capacity = 0; \
    )

#define ps_darray_resize(arr, new_size) \
    PS_BLK( \
        ps_darray_reserve(arr, (new_size)); \
        (arr)->size = (new_size); \
    )

#define ps_darray_append(arr, value) \
    PS_BLK( \
        ps_darray_reserve(arr, (arr)->size + 1); \
        (arr)->items[(arr)->size++] = (value); \
    )

#define ps_darray_append_many(arr, values, count) \
    PS_BLK( \
        ps_darray_reserve(arr, (arr)->size + (count)); \
        PS_MEMCPY(&(arr)->items[(arr)->size], (values), (count) * sizeof(*(arr)->items)); \
        (arr)->size += (count); \
    )

#define ps_darray_pop(arr) (arr)->items[PS_ASSERT((arr)->size > 0), --(arr)->size]
#define ps_darray_first(arr) (arr)->items[PS_ASSERT((arr)->size > 0), 0]
#define ps_darray_last(arr) (arr)->items[PS_ASSERT((arr)->size > 0), (arr)->size - 1]
#define ps_darray_get(arr, index) (arr)->items[PS_ASSERT((index) < (arr)->size && "Index out of bounds for dynamic array"), (index)]
#define ps_darray_set(arr, index, value) \
    PS_BLK( \
        PS_ASSERT((index) < (arr)->size && "Index out of bounds"); \
        (arr)->items[(index)] = (value); \
    )
#define ps_darray_unordered_remove(arr, index) \
    PS_BLK( \
        PS_ASSERT((index) < (arr)->size && "Index out of bounds"); \
        (arr)->items[(index)] = (arr)->items[--(arr)->size]; \
    )

#define ps_darray_clear(arr) \
    PS_BLK( \
        (arr)->size = 0; \
    )

#define ps_darray_is_empty(arr) ((arr)->size == 0)

#define ps_darray_foreach(T, it, arr) \
    for (T* it = (arr)->items; it < (arr)->items + (arr)->size; ++it)

#define ps_darray_init_size(arr,size)\
    ps_darray_resize(arr,size);\
    PS_MEMSET((arr)->items,0,sizeof(*(arr)->items) * (size))

#define ps_slice_from_darray(arr)\
    (PS_SLICE_T(typeof(*(arr)->items))){arr->items, arr->size}

#define ps_slice_from_array(arr)\
    (PS_SLICE_T(typeof(*(arr)))){arr,PS_SARRAY_SIZE(arr)}

#define ps_slice_sub(slice, start, len) \
    ( \
        PS_ASSERT((start) + (len) <= (slice)->size && "Out of bounds"), \
        (typeof(*(slice))){ (slice)->items + (start), (len) } \
    )

#define ps_slice_get(slice, index) ps_darray_get(slice, index)
#define ps_slice_set(slice, index, value) ps_darray_set(slice, index, value)

#define ps_slice_tl(slice, n)\
    PS_BLK(\
        size_t N = n;\
        if (N > (slice)->size) N = (slice)->size;\
        (slice)->items += N;\
        (slice)->size -= N;\
    )

#define ps_view_tr(slice, n)\
    PS_BLK(\
        size_t N = n;\
        if( N > (slice)->size) N = (slice)->size;\
        (slice)->size -= N;\
    )

#endif // PSARRAY_H
