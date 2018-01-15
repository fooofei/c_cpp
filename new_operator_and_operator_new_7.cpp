
#include <cstdio>
#include <string>
#include <string.h>
#include <stdlib.h>

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
    // 这里有一句话，一旦有如下重载，则一定要重载 void operator delete (void * p),
    // 接着推，又要重载 void * operator new (size_t size), 为什么？因为 node_t * p1 = new node_t; 将无法使用。
    static void * operator new (size_t size,const char * , long)
    {
        printf("call operator new\n");
        return malloc(size);
    }

    // if not write, then will have one warning,
    // “void *node_t::operator new(size_t,const char *,long)”: 未找到匹配的删除运算符；如果初始化引发异常，则不会释放内存
    // 说明这个重载只有在初始化失败的时候才使用到，正常 delete 是使用不到的
    //

    static void operator delete (void * p,const char *, long)
    {
        printf("call operator delete(void *, const char *, long)\n");
        free(p);
    }

    // 写了 void operator delete (void * p,const char *, long) ，却不写这个，会编译失败，没有可用的 delete (void *) 使用
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
