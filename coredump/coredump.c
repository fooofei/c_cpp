

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>






#include "coredump.h"


#ifndef __max
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef __min
#define __min(a,b) (((a) > (b)) ? (b) : (a))
#endif


// 使用命令 ulimit -a 查看大小 还是 0
//   core file size          (blocks, -c) 0
// 纠正：通过 ssh 登录后，ulimit -c 的设置就是该会话生命周期的
// API 是对该进程的设置 两个设置无必然联系
static void 
core_filesize_max()
{
    struct rlimit rli;
    // size is in bytes
    getrlimit(RLIMIT_CORE, &rli);
    // cannot be smaller than 500Mib
    //rli.rlim_cur = __max(500 * 1024 * 1024, rli.rlim_cur);
    // cannot be larger than 5 Gb

    // unlimited = RLIM_INFINITY
    // 
    //rli.rlim_cur = __min(5000000000, rli.rlim_max);
    rli.rlim_cur = rli.rlim_max;

    setrlimit(RLIMIT_CORE, &rli);
}
static void 
core_filesize_zero()
{
    struct rlimit rli;
    getrlimit(RLIMIT_CORE, &rli);
    rli.rlim_cur = 0;
    setrlimit(RLIMIT_CORE, &rli);
}


// 
/*  
  make sure current user have right to write /proc/sys

  gzip 对 pipe 不友好 output 一定要使用 > (重定向符号) 这个在 core_pattern 里不适用


  失败:
    ret = fprintf(f, "|/usr/bin/gzip > %s/core_%%t_%%p_%%u_%%e.gz", coredump_gen_path);
    应该是 gzip 使用有问题
  
  失败
    echo "|/bin/bash exec "echo" %p>$PWD/test.info  " > /proc/sys/kernel/core_pattern
    echo "|exec "echo" %p>$PWD/test.info  " > /proc/sys/kernel/core_pattern

  成功
    less Makefile | exec  "gzip" > x.gz  

  
  失败
    echo "|/usr/bin/tar - -zcvf /home/test_samba_share/source/c_proper_coredump/unix/coredumps/whatever.tar.gz" > /proc/sys/kernel/core_pattern
    创建了空文件
  
  失败
    less Makefile |tar - -zcvf  x.tar.gz   

  成功 
    tar -zcvf x.tar.gz </bin/cat Makefile

  ret = fprintf(f, "%s", "|/root/compress_core.sh %t %p %u %e");
  time, pid, uid, executable_name,
  please see man 5 core
  缺陷 在 ulimit -c 为 0 的情况下， 就是不输出 coredump  这个 tar 命令也会被调用 并且生成了空文件

  kill -3/-6 to make a run process to generate core dump.
  kill -s -QUIT/-ABRT

  https://www.linuxquestions.org/questions/programming-9/core-dump-to-a-pipe-is-failing-4175445816/

  分析源代码:
    download tar http://vault.centos.org/7.2.1511/os/Source/SPackages/tar-1.26-29.el7.src.rpm
    http://ftp.gnu.org/gnu/tar/

  ulimit -a 查看包括 core 在内的 其他文件大小限制
  ulimit -c 控制产生单个 core 文件的大小 单位 KB

  另一个配置文件 
    /etc/systemd/coredump.conf
  
  另一个配置文件
    /etc/security/limits.conf
    配置 用户 test 的 文件大小限制为 1GB，如下
    test       hard  fsize  1024000

  coredumpctl 操作 core 文件
    https://www.freedesktop.org/software/systemd/man/coredumpctl.html

  资料阅读
    超大 coredump （20G）的曲折救国方法
    [拒绝超大coredump - 用backtrace和addr2line搞定异常函数栈] https://zhuanlan.zhihu.com/p/31630417

  core_pattern 文件的 global 性质讨论：
    https://stackoverflow.com/questions/30637720/how-to-change-core-pattern-only-for-a-particular-application?answertab=active#tab-top
*/

// out the created file path
static int 
_core_helper_script(char ** out_path)
{
    FILE * f;
    int ret = 0;

    // env is case-sensitive, `home` is not right, `HOME` is right
    // thread-safe
    // the path not include the tail `/`
    const char * prefix = getenv("HOME");
    if (!prefix) {
        prefix = "";
    }

    enum{_size=1024,};
    char name[_size] = {0};

    for (;;)
    {
        ret = snprintf(name, _size, "%s/hw_gzip_stdin.sh", prefix);
        if (!(ret > 0 && ret < _size)) {
            break;
        }

        f = fopen(name, "wb");
        if (!f)
        {
            ret = -1;
            break;
        }

        fprintf(f, "%s", "#!/bin/bash\n");
        fprintf(f, "%s", "/bin/gzip >$1\n");
        
        ret = 0;
        *out_path = strdup(name);
        fclose(f); f = 0;
        break;
    }

    if (f)
    {
        fclose(f);
    }

    return ret;
}


