
/*

chenshuo 问了一个思考题目。

思考题：如果用两个 mutex，一个用于保护“条件”，另一个专门用于和 cond 配合 wait()，会出现什么情况？

经过实践 ， 这样做是可以的

*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include <signal.h>

struct waiter
{
    pthread_cond_t cond;
    pthread_mutex_t cond_mutex;
    pthread_mutex_t signal_mutex;
    bool signaled;
    char info[0x100];
    bool force_close;
    void(*wait)(struct waiter *);
    void(*signal)(struct waiter *);
};

void waiter_init(struct waiter * self);
void waiter_destroy(struct waiter * self);

void while_wait(struct waiter * self);
void signal_after_variable(struct waiter * self);
void signal_before_variable(struct waiter * self);

static void * threadfunc(void * arg)
{
    struct waiter * self = (struct waiter *)arg;
    
    struct timespec ts = { 0, 500 * 1000 * 1000 };
    nanosleep(&ts, NULL);  // initialization takes 500ms

    fprintf(stdout, "->   in threadfunc call signal\n"); fflush(stdout);
    self->signal(self);
    return 0;
}

static void run(struct waiter * w, int sec)
{
    int rc;
    pthread_t th;
    memset(&th, 0, sizeof(th));

    rc = pthread_create(&th, NULL, threadfunc, w);
    printf("->   %s() -%s- pthread_create() return %d\n", __FUNCTION__,w->info, rc); fflush(stdout);
    
    struct timespec ts = { sec, 0 };
    nanosleep(&ts, NULL); // do some work before calling wait()

    printf("->   %s() -%s- waiting\n", __FUNCTION__, w->info); fflush(stdout);
    w->wait(w);
    printf("->   %s() -%s- wait return\n", __FUNCTION__, w->info); fflush(stdout);
}

static struct waiter * g_p_current_waiter;
static void sigalarm(int arg)
{
    fprintf(stdout, "[!] %s- clock hit %d, force signal \n",g_p_current_waiter->info, arg); fflush(stdout);
    // exit(1);
    g_p_current_waiter->signal(g_p_current_waiter);
    g_p_current_waiter->force_close = true;
}


static void test_framework(
    void(*fwait)(struct waiter *)
    , void(*fsignal)(struct waiter *)
    , const char * info
)
{
    struct waiter self;
    memset(&self, 0, sizeof(self));
    waiter_init(&self);
    self.wait = fwait;
    self.signal = fsignal;
    snprintf(self.info, sizeof(self.info), "%s", info);

    g_p_current_waiter = &self;

    alarm(5);
    fprintf(stdout, "-> %s() -%s- wait before signal\n", __FUNCTION__, self.info); fflush(stdout);
    run(&self, 0);


    //经过 5 sec 给当前进程发送消息 也就是回调函数 闹钟
    alarm(5); // the old one will be canceled.
    fprintf(stdout, "-> %s() -%s- wait after signal\n", __FUNCTION__, self.info); fflush(stdout);
    run(&self, 2);

    
    waiter_destroy(&self);
    printf("%s() -%s- %s\n", __FUNCTION__, self.info, (self.force_close ? "FAIL":"PASS"));
   
}

int main()
{
    signal(SIGALRM, sigalarm);
   

    test_framework(while_wait, signal_before_variable, "wait--signal-before-variable_ok"); fprintf(stdout, "\n\n");
    test_framework(while_wait, signal_after_variable, "wait--signal-after-variable_ok");  fprintf(stdout, "\n\n");

    alarm(0); // cancel all.
    return 0;
}

void waiter_init(struct waiter * p)
{
    pthread_mutex_init(&p->cond_mutex, NULL);
    pthread_cond_init(&p->cond, NULL);
    pthread_mutex_init(&p->signal_mutex, NULL);
    p->signaled = false;
}

void waiter_destroy(struct waiter *p)
{
    pthread_mutex_destroy(&p->cond_mutex);
    pthread_cond_destroy(&p->cond);
    pthread_mutex_destroy(&p->signal_mutex);
    p->signaled = false;
}


void while_wait(struct waiter * self)
{
    // 不可以继续使用原来的 wait， 会直接死锁
    pthread_mutex_lock(&self->cond_mutex);
    pthread_mutex_lock(&self->signal_mutex);
    fprintf(stdout, "->   %s() -%s- self->signaled=%d\n", __FUNCTION__, self->info, self->signaled); fflush(stdout);
    while (!self->signaled)
    {
        pthread_mutex_unlock(&self->signal_mutex);
        fprintf(stdout, "->   %s() -%s- in pthread_cond_wait\n", __FUNCTION__, self->info); fflush(stdout);
        pthread_cond_wait(&self->cond, &self->cond_mutex);
        // 经过测试发现 signal 端在 pthread_mutex_unlock之后 这里 wait 才会返回
        fprintf(stdout, "->   %s() -%s- out pthread_cond_wait\n", __FUNCTION__, self->info); fflush(stdout);
        pthread_mutex_lock(&self->signal_mutex);
    }
    fprintf(stdout, "->   %s() -%s- out while\n", __FUNCTION__, self->info); fflush(stdout);
    // need self->signaled=false; ?
    self->signaled = false;
    pthread_mutex_unlock(&self->signal_mutex);
    pthread_mutex_unlock(&self->cond_mutex);
}


void signal_after_variable(struct waiter * self)
{
    fprintf(stdout, "->    in %s()\n", __FUNCTION__); fflush(stdout);
    pthread_mutex_lock(&self->signal_mutex);
    fprintf(stdout, "->   %s() -%s- signaled sleep to change variable\n", __FUNCTION__, self->info); fflush(stdout);
    // 不加计时不容易看出来
    struct timespec ts = { 2,0 };
    nanosleep(&ts, NULL);
    self->signaled = true;
    fprintf(stdout, "->   %s() -%s- signaled changed variable\n", __FUNCTION__, self->info); fflush(stdout);
    pthread_mutex_unlock(&self->signal_mutex);

    pthread_cond_signal(&self->cond);
    fprintf(stdout, "->    out %s()\n", __FUNCTION__); fflush(stdout);
}

void signal_before_variable(struct waiter * self)
{
    fprintf(stdout, "->    in %s()\n", __FUNCTION__); fflush(stdout);
    pthread_cond_signal(&self->cond);
        
    pthread_mutex_lock(&self->signal_mutex);
    fprintf(stdout, "->   %s() -%s- signaled sleep to change variable\n", __FUNCTION__, self->info); fflush(stdout);
    struct timespec ts = { 2,0 };
    nanosleep(&ts, NULL);
    self->signaled = true;
    fprintf(stdout, "->   %s() -%s- signaled changed variable\n", __FUNCTION__, self->info); fflush(stdout);
    pthread_mutex_unlock(&self->signal_mutex);
    fprintf(stdout, "->    out %s()\n", __FUNCTION__); fflush(stdout);

}
