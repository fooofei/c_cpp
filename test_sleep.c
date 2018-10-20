/**
 * the file is go to test usleep if can be terminated by signal
 * gcc test_sleep.c -o test
 * 
 * 1 sec=1 000 milisec=1 000 000 microsec=1 000 000 000 nanosec
 * 1s = 1000ms
 * 1ms = 1000μs
 * 1μs = 1000ns
 */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>

static bool g_force_quit = false;

static void signal_handler(int signum) {
    char buf[0x100] = { 0 };
    fprintf(stdout, "Received signal %d", signum);
    if (signum == SIGINT || signum == SIGTERM || signum == SIGALRM) {
        switch (signum)
        {
        case SIGINT: fprintf(stdout, " SIGINT");g_force_quit=true; break;
        case SIGTERM: fprintf(stdout, " SIGTERM"); g_force_quit=true; break;
        case SIGALRM: fprintf(stdout, " SIGALRM"); break;
        default:
            break;
        }
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}

static void main_setup_signals(void) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGALRM, signal_handler);
}

/*
<--  we use kill <pid>  被打断后继续sleep 完剩下的 比较麻烦
Received signal 15 SIGTERM
main exit, take 4(s)

<-- called alarm(3)
Received signal 14 SIGALRM
main exit, take 3(s)


 */
static inline void delay_us1(unsigned us){
    usleep(us); // ALARM  signal
}


/*
<-- kill 24601  所以也能被 kill 打断，然后能继续执行
Received signal 15 SIGTERM
nanosleep return value=-1 errno=4
g_force_quit=true, break sleep
main exit, take 5(s)

<-- call alarm 设置不被打断就不会退出
Received signal 14 SIGALRM
nanosleep return value=-1 errno=4
nanosleep return value=0 errno=4
main exit, take 10(s)


*/
static inline void delay_us(unsigned us){
    struct timespec res={0};
    res.tv_sec=0;
    res.tv_nsec = us; res.tv_nsec *= 1000;
    if(res.tv_nsec/(1000*1000*1000)){
        res.tv_sec += res.tv_nsec/(1000*1000*1000);
        res.tv_nsec = res.tv_nsec%(1000*1000*1000);
    }
    #if 0
    for(;;){
        int r = nanosleep(&res, &res);
        printf("nanosleep return value=%d errno=%d\n", r, errno); fflush(stdout);
        if(r==0){
            break;
        }
        if(g_force_quit){
            printf("g_force_quit=true, break sleep\n"); fflush(stdout);
            break;
        }
    }
    #endif
    for (;nanosleep(&res, &res) && errno == EINTR && !g_force_quit;);
}

static inline void delay_ms(unsigned ms){
    delay_us(ms * 1000);
}

typedef unsigned long long uint64;


/**
 * 实现一个循环定期事件 固定几秒做某事
 * 
 * 
 * hit at 3 work take 0(s)
hit at 6 work take 2(s)
hit at 9 work take 2(s)
hit at 12 work take 2(s)
hit at 15 work take 1(s)
hit at 18 work take 1(s)
hit at 21 work take 0(s)
hit at 24 work take 1(s)
hit at 27 work take 2(s)
hit at 30 work take 2(s)
hit at 33 work take 2(s)
hit at 36 work take 0(s)
hit at 39 work take 0(s)
hit at 42 work take 1(s)
hit at 45 work take 1(s)

 */
static void cycle1(){
    time_t start=0;
    time_t now=0;
    time_t hit_time=0;

    time(&now);
    start = now;
    hit_time = now;
    srand(now);
    for(;!g_force_quit;){
        time(&now);

        if(now>hit_time && now-hit_time > 2){
            //
            printf("hit at %d ", now-start);
            time_t t1;
            time(&t1);
            delay_ms(1000 * (rand()%3));
            time_t t2;
            time(&t2);
            printf("work take %d(s)\n", t2-t1);
            hit_time = now;
        }
        delay_ms(100);

    }

}

/*
hit at 6 work take 1(s)
hit at 9 work take 0(s)
hit at 12 work take 1(s)
hit at 15 work take 2(s)
hit at 18 work take 1(s)
hit at 21 work take 1(s)
hit at 24 work take 0(s)
hit at 27 work take 0(s)
hit at 30 work take 1(s)
hit at 33 work take 2(s)
hit at 36 work take 1(s)
hit at 39 work take 2(s)
hit at 42 work take 1(s)
hit at 45 work take 2(s)
hit at 48 work take 1(s)
hit at 51 work take 0(s)
hit at 54 work take 0(s)
hit at 57 work take 1(s)
hit at 60 work take 1(s)
hit at 63 work take 2(s)
hit at 66 work take 2(s)
hit at 69 work take 0(s)
hit at 72 work take 0(s)
hit at 75 work take 2(s)
hit at 78 work take 2(s)
hit at 81 work take 2(s)
hit at 84 work take 1(s)
hit at 87 work take 1(s)
*/
static void cycle2(){
    struct timespec res;
    time_t start;
    time_t now;
    time(&start);
    clock_gettime(CLOCK_REALTIME, &res);
    res.tv_sec += 3;
    for(;!g_force_quit;){

        for(;0 != clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &res, &res) && errno == EINTR && !g_force_quit;);
        clock_gettime(CLOCK_REALTIME, &res);
        res.tv_sec += 3; // 相对于这个时间的 3s 即便后面拖延了几秒 但是也能跳跃过去
        time(&now);
        printf("hit at %d ", now-start);
        time_t t1;
        time(&t1);
        delay_ms(1000 * (rand()%3));
        time_t t2;
        time(&t2);
        printf("work take %d(s)\n", t2-t1);
    }
}

int main(){
    main_setup_signals();
    uint64 pid = getpid();
    printf("pid= %llu go to sleep 10 s\n", pid);
    printf("kill %llu\n", pid);
    printf("kill -9 %llu\n", pid);
    fflush(stdout);
    //alarm(3); // sec
    time_t start;
    time(&start);
    // test sleep
    //delay_ms(10*1000);
    //
    // test cycle
    //cycle1();
    cycle2();
    //
    time_t end;
    time(&end);
    printf("main exit, take %d(s)\n", (int)(end-start));
    return 0;
}

