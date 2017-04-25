
#include <cstdio>
#include <string.h>

/*
    An simple single list.

    as rule , we not use first node.
*/


struct node_t
{
    struct node_t * next;
    int value;

    node_t() :next(0), value(0) {}
};

// log for is all node freed
static int g_count_node = 0;

node_t * node_alloc()
{
    ++g_count_node;
    return new node_t;
}

void node_free(node_t ** p)
{
    delete *p;
    *p = NULL;
    --g_count_node;
}

/*
    一般的链表，不考虑头结点是否是占位的，使用 pre next 两个指针移动
*/
void release_1(node_t ** head)
{
    if (!(head && *head)) return;

    node_t * pre = *head;
    node_t * next = NULL;
    
    for (;pre;)
    {
        next = pre->next;
        node_free(&pre);
        pre = next;
    }
    *head = NULL;
}

/*
    如果把链表视作头结点是占位的
    就不断的把 head->next 指针往下移动, 删除 head->next 指向的结点, 最后把单独的 head 结点释放即可
*/

void release(node_t ** head)
{
    if (!(head && *head)) return;

    for (;(*head)->next; )
    {
        node_t * next = (*head)->next;
        (*head)->next = next->next;
        node_free(&next);
    }
    
    node_free(head);
}
/*
    使用二级指针对上面的思路做了一个改变，看起来更奇怪了
*/
void release_weired(node_t ** head)
{
    if (!(head && *head)) return;
       
    node_t ** pp = &((*head)->next);
    for (;*pp;)
    {
        node_t * p = *pp;
        *pp = p->next;
        node_free(&p);
    }
    node_free(head);
}

/*
    使用二级指针后最后的改变，没考虑头结点是否特殊，只需随着二级指针移动结点, 推荐这种方式书写
*/
void release_second_rank_pointer(node_t ** head)
{
    if (!(head && *head)) return;

    for (;*head;)
    {
        node_t * next = (*head)->next;
        node_free(head);
        *head = next; 

        // 错误用法
        //node_t ** next = &((*head)->next);
        //node_free(head);
        //head = next;
    }
}


void node_append(node_t ** p, int value)
{
    *p = node_alloc();
    (*p)->value = value;
}

void test_remove(node_t ** head, int value)
{
    for (;*head;)
    {
        if ((*head)->value == value)
        {
            node_t * next = (*head)->next;
            node_free(head);
            *head = next;
        }
        else
        {
            head = &((*head)->next);
        }
    }
}

int main()
{

    node_t * head = node_alloc();
    head->value = 0;


    node_append(&(head->next), 1);
    node_append(&(head->next->next), 2);
    node_append(&(head->next->next->next), 3);


    //test_remove(&head, 0);

    //release(&head);
    release_second_rank_pointer(&head);
    if ( 0 != g_count_node)
    {
        printf("memory leak\n");
    }

    return 0;
}



