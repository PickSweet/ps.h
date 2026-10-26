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


#ifndef PSCORE_H
#define PSCORE_H

#define CONCAT_(a, b, ...) a ## b
#define CONCAT(a,...) CONCAT_(a,__VA_ARGS__)
#define STRINGIFY_(a) #a
#define STRINGIFY(a, ...) STRINGIFY_(a)

#ifndef PS_DEF
#define PS_DEF
#endif // PS_DEF

#ifdef __cplusplus
#define PS_DECLTYPE_CAST(T) (decltype(T))
#else
#define PS_DECLTYPE_CAST(T)
#endif // PS_DECLTYPE_CAST

#ifdef __cplusplus
#define PS_TYPEOF(...) typeof(__VA_ARGS__)
#else
#define PS_TYPEOF(...) __typeof__(__VA_ARGS__)
#endif // PS_TYPEOF

#ifdef __cplusplus
#define PS_INLINE inline
#else
#define PS_INLINE __inline
#endif // PS_INLINE

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

#define PS_BLK(...)\
    do {__VA_ARGS__} while (0)

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

#define PS_CLOSURE_T(R,T)\
    struct {\
        R(*fn)(T*);\
        void *ctx;\
    }


#endif // PSCORE_H