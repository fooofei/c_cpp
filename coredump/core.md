

1 `$ ulimit -c` 修改的作用域
  ulimit 修改不影响 pipe 方式存储的 core 只在 core_pattern=core 时起作用
  在 login shell 中做出的修改只影响到 login shell  
  不同的 login shell 窗口之间也互不影响
  .sh 和 ELF 二进制运行时会继承系统的
  .sh 和 ELF 二进制运行时修改的 ulimit -c 与 login shell 之间都互不影响
  
2 gzip 的结果输出必须使用重定向符号 `>`，对 core_pattern 不友好， 
  core_pattern 不可以写这个符号
  以下都是失败
  ```
  |/usr/bin/gzip > %s/core_%%t_%%p_%%u_%%e.gz
  |/bin/bash exec "echo" %p>$PWD/test.info
  |exec "echo" %p>$PWD/test.info
  |/usr/bin/tar - -zcvf /home/test_samba_share/source/c_proper_coredump/unix/coredumps/whatever.tar.gz
  ```
  测试压缩命令
  ```
  Pass: cat Makefile | exec  "gzip" > x.gz  
  Fail: cat Makefile |tar - -zcvf  x.tar.gz   
  Pass: tar -zcvf x.tar.gz </bin/cat Makefile
  ```
  download tar http://vault.centos.org/7.2.1511/os/Source/SPackages/tar-1.26-29.el7.src.rpm
    http://ftp.gnu.org/gnu/tar/

3   https://www.linuxquestions.org/questions/programming-9/core-dump-to-a-pipe-is-failing-4175445816/


4 
  kill -3/-6 to make a run process to generate core dump.
  kill -s -QUIT/-ABRT

5
  另一个配置文件 
    /etc/systemd/coredump.conf
  
  另一个配置文件
    /etc/security/limits.conf
    配置 用户 test 的 文件大小限制为 1GB，如下
    test       hard  fsize  1024000

6 
  coredumpctl 操作 core 文件
    https://www.freedesktop.org/software/systemd/man/coredumpctl.html

7 
  资料阅读
    超大 coredump （20G）的曲折救国方法
    [拒绝超大coredump - 用backtrace和addr2line搞定异常函数栈] https://zhuanlan.zhihu.com/p/31630417

8
  core_pattern 文件的 global 性质讨论：
    https://stackoverflow.com/questions/30637720/how-to-change-core-pattern-only-for-a-particular-application?answertab=active#tab-top

9 
|/root/compress_core.sh %t %p %u %e
  time, pid, uid, executable_name

10 Docker 不要给 Docker 任何为了修改 core 的权限，不必要
  core 的设置就在 host 进行就好，host 设置了后，Docker Container 什么都不要配置。

  man 5 core 又这样一句话需要解读 
  The process created to run the program runs as user and group root.
  猜测会跟 Docker Container 运行的用户组 命名空间有关系。
