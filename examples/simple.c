int scanf( const char *, ... );
int printf( const char*, ... );
int main() {
    int pole[3];
    pole[0] = 1;
    pole[1] = 2;
    pole[2] = 3;

    int *p = pole;
    printf( "%d %d %d\n", p[0], p[1], p[2] );
    printf( "%d %d %d\n", pole[0], pole[1], pole[2] );
    return 0;
}