

#include <stdio.h>

#ifdef WIN32


#include "../utilshjf/detectmemleak.h"

#ifdef _DEBUG
struct _crt_dbg_leak_t g_mem(0);
#endif
#endif



#include <list>
#include <thread>
#include <mutex>


struct thread_global_data_t
{
    uint32_t count;
    std::mutex mutex;
    void clear() {
        count = 0;
    }
};
struct thread_wrapper_t
{
    uint32_t v;
    thread_global_data_t * p;
    void clear() {
        v = 0;
        p = 0;
    }
};
uint32_t thread_func(void * arg)
{
    thread_wrapper_t * p = (thread_wrapper_t *)arg;
 
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    {
        std::lock_guard<std::mutex> l(p->p->mutex);
        for (uint32_t i = 0; i < 100; ++i)
        {
            if (i % 2) {
                p->p->count++;
            }
            else {
                p->p->count--;
            }

        }
    }


    delete p;
    return 0;
}



int test_thread()
{
    typedef std::shared_ptr<std::thread> thread_ptr_t;
    typedef std::list<thread_ptr_t > thread_set_t;
    thread_set_t ths;

    std::shared_ptr<thread_global_data_t> g_p(new(std::nothrow)thread_global_data_t());

    g_p->clear();

    for (uint32_t i=0;i<20;++i)
    {
        thread_wrapper_t * p = new(std::nothrow)thread_wrapper_t();
        p->clear();
        p->v = i + 1;
        p->p = g_p.get();
        thread_ptr_t  th;
        th.reset(new(std::nothrow)std::thread(thread_func, p));
        ths.push_back(th);
    }

    for (thread_set_t::iterator it=ths.begin();it != ths.end(); ++it)
    {
        if ((*it)->joinable())
        {
            (*it)->join();
        }
    }

    if (g_p->count) {
        /* if print count !=0, there have a condition race. */
        printf("g_count=%u\n", g_p->count);
    }
    

    return 0;
}

int main()
{
    for (uint32_t i=0; i< 40; ++i )
    {
        test_thread();
    }
   
    return 0;
}