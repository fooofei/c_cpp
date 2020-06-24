#include "nmutex.h"
#include <assert.h>


#ifdef WIN32

void nmutex_init(struct nmutex *m)
{
    InitializeCriticalSection(&m->cs);
    m->owner = 0;
}


void nmutex_uninit(struct nmutex *m)
{
    assert(m->owner == 0);
    DeleteCriticalSection(&m->cs);
}

void nmutex_lock(struct nmutex *m)
{
    EnterCriticalSection(&m->cs);
    m->owner = GetCurrentThreadId();
}

void nmutex_unlock(struct nmutex *m)
{
    assert(m->owner == GetCurrentThreadId());
    m->owner = 0;
    LeaveCriticalSection(&m->cs);
}


#else


void nmutex_init(struct nmutex *m)
{
    int rc;
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    // rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    assert(rc == 0);
    rc = pthread_mutex_init(&m->mutex, NULL);
    assert(rc == 0);
    pthread_mutexattr_destroy(&attr);
}

void nmutex_uninit(struct nmutex *m)
{
    int rc;

    rc = pthread_mutex_destroy(&m->mutex);
    assert(rc == 0);
}

void nmutex_lock(struct nmutex *m)
{
    int rc;

    rc = pthread_mutex_lock(&m->mutex);
    assert(rc == 0);
}

void nmutex_unlock(struct nmutex *m)
{
    int rc;

    rc = pthread_mutex_unlock(&m->mutex);
    assert(rc == 0);
}

#endif
