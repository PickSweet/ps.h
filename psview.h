#ifndef PSVIEW_H
#define PSVIEW_H

#include "pscore.h"

#define PS_VIEW_T(T)\
    struct {\
        const T *data;\
        size_t size;\
    }

#define ps_view_slice(view, start, end)\
    (typeof(*(view))){(view)->data + (start), (end) - (start)}

#define ps_view_tl(view, n)\
    PS_BLK(\
        size_t N = n;\
        if (N > (view)->size) N = (view)->size;\
        (view)->data += N;\
        (view)->size -= N;\
    )

#define ps_view_tr(view, n)\
    PS_BLK(\
        size_t N = n;\
        if( N > (view)->size) N = (view)->size;\
        (view)->size -= N;\
    )

#define ps_view_copy_to_mem_with_sentinal(view, mem, sentinal)\
    PS_BLK(\
        PS_MEMCPY((typeof((view)->data))(mem), (view), (view)->size);\
        ((typeof((view)->data))(mem))[(view)->size] = sentinal;\
    )

#define ps_view_copy_to_mem(view, mem)\
    PS_BLK(\
        PS_MEMCPY((typeof((view)->data))mem, (view), (view)->size);\
    )

int ps_memcompare(void* a, void* b, size_t size);
size_t ps_memlen_(const void *mem, void* sentinal, size_t element_size);

#define ps_memlen(mem,sentinal)\
    ps_memlen_((void*)(mem), (void*)((sentinal)), sizeof(*(mem)))



#define ps_mem_to_view_from_sentinal(mem, sentinal)\
    PS_VIEW_T(typeof(*(mem))){mem, ps_memlen(mem,sentinal)}


#ifdef PSVIEW_IMPLEMENTATION

int ps_memcompare(void* a, void* b, size_t size){
    const char *ba = (const char*)a;
    const char *bb = (const char*)b;
    size_t i = size - 1;
    do {
        if (*(ba + i) != *(bb + i)) return 0;
    } while (i);
    return 1;
}

size_t ps_memlen_(const void *mem, void* sentinal, size_t element_size){
    char* element_bytes = PS_REALLOC(NULL, element_size);
    size_t i = 0;
    const char *bytes = (const char *)mem;
    PS_MEMCPY(element_bytes, bytes, element_size);
    while (!ps_memcompare(element_bytes,sentinal,element_size)){
        bytes += element_size;
        PS_MEMCPY(element_bytes, bytes, element_size);
        i++;
    }
    PS_FREE(element_bytes);
    return i;
}

#endif // PSVIEW_IMPLEMENTATION
#endif // PSVIEW_H
