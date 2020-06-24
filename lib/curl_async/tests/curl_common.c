#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

#include "curl_common.h"


struct curl_async_private_data {
    struct curl_slist *headers;
};

static int make_curl(const char *url, int sleep_time, CURL **outcurl)
{
    CURL *curl_handle = curl_easy_init();
    char fields[0x40] = { 0 };
    CURLcode ret;
    (void)ret;

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);

    // struct curl_slist *headers = NULL;
    // must use `=`
    // headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    // curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    // 注释这里 观察内存泄漏
    // curl_slist_free_all(headers);


    // notice the CURLOPT_POSTFIELDS not copy the param data
    //   so we must preserved by the calling application until the associated transfer finishes.
    snprintf(fields, sizeof(fields), "sleep_time=%d", sleep_time);
    // ret = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, fields);
    // curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, strlen(fields)); // 非必需
    // CURLOPT_COPYPOSTFIELDS 比 CURLOPT_POSTFIELDS 多的功能是会对参数 fields 做拷贝，用于非阻塞场景
    //     方便在这里就对 fields 做释放
    ret = curl_easy_setopt(curl_handle, CURLOPT_COPYPOSTFIELDS, fields);

    // 尝试做带 post fields 的 GET 请求
    // 在 CURLOPT_POSTFIELDS 之后设置
    // 依旧失败  fields 都被屏蔽了
    // ret = curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1);
    // 这个倒是很厉害
    ret = curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");

    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1);

    *outcurl = curl_handle;

    return 0;
}


int make_curl_block(const char *url, int sleep_time, CURL **outcurl)
{
    int rc;

    rc = make_curl(url, sleep_time, outcurl);
    if (*outcurl) {
        //         CURL * curl_handle = *outcurl;
        //         // 测试 headers 内存应该在哪里释放
        //         struct curl_slist *headers = NULL;
        //         headers = curl_slist_append(headers, "Content-Type: application/json");
        //         fprintf(stdout, "headers addr = %p %s:%d\n", headers, __FILE__, __LINE__); fflush(stdout);
        //         curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
        //         curl_slist_free_all(headers);
    }
    return rc;
}

int make_curl_noblock(const char *url, int sleep_time, CURL **outurl)
{
    int rc;

    rc = make_curl(url, sleep_time, outurl);
    if (*outurl) {
        CURL *curl_handle = *outurl;

        // 测试 headers 内存应该在哪里释放
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        fprintf(stdout, "headers addr = %p %s:%d\n", headers, __FILE__, __LINE__);
        fflush(stdout);
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
        // curl_slist_free_all(headers);

        /*
        当在这里 free list 后，非阻塞使用时就会崩溃 ，调用堆栈在下（linux 与这个一致）
        curl_async_test.exe!Curl_strncasecompare(const char * first, const char * second, unsigned int max) 行 139	C
        已加载符号。
        curl_async_test.exe!Curl_checkheaders(const connectdata * conn, const char * thisheader) 行 100	C	已加载符号。
        curl_async_test.exe!Curl_http(connectdata * conn, bool * done) 行 1872	C	已加载符号。
        curl_async_test.exe!multi_do(connectdata * * connp, bool * done) 行 1216	C	已加载符号。
        curl_async_test.exe!multi_runsingle(Curl_multi * multi, curltime now, Curl_easy * data) 行 1626	C	已加载符号。
        curl_async_test.exe!curl_multi_perform(Curl_multi * multi, int * running_handles) 行 2136	C	已加载符号。
        curl_async_test.exe!curl_async_push_handle(curl_async * self, void * handle) 行 62	C	已加载符号。
        curl_async_test.exe!test_async(...) 行 58	C	已加载符号。
        curl_async_test.exe!main(...) 行 92	C	已加载符号。

        */

        struct curl_async_private_data *context = calloc(1, sizeof(struct curl_async_private_data));
        context->headers = headers;
        curl_easy_setopt(curl_handle, CURLOPT_PRIVATE, context);
    }
    return rc;
}

void curl_async_private_cleanup(void *context)
{
    struct curl_async_private_data *self = context;

    curl_slist_free_all(self->headers);
    free(context);
}
