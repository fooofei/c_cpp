
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>


// ref https://coolshell.cn/articles/11377.html

typedef struct
{
    void * handle;
    char name[]; // same with char name[0];
}test_t1;


// x86 sizeof() = 4
// x64 sizeof() = 8
typedef struct
{
    void * handle;
    char  name[0]; // msvc c++ will have warning warning C4200: 使用了非标准扩展: 结构/联合中的零大小数组
                   //  note: 默认构造函数或 copy/move 赋值运算符将忽略此成员
}test_t2;

// x86 sizeof() = 8
// x64 sizeof() = 16
typedef struct
{
    void * handle;
    char name[1];  // recommended, will have redundant memory, but no warning
}test_t3;

// x86 sizeof() = 8
// x64 sizeof() = 16
typedef struct
{
    void * handle;
    char * name;
}test_t4;


void 
test_flexible_array()
{
    size_t test_t1_size = sizeof(test_t2) + 6;
    size_t test_t2_size = sizeof(test_t3) + 6;
    test_t2 * p1 = (test_t2*)calloc(1, test_t1_size);
    test_t3 * p2 = (test_t3*)calloc(1, test_t2_size);

    // x86 = 6
    printf("test_t1 rest size=%zu\n", test_t1_size - ((uint8_t*)(&p1->name) - (uint8_t*)p1));
    // x86 = 10
    printf("test_t2 rest size=%zu\n", test_t2_size - ((uint8_t*)(&p2->name) - (uint8_t*)p2));

    memcpy(p1->name, "hello", 5);
    memcpy(p2->name, "hello", 5);

    free(p1);
    free(p2);

    printf("pass %s()\n", __FUNCTION__);
}

int 
main()
{
    test_flexible_array();
    return 0;
}
