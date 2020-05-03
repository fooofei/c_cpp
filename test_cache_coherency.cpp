
/*
  这个文件用来测试缓存一致性

  文件的代码来自 http://blackfox1983.github.io/posts/2015/10/11/intro-of-cache-coherency/
  http://www.infoq.com/cn/articles/cache-coherency-primer

    timing code  in c++11
      std::chrono::steady_clock
      std::chrono::system_clock
      std::chrono::high_resolution_clock
      https://solarianprogrammer.com/2012/10/14/cpp-11-timing-code-performance/
*/

#include <stdio.h>
#include <thread>
#include <chrono>
#include <ctime>

static uint32_t thread_func(void* arg)
{
    int* p = (int*)arg;
    int i;
    for (i = 0; i < 1000000; i += 1) {
        *p += 1;
    }
    return 0;
}

int counter1[16];
int counter2[16];
int counter3[16];

void test_in_same_array()
{
    std::thread th1;
    std::thread th2;
    std::thread th3;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;

    start = std::chrono::steady_clock::now();

    th1 = std::thread(thread_func, &counter1[0]);
    th2 = std::thread(thread_func, &counter1[1]);
    th3 = std::thread(thread_func, &counter1[2]);

    if (th1.joinable()) {
        th1.join();
    }
    if (th2.joinable()) {
        th2.join();
    }
    if (th3.joinable()) {
        th3.join();
    }

    end = std::chrono::steady_clock::now();

    double v = std::chrono::duration<double, std::milli>(end - start).count();

    printf("%s() timeit %.3f %s:%d\n", __FUNCTION__, v, __FILE__, __LINE__);
}

void test_in_multi_array()
{
    std::thread th1;
    std::thread th2;
    std::thread th3;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;

    start = std::chrono::steady_clock::now();

    th1 = std::thread(thread_func, &counter1[0]);
    th2 = std::thread(thread_func, &counter2[0]);
    th3 = std::thread(thread_func, &counter3[0]);

    if (th1.joinable()) {
        th1.join();
    }
    if (th2.joinable()) {
        th2.join();
    }
    if (th3.joinable()) {
        th3.join();
    }

    end = std::chrono::steady_clock::now();

    double v = std::chrono::duration<double, std::milli>(end - start).count();

    printf("%s() timeit %.3f %s:%d\n", __FUNCTION__, v, __FILE__, __LINE__);
}

/*
output
test_in_same_array() timeit 10.899 I:\source\test_cache.cpp:68
test_in_multi_array() timeit 3.386 I:\source\test_cache.cpp:105
test_in_same_array() timeit 13.032 I:\source\test_cache.cpp:68
test_in_multi_array() timeit 3.177 I:\source\test_cache.cpp:105
test_in_same_array() timeit 12.036 I:\source\test_cache.cpp:68
test_in_multi_array() timeit 3.264 I:\source\test_cache.cpp:105
*/

int main()
{
    test_in_same_array();
    test_in_multi_array();

    test_in_same_array();
    test_in_multi_array();

    test_in_same_array();
    test_in_multi_array();

    return 0;
}
