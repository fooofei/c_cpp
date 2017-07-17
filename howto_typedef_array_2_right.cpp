#include <stdio.h>


struct two_int_type
{
    int data[2];
    int& operator[](size_t i) { return data[i]; }
};

void print_two_int(two_int_type * p)
{
    // printf("%d %d\n",(p->data)[0], (p->data)[1]);
    // or
    printf("%d %d\n", (*p)[0], (*p)[1]);
}


void intermediate(two_int_type v)
{
    print_two_int(&v);
}

int main()
{

    return 0;
}