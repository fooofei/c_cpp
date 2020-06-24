

## libcURL HTTP POST Compress

预期是 POST 服务端多量数据，因此期望 HTTP PAYLOAD 能够压缩。

libcURL 能在 POST 时自动压缩吗？

官方回复，不能。

https://stackoverflow.com/questions/25641337/is-it-possible-to-make-libcurl-compressing-data-of-the-http-messages

官方建议自己压缩，然后通过 CURLOPT_HTTPHEADER 设置 HTTP HEADER，

与服务端开发人员协商示意压缩和解压。

通过看 libcURL 开放的源代码，能够看到当服务端回复的内容是 `Content-Encoding: gzip`,

就会自动解压，这是一个 libcURL 帮我们做的功能。

还有要明确的是，c 语言的 zlib 库，能压缩 zlib 和 gzip 两种格式，但是什么 API 压缩什么格式，

要注意区分。

## 非阻塞(easy API)的几个级别

#### 第一个级别是总是使用 `curl_multi*` API

这种方案的特点是通过 cURL 内部的 select/poll 来接管多个 curl easy 具柄

的 SOCKET 网络IO事件。

典型的 API 是
```
curl_multi_perform();
r = curl_multi_wait(self->curl, NULL, 0, wait_max_time_ms, &numfds);
```

例子参考 `curl_async.c`

#### 第二个级别是 `curl_multi_fdset` 提取出给 `select` 使用的SOCKET集合

取出集合后，可以在自己的业务代码向 `fd_set` 增加其他的 fd，

然后填入 `select()` 来做IO循环。

#### 第三个级别是 `CURLMOPT_SOCKETDATA` 回调FD

通过 cURL 提供的回调，把回调中的 FD 以及想要的事件，加入到自己的`select/poll/epoll/kqueue`事件循环中
```
curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, curlnb);
```
这种方式也更加复杂，例子参考 `noblock_curl.c`

## muduo cURL

    https://github.com/chenshuo/muduo/blob/master/examples/curl/Curl.cc

    libcurl 的 multi 用法 很好的参考资料

## libcURL timeout

    easy 的API  `curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1); // seconds`  之后， 

    超时的 errbuf 总是 `Operation timed out after 1001 milliseconds with 0 out of -1 bytes received`

    换成 multi action 的API 后， 在同样设置该opt的情况下就成了 

    `Operation timed out after 2959 milliseconds with 0 out of -1 bytes received` 

    时间不等，还可能是 1225 等其他值
    

## libcURL mulit conn limit

    20180423 想要在 linux 使用 CMake 静态库使用 curl， 发现没使用过去。

    curl 的 CMake 难用，我下载了 curl-7.59.0.tar.gz 使用 .configure 方式才没有错误的编译过去。

    curl 貌似要搭配 openssl，注意这个，不推荐使用系统默认的 openssl，会出现依赖版本问题，不如静态构建

    测试调整 ulimit -n 100, 不设置 curl MAX* 的那些选项，不断增加新的 curl 请求，他也能 add 成功，return code=0，
    still_running 能达到 3000 多

    即便是设置了 MAX* 选项 
    ```
       curl_multi_setopt(self->curl,CURLMOPT_MAXCONNECTS,10);
       curl_multi_setopt(self->curl,CURLMOPT_MAX_TOTAL_CONNECTIONS,10);
    ```
    不断 add 超过限制，也是能添加成功，不知道这个选项有什么用。

    这次测试没有出现想象中的 curl error buffer 是 Cannot assign request address 类错误。

    查看当前进程打开的 socket 数目（不精确）# ll /proc/<pid>/fd | wc –l 

    这个数字跟 
    ```
    # cat /proc/net/tcp | wc –l
    6
    # cat /proc/net/udp | wc –l 
    1
    ```
    没有必然联系， 不存在加和关系。



