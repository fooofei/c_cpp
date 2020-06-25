/*
 1 两个线程
 2 线程1 是数据更新线程 线程2 是数据读取线程
 3 在线程1更新数据时 线程2 不能读取，并且不能阻塞，不需要及时读取到最新数据，允许在下次循环使用到新数据
 4 在线程2 读取时 线程1 不能更新  ， 可以阻塞，保证更新成功
*/


// gcc test_atomic_sync_read_write.c -lpthread -o atomic -g

#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h> // memset
#include <time.h>

struct arg {
    uint8_t state;
    uint32_t number;
    uint32_t count;
    uint8_t exit;
};

void nsleep(long sec, long nsec)
{
    struct timespec t;
    t.tv_sec = sec;
    t.tv_nsec = nsec;
    nanosleep(&t, NULL);
}


void *thread_update(void *p)
{
    struct arg *a = p;
    while (!a->exit) {
        nsleep(1, 0);

        if (a->state > 0) {
            fprintf(stdout, "[y]in while %lld\n", (long long)time(NULL));
            fflush(stdout);
            while (!__sync_bool_compare_and_swap(&(a->state), 1, 0))
                ;
            fprintf(stdout, "[y]out while %lld\n", (long long)time(NULL));
            fflush(stdout);
        }
        nsleep(0, 1000);
        // fprintf(stdout, "[y]last number %u count %u\n", a->number, a->count); fflush(stdout);
        a->number += 1;
        a->number = a->number % 6;
        a->count = 0;

        __sync_add_and_fetch(&a->state, 1);

        fprintf(stdout, "[y] out work\n");
        fflush(stdout);
    }
}

static void _update(struct arg *a, uint32_t number)
{
    if (a->number == number) {
        a->count += 1;
    }
}

static void *thread_count(void *p)
{
    uint32_t i;
    uint32_t j;
    struct arg *a = p;

    for (i = 0; i < 3; i++) {
        for (j = 1; j <= 5; j += 1) {
            fprintf(stdout, "                      [x]i %u j %u  a->state=%u\n", i, j, a->state);
            fflush(stdout);
            if (a->state == 1) {
                __sync_add_and_fetch(&a->state, 1);
                fprintf(stdout, "                      [x]i %u j %u in work\n", i, j);
                fflush(stdout);
                nsleep(1, 100);
                _update(a, j);
                fprintf(stdout, "                      [x]i %u j %u  out work\n", i, j);
                fflush(stdout);
                __sync_sub_and_fetch(&a->state, 1);
            } else {
                nsleep(1, 0);
            }
        }
    }

    a->exit = 1;
}


int main()
{
    struct arg a;
    memset(&a, 0, sizeof(a));
    pthread_t thr;

    pthread_create(&thr, NULL, thread_count, &a);
    pthread_create(&thr, NULL, thread_update, &a);

    while (!a.exit) {
    }

    nsleep(1, 0);
    return 0;
}


/* 相互持有

$ ./atomic
                      [x]i 0 j 1  a->state=0
                      [x]i 0 j 2  a->state=0
[y] out work
                      [x]i 0 j 3  a->state=1
                      [x]i 0 j 3 in work
[y]in while 1526560850     --->   x 进入工作区 y 要等待
                      [x]i 0 j 3  out work
                      [x]i 0 j 4  a->state=1
[y]out while 1526560851    -----> y 等待结束
[y] out work
                      [x]i 0 j 5  a->state=1
                      [x]i 0 j 5 in work
[y]in while 1526560852
                      [x]i 0 j 5  out work
                      [x]i 1 j 1  a->state=1
[y]out while 1526560853
[y] out work
                      [x]i 1 j 2  a->state=1
                      [x]i 1 j 2 in work
[y]in while 1526560854
                      [x]i 1 j 2  out work
                      [x]i 1 j 3  a->state=1
[y]out while 1526560855
[y] out work
                      [x]i 1 j 4  a->state=1
                      [x]i 1 j 4 in work
[y]in while 1526560856
                      [x]i 1 j 4  out work
                      [x]i 1 j 5  a->state=1
                      [x]i 1 j 5 in work
                      [x]i 1 j 5  out work
                      [x]i 2 j 1  a->state=1
[y]out while 1526560858
[y] out work
                      [x]i 2 j 2  a->state=1
                      [x]i 2 j 2 in work
[y]in while 1526560859
                      [x]i 2 j 2  out work
                      [x]i 2 j 3  a->state=1
[y]out while 1526560860
[y] out work
                      [x]i 2 j 4  a->state=1
                      [x]i 2 j 4 in work
[y]in while 1526560861
                      [x]i 2 j 4  out work
                      [x]i 2 j 5  a->state=1
[y]out while 1526560862
[y] out work
[y]in while 1526560863
[y]out while 1526560863
[y] out work



下一个案例

$ ./atomic
                      [x]i 0 j 1  a->state=0
                      [x]i 0 j 2  a->state=0
[y] out work
                      [x]i 0 j 3  a->state=1
                      [x]i 0 j 3 in work
[y]in while 1526561014
                      [x]i 0 j 3  out work
                      [x]i 0 j 4  a->state=1
[y]out while 1526561015
[y] out work
                      [x]i 0 j 5  a->state=1
                      [x]i 0 j 5 in work
[y]in while 1526561016     --->  y 在等待 x 工作结束
                      [x]i 0 j 5  out work   -> x 工作结束
                      [x]i 1 j 1  a->state=0  -> x 工作结束后被 y 抢占 x 无法工作
[y]out while 1526561017
[y] out work                         ----------------> y 工作结束
                      [x]i 1 j 2  a->state=1
                      [x]i 1 j 2 in work
[y]in while 1526561018
                      [x]i 1 j 2  out work
                      [x]i 1 j 3  a->state=1
[y]out while 1526561019
[y] out work
                      [x]i 1 j 4  a->state=1
                      [x]i 1 j 4 in work
[y]in while 1526561020
                      [x]i 1 j 4  out work
                      [x]i 1 j 5  a->state=1
[y]out while 1526561021
[y] out work
                      [x]i 2 j 1  a->state=1
                      [x]i 2 j 1 in work
[y]in while 1526561022
                      [x]i 2 j 1  out work
                      [x]i 2 j 2  a->state=1
[y]out while 1526561023
[y] out work
                      [x]i 2 j 3  a->state=1
                      [x]i 2 j 3 in work
[y]in while 1526561024
                      [x]i 2 j 3  out work
                      [x]i 2 j 4  a->state=1
[y]out while 1526561025
[y] out work
                      [x]i 2 j 5  a->state=1
                      [x]i 2 j 5 in work
[y]in while 1526561026
                      [x]i 2 j 5  out work
[y]out while 1526561027
[y] out work

*/
