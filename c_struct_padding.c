
#include <stdio.h>


// 如果一个结构体是 1M 个(1,000,000) ，数目这么大，缩减每个结构体的内存大小，对于进程整体占用内存减小是有效果的

// gcc -Wpadded 能以 warning 形式告知哪些结构体的哪些域是被 padding 填充的

// 希望有一个技术或者工具能实时显示一个结构体大小，并且不断在调整域的位置时，能显示出结构体大小的差异改变

// 精品文章 失传的C结构体打包技艺 https://github.com/ludx/The-Lost-Art-of-C-Structure-Packing 
//     http://www.catb.org/esr/structure-packing/

// 一个非实时查看的工具 
    // 1 https://github.com/jmesmon/pahole
    // 2 yum -y install elfutils  elfutils-devel elfutils-libs elfutils-libelf 
    // 3 cmake 常规编译就行， 有个文件要改下

    // diff --git a/dwarves_fprintf.c b/dwarves_fprintf.c
    // index cd0ad3e..0701a96 100644
    // --- a/dwarves_fprintf.c
    // +++ b/dwarves_fprintf.c
    // @@ -18,6 +18,10 @@
    //  #include "config.h"
    //  #include "dwarves.h"
    // 
    // +#ifndef DW_TAG_mutable_type
    // +#define DW_TAG_mutable_type 0x3e /* Withdrawn from DWARF3 by DWARF3f. */
    // +#endif
    // +
    //  static const char *dwarf_tag_names[] = {
    //         [DW_TAG_array_type]               = "array_type",
    //         [DW_TAG_class_type]               = "class_type",
    // 
    // */
    // 3 不带 -O 带 -g 编译源代码 生成二进制 test  然后 pahole -V test > 1.log 就行,在输出日志里找到关心的结构体 

    // 举例：优化前
    // struct hash_table_s {
    // 	uint32_t                   count;                /*     0     4 */
    // 
    // 	/* XXX 4 bytes hole, try to pack */
    // 
    // 	hash_item_t *              items;                /*     8     8 */
    // 	rte_rwlock_t               t_rwlock;             /*    16     4 */
    // 
    // 	/* size: 24, cachelines: 1, members: 3 */
    // 	/* sum members: 16, holes: 1, sum holes: 4 */
    // 	/* padding: 4 */
    // 	/* last cacheline: 24 bytes */
    // };
    // 
    // 优化后：
    // struct hash_table_s {
    // 	hash_item_t *              items;                /*     0     8 */
    // 	uint32_t                   count;                /*     8     4 */
    // 	rte_rwlock_t               t_rwlock;             /*    12     4 */
    // 
    // 	/* size: 16, cachelines: 1, members: 3 */
    // 	/* last cacheline: 16 bytes */
    // };

struct test
{
    uint8_t a;
    uint32_t b;
    uint8_t c;
};

int main()
{
    char s[sizeof(struct test)];
    return 0;
}
