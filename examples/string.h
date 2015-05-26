#ifndef _STRING_H
#define _STRING_H

#include "stdlib.h"

void *memset( void *s, int value, size_t n ) {
    char *c = (char *)s;
    for ( size_t i = 0; i != n; ++i )
        c[ i ] = value;
    return s;
}

#endif
