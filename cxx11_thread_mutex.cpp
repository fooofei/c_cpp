
/* the file shows the use of std::thread, std::mutex, std::lock_guard
*/

#include <stdio.h>

#ifdef WIN32
#include "detectmemleak.h"
#ifdef _DEBUG
/* auto report memory leak on Windows debug */
struct _crt_dbg_leak_t g_mem(0);
#endif
#endif

#include <list>
#include <mutex>
#include <thread>

/* every thread shared the global data */
struct thread_global_data_t
{
  uint32_t count;
  std::mutex mutex;
  void clear() { count = 0; }

  void do_work_on_thread()
  {
    std::lock_guard<std::mutex> l(mutex);
    for (uint32_t i = 0; i < 100; ++i)
    {
      if (i % 2)
      {
        count ++;
      }
      else
      {
        count --;
      }
    }
  }

};

/* per thread one data */
struct thread_local_data_t
{
  uint32_t v;
  thread_global_data_t * global_data;
  void clear()
  {
    v = 0;
    global_data = 0;
  }
};
uint32_t thread_func(void *arg)
{
  thread_local_data_t *p = (thread_local_data_t *)arg;
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  p->global_data->do_work_on_thread();
  delete p;
  return 0;
}

int test_thread()
{
  typedef std::shared_ptr<std::thread> thread_ptr_t;
  typedef std::list<thread_ptr_t> thread_set_t;
  thread_set_t ths;

  std::shared_ptr<thread_global_data_t> g_p(new (std::nothrow)
                                                thread_global_data_t());
  g_p->clear();

  for (uint32_t i = 0; i < 20; ++i)
  {
    /* released in thread func after use */
    thread_local_data_t *p = new (std::nothrow) thread_local_data_t();
    p->clear();
    p->v = i + 1;
    p->global_data = g_p.get();
    thread_ptr_t th;
    /* notice the use way, maybe you can use std::bind */
    th.reset(new (std::nothrow) std::thread(thread_func, p));
    ths.push_back(th);
  }

  /* wait all threads finish */
  for (thread_set_t::iterator it = ths.begin(); it != ths.end(); ++it)
  {
    if ((*it)->joinable())
    {
      (*it)->join();
    }
  }

  /* if all test g_p->count is 0, then the std::thread use right */
  if (g_p->count)
  {
    /* if print count !=0, there have a condition race. */
    printf("g_count=%u\n", g_p->count);
  }
  printf("pass all\n");
  return 0;
}

int main()
{
  for (uint32_t i = 0; i < 40; ++i)
  {
    test_thread();
  }

  return 0;
}