
/* This file is only for backup 
  A search file way use NTFS feature
*/

#ifndef FILE_ENUM_USN_H_
#define FILE_ENUM_USN_H_

#include <map>
#include <windows.h>
#include "../noncopyable.h"
#include "../PathBuffer.h"
#include "../array_buffer.h"
#include "../macros.h"
#include "../ensurecleanup.h"


namespace path{ 

class FileEnumUsn : public noncopyable
{

public:
	explicit FileEnumUsn()
		:namemap_it_(namemap_.end()) , 
		namemap_it_end_( namemap_.end()),
		frnmap_it_end_( frnmap_.end()),
		driveRootLen_(0)
	{
		driveRoot_[0] = TEXT('\0');
	}
	~FileEnumUsn()
	{}

	// 时间长
	bool begin( const wchar_t* search_path , size_t path_len )

	{
		base::PathBuffer driveRoot,VolName;
		driveRoot.assign(search_path, path_len);
		driveRoot.striptoroot();

		wchar_t szSystemNameBuf[12] = {0};
		BOOL bRet  = FALSE;
		do 
		{
			bRet = GetVolumeInformationW(driveRoot, NULL, 0,
				NULL,NULL,NULL, szSystemNameBuf, 12);
			if(!bRet) break;

			if(CompareStringW(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,szSystemNameBuf,-1,L"NTFS",-1)
				!=CSTR_EQUAL) break;

			// 判断为NTFS格式 才判断结束
			

			if (! VolName.assign(L"\\\\.\\",4) ) break;
			if( !VolName.append(driveRoot.c_str(),driveRoot.size()) ) break;

			// erase the last '\'
			VolName.removebackslash();
			wcscpy_s(driveRoot_,driveRoot.c_str());
			driveRootLen_ = driveRoot.size();
			bRet = TRUE;

		} while ( 0 );

		if ( !bRet )
		{
			return false ;
		}
		

		base::EnsureCloseFile hVol;

		do 
		{
			hVol = CreateFileW(VolName, GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ | FILE_SHARE_WRITE, // 必须包含有FILE_SHARE_WRITE
				NULL, // 这里不需要
				OPEN_EXISTING, // 必须包含OPEN_EXISTING, CREATE_ALWAYS可能会导致错误
				FILE_ATTRIBUTE_READONLY, // FILE_ATTRIBUTE_NORMAL可能会导致错误
				NULL);
			// run as admin
			if (hVol.IsInvalid()) break;


			// 得到总的文件数
			DWORD BytesReturned;
			NTFS_VOLUME_DATA_BUFFER ntfsVolData;
			bRet = DeviceIoControl(hVol, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, 
				&ntfsVolData, sizeof(ntfsVolData), &BytesReturned, NULL);
			if(!bRet) break;


			LARGE_INTEGER num;
			num.QuadPart = 1024;
			LONGLONG total_file_count = (ntfsVolData.MftValidDataLength.QuadPart/num.QuadPart);

			

			// 初始化USN日志文件 
			//  msdn:http://msdn.microsoft.com/en-us/library/aa364558%28v=VS.85%29.aspx

			CREATE_USN_JOURNAL_DATA cujd;
			cujd.MaximumSize = 0;
			cujd.AllocationDelta = 0;
			bRet = DeviceIoControl(hVol, FSCTL_CREATE_USN_JOURNAL, &cujd,
				sizeof(cujd), NULL, 0, &BytesReturned, NULL);
			if(!bRet) break;

			// 获取USN日志基本信息(用于后续操作)
			// msdn:http://msdn.microsoft.com/en-us/library/aa364583%28v=VS.85%29.aspx
			USN_JOURNAL_DATA usn;
			bRet = DeviceIoControl(hVol, FSCTL_QUERY_USN_JOURNAL, NULL,
				0, &usn, sizeof(USN_JOURNAL_DATA), &BytesReturned, NULL);
			if(!bRet) break;


			// 枚举USN日志文件中的所有记录
			// msdn:http://msdn.microsoft.com/en-us/library/aa364563%28v=VS.85%29.aspx
			MFT_ENUM_DATA med;
			med.StartFileReferenceNumber = 0;
			med.LowUsn = 0;
			med.HighUsn = usn.NextUsn;

			const DWORD kBufferSize = 4096UL;

			base::array_buffer<BYTE>	buf;
			buf.alloc_memory(kBufferSize);
			buf.zero_memory();
			DWORD dwUsnDataSize;
			PUSN_RECORD pUsnRecord = NULL;
			PBYTE		cursor = NULL;
			DWORD dwRetBytes ;
			DWORD  totalFileCount = 0;
			while (DeviceIoControl(hVol, FSCTL_ENUM_USN_DATA, &med, sizeof(med),
				buf.get(), kBufferSize, &dwUsnDataSize, NULL))
			{
				if (dwUsnDataSize<=sizeof(USN))
				{
					break;
				}
				dwRetBytes = dwUsnDataSize - sizeof(USN);//跳过了1个USN，此USN是下一论查询起点
				pUsnRecord = reinterpret_cast<PUSN_RECORD>(buf.get()+sizeof(USN));
				cursor = reinterpret_cast<PBYTE>(pUsnRecord);
				while (dwRetBytes>0)
				{
					//Ref:http://blog.csdn.net/xexiyong/article/details/17151573
					//Ref:http://www.microsoft.com/msj/1099/journal2/journal2.aspx

					namemap_[pUsnRecord->FileReferenceNumber] = std::wstring(reinterpret_cast<const wchar_t*>(cursor +  pUsnRecord->FileNameOffset)
						,pUsnRecord->FileNameLength/sizeof(wchar_t));
					frnmap_[pUsnRecord->FileReferenceNumber] = pUsnRecord->ParentFileReferenceNumber;
					++totalFileCount;
					dwRetBytes -= pUsnRecord->RecordLength;

					// 2014_05_03 try to get the file Real path butfailed .
					//GetPathFROMFRN(hVol,pUsnRecord->FileReferenceNumber,fileVolPath,MAX_PATH*2);

					pUsnRecord = reinterpret_cast<PUSN_RECORD>(cursor+pUsnRecord->RecordLength);
					cursor = reinterpret_cast<PBYTE>(pUsnRecord);
				}
				med.StartFileReferenceNumber = *(reinterpret_cast<USN*>(buf.get()));
				buf.zero_memory();
			}

			// 删除USN日志文件(当然也可以不删除)
			// msdn:http://msdn.microsoft.com/en-us/library/aa364561%28v=VS.85%29.aspx
			DELETE_USN_JOURNAL_DATA dujd;
			dujd.DeleteFlags = USN_DELETE_FLAG_DELETE;
			dujd.UsnJournalID = usn.UsnJournalID;
			bRet = DeviceIoControl(hVol, FSCTL_DELETE_USN_JOURNAL,
				&dujd, sizeof(dujd),
				NULL, 0, &BytesReturned, NULL);
			if (! bRet ) break;

			namemap_it_ = namemap_.begin();
			namemap_it_end_ = namemap_.end();
			frnmap_it_end_ = frnmap_.end();
			return true;
		} while ( 0 );
		return false;
	}


