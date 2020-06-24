#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <assert.h>
#include <mcheck.h>

#include <curl/curl.h>
#include "datetime.h"

typedef unsigned long long uint64;
typedef long long int64;


// 只支持1个fd
// 只支持 poll，不支持 epoll， select 等其他
struct curl_noblock {
    CURLM *multi;
    struct pollfd *fds;
    int *nfd;
    int idx;
};
/* 当 curl change fd list 时候，被调用  */
static int curlnb_sock_cb(CURL *curl, curl_socket_t fd, int what, void *cbp, void *sockp0)
{
    struct curl_noblock *curlnb = cbp;
    struct curl_noblock *sockp = sockp0;
    switch (what) {
        case CURL_POLL_NONE:
            printf("CURL_POLL_NONE\n");
            break;
        case CURL_POLL_IN:
            printf("CURL_POLL_IN\n");
            if (NULL == sockp) {
                assert(curlnb->idx == -1);
                curl_multi_assign(curlnb->multi, fd, curlnb);
                curlnb->idx = *(curlnb->nfd);
                *(curlnb->nfd) += 1;
                sockp = cbp;
            }
            assert(sockp->idx >= 0);
            curlnb->fds[sockp->idx].events = POLLIN;
            curlnb->fds[sockp->idx].fd = fd;
            break;
        case CURL_POLL_OUT:
            printf("CURL_POLL_OUT\n");
            if (NULL == sockp) {
                assert(curlnb->idx == -1);
                curl_multi_assign(curlnb->multi, fd, curlnb);
                curlnb->idx = *(curlnb->nfd);
                *(curlnb->nfd) += 1;
                sockp = cbp;
            }
            assert(sockp->idx >= 0);
            curlnb->fds[sockp->idx].events = POLLOUT;
            curlnb->fds[sockp->idx].fd = fd;
            break;
        case CURL_POLL_REMOVE:
            printf("CURL_POLL_REMOVE\n");
            if (sockp) {
                curlnb->fds[sockp->idx].fd = -1;
                sockp->idx = -1;
                curl_multi_assign(curlnb->multi, fd, NULL);
                *(curlnb->nfd) -= 1;
            }
            break;
    }
}
/* called when cURL chagned the timeout */
// static int curlnb_timer_cb(CURLM * multi, long timeout_ms, void * userp)
// {
//     printf("timeout_ms=%lld %s:%d\n", (int64)timeout_ms, __FILE__, __LINE__);
// }
int curlnb_init(struct curl_noblock *curlnb, struct pollfd *fds, int *nfd)
{
    CURLM *multi = NULL;
    multi = curl_multi_init();
    curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, curlnb_sock_cb);
    // 我们废弃
    // curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, curlnb_timer_cb);
    curl_multi_setopt(multi, CURLOPT_VERBOSE, 1);
    curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, curlnb);

    curlnb->multi = multi;
    curlnb->fds = fds;
    curlnb->nfd = nfd;
    curlnb->idx = -1;
}
void curlnb_term(struct curl_noblock *curlnb)
{
    curl_multi_cleanup(curlnb->multi);
    curlnb->multi = 0;
    curlnb->idx = -1;
}
void curlnb_kickstart(struct curl_noblock *curlnb)
{
    int remain = 0;
    curl_multi_socket_action(curlnb->multi, CURL_SOCKET_TIMEOUT, 0, &remain);
}
void curlnb_action(struct curl_noblock *curlnb, int fd, uint32_t revents, int *remain)
{
    int mask = 0;

    if (revents) {
        if (revents & POLLIN) {
            mask |= CURL_CSELECT_IN;
        }
        if (revents & POLLOUT) {
            mask |= CURL_CSELECT_OUT;
        }
        if (revents & (POLLHUP | POLLERR)) {
            mask |= CURL_CSELECT_ERR;
        }
        curl_multi_socket_action(curlnb->multi, fd, mask, remain);
    }
}

static void poll_curl()
{
    const char *url = "http://127.0.0.1:8000";
    CURLcode ret;
    CURL *curl_handle = NULL;
    clock2_t start;
    clock2_t end;
    char *str = 0;
    size_t size = 0;
    struct curl_noblock curlnb = { 0 };
    struct pollfd fds[5] = { 0 };
    int fd_cnt = 0;
    int r;

    clock2_now(&start);
    make_curl_block(url, 50, &curl_handle);

    //  both worked
    // curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, 2 * 1000);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 7);

    curlnb_init(&curlnb, fds, &fd_cnt);

    curl_multi_add_handle(curlnb.multi, curl_handle);
    curlnb_kickstart(&curlnb);

    for (; fd_cnt > 0;) {
        r = poll(fds, fd_cnt, 5000);
        if (r == 0) {
            printf("poll timeout %s:%d\n", __FILE__, __LINE__);

            break;
        }
        if (r > 0) {
            int remain = 0;
            curlnb_action(&curlnb, fds[0].fd, fds[0].revents, &remain);
        }
    }

    int msgleft = 0;
    CURLMsg *msg = NULL;

    // http 的 response content 还是在 write_cb 里拿到
    // 这里拿不到
    for (; (msg = curl_multi_info_read(curlnb.multi, &msgleft));) {
        if (msg->msg == CURLMSG_DONE) {
            int code = 0;
            curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &code);
        }
    }


    curl_multi_remove_handle(curlnb.multi, curl_handle);
    curl_easy_cleanup(curl_handle);
    curlnb_term(&curlnb);
    clock2_now(&end);
    str = 0;
    size = 0;
    clock2_elapsed(&start, &end, &str, &size);

    printf("%s() take time %s\n", __FUNCTION__, str);
    free(str);
}

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    mtrace();
    getchar();

    poll_curl();

    curl_global_cleanup();


    printf("no block curl end\n");
    return 0;
}
