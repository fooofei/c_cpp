/* This file is only for backup
  A multi-thread way to search a file.
  From wangyanping's book.
*/

#ifndef FILE_ENUM_MD_H_
#define FILE_ENUM_MD_H_

#include <functional>
#include <process.h>

#include "../mutex.h"
#include "../ensurecleanup.h"
#include "../thread_data.h"
#include "FileEnum.h"
#include "PathQueue.h"
#include "ISearchCallback.h"

// 2014_05_01 归纳三种搜索方式到一个类
// 2014_05_03 dll封装
// 2014_09_18 修改为传参可以不带'\\'结尾 ，增加传参为目录列表的方法
// 2014_12_04 修改为命名空间形式，去掉类，优化代码，更美观，多线程使用_beginthreadex创建，方便用户管理
// 2014_12_05 增加 CSimpleString 替换std::list<std::wstring>做参数
// 2014_12_13 增加消息响应,WTL使用更方便，更安全
// 2015_03_02 增加x64
// 2015_05_01 使用const wchar_t* 时增加长度参数方便快速拷贝
// 2015_05_02 千万记住不要在dll中对外界的函数接口中使用STL容器
// 2015_05_23 编译选项 C/C++ 代码生成，运行库。Release版本更改为MT。取消运行时msvcr120.dll依赖
// 2015_05_24 更改为Com调用方式
// 2015_09_08 更改为头文件的形式，分成各个模块，不再提供繁琐的dll文件 ， 使用std::bind技术

namespace base {
namespace path {
class FileEnumMd {
    ISearchCallback *pinterface_;
    unsigned int maxThreadCount_; // 最大线程数目
    volatile unsigned long activeThreadCount_; // 活动线程数目
    PathQueue queue_dir_; // 目录列表
    base::MutexLock mutex_;
    base::ensure_close_handle_t not_empty_; // 向m_listDir中添加新的目录后置位（受信）
    base::ensure_close_handle_t event_exit_; // 各搜索线程将要退出时置位（受信）
    base::ensure_close_handle_t th_manage_;
    base::ptr_threaddata_t thread_data_;
    base::ptr_threaddata_t thread_manage_data_;

