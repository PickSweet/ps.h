#ifndef _PS_H
#define _PS_H

#ifndef PS_DEF
#define PS_DEF
#endif // PS_DEF

#ifdef __cplusplus
#define PS_DECLTYPE_CAST(T) (decltype(T))
#else
#define PS_DECLTYPE_CAST(T)
#endif // PS_DECLTYPE_CAST


#ifndef PS_REALLOC
#include <stdlib.h>
#define PS_REALLOC realloc
#endif // PS_REALLOC

#ifndef PS_FREE
#include <stdlib.h>
#define PS_FREE free
#endif // PS_FREE

#ifndef PS_MEMCPY
#include <string.h>
#define PS_MEMCPY memcpy
#endif // PS_MEMCPY

#ifndef PS_MEMSET
#include <string.h>
#define PS_MEMSET memset
#endif // PS_MEMSET

#ifndef PS_ASSERT
#include <assert.h>
#define PS_ASSERT assert
#endif // PS_ASSERT


#define PS_BLK(stmnt)\
    do { \
        stmnt \
    } while (0)

#ifndef PS_UNUSED
#define PS_UNUSED(x) (void)(x)
#endif // PS_UNUSED

#define PS_TODO(msg) \
    PS_BLK( \
        PS_ASSERT(!msg && "TODO: " msg); \
    )

#define PS_UNREACHABLE() \
    PS_BLK( \
        PS_ASSERT(!"Unreachable code reached"); \
    )

#define PS_AFTER_GOTO(stmnt, label)\
    PS_BLK(\
        PS_BLK(stmnt); \
        goto label;\
    )


#define PS_VA_FIRST(X, ...) (X)
#define PS_VA_REST(X, ...) (__VA_ARGS__)


#endif // _PS_H