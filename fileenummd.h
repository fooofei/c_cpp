
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


// 2014_05_01 ��������������ʽ��һ����
// 2014_05_03 dll��װ
// 2014_09_18 �޸�Ϊ���ο��Բ���'\\'��β �����Ӵ���ΪĿ¼�б�ķ���
// 2014_12_04 �޸�Ϊ�����ռ���ʽ��ȥ���࣬�Ż����룬�����ۣ����߳�ʹ��_beginthreadex�����������û�����
// 2014_12_05 ���� CSimpleString �滻std::list<std::wstring>������
// 2014_12_13 ������Ϣ��Ӧ,WTLʹ�ø����㣬����ȫ
// 2015_03_02 ����x64
// 2015_05_01 ʹ��const wchar_t* ʱ���ӳ��Ȳ���������ٿ���
// 2015_05_02 ǧ���ס��Ҫ��dll�ж����ĺ����ӿ���ʹ��STL����
// 2015_05_23 ����ѡ�� C/C++ �������ɣ����п⡣Release�汾����ΪMT��ȡ������ʱmsvcr120.dll����
// 2015_05_24 ����ΪCom���÷�ʽ
// 2015_09_08 ����Ϊͷ�ļ�����ʽ���ֳɸ���ģ�飬�����ṩ������dll�ļ� �� ʹ��std::bind����

namespace base{ 

namespace path{

	class FileEnumMd
	{

        ISearchCallback						*pinterface_;
        unsigned int						maxThreadCount_;				// ����߳���Ŀ
        volatile unsigned long				activeThreadCount_;				// ��߳���Ŀ
        PathQueue							queue_dir_;						// Ŀ¼�б�
        base::MutexLock						mutex_;		
        base::ensure_close_handle_t			not_empty_;						// ��m_listDir������µ�Ŀ¼����λ�����ţ�
        base::ensure_close_handle_t			event_exit_;					// �������߳̽�Ҫ�˳�ʱ��λ�����ţ�
        base::ensure_close_handle_t			th_manage_;
        base::ptr_threaddata_t				thread_data_;
        base::ptr_threaddata_t				thread_manage_data_;

        DISALLOW_EVIL_CONSTRUCTORS(FileEnumMd);
	public:	

		FileEnumMd()
			:maxThreadCount_(0),
			pinterface_(NULL),
			activeThreadCount_(0)
		{
			not_empty_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			event_exit_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		}
		~FileEnumMd(void)
		{stop();}

		void pushdir( ptr_pathbuffer_t& p )
		{
			queue_dir_.push(p);
		}
		BOOL pushdir( const wchar_t* p , size_t l)
		{
			ptr_pathbuffer_t tp (new(std::nothrow)base::pathbuffer());
			if( NULL == tp) return FALSE;
			if ( tp->assign(p,l))
			{
				pushdir(tp);
				return TRUE;
			}
			return FALSE;
		}

		BOOL begin(ISearchCallback* pi, UINT nMaxThread , PHANDLE phThread  = NULL,  UINT* threadId =NULL)
		{
			pinterface_ = pi;
			maxThreadCount_ = nMaxThread;
			thread_manage_data_.reset(new(std::nothrow)base::ThreadData());
			thread_manage_data_->func_ = std::bind(&FileEnumMd::manage_thread_func,this);
			th_manage_ =  base::beginthreadcx(thread_manage_data_, threadId);
			if( NULL == th_manage_ )
			{
				return FALSE;
			}
			if( NULL != phThread )
				*phThread = th_manage_ ;
			return TRUE;
		}

		void wait()
		{
			WaitForSingleObject(th_manage_,INFINITE);
		}
		size_t size() const 
		{
			return queue_dir_.size();
		}

