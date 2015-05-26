#ifndef _STDLIB_H
#define _STDLIB_H

#define size_t unsigned long
#define NULL 0

void *malloc( size_t );
void *realloc( void *, size_t );
void free( void * );

#endif
