
/*
Windows 平台获取文件大小使用 GetFileSizeEx

posix 平台

  使用 fstat/stat 的缺陷

  1 struct stat.st_size 某些平台是 32位(4 字节)的，对于大于 4GB 文件就无能为力，

  2 某些平台，在 struct stat.st_size 是 32 位， 4 字节时，右移大于等于当前数据宽度，结果是未定义的，
  测试 mips32 位平台，IDA 查看编译生成结果二进制汇编，位移操作被优化省去。
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>


int get_filesize_bad(const char * fullpath, uint64_t * outsize)
{
    struct stat buf;
    int rc;

    memset(&buf, 0, sizeof(buf));
    // fd = open(fullpath,"rb");
    //rc = fstat(fd, &buf);
    // https://stackoverflow.com/questions/23169241/what-are-the-advantages-of-using-fstat-vs-stat
    // fstat stat 的区别在于使用的参数
    // fstat 因为要提前获得文件句柄，占有文件时间更长，所以安全性会更好
   
    
    rc = stat(fullpath, &buf);
    if (rc != 0)
    {
        return -1;
    }
    // st_size type = long
    *outsize += buf.st_size >> 32; // 可能是未定义的行为
    *outsize += buf.st_size;
    return 0;
}

int get_filesize_fix(const char * fullpath, uint64_t * outsize)
{
    struct stat buf;
    int rc;

    memset(&buf, 0, sizeof(buf));

    rc = stat(fullpath, &buf);
    if (rc != 0)
    {
        return -1;
    }
    // st_size type = long
    uint64_t v = (uint64_t)buf.st_size;
    *outsize += v >> 32;
    *outsize += buf.st_size;
    return 0;
}



int main()
{
    return 0;
}
