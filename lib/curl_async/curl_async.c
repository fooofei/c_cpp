#include <curl/curl.h>

#include "curl_async.h"

// https://curl.haxx.se/libcurl/c/libcurl-multi.html
// https://ec.haxx.se/libcurl-drive-multi.html
// curl_multi_perform() + curl_multi_wait()
// http://www.cnblogs.com/tinyfish/p/4719467.html
// https://gist.github.com/clemensg/4960504
// https://github.com/bagder/everything-curl

// curl 坑 call curl_easy_init() 前要自己 call curl_global_init()
//  否则 curl_easy_init() 内部自己 call curl_global_init()
//  本来 curl_easy_init() 是 thread-safe 的 API 就退化为 thread-unsafe 的 API 了
//   https://curl.haxx.se/libcurl/c/curl_easy_init.html

// 这是一种通过 cURL 内部的 select 来做 IO 事件循环的

#define _trace_details(fmt, ...) do {                              \
        FILE *f = stdout;             \
        fprintf(f, fmt, __VA_ARGS__); \
        fflush(f);                    \
    } while (0)
// #define _trace_details(fmt,...)

int curl_async_init(struct curl_async *self)
{
    if (self->curl) {
        curl_async_cleanup(self);
    }
    // when to call ?
    // curl_global_init(CURL_GLOBAL_ALL);

    self->curl = curl_multi_init();
    if (!self->curl) {
        return -1;
    }

    // use this to limit the queue max size
    curl_multi_setopt(self->curl, CURLMOPT_MAXCONNECTS, 1);
    curl_multi_setopt(self->curl, CURLMOPT_MAX_TOTAL_CONNECTIONS, 1);
    curl_multi_setopt(self->curl, CURLOPT_VERBOSE, 1);


    return 0;
}
int curl_async_cleanup(struct curl_async *self)
{
    if (self->curl) {
        curl_async_age(self);
        curl_multi_cleanup(self->curl);
    }
    self->curl = 0;
    return 0;
}

// not care about the server response
// only age the added handle

int curl_async_push_handle(struct curl_async *self, CURL *handle)
{
    int ret;
    ret = curl_multi_add_handle(self->curl, handle);
    if (ret != 0) {
        return -1;
    }
    int still_running = 0;
    ret = curl_multi_perform(self->curl, &still_running);
    // not care about the return value
    // libcurl says it should called multi time, every call will do different thing

    _trace_details("%s() still_running=%d %s:%d\n", __FUNCTION__, still_running, __FILE__, __LINE__);

    if (ret != 0) {
        _trace_details("%s() curl_multi_perform() return %d %s %s:%d\n", __FUNCTION__, ret, curl_multi_strerror(ret),
            __FILE__, __LINE__);
    }

    curl_async_age(self);

    return 0;
}
int curl_async_age(struct curl_async *self)
{
    int msgleft = 0;

    CURLMsg *msg = 0;
    CURL *handle = 0;
    void *private_data;

    if (!self->curl) {
        return -1;
    }

    for (; (msg = curl_multi_info_read(self->curl, &msgleft));) {
        if (msg->msg == CURLMSG_DONE) {
            handle = msg->easy_handle;

            // return_code = msg->data.result;

            // http_status_code = 0;
            // szUrl = 0;

            // curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_status_code);
            private_data = 0;
            curl_easy_getinfo(handle, CURLINFO_PRIVATE, (char **)&private_data);
            curl_async_private_cleanup(private_data);
            // this is set by curl_easy_setopt(curl, CURLOPT_PRIVATE, pointer);

            curl_multi_remove_handle(self->curl, handle);
            curl_easy_cleanup(handle);
            handle = 0;
        }
    }

    _trace_details("%s() msgleft=%d %s:%d\n", __FUNCTION__, msgleft, __FILE__, __LINE__);

    return 0;
}

// ref https://curl.haxx.se/libcurl/c/curl_multi_wait.html
int curl_async_wait_all(struct curl_async *self)
{
    const int wait_max_time_ms = 60 * 1000;
    CURLMcode r;
    int numfds;
    (void)r;
    int still_running;
    int repeats = 0;

    for (;;) {
        numfds = 0;
        still_running = 0;

        /*
        >	singleipconnect(struct connectdata * conn=0x11b4468, const Curl_addrinfo * ai=0x11bb6e8, curl_socket_t *
        sockp=0x11b46c0) Line 1071	C 观察到调用，curl 内部是在把 socket 设置为 no-blocking
        */
        r = curl_multi_perform(self->curl, &still_running);
        if (r == CURLM_OK) {
            r = curl_multi_wait(self->curl, NULL, 0, wait_max_time_ms, &numfds);
        }

        if (CURLM_OK != r) {
            return -1;
        }

        // 拟定前提条件：同类的访问请求 时间差不能大
        // 因此 如果有完成的请求 numfds>0 ，还有未完成的请求 still_running>0 只在这种情况下轮询

        if (still_running == 0) {
            break;
        }

        if (numfds == 0) {
            repeats += 1;
            if (repeats > 100) {
                return -1;
            }
        } else {
            repeats = 0;
        }
    }

    return 0;
}

void curl_async_still_running(struct curl_async *self, int *runnings)
{
    curl_multi_perform(self->curl, runnings);
}
