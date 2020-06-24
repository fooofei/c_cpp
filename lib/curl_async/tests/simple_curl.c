#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "curl_common.h"
#include "crt_dbg_leak.h"
#include "datetime.h"


#define EXPECT(expr) do {                                                                          \
        if (!(expr)) {                                                            \
            fprintf(stderr, "unexpect %s  (%s:%d)\n", #expr, __FILE__, __LINE__); \
            fflush(stderr);                                                       \
        }                                                                         \
    } while (0)

static void test_sleep(int sleep_time)
{
    const char *url = "http://127.0.0.1:8000";
    // const char * url = "http://192.168.0.103:8000";
    CURLcode ret;
    CURL *curl_handle = 0;
    clock2_t start;
    clock2_t end;
    char *str = 0;
    size_t size = 0;


    clock2_now(&start);
    make_curl_block(url, 5, &curl_handle);

    //  both worked
    // curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, 2 * 1000);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 2);

    ret = curl_easy_perform(curl_handle);
    /*
    curl 内部处理，把 socket 设置为 no-blocking 的

    还是启动了一个线程的
    (gdb) bt
#0  0x00007ffff7555040 in pthread_create@@GLIBC_2.2.5 () from /lib64/libpthread.so.0
#1  0x000000000045acaf in Curl_thread_create (func=0x412338 <getaddrinfo_thread>, arg=0x6ad330) at curl_threads.c:72
#2  0x0000000000412691 in init_resolve_thread (conn=0x6ac658, hostname=0x6acfc8 "localhost", port=8000,
hints=0x7fffffffdcc0) at asyn-thread.c:416 #3  0x0000000000412ce2 in Curl_resolver_getaddrinfo (conn=0x6ac658,
hostname=0x6acfc8 "localhost", port=8000, waitp=0x7fffffffdd84) at asyn-thread.c:685 #4  0x000000000044a64d in
Curl_getaddrinfo (conn=0x6ac658, hostname=0x6acfc8 "localhost", port=8000, waitp=0x7fffffffdd84) at hostasyn.c:125 #5
0x00000000004245b1 in Curl_resolv (conn=0x6ac658, hostname=0x6acfc8 "localhost", port=8000, entry=0x7fffffffde38) at
hostip.c:576 #6  0x00000000004246e9 in Curl_resolv_timeout (conn=0x6ac658, hostname=0x6acfc8 "localhost", port=8000,
entry=0x7fffffffde38, timeoutms=2000) at hostip.c:757 #7  0x00000000004384c4 in resolve_server (data=0x6a1038,
conn=0x6ac658, async=0x7fffffffdfc5) at url.c:3427 #8  0x0000000000439988 in create_conn (data=0x6a1038,
in_connect=0x6a1048, async=0x7fffffffdfc5) at url.c:4029 #9  0x0000000000439bd9 in Curl_connect (data=0x6a1038,
in_connect=0x6a1048, asyncp=0x7fffffffdfc5, protocol_done=0x7fffffffdfc4) at url.c:4122 #10 0x000000000040a850 in
multi_runsingle (multi=0x6a69a8, now=..., data=0x6a1038) at multi.c:1442 #11 0x000000000040c13b in curl_multi_perform
(multi=0x6a69a8, running_handles=0x7fffffffe0c4) at multi.c:2178 #12 0x0000000000404649 in easy_transfer
(multi=0x6a69a8) at easy.c:686 #13 0x000000000040482c in easy_perform (data=0x6a1038, events=false) at easy.c:779 #14
0x0000000000404864 in curl_easy_perform (data=0x6a1038) at easy.c:798
    */

    curl_easy_cleanup(curl_handle);

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
    // _CrtSetBreakAlloc(75);


    curl_global_init(CURL_GLOBAL_ALL);

    // 1 超时有效，比如给对方 3s 才返回，这里设置超时为1s，则curl会提前返回

    test_sleep(1);


    curl_global_cleanup();

    crt_dbg_leak_unlock(&cdl);

    printf("main() end.\n");

    return 0;
}