// param: make sure coredump_gen_path is exists
static int 
core_corepattern_set(const char * coredump_gen_path)
{
    FILE * f = 0;
    int ret = 0;
    char * name=0;

    for (;;)
    {
        ret = _core_helper_script(&name);
        if (0 != ret) {
            break;
        }
        f = fopen("/proc/sys/kernel/core_pattern", "wb");
        if (!f) {
            ret = -EIO;
            break;
        }

        ret = fprintf(f, "|/bin/sh %s %s/core_%%t_%%p_%%u_%%e.gz", name,coredump_gen_path);
        if (ret <= 0) {
            ret = -2;
            break;
        }
        ret = 0;
        break;
    }


    if (f)
    {
        fclose(f);
    }
    if (name) {
        free(name);
    }

    return ret;
}

static int
core_corepattern_origin()
{
    FILE * f = 0;
    int ret = 0;

    for (;;)
    {
        f = fopen("/proc/sys/kernel/core_pattern", "wb");
        if (!f) {
            ret = -EIO;
            break;
        }

        ret = fprintf(f, "%s", "core");
        if (ret <= 0) {
            ret = -2;
            break;
        }
        ret = 0;
        break;
    }


    if (f)
    {
        fclose(f);
    }

    return ret;
}


static int 
core_open_dump(const char * coredump_gen_path)
{
    core_filesize_max();
    return core_corepattern_set(coredump_gen_path);
}

static int 
core_close_dump()
{
    core_filesize_zero();
    return core_corepattern_origin();
}


static int
_list_directory(const char * directory
    , void(*file_callback)(void * context, const char * path), void * context)
{
    DIR * hd = 0;
    struct dirent * fd = 0;
    enum { _name_size = 1024, };
    char name[_name_size + 1];
    int ret = 0;


    hd = opendir(directory);

    if (!hd) {
        return -1;
    }

    for (; NULL != (fd = readdir(hd));)
    {
        if (fd->d_name[0] == 'c' && 0 == memcmp(fd->d_name, "core_", 5))
        {
            ret = snprintf(name, _name_size, "%s/%s", directory, fd->d_name);
            if (ret > 0 && ret < _name_size)
            {
                file_callback(context, name);
            }
        }
    }
    closedir(hd); hd = 0;
    return 0;
}

// read the directory coredump_gen_path to see eithor we need core dump
// 通过指定文件名字的文件大小来决定要不要进行 core dump
// 目录便利层级 1， 不进行子目录
// ftw() 也是遍历目录的  但非线程安全
static void 
__callback_size(void * context, const char * name)
{
    struct stat * st = (struct stat *)context;
    off_t * size = (off_t*)(st+1);
    if (stat(name,st) == 0 && S_ISREG(st->st_mode)) // is regular file ?
    {
        *size += st->st_size;
    }
}

static bool
core_is_need_size(const char * coredump_gen_path)
{
    bool core_needed = true;

    struct
    {
        struct stat st;
        off_t total_size;
    }context;
        
    memset(&context, 0, sizeof(context));

    _list_directory(coredump_gen_path, __callback_size, &context);

    if (context.total_size > 100 * 1000 * 1000)
    {
        core_needed = false;
    }
    return core_needed;
}

// read the directory coredump_gen_path to see eithor we need core dump
// 通过指定文件个数来决定要不要进行 core dump
// 目录便利层级 1， 不进行子目录
static void 
__callback_count(void * context, const char * name)
{
    int * core_count = (int *)(context);
    *core_count += 1;

}
static bool
core_is_need_count(const char * coredump_gen_path)
{
    bool core_needed = true;

    struct
    {
        int core_count;
    }context;

    memset(&context, 0, sizeof(context));

    _list_directory(coredump_gen_path, __callback_count, &context);

    if (context.core_count > 5)
    {
        core_needed = false;
    }
    return core_needed;
}

