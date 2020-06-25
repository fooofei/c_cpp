#include <cstdio>
#include <string>
#include <string.h>
#include <stdlib.h>


struct node_t {
    node_t()
    {
        printf("node ctor\n");
    }

    ~node_t()
    {
        printf("node dtor\n");
    }
};

struct function_identifier_t {
    const char *idf;
    function_identifier_t(const char *arg) : idf(arg)
    {
        printf("in %s\n", idf);
    }
    ~function_identifier_t()
    {
        printf("out %s\n", idf);
    }
};


void normal_new()
{
    function_identifier_t f("normal_new");
    node_t *p = new node_t;
    delete p;

    //  construct node & destruct node
}

void operator_new()
{
    function_identifier_t f("operator_new");
    node_t *p = (node_t *)operator new(sizeof(node_t));
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
node ctor
node dtor
out normal_new

in operator_new
out operator_new

*/
