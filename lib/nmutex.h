#ifndef NMUTEX_H
#define NMUTEX_H


#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif


#ifdef WIN32
struct nmutex {
    CRITICAL_SECTION cs;
    DWORD owner;
};

#else
struct nmutex {
    pthread_mutex_t mutex;
};

#endif


void nmutex_init(struct nmutex *m);
void nmutex_uninit(struct nmutex *m);

void nmutex_lock(struct nmutex *m);
void nmutex_unlock(struct nmutex *m);


#endif // NMUTEX_H
