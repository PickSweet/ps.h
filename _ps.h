#ifndef _PS_H
#define _PS_H

#ifndef PS_REALLOC
#include <stdlib.h>
#define PS_REALLOC realloc
#endif

#ifndef PS_FREE
#include <stdlib.h>
#define PS_FREE free
#endif

#ifndef PS_MEMCPY
#include <string.h>
#define PS_MEMCPY memcpy
#endif

#ifndef PS_ASSERT
#include <assert.h>
#define PS_ASSERT assert
#endif


#define PS_BLK(stmnt)\
    do { \
        stmnt \
    } while (0)

#endif // _PS_H