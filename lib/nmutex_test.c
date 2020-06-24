#include <stdio.h>
#include <string.h>

#include "nmutex.h"


void test_nmutex()
{
    struct nmutex mutex;

    memset(&mutex, 0, sizeof(mutex));

    nmutex_init(&mutex);

    nmutex_lock(&mutex);

    nmutex_unlock(&mutex);


    nmutex_uninit(&mutex);


    printf("pass %s()\n", __FUNCTION__);
}


int main()
{
    test_nmutex();
    return 0;
}
