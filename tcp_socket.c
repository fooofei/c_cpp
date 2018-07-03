
/* 一个健壮的 TCP sender 示例 包括
 -- REUSE ADDR
 -- SND TIMEOUT
 -- 发送失败重连(依旧会损失数据)
 
*/

struct sock_sender
{
    int sockfd;
    struct timespec connect_time;
    /* for debug info */
    char svr_ip[0x20];
    uint16_t svr_port;
};

void sock_sender_dtor(struct sock_sender * s)
{
    if (s->sockfd > 0)
    {
        shutdown(s->sockfd, SHUT_RDWR);
        close(s->sockfd);
    }
    memset(s, 0, sizeof(*s));
    s->sockfd = -1;
    /* you can update stat here. */
}

inline bool sock_sender_fd_is_valid(struct sock_sender * s)
{
    return s->sockfd > 0;
}

inline int flow_sender_ctor(struct sock_sender * s)
{
    const char * svr_addr_str = 0;
    uint16_t svr_port = 0;
    int value = 0;
    struct timeval sndtime = { 0 };
    struct sockaddr_in svr_addr = { 0 };

    /* get addr here. */
    svr_addr_str;
    svr_port;

    svr_addr.sin_family = AF_INET;
    ipaddr_pton(svr_addr_str, (uint32_t *)&svr_addr.sin_addr.s_addr);
    svr_addr.sin_port = htons(svr_port);

    s->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (s->sockfd == -1)
    {
        return -1;
    }
    s->svr_port = svr_port;
    snprintf(s->svr_ip, sizeof(s->svr_ip), svr_addr_str);
    /* update sock stat here. */
    value = 1;
    setsockopt(s->sockfd, SOL_SOCKET, SO_REUSEPORT, &value, sizeof(value));
    setsockopt(s->sockfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    setsockopt(s->sockfd, SOL_SOCKET, SO_LINGER, &value, sizeof(value));
    sndtime.tv_sec = 3;
    setsockopt(s->sockfd, SOL_SOCKET, SO_SNDTIMEO, &sndtime, sizeof(sndtime));
    clock_gettime(CLOCK_REALTIME, &s->connect_time);
    if (0 != connect(s->sockfd, (struct sockaddr *) &svr_addr, sizeof(svr_addr)))
    {
        sock_sender_dtor(s);
        return -1;
    }
    return 0;
}

inline int sock_sender_send(struct sock_sender * s, const void * data, size_t size)
{
    int sended_size = 0;
    struct timespec now = { 0 };
    
    if (!sock_sender_fd_is_valid(s))
    {
        clock_gettime(CLOCK_REALTIME, &now);
        if (now.tv_sec > s->connect_time.tv_sec && now.tv_sec - s->connect_time.tv_sec > 3)
        {
            if (0 != sock_sender_ctor(s))
            {
                // 函数进来 sockfd 不可用 再次初始化还失败
                return -1;
            }
        }
    }
    if (sock_sender_fd_is_valid(s))
    {
        errno = 0;
        sended_size = send(s->sockfd, data, size, MSG_NOSIGNAL);
        if (sended_size == sizeof(*flow))
        {
            return 0;
        }
        // 文件描述符存在但是发送失败 就清空文件描述符
        if (errno == EPIPE)
        {
            sock_sender_dtor(s);
        }
    }
    return -1;
}
