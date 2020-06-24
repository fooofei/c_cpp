#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "curl_common.h"
#include "crt_dbg_leak.h"
#include "datetime.h"
#include "../curl_async.h"

#define EXPECT(expr) do {                                                                          \
        if (!(expr)) {                                                            \
            fprintf(stderr, "unexpect %s  (%s:%d)\n", #expr, __FILE__, __LINE__); \
            fflush(stderr);                                                       \
        }                                                                         \
    } while (0)

/*
使用命令
// (gdb)  b __pthread_create_2_1
观察curl 对thread 的处理，是在什么时候启动线程
*/

static CURL *make_curl2(int sleep_time)
{
    const char *url = "http://127.0.0.1:8000";
    // const char * url = "http://192.168.0.103:8000";
    CURLcode ret;
    (void)ret;
    CURL *curl_handle = 0;

    make_curl_noblock(url, sleep_time, &curl_handle);

    return curl_handle;
}

static void test_async()
{
    struct curl_async cat;
    memset(&cat, 0, sizeof(cat));
    int ret;
    (void)ret;
    const int *p;
    clock2_t start;
    clock2_t end;
    char *str = 0;
    size_t size = 0;

    clock2_now(&start);
    ret = curl_async_init(&cat);
    EXPECT(ret == 0);
    const int times[] = { 116, 115, 114, 114, 114, 113, 112, 11, 0 };

    for (p = times; *p; p += 1) {
        CURL *handle = make_curl2(*p);
        // it worked
        // curl_easy_setopt(handle, CURLOPT_TIMEOUT, 2);
        ret = curl_async_push_handle(&cat, handle);
        EXPECT(ret == 0);
    }


    ret = curl_async_wait_all(&cat);
    EXPECT(ret == 0);

    ret = curl_async_cleanup(&cat);
    EXPECT(ret == 0);

    clock2_now(&end);
    str = 0;
    size = 0;
    clock2_elapsed(&start, &end, &str, &size);

    printf("%s() cost time %s\n", __FUNCTION__, str);
    free(str);
}


int main()
{
    struct _crt_dbg_leak cdl;
    memset(&cdl, 0, sizeof(&cdl));
    crt_dbg_leak_lock(&cdl);
    // _CrtSetBreakAlloc(97);


    curl_global_init(CURL_GLOBAL_ALL);

    test_async();


    curl_global_cleanup();

    crt_dbg_leak_unlock(&cdl);

    printf("main() end.\n");

    return 0;
}