	bool next()
	{
		LONGLONG i = 0;
		ULONGLONG parent;
		current_file_.clear();
		if ( namemap_it_ != namemap_it_end_ )
		{
			
			current_file_.assign(namemap_it_->second.c_str() , namemap_it_->second.size());
			reversepath();
			// 向上递归查找层级
			namemap_t::const_iterator nameit ;
			frnmap_t::const_iterator frnit;
			parent  = namemap_it_->first;
			do 
			{
				parent = frnmap_.at(parent);
				nameit = namemap_.find(parent);
				if( nameit == namemap_it_end_ ) break;

				appendpath(nameit->second.c_str(),nameit->second.size() );
				frnit = frnmap_.find(parent);
				if( frnit == frnmap_it_end_ ) break;
			} while ( true );
			
			appendpath(driveRoot_,driveRootLen_);
			reversepath();
			++ namemap_it_;
			return true;
		}

		return false;
	}
	const std::wstring& currentfilepath() const
	{
		return current_file_;
	}

	private:
		void reversepath()
		{
			std::reverse(current_file_.begin() , current_file_.end());
		}
		void appendpath( const wchar_t* dir , size_t dirlen)
		{
			if( dirlen == 0 ) return;

			if( dir[dirlen-1] != TEXT('\\')){
				if( current_file_.back() != TEXT('\\')){
					current_file_.append(1,TEXT('\\'));
				}
			}
			for(int i = (int)dirlen-1 ; i>=0 ; --i ){
				current_file_.append(1,dir[i]);
			}
		}

private:
	typedef std::map<DWORDLONG,std::wstring>	namemap_t;
	typedef std::map<DWORDLONG,DWORDLONG>		frnmap_t;

	wchar_t										driveRoot_[0x10];	
	size_t										driveRootLen_;			
	namemap_t									namemap_;
	frnmap_t									frnmap_;
	std::wstring								current_file_;
	namemap_t::const_iterator					namemap_it_;
	namemap_t::const_iterator					namemap_it_end_;
	frnmap_t::const_iterator					frnmap_it_end_;
};

};//namespace path

#endif //FILE_ENUM_EX_H_