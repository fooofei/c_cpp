#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cstring.h"

#include "crt_dbg_leak.h"


static string_t foo(string_t t)
{
    string_literal(hello, "hello");
    string_buffer_create(ret);
    if (string_equal(hello, t)) {
        string_cat(ret, "equal");
    } else {
        string_cat(ret, "not equal");
    }
    return string_grab((string_buffer_tostring(ret)));
}

static void test()
{
    string_buffer_create(a);
    string_printf(a, "%s", "hello");
    string_t b = foo(a->str);
    printf("%s\n", b->cstr);
    string_printf(a, "very long string %01024d", 0);
    printf("%s\n", string_char(string_buffer_tostring(a)));
    string_buffer_close(a);
    string_release(b);
}


int main()
{
    struct _crt_dbg_leak cdl;
    memset(&cdl, 0, sizeof(cdl));
    crt_dbg_leak_lock(&cdl);

    test();


    memory_pool_release();

    crt_dbg_leak_unlock(&cdl);
    return 0;
}
