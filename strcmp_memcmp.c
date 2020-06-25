#include <stdio.h>
#include <string.h>

/*
Visual Studio : 运行都不会崩溃，没问题。 memcmp 不崩溃是因为比较没到尾部。

gcc-6 asan 检测：可以检测到 memcmp 问题。
*/
void test_different_string()
{
    if (0 == strncmp("hello", "world", 0x10000)) {
    }

    if (0 == memcmp("hello", "world", 0x10000)) {
    }
}


/*
Visual Studio : memcmp 崩溃

 20180325 我用 Visual Studio 2017 debug release 都没有 crash

gcc-6 asan 检测：可以检测到 memcmp
*/
void test_same_string()
{
    if (0 == strncmp("hello", "hello", 0x10000)) {
        printf("nocrash\n");
    }


    if (0 == memcmp("hello", "hello", 0x10000)) {
    }
}

int main()
{
    test_same_string();
    return 0;
}