##  查看 libcurl 启动线程情况

    // (gdb)  b __pthread_create_2_1
    
    全工程搜索 pthread 
    只有 Curl_resolv->Curl_getaddrinfo ->Curl_resolver_getaddrinfo
    -> init_resolve_thread 用到了 pthread 
    
## 来调试 libcurl

    //  静态库
    sh ./configure --disable-shared --without-ssl  --enable-debug --prefix=/home/test_samba_share/curl-noblock/curl_bin

    // 动态库

    编译代码的时候，使用的 curl 是动态库，在运行中，也是使用动态库，

    运行中发生崩溃， gdb core 文件能看到 libcurl.so 中的函数栈，但是看不到文件行号，

    看不到 frame 中的栈变量。

    怎么解决？

    20180419 的解决办法是临时做了一个调试版本的 libcurl.so ，然后让进程使用调试版本的 so，发生崩溃复现，

    进而调试。没有找到使用  libcurl.so.debug 的 RPM 方法。

    编译这个调试版本的 curl 使用的命令：
    ```
    sh ./configure --prefix=$PWD/../gen --enable-shared --enable-debug
    make -j
    make install
    ```
    生成到了 $PWD/../gen 目录

    运行时

    LD_LIBRARY_PATH=$PWD/../gen  ./mydemo


## curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, multi_timer_cb);
    
    // 这个回调跟 cURL中 update_timer 函数息息相关

## cURL poll in out 宏定义

    // curl-7.62.0\include\curl\multi.h

## curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, sock_cb);

    // 该回调中尾部两个参数的含义
    multi->socket_userp, 
                    // 被 CURLMOPT_SOCKETDATA 传递
                    // https://curl.haxx.se/libcurl/c/CURLMOPT_SOCKETDATA.html
                    //  类似全局性质的，cURL 中是与 multi 指针绑定的，不是与 easy 绑定
    entry->socketp 
                    // 被curl_multi_assign 传递
                    //与 easy绑定，类似局部性质的

    Curl_multi_closed 中会调用 sock_cb 回调 CURL_POLL_REMOVE

## 博客

    //
    https://moz.com/devblog/high-performance-libcurl-tips/
    curl_easy_reset 用来重复使用 easy handle

## 遇到的项目有使用 cURL multi API 的吗？

    //   
    没有

## cURL mulit 遇到的问题:
   
    http://biancheng.dnbcw.net/c/289289.html
    fd 临时问题