		void stop()
		{
			DWORD rt = WaitForSingleObject(th_manage_,0);
			switch (rt)
			{
			case WAIT_OBJECT_0:
				break;
			case WAIT_TIMEOUT:
                OutputDebugString(_T("kill"));
				TerminateThread(th_manage_,0);
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

			thread_data_.reset(new(std::nothrow)base::ThreadData());
			thread_data_->func_ = std::bind(&FileEnumMd::search_in_work_thread,this);
			// handle search dirs end
			BOOL bRet = FALSE ;
			for (UINT i=0;i<maxThreadCount_;++i)
			{
				bRet = QueueUserWorkItem(base::funcInThread<unsigned long>,thread_data_.get(),WT_EXECUTELONGFUNCTION);
			}
			if( bRet )
				WaitForSingleObject(event_exit_,INFINITE); // �ȴ����������߳̽���

			pinterface_->search_stop();// ֪ͨUI ����
		}

	private:
	//
	// ���� 
	//  1. ���� event �� event_not_empty ,event_exit_ , 1�� active_thread_count
	//  2. �����µ������ SetEvent(event_not_empty);
	//  3. �����񣬵����� active thread (active_thread_count > 0), ��ô����  WaitForSingleObject(event_not_empty,INFINITE)
	//  4. �����񣬶���  active_thread_count ==0 �� ��ô���˳����������� SetEvent(event_not_empty); WaitForSingleObject(event_not_empty,0) != WAIT_TIMEOUT
	//  			��δ��룬�������ţ�Ȼ��ȴ������û������������ŵ�״̬��Ҳ����û�������߳��� event_not_empty �� wait�� ˵���������һ���̣߳�����Ĵ�������̷��أ�
	//			  ��˾���Ҫ SetEvent(event_exit_) , ���н���
	//  
	//
		void search_in_work_thread()
		{
			base::pathbuffer filepath;
			BOOL bActive (TRUE);
			size_t namelen(0);
			base::ptr_pathbuffer_t fetchdir(NULL);
			path::FileIterator fiter;

			while (TRUE)
			{

				{
					// ȡ�µ�Ŀ¼
					base::MutexLockGuard lock(&mutex_);
					if (queue_dir_.empty())
					{
                        bActive = FALSE;
                        fetchdir.reset();
                    }
					else
					{
						fetchdir = queue_dir_.pop();
					}
				}

				if (!bActive) // ��ǰ������
				{
					if (InterlockedDecrement(&activeThreadCount_) == 0)
					{
						break;
					}
					ResetEvent(not_empty_); // �ȴ�����߳����
					WaitForSingleObject(not_empty_,INFINITE);
					InterlockedIncrement(&activeThreadCount_);
					bActive = TRUE;
					continue;
				}

				if( !fiter.begin(fetchdir->c_str(),fetchdir->size()) ) continue;

				while ( fiter.next() )
				{
					if( ! filepath.make_path(fiter.currentdir()->c_str(),fiter.currentdir()->size(),
						fiter.currentname(),fiter.currentnamelen())){
                            continue;
                    }
					if ( fiter.curIsDir())
					{
						{
							// ���������ļ�
							base::MutexLockGuard lock(&mutex_);
							base::ptr_pathbuffer_t p(new(std::nothrow)base::pathbuffer());
							if( NULL == p ) continue;
							p->assign(filepath);
							queue_dir_.push(p);
						}
						SetEvent(not_empty_);
					}
					else
					{
						{
							base::MutexLockGuard lock(&mutex_);
							enumFile(fiter.currentname(),fiter.currentnamelen(),
								filepath,filepath.size());
						}
					}
				}
			}
			SetEvent(not_empty_);
			if (WaitForSingleObject(not_empty_,0) != WAIT_TIMEOUT)
			{
				SetEvent(event_exit_);
			}
		}
		void enumFile(const wchar_t* filename, size_t namelen,
			const wchar_t* filepath, size_t pathlen)
		{
			if( pinterface_ != NULL)
			{
				pinterface_->search_enum_fileW(filename,namelen,
					filepath,pathlen);
			}
		}
	};


};//namespace path
};//namespace base

#endif //FILE_ENUM_MD_H_