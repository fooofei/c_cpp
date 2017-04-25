
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
    һ�������������ͷ����Ƿ���ռλ�ģ�ʹ�� pre next ����ָ���ƶ�
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
    �������������ͷ�����ռλ��
    �Ͳ��ϵİ� head->next ָ�������ƶ�, ɾ�� head->next ָ��Ľ��, ���ѵ����� head ����ͷż���
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
    ʹ�ö���ָ��������˼·����һ���ı䣬�������������
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
    ʹ�ö���ָ������ĸı䣬û����ͷ����Ƿ����⣬ֻ�����Ŷ���ָ���ƶ����, �Ƽ����ַ�ʽ��д
*/
void release_second_rank_pointer(node_t ** head)
{
    if (!(head && *head)) return;

    for (;*head;)
    {
        node_t * next = (*head)->next;
        node_free(head);
        *head = next; 

        // �����÷�
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



