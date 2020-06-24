#pragma once


// 心得体会 使用 CURL 异步带来的好处还不如自己加 ring + 单独线程阻塞发
//   这样的架构稳定
// All api return 0 on success.
// Please use CURLOPT_TIMEOUT_MS/CURLOPT_TIMEOUT to set curl easy handle's timeout
//   both worked
//   curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, 2 * 1000);
//   curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 2);
// Remember to call curl_global_init(CURL_GLOBAL_ALL); ..  curl_global_cleanup();
//   at before the all threads's beginning.
//   the curl_global_init() is not thread-safe
//   If you not call before all threads beginning, then the call of curl_multi_init()/curl_easy_init()
//   both call curl_global_init() internal.
//   curl_multi_init()/curl_easy_init() is thread-safe API, but internal called curl_global_init()
//   will be thread-unsafe API.

struct curl_async {
    void *curl;
};

// Please call curl_global_init(CURL_GLOBAL_ALL); before this.
int curl_async_init(struct curl_async *self);
int curl_async_cleanup(struct curl_async *self);

// not care about the server response
// only age the added handle

int curl_async_push_handle(struct curl_async *self, CURL *handle);
// no-block clean up curl easy handles
int curl_async_age(struct curl_async *self);
void curl_async_still_running(struct curl_async *self, int *runnings);

// return 0 for every curl handle is get responsed
// return <0 for already have curl running
int curl_async_wait_all(struct curl_async *self);


/* multi 用法有些内存不能在 push handle 后就能释放，比如 slist * 类型的 headers
    通过 private 设置私有数据，然后在老化时清理
    这函数在外部实现
*/
void curl_async_private_cleanup(void *context);
