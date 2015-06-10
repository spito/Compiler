#include "stdio.h"

int foo( int, int, int );

int foo( int a, int b, int c ) {
    return a * b + c;
}

int foo( int, int, int );


int main() {
    int a = 5;
    const int b = - 6;
    int c = -b;
    printf( "%d * %d + %d = %d\n", a, b, c, foo( a, b, c ) );
    printf( "%d\n", a );
    return 0;
}
