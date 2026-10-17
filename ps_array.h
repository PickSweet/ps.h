#ifndef PS_ARRAY_H
#define PS_ARRAY_H

#include "_ps.h"

#define PS_DARRAY_T(T) \
    struct { \
        size_t size; \
        size_t capacity; \
        T* items; \
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
            if((arr)->capacity == 0) { \
                ps_darray_reserve_precise(arr, 1); \
            } else { \
                while (new_capacity > (arr)->capacity) { \
                    (arr)->capacity *= 2; \
                } \
                ps_darray_reserve_precise(arr, (arr)->capacity); \
            } \
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
        PS_ASSERT((index) < (arr)->size && "Index out of bounds for dynamic array"); \
        (arr)->items[(index)] = (value); \
    )
#define ps_darray_unordered_remove(arr, index) \
    PS_BLK( \
        PS_ASSERT((index) < (arr)->size && "Index out of bounds for dynamic array"); \
        (arr)->items[(index)] = (arr)->items[--(arr)->size]; \
    )

#define ps_darray_clear(arr) \
    PS_BLK( \
        (arr)->size = 0; \
    )

#define ps_darray_is_empty(arr) ((arr)->size == 0)

#define ps_darray_foreach(T, it, arr) \
    for (T* it = (arr)->items; it < (arr)->items + (arr)->size; ++it)

#endif // PS_ARRAY_H
