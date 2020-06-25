/*
http://originlee.com/2015/01/21/trick-in-conditon-variable/
https://github.com/forkhope/apue2/blob/master/11-thread/10-cond/cond.c
https://docs.oracle.com/cd/E19253-01/819-7051/6n919hpai/index.html
https://gist.github.com/chenshuo/6430925
http://www.cppblog.com/Solstice/archive/2013/09/09/203094.html
https://stackoverflow.com/questions/4544234/calling-pthread-cond-signal-without-locking-mutex

我是从 Windows Event 出发的，这与 linux 是不同的。

当 Event 受信时（强调状态）即 SetEvent() 之后，再来 Wait*，

Wait 也会立刻返回，但是 linux pthread_cond_t  就不同了

当 pthread_cond_signal 调用时，另一侧不阻塞在 pthread_cond_wait

等到另一侧有机会 pthread_cond_wait 了，也不会立刻返回，而是陷入阻塞。

有人这样形容这个现象“作为 Event 和 Windows 的 Event 语义不同，这个实现的 Event 是 edge - triggered，Windows 那个是 level
- triggered。”

[Edge和Level触发的中断] http://www.cnblogs.com/liloke/archive/2011/04/12/2014205.html

注意 pthread_cond_timedwait 中的时间，是绝对时间，每次重新 wait 时都要重新获取当前时间，在

当前时间上堆加间隔时间。

摘自 chenshuo
条件变量只有一种正确使用的方式，几乎不可能用错。对于 wait 端：
1. 必须与 mutex 一起使用，该布尔表达式的读写需受此 mutex 保护。
2. 在 mutex 已上锁的时候才能调用 wait()。
3. 把判断布尔条件和 wait() 放到 while 循环中。
4. pthread_cond_wait() 包含 1unlock() -> 2internal_wait() -> 3lock() ， signal 端在 signal 之后，unlock 之前，
2internal_wait() 返回，但 3lock() 会阻塞，因此 pthread_cond_wait() 在 signal 端 unlock() 之前都不会 return

第 4 条是我自己补充的

对于 signal / broadcast 端：
1. 不一定要在 mutex 已上锁的情况下调用 signal （理论上）。
2. 在 signal 之前一般要修改布尔表达式。
3. 修改布尔表达式通常要用 mutex 保护（至少用作 full memory barrier）。
4. 注意区分 signal 与 broadcast：“broadcast 通常用于表明状态变化，signal 通常用于表示资源可用。
（broadcast should generally be used to indicate state change rather than resource availability。）”


[重读APUE(15)-pthread_cond_wait与while循环] http://www.linuxtcpipstack.com/1029.html
wait 被唤醒不代表什么.

*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include <signal.h>

struct waiter {
    pthread_cond_t cond;
    pthread_mutex_t cond_mutex;
    bool signaled;
    char info[0x100];
    bool force_close;
    void (*wait)(struct waiter *);
    void (*signal)(struct waiter *);
};

void waiter_init(struct waiter *self);
void waiter_destroy(struct waiter *self);

void if_wait(struct waiter *self);
void while_wait(struct waiter *self);
void only_wait(struct waiter *self);

void only_signal(struct waiter *self);
void mutex_signal(struct waiter *self);
void signal_after_variable(struct waiter *self);
void signal_before_variable(struct waiter *self);
void signal_before_v2_variable(struct waiter *self);
void signal_after_v2_variable(struct waiter *self);
void best_solution_signal(struct waiter *self);

static void *threadfunc(void *arg)
{
    struct waiter *self = (struct waiter *)arg;

    struct timespec ts = { 0, 500 * 1000 * 1000 };
    nanosleep(&ts, NULL); // initialization takes 500ms

    fprintf(stdout, "->   in threadfunc call signal\n");
    fflush(stdout);
    self->signal(self);
    return 0;
}

static void run(struct waiter *w, int sec)
{
    int rc;
    pthread_t th;
    memset(&th, 0, sizeof(th));

    rc = pthread_create(&th, NULL, threadfunc, w);
    printf("->   %s() -%s- pthread_create() return %d\n", __FUNCTION__, w->info, rc);
    fflush(stdout);

    struct timespec ts = { sec, 0 };
    nanosleep(&ts, NULL); // do some work before calling wait()

    printf("->   %s() -%s- waiting\n", __FUNCTION__, w->info);
    fflush(stdout);
    w->wait(w);
    printf("->   %s() -%s- wait return\n", __FUNCTION__, w->info);
    fflush(stdout);
}

static struct waiter *g_p_current_waiter;
static void sigalarm(int arg)
{
    fprintf(stdout, "[!] %s- clock hit %d, force signal \n", g_p_current_waiter->info, arg);
    fflush(stdout);
    // exit(1);
    g_p_current_waiter->signal(g_p_current_waiter);
    g_p_current_waiter->force_close = true;
}


static void test_framework(void (*fwait)(struct waiter *), void (*fsignal)(struct waiter *), const char *info)
{
    struct waiter self;
    memset(&self, 0, sizeof(self));
    waiter_init(&self);
    self.wait = fwait;
    self.signal = fsignal;
    snprintf(self.info, sizeof(self.info), "%s", info);

    g_p_current_waiter = &self;

    alarm(5);
    fprintf(stdout, "-> %s() -%s- wait before signal\n", __FUNCTION__, self.info);
    fflush(stdout);
    run(&self, 0);


    // 经过 5 sec 给当前进程发送消息 也就是回调函数 闹钟
    alarm(5); // the old one will be canceled.
    fprintf(stdout, "-> %s() -%s- wait after signal\n", __FUNCTION__, self.info);
    fflush(stdout);
    run(&self, 2);


    waiter_destroy(&self);
    printf("%s() -%s- %s\n", __FUNCTION__, self.info, (self.force_close ? "FAIL" : "PASS"));
}

int main()
{
    signal(SIGALRM, sigalarm);


    test_framework(only_wait, only_signal, "only-wait--only-signal_bad");
    fprintf(stdout, "\n\n");
    test_framework(only_wait, mutex_signal, "only-wait--mutex-sigal_bad");
    fprintf(stdout, "\n\n");
    test_framework(if_wait, signal_after_variable, "if-wait--signal-after-variable_bad");
    fprintf(stdout, "\n\n");
    test_framework(while_wait, signal_after_variable, "while-wait--signal-after-variable_ok");
    fprintf(stdout, "\n\n");
    test_framework(while_wait, signal_after_v2_variable, "while-wait--signal-nomutex-after-variable_ok");
    fprintf(stdout, "\n\n");
    test_framework(while_wait, signal_before_variable, "while-wait--signal-before-variable_ok");
    fprintf(stdout, "\n\n");
    test_framework(while_wait, signal_before_v2_variable, "while-wait--signal-before-v2-variable_bad");
    fprintf(stdout, "\n\n");
    test_framework(while_wait, best_solution_signal, "best_solution");

    alarm(0); // cancel all.
    return 0;
}

void waiter_init(struct waiter *p)
{
    pthread_mutex_init(&p->cond_mutex, NULL);
    pthread_cond_init(&p->cond, NULL);
    p->signaled = false;
}

void waiter_destroy(struct waiter *p)
{
    pthread_mutex_destroy(&p->cond_mutex);
    pthread_cond_destroy(&p->cond);
    p->signaled = false;
}

void only_wait(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);
    pthread_cond_wait(&self->cond, &self->cond_mutex);
    pthread_mutex_unlock(&self->cond_mutex);
}

void while_wait(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);
    fprintf(stdout, "->   %s() -%s- self->signaled=%d\n", __FUNCTION__, self->info, self->signaled);
    fflush(stdout);
    while (!self->signaled) {
        fprintf(stdout, "->   %s() -%s- in pthread_cond_wait\n", __FUNCTION__, self->info);
        fflush(stdout);
        pthread_cond_wait(&self->cond, &self->cond_mutex);
        // 经过测试发现 signal 端在 pthread_mutex_unlock之后 这里 wait 才会返回
        fprintf(stdout, "->   %s() -%s- out pthread_cond_wait\n", __FUNCTION__, self->info);
        fflush(stdout);
    }
    fprintf(stdout, "->   %s() -%s- out while\n", __FUNCTION__, self->info);
    fflush(stdout);
    // need self->signaled=false; ?
    self->signaled = false;
    pthread_mutex_unlock(&self->cond_mutex);
}

void if_wait(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);
    if (!self->signaled) {
        pthread_cond_wait(&self->cond, &self->cond_mutex);
    }
    // self->signaled = true;
    pthread_mutex_unlock(&self->cond_mutex);
}

void only_signal(struct waiter *self)
{
    pthread_cond_signal(&self->cond);
}

void mutex_signal(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);
    pthread_cond_signal(&self->cond);
    pthread_mutex_unlock(&self->cond_mutex);
}

void signal_after_variable(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);
    self->signaled = true;

    // 不加计时不容易看出来
    struct timespec ts = { 2, 0 };
    nanosleep(&ts, NULL);

    pthread_cond_signal(&self->cond);
    pthread_mutex_unlock(&self->cond_mutex);
}

void signal_after_v2_variable(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);
    self->signaled = true;

    // 不加计时不容易看出来
    struct timespec ts = { 2, 0 };
    nanosleep(&ts, NULL);

    pthread_mutex_unlock(&self->cond_mutex);

    pthread_cond_signal(&self->cond);
}


void signal_before_variable(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);
    pthread_cond_signal(&self->cond);

    fprintf(stdout, "->   %s() -%s- signaled sleep to change variable\n", __FUNCTION__, self->info);
    fflush(stdout);
    // 不加计时不容易看出来 加计时制造资源竞争
    struct timespec ts = { 2, 0 };
    nanosleep(&ts, NULL);

    fprintf(stdout, "->   %s() -%s- signaled changed variable\n", __FUNCTION__, self->info);
    fflush(stdout);
    self->signaled = true;
    pthread_mutex_unlock(&self->cond_mutex);
}

void signal_before_v2_variable(struct waiter *self)
{
    pthread_cond_signal(&self->cond);

    fprintf(stdout, "->   %s() -%s- signaled sleep to change variable\n", __FUNCTION__, self->info);
    fflush(stdout);
    // 不加计时不容易看出来
    struct timespec ts = { 2, 0 };
    nanosleep(&ts, NULL);

    fprintf(stdout, "->   %s() -%s- signaled changed variable\n", __FUNCTION__, self->info);
    fflush(stdout);

    pthread_mutex_lock(&self->cond_mutex);
    self->signaled = true;
    pthread_mutex_unlock(&self->cond_mutex);
}


void best_solution_signal(struct waiter *self)
{
    pthread_mutex_lock(&self->cond_mutex);

    self->signaled = true;

    // 不加计时不容易看出来
    struct timespec ts = { 2, 0 };
    nanosleep(&ts, NULL);

    pthread_cond_broadcast(&self->cond); // 这两句的先后顺序有要求

    pthread_mutex_unlock(&self->cond_mutex);
}
