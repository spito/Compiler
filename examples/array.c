#include "stdio.h"

void sortArray( int *pole, int n ) {

    for ( int i = 0; i < n; ++i ) {
        for ( int j = 0; j + 1 < n; ++j ) {
            if ( pole[ j ] > pole[ j + 1 ] ) {
                int tmp = pole[ j + 1 ];
                pole[ j + 1 ] = pole[ j ];
                pole[ j ] = tmp;
            }
        }
    }
}

void printArray( const int *pole, int n ) {
    for ( int i = 0; i < n; ++i ) {
        printf( "%i: %i\n", i, pole[ i ] );
    }
}

int main() {

    int pole[6] = { 0,1,2,3, };
   
    int *p = pole;
    int a = p[2];
    int b = pole[2];

    sortArray( pole, sizeof( pole ) / sizeof( *pole ) );
    printArray( pole, sizeof( pole ) / sizeof( *pole ) );
    
    printf( "---------\n" );
    printf( "a: %d\nb: %d\nshould be 2\n", a, b );

    return 0;
}