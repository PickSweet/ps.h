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

#ifndef PSLOG_H
#define PSLOG_H

#include "pscore.h"

#include <stdarg.h>
#include <stdio.h>

typedef unsigned char ps_log_level_e;
#define PS_LOG_NONE 0x00
#define PS_LOG_INFO 0x01
#define PS_LOG_WARNING 0x02
#define PS_LOG_ERROR 0x04
#define PS_LOG_ALL 0xFF

//  borrowing from nob.h here
#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    ifdef __MINGW_PRINTF_FORMAT
#        define PS_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define PS_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
//   TODO: implement NOB_PRINTF_FORMAT for MSVC
#    define PS_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typedef void (ps_log_handler)(ps_log_level_e level, const char* fmt, va_list args);

PS_DEF ps_log_level_e ps_log_levels_enabled = PS_LOG_ALL; // set flags to 0 to disable certain log levels

PS_DEF void ps_log_level_disable(ps_log_level_e levels);
PS_DEF void ps_log_level_enable(ps_log_level_e levels);
PS_DEF void ps_log_level_toggle(ps_log_level_e levels);

PS_DEF void ps_set_log_handler(ps_log_handler *handler);
PS_DEF ps_log_handler *ps_get_log_handler(void);

PS_DEF ps_log_handler ps_default_log_handler;

PS_DEF void ps_log(ps_log_level_e level, const char* fmt, ...) PS_PRINTF_FORMAT(2,3);

PS_DEF const char *ps_log_level_prefixes[8] = {"[INFO]", "[WARNING]", "[ERROR]",0,0,0,0,0};

PS_DEF unsigned char _ps_get_log_level_index(ps_log_level_e level);
PS_DEF void ps_set_log_prefix(ps_log_level_e level, const char *prefix);
PS_DEF const char *ps_get_log_prefix(ps_log_level_e level);

#ifdef PSLOG_IMP

PS_DEF void ps_log_level_disable(ps_log_level_e levels){
    ps_log_levels_enabled &= (~levels);
}

PS_DEF void ps_log_level_enable(ps_log_level_e levels){
    ps_log_levels_enabled |= levels;
}

PS_DEF void ps_log_level_toggle(ps_log_level_e levels){
    ps_log_levels_enabled ^= levels;
}

static ps_log_handler *ps__log_handler = &ps_default_log_handler;

PS_DEF void ps_set_log_handler(ps_log_handler *handler){
    ps__log_handler = handler;
}

PS_DEF ps_log_handler *ps_get_log_handler(void){
    return ps__log_handler;
}

PS_DEF void ps_log(ps_log_level_e level, const char* fmt, ...){
    va_list args;
    va_start(args, fmt);
    if ((ps_log_levels_enabled & level) == level){
        ps__log_handler(level, fmt, args);
    }
    va_end(args);
}

PS_DEF unsigned char _ps_get_log_level_index(ps_log_level_e level){
    unsigned char idx = 0;
    ps_log_level_e l = level;
    if (l == 0) return 0xFF;
    while(l >>= 1) idx++;
    return idx;
}

PS_DEF void ps_set_log_prefix(ps_log_level_e level, const char * prefix){
    unsigned char idx = _ps_get_log_level_index(level);
    ps_log_level_prefixes[idx] = prefix;
}

PS_DEF const char *ps_get_log_prefix(ps_log_level_e level){
    unsigned char idx = _ps_get_log_level_index(level);
    return ps_log_level_prefixes[idx];
}

PS_DEF void ps_default_log_handler(ps_log_level_e level, const char* fmt, va_list args){
    ps_log_level_e reporting_level = ps_log_levels_enabled & level;
    const char* prefix = ps_get_log_prefix(reporting_level);
    FILE *out = stdout;
    if( level & (PS_LOG_WARNING | PS_LOG_ERROR)){
        out = stderr;
    }
    if (prefix == NULL){
        prefix = "[LOG]";
    }
    fprintf(out, "%s ", prefix);
    vfprintf(out, fmt, args);
    fflush(out);
}


#endif // PSLOG_IMP

#endif // PSLOG_H