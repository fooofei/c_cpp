
#include <cstdio>
#include <cstring>
#ifdef __cplusplus

#ifdef  _WIN64
#define offset(s,m)   (size_t)( (ptrdiff_t)&reinterpret_cast<const volatile char&>((((s *)0)->m)) )
#else
#define offset(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif

#else

#ifdef  _WIN64
#define offset(s,m)   (size_t)( (ptrdiff_t)&(((s *)0)->m) )
#else
#define offset(s,m)   (size_t)&(((s *)0)->m) //#define offset(s,m) (size_t)(&((s*)0)->m)
#endif

#endif	// __cplusplus 

// 这其实是微软的 afxtls.h 中提供的类


struct simple_list_t
{
    size_t cb_next_offset_;
    void * head_;

    explicit simple_list_t(size_t cb = 0) : cb_next_offset_(cb),head_(0)
    {
        ;
    }
    ~simple_list_t()
    {

    }

    void construct(size_t cb_next_offset)
    {
        cb_next_offset_ = cb_next_offset;
    }

    bool is_empty() const 
    {
        return (head_ == NULL);
    }

    void ** get_next_ptr(void * p) const
    {
        // 在 Node 结点结构体中 cb_next_offset_ 偏移的位置，是存放的下一个结点的指针，因此 这里可以看作 二级指针
        return (void**)((unsigned char *)p+cb_next_offset_);
    }

    void * get_next(void * p) const 
    {
        return *get_next_ptr(p);
    }

    void append_head(void * p)
    {
        *get_next_ptr(p) = head_;
        head_ = p;
    }

    void * get_head() const
    {
        return head_;
    }

    void clear()
    {
        head_ = NULL;
    }

    bool remove(void * p_remove)
    {
        if(!(p_remove && head_)) return false;

        if (head_ == p_remove)
        {
            head_ = get_next(p_remove);
            *get_next_ptr(p_remove) = NULL;
            return true;
        }
        else
        {
            void * p = head_;
            for (;p && get_next(p)!= p_remove;)
            {
                p = get_next(p);
            }

            if (p)
            {
                *get_next_ptr(p) = get_next(p_remove);
                *get_next_ptr(p_remove) = NULL;
                return true;
            }
        }
        return false;
    }

};


struct node_t 
{
    size_t data1;
    node_t * next;
    size_t data2;

    node_t()
    {
        memset(this,0,sizeof(*this));
    }
};

static size_t g_node_memory_log = 0;

node_t * node_alloc()
{
    ++g_node_memory_log;
    return new node_t;
}

void node_free(node_t ** p)
{
    delete *p;
    *p = NULL;
    --g_node_memory_log;
}


void list_free(node_t ** head)
{
    if(!(head && *head)) return;
    for (;*head;)
    {
        node_t * p = ((*head)->next);
        node_free(head);
        *head = p;
    }
}


int main()
{

    size_t cb = offset(node_t,next);

    simple_list_t list(cb);
    size_t node_count = 0;

    size_t i;
    for ( i=1;i<4;i++)
    {
        node_t * p = node_alloc();
        p->data1 = i;
        list.append_head(p);
        ++node_count;
    }


    size_t index=1;
    node_t * head = (node_t *)list.get_head();

    for (node_t * p = head;p;p=p->next)
    {
        printf("[%d/%d]data:%d\n",index,node_count,p->data1);
        ++index;
    }

    list_free(&head);      
    list.clear();

    if (g_node_memory_log != 0)
    {
        printf("memory leak\n");
    }

    return 0;

}