## Build with libcurl  so
      ```
      20180312 把 curl 当作库放入项目中，要求不要使用 yum -y install libcurl-devel 这种安装方式才能使用 libcurl

      下载  https://curl.haxx.se/download.html 版本 7.58.0
      尝试把 curl 编译为静态库
      使用命令 cmake -DCMAKE_BUILD_TYPE=release -DCURL_STATICLIB=ON -DBUILD_TESTING=OFF -DCURL_WERROR=ON  ..  
      直接编译 libcurl 工程，无法编译成功 
      错误 为 https://github.com/curl/curl/issues/2358

      更改为原始的 configure 方式编译
      mkdir output
      readlink -f ./output
      sh ./configure --disable-shared --with-ssl --prefix=<output abs path>
      make -j
      make install  对于 curl 这个项目来说，推荐使用 make install， 不然不好找 libcurl.a 和 include 文件
      然后使用 include/curl/include 目录和 ./lib/libcurl.a 
        增加的链接选项  $ curl-config --static-libs
        然后接着就遇到编译报错
         /libcurl.a(libcurl_la-http.o): unrecognized relocation (0x2a) in section `.text'
        在A上编译出的curl加入项目，在B上编译项目，会报错。在B上编译curl加入项目，在A上编译项目，没有问题。
      我发现 $ yum -y install libcurl-devel 干了的包括两件事（可能有其他） 1 添加 include , 2 $ ln -s /usr/lib64/libcurl.so.4.3.0 /usr/lib64/libcurl.so
      如果我们有机会手动 ln -s /usr/lib64/libcurl.so.4.3.0 /usr/lib64/libcurl.so ，也能使用动态库这个方法

      20180317 找到一个使用 so 的解决方案
      1 把curl include 都搞到项目里 
      2 在不修改本机的环境的前提条件下，找到本机的 /usr/lib64/libcurl.so.4.3.0 文件 拷贝到项目里
      3 ln -s libcurl.so.4.3.0  libcurl.so 这样项目里有了 libcurl.so
      4 makefile 增加  -L${abs_builddir}/lib/curl -lcurl 
      ```

 ## Build libcurl .a
     
     ```
    #!/bin/env bash
    curdir=$(dirname $(readlink -f $0))
    curl_package=$curdir/curl-7.61.0.tar.gz
    curl_unpacked_dir=$curdir/curl-7.61.0
    curl_install_dir=$curdir/curl

    ## unpack curl
    rm -rf $curl_unpacked_dir
    mkdir $curl_unpacked_dir
    # 不附带 directory 选项解压，该选项会解压为 $curl_unpacked_dir/curl-7.61.0 目录结构
    # 不符合预期
    #tar xf $curl_package --directory=$curl_unpacked_dir
    tar xf $curl_package 

    ## make curl
    mkdir -p $curl_install_dir
    cd $curl_unpacked_dir
    # with-ssl 表示使用本机安装的 openssl，一般在 /usr/local 这种目录
    # 那么构建好的二进制在其他机器运行也会去这样的目录寻找openssl 如果遇到版本与构建的不合适，
    # 报错为  version `OPENSSL_1.0.2' not found
    # 所以我们构建 curl 使用 --without-ssl 我们自己提供一个openssl一起参与构建
    sh $curl_unpacked_dir/configure --disable-shared --without-ssl --prefix=$curl_install_dir
    --enable-debug for debug
    make -j

    ## install curl to current directory
    make install
    rm -rf $curl_unpacked_dir

    # LD_FLAGS += ./curl/lib/libcurl.a -lldap -llber -lz
    ```

##  20180312 把 curl 当作库放入项目中，要求不要使用 yum -y install libcurl-devel 这种安装方式才能使用 curl
  
    //
    下载  https://curl.haxx.se/download.html 版本 7.58.0
    尝试把 curl 编译为静态库
    使用命令 cmake -DCMAKE_BUILD_TYPE=release -DCURL_STATICLIB=ON -DBUILD_TESTING=OFF -DCURL_WERROR=ON  ..  
    直接编译 curl 工程，无法编译成功 
    错误 为 https://github.com/curl/curl/issues/2358
    更改为原始的 configure 方式编译
    mkdir output
    readlink -f ./output
    sh ./configure --disable-shared --with-ssl --prefix=<output abs path>
    make -j18
    make install  对于 curl 这个项目来说，推荐使用 make install， 不然不好找 libcurl.a 和 include 文件
    然后使用 include/curl/include 目录和 ./lib/libcurl.a 
    增加的链接选项  $ curl-config --static-libs
    然后接着就遇到编译报错
     /libcurl.a(libcurl_la-http.o): unrecognized relocation (0x2a) in section `.text'
    在A上编译出的curl加入项目，在B上编译项目，会报错。在B上编译curl加入项目，在A上编译项目，没有问题。
    我发现 $ yum -y install libcurl-devel 干了的包括两件事（可能有其他） 1 添加 include , 2 $ ln -s /usr/lib64/libcurl.so.4.3.0 /usr/lib64/libcurl.so
    如果我们有机会手动 ln -s /usr/lib64/libcurl.so.4.3.0 /usr/lib64/libcurl.so ，也能使用动态库这个方法

    20180317 找到一个使用 so 的解决方案
    1 把curl include 都搞到项目里 
    2 在不修改本机的环境的前提条件下，找到本机的 /usr/lib64/libcurl.so.4.3.0 文件 拷贝到项目里
    3 ln -s libcurl.so.4.3.0  libcurl.so 这样项目里有了 libcurl.so
    4 makefile 增加  -L${abs_builddir}/lib/curl -lcurl 
