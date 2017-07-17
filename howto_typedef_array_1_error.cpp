#include <stdio.h>

typedef int two_int_type[2];


void print_two_int(two_int_type * p)
{
    printf("%d %d\n", (*p)[0], (*p)[1]);
}

void intermediate(two_int_type v)
{
    print_two_int(&v);  // compile error
    // : error: cannot convert 'int**' to 'int (*)[2]' for argument '1' to 'void print_two_int(int (*)[2])'
    // msvc or g++ all error
}

int main()
{
    return 0;
}