// remove the core files
static void 
__callback_clear(void * context, const char * name)
{
    int * remove_files = (int *)context;
    struct stat * st = (struct stat *)(remove_files + 1);
    if (stat(name, st) == 0 && S_ISREG(st->st_mode))
    {
        remove(name);
        *remove_files += 1;
    }
}

// return removed files 
// return -1 on error
static int 
core_clear_cores(const char * coredump_gen_path)
{
    struct
    {
        int remove_files;
        struct stat st;
    }context;
    int ret;

    memset(&context, 0, sizeof(context));

    ret = _list_directory(coredump_gen_path, __callback_clear, &context);
    if (ret != 0) {
        return -1;
    }
    return context.remove_files;
}

int 
set_coredump(const char * coredump_gen_path)
{
    // 可调整这里是按照 count 还是 size 做限制

    // 如果 core 超出限制 则移除 older 的 core
    if (!core_is_need_count(coredump_gen_path))
    {
        core_clear_cores(coredump_gen_path);
    }

    // 这里不应该到达 为了健壮性 出现错误 就不再生成了
    if (!core_is_need_count(coredump_gen_path))
    {
        return core_close_dump();
    }

    return core_open_dump(coredump_gen_path);
}

void 
set_core_debug_mode()
{
    core_filesize_max();
    core_corepattern_origin();
}



/* 

echo "|$PWD/official_core_dump %p UID=%u GID=%g sig=%s" > /proc/sys/kernel/core_pattern

output 

argc=5
argc[0]=</home/source/c_proper_coredump/unix/official_core_dump>
argc[1]=<125711>
argc[2]=<UID=0>
argc[3]=<GID=0>
argc[4]=<sig=11>
Total bytes in core dump: 12722176


  Ok：
    echo "|$PWD/official_core_dump %p" > /proc/sys/kernel/core_pattern
    echo "|$PWD/official_core_dump %p 22" > /proc/sys/kernel/core_pattern

  fail:
    echo "|$PWD/official_core_dump 22" > /proc/sys/kernel/core_pattern
    echo "|$PWD/official_core_dump" > /proc/sys/kernel/core_pattern

*/

static int 
_official_source_code(int argc, const char * argv[])
{
    int tot, j;
    ssize_t nread;
    enum{ BUF_SIZE =0x200, };
    char buf[BUF_SIZE];
    FILE *fp;
    char cwd[PATH_MAX];

    /* Change our current working directory to that of the
    crashing process */

    snprintf(cwd, PATH_MAX, "/proc/%s/cwd", argv[1]);
    chdir(cwd);

    /* Write output to file "core.info" in that directory */

    fp = fopen("core.info", "w+");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    /* Display command-line arguments given to core_pattern
    pipe program */

    fprintf(fp, "argc=%d\n", argc);
    for (j = 0; j < argc; j++)
        fprintf(fp, "argc[%d]=<%s>\n", j, argv[j]);

    /* Count bytes in standard input (the core dump) */

    tot = 0;
    while ((nread = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
        tot += nread;
    fprintf(fp, "Total bytes in core dump: %d\n", tot);

    return 0;
}


void 
test()
{
    int ret = 0;
    int * crash = 0;

    ret = set_coredump("/home/source/core_dump/cores");
    printf("open_coredump ret = %d\n", ret);

    //_official_source_code(argc, argv);
    //open_coredump("/home");
    *crash = 0;
}

// 测试 setrlimit() 设置的生命周期 在进程结束后就会结束 无记忆效果
static void
test_rlimit()
{
    struct rlimit rli;
    // size is in bytes
    getrlimit(RLIMIT_CORE, &rli);
    // cannot be smaller than 500Mib
    //rli.rlim_cur = __max(500 * 1024 * 1024, rli.rlim_cur);
    // cannot be larger than 5 Gb

    // unlimited = RLIM_INFINITY
    // 
    //rli.rlim_cur = __min(5000000000, rli.rlim_max);
    printf("%s() rlim_cur=%lld %s:%d\n", __FUNCTION__, (long long)rli.rlim_cur, __FILE__, __LINE__);
    rli.rlim_cur = 120;

    setrlimit(RLIMIT_CORE, &rli);

    memset(&rli, 0, sizeof(rli));
    getrlimit(RLIMIT_CORE, &rli);
    printf("%s() rlim_cur=%lld %s:%d\n", __FUNCTION__, (long long)rli.rlim_cur, __FILE__, __LINE__);
}


int main()
{   
    char * p;
    _core_helper_script(&p);
    printf("path = %s %s:%d\n", p, __FILE__, __LINE__);
    free(p);
}