    DISALLOW_EVIL_CONSTRUCTORS(FileEnumMd);

public:
    FileEnumMd() : maxThreadCount_(0), pinterface_(NULL), activeThreadCount_(0)
    {
        not_empty_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        event_exit_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    ~FileEnumMd(void)
    {
        stop();
    }

    void pushdir(ptr_pathbuffer_t &p)
    {
        queue_dir_.push(p);
    }
    BOOL pushdir(const wchar_t *p, size_t l)
    {
        ptr_pathbuffer_t tp(new (std::nothrow) base::pathbuffer());
        if (NULL == tp)
            return FALSE;
        if (tp->assign(p, l)) {
            pushdir(tp);
            return TRUE;
        }
        return FALSE;
    }

    BOOL begin(ISearchCallback *pi, UINT nMaxThread, PHANDLE phThread = NULL, UINT *threadId = NULL)
    {
        pinterface_ = pi;
        maxThreadCount_ = nMaxThread;
        thread_manage_data_.reset(new (std::nothrow) base::ThreadData());
        thread_manage_data_->func_ = std::bind(&FileEnumMd::manage_thread_func, this);
        th_manage_ = base::beginthreadcx(thread_manage_data_, threadId);
        if (NULL == th_manage_) {
            return FALSE;
        }
        if (NULL != phThread)
            *phThread = th_manage_;
        return TRUE;
    }

    void wait()
    {
        WaitForSingleObject(th_manage_, INFINITE);
    }
    size_t size() const
    {
        return queue_dir_.size();
    }

    void stop()
    {
        DWORD rt = WaitForSingleObject(th_manage_, 0);
        switch (rt) {
            case WAIT_OBJECT_0:
                break;
            case WAIT_TIMEOUT:
                OutputDebugString(_T("kill"));
                TerminateThread(th_manage_, 0);
                break;
            default:
                break;
        }
        th_manage_.close();
    }

private:
    void manage_thread_func()
    {
        pinterface_->search_start();

        activeThreadCount_ = maxThreadCount_;

        base::pathbuffer tmp;
        ptr_pathbuffer_t path(NULL);

        thread_data_.reset(new (std::nothrow) base::ThreadData());
        thread_data_->func_ = std::bind(&FileEnumMd::search_in_work_thread, this);
        // handle search dirs end
        BOOL bRet = FALSE;
        for (UINT i = 0; i < maxThreadCount_; ++i) {
            bRet = QueueUserWorkItem(base::funcInThread<unsigned long>, thread_data_.get(), WT_EXECUTELONGFUNCTION);
        }
        if (bRet)
            WaitForSingleObject(event_exit_, INFINITE); // 等待所有搜索线程结束

        pinterface_->search_stop(); // 通知UI 结束
    }

private:
    //
    // 精髓
    //  1. 两个 event ， event_not_empty ,event_exit_ , 1个 active_thread_count
    //  2. 添入新的任务就 SetEvent(event_not_empty);
    //  3. 无任务，但是有 active thread (active_thread_count > 0), 那么就在
    //  WaitForSingleObject(event_not_empty,INFINITE)
    //  4. 无任务，而且  active_thread_count ==0 ， 那么就退出工作，并且 SetEvent(event_not_empty);
    //  WaitForSingleObject(event_not_empty,0) != WAIT_TIMEOUT
    //  			这段代码，先置受信，然后等待，如果没有拿走这个置信的状态，也就是没有其他线程在 event_not_empty 上
    //  wait， 说明这是最后一个线程，后面的代码会立刻返回，
    // 			  因此就需要 SetEvent(event_exit_) , 运行结束
    //
    //
    void search_in_work_thread()
    {
        base::pathbuffer filepath;
        BOOL bActive(TRUE);
        size_t namelen(0);
        base::ptr_pathbuffer_t fetchdir(NULL);
        path::FileIterator fiter;

        while (TRUE) {
            {
                // 取新的目录
                base::MutexLockGuard lock(&mutex_);
                if (queue_dir_.empty()) {
                    bActive = FALSE;
                    fetchdir.reset();
                } else {
                    fetchdir = queue_dir_.pop();
                }
            }

            if (!bActive) { // 当前不运行
                if (InterlockedDecrement(&activeThreadCount_) == 0) {
                    break;
                }
                ResetEvent(not_empty_); // 等待别的线程添加
                WaitForSingleObject(not_empty_, INFINITE);
                InterlockedIncrement(&activeThreadCount_);
                bActive = TRUE;
                continue;
            }

            if (!fiter.begin(fetchdir->c_str(), fetchdir->size()))
                continue;

            while (fiter.next()) {
                if (!filepath.make_path(fiter.currentdir()->c_str(), fiter.currentdir()->size(), fiter.currentname(),
                    fiter.currentnamelen())) {
                    continue;
                }
                if (fiter.curIsDir()) {
                    {
                        // 向队列添加文件
                        base::MutexLockGuard lock(&mutex_);
                        base::ptr_pathbuffer_t p(new (std::nothrow) base::pathbuffer());
                        if (NULL == p)
                            continue;
                        p->assign(filepath);
                        queue_dir_.push(p);
                    }
                    SetEvent(not_empty_);
                } else {
                    {
                        base::MutexLockGuard lock(&mutex_);
                        enumFile(fiter.currentname(), fiter.currentnamelen(), filepath, filepath.size());
                    }
                }
            }
        }
        SetEvent(not_empty_);
        if (WaitForSingleObject(not_empty_, 0) != WAIT_TIMEOUT) {
            SetEvent(event_exit_);
        }
    }
    void enumFile(const wchar_t *filename, size_t namelen, const wchar_t *filepath, size_t pathlen)
    {
        if (pinterface_ != NULL) {
            pinterface_->search_enum_fileW(filename, namelen, filepath, pathlen);
        }
    }
};
}; // namespace path
}; // namespace base

#endif // FILE_ENUM_MD_H_