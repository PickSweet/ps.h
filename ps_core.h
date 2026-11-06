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


#ifndef _H_PS_CORE_H_
#define _H_PS_CORE_H_

#ifndef PS_DEF
#define PS_DEF
#endif // PS_DEF

#ifdef __cplusplus
#define _ps_decl_type_cast(T) (decltype(T))
#else
#define _ps_decl_type_cast(T)
#endif // _ps_decl_type_cast

#if defined(__cplusplus) || defined(__clang__)
#define _ps_typeof(...) typeof(__VA_ARGS__)
#elif defined(_MSC_VER)
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201710L
#define _ps_typeof(...) typeof(__VA_ARGS__)
#else
#define _ps_typeof(...) __UNSUPPORTED__
#endif
#else
#define _ps_typeof(...) __typeof__(__VA_ARGS__)
#endif // _ps_typeof

#ifdef __cplusplus
#define PS_INLINE inline
#else
#define PS_INLINE __inline
#endif // PS_INLINE

#ifndef _ps_realloc
#include <stdlib.h>
#define _ps_realloc realloc
#endif // _ps_realloc

#ifndef _ps_free
#include <stdlib.h>
#define _ps_free free
#endif // _ps_free

#ifndef _ps_memcpy
#include <string.h>
#define _ps_memcpy memcpy
#endif // _ps_memcpy

#ifndef _ps_memset
#include <string.h>
#define _ps_memset memset
#endif // _ps_memset

#ifndef _ps_assert
#include <assert.h>
#define _ps_assert assert
#endif // _ps_assert

#define _ps_blk(...)\
    do {__VA_ARGS__} while (0)

#ifndef _ps_unused
#define _ps_unused(x) (void)(x)
#endif // _ps_unused

#define _ps_todo(msg) \
    _ps_blk( \
        _ps_assert(!msg && "TODO: " msg); \
    )

#define _ps_unreachable() \
    _ps_blk( \
        _ps_assert(!"Unreachable code reached"); \
    )

#define _ps_after_goto(stmnt, label)\
    _ps_blk(\
        _ps_blk(stmnt); \
        goto label;\
    )

#define _ps_closure_t(R,T)\
    struct {\
        R(*fn)(T*);\
        void *ctx;\
    }

//  borrowing from nob.h here
#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    ifdef __MINGW_PRINTF_FORMAT
#        define _ps_printf_format(string_index, first_to_check) __attribute__ ((format (__MINGW_PRINTF_FORMAT, string_index, first_to_check)))
#    else
#        define _ps_printf_format(string_index, first_to_check) __attribute__ ((format (printf, string_index, first_to_check)))
#    endif // __MINGW_PRINTF_FORMAT
#else
//   TODO: implement NOB_PRINTF_FORMAT for MSVC
#    define _ps_printf_format(string_index, first_to_check)
#endif // _ps_printf_format

#ifndef PS_FULL_SIZE_T
#define PS_FULL_SIZE_T long long
#endif // PS_FULL_SIZE_T

#ifndef PS_HALF_SIZE_T
#define PS_HALF_SIZE_T int
#endif // PS_HALF_SIZE_T

typedef PS_FULL_SIZE_T ps_isize_t;
typedef unsigned PS_FULL_SIZE_T ps_size_t;
typedef PS_HALF_SIZE_T ps_ihalf_t;
typedef unsigned PS_HALF_SIZE_T ps_half_t;


PS_DEF int ps_mem_eql(const void *a, const void *b, size_t size);

#ifdef PS_CORE_IMP

PS_DEF int ps_mem_eql(const void *a, const void *b, size_t size) {
    if (a == b)return 1;
    if (size == 0) return 1;
    if (!a || !b) return 0;
    const char* ba = (const char*)a;
    const char* bb = (const char*)b;
    char cmp = 0;
    do {
        cmp |= *ba++ ^ *bb++;
    } while (--size && !cmp);
    return !cmp;
}

#endif // PS_CORE_IMP

#endif // _H_PS_CORE_H_