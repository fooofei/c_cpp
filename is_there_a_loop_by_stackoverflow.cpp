#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
int main()
{
    // 目的是让对 a 的访问覆盖 i 的值 达到死循环的目的
    // 很不容易生效

    const int SIZE= 0x10;
    int a[SIZE];
    int i;


    printf("&i=0x%p, &a=0x%p\n", &i, a);

    if (&i > a) {
        printf("address of &i is larger\n");
    } else {
        printf("address of a is larger\n");
    }

    for (i = 1; i <= SIZE + 1; ++i) {
        printf("distance = %ld ", &a[i] - &i);
        fflush(stdout);
        a[i] = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }


    return 0;
}
