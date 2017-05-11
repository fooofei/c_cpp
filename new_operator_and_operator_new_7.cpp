
#include <cstdio>
#include <string>

struct node_t
{
    node_t()
    {
        printf("node ctor\n");
    }

    ~node_t()
    {
        printf("node dtor\n");
    }

    // http://blog.csdn.net/solstice/article/details/6198937
    // ������һ�仰��һ�����������أ���һ��Ҫ���� void operator delete (void * p),
    // �����ƣ���Ҫ���� void * operator new (size_t size), Ϊʲô����Ϊ node_t * p1 = new node_t; ���޷�ʹ�á�
    static void * operator new (size_t size,const char * , long)
    {
        printf("call operator new\n");
        return malloc(size);
    }

    // if not write, then will have one warning,
    // ��void *node_t::operator new(size_t,const char *,long)��: δ�ҵ�ƥ���ɾ��������������ʼ�������쳣���򲻻��ͷ��ڴ�
    // ˵���������ֻ���ڳ�ʼ��ʧ�ܵ�ʱ���ʹ�õ������� delete ��ʹ�ò�����
    //

    static void operator delete (void * p,const char *, long)
    {
        printf("call operator delete(void *, const char *, long)\n");
        free(p);
    }

    // д�� void operator delete (void * p,const char *, long) ��ȴ��д����������ʧ�ܣ�û�п��õ� delete (void *) ʹ��
    // 
    static void operator delete (void * p)
    {
        printf("call operator delete(void *)\n");
        free(p);
    }
};

struct function_identifier_t
{
    const char * idf;
    function_identifier_t(const char * arg):idf(arg)
    {
        printf("in %s\n",idf);
    }
    ~function_identifier_t()
    {
        printf("out %s\n",idf);
    }
};

void normal_new()
{
    function_identifier_t f("normal_new");
    node_t * p = new(__FILE__,__LINE__) node_t;
    // node_t * p1 = new node_t; // error
    delete p;

    //  construct node & destruct node
}

void operator_new()
{
    function_identifier_t f("operator_new");
    node_t * p = (node_t *)operator new(sizeof(node_t)); // error // cannot use
    // node_t * p = (node_t*)operator new(node_t); // error
    free(p);
    // no construct node
}

int main()
{
    normal_new();
    printf("\n");
    operator_new();
    return 0;
}


/*

in normal_new
call operator new
node ctor
node dtor
call operator delete(void *)
out normal_new

in operator_new
out operator_new




*/
