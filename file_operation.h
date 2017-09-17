
/************************************************************************/
/*                     
IFileOperation interface replace the SHFileOperation
2012_11_29 02:38 created
2014_04_19	重新封装为Dll，加入多字节处理，更改方法名字首字母小写
2014_10_19  IFileOperation不支持XP，加入XP的判断
2014_10_31	重新构造，不再XP\Win7分开编译，完善异常处理，完善函数名字,去掉类名改为命名空间
2014_11_22  goto语句重新换为 d
2015_01_08	参数增加HWND和操作标志flags。do {}  while( 0 ) ;
2015_01_10	为MoveFile和CopyFile增加fCreateDesFolder参数。
2015_03_03	新增x64之后，因为总是调用成kernel32.dll里的函数，所以去掉了命名空间
2015_07_28	修改为Com调用方式，简化调用
2015_08_22	尝试使用 pimpl 手法封装dll，但是不能使用LoadLibrary使用class,尝试失败
2017_09_10  00:20 重新梳理代码 pimpl 手法是能用在这里的，不要考虑 dll 了，并不方便
            Not compitable with Windows XP / NOT TEST
/************************************************************************/
#ifndef BASE_FILE_FILE_OPEARTION_H_
#define BASE_FILE_FILE_OPEARTION_H_
#pragma once


#include <Windows.h>
#include <string>

class file_operation_t
{
  file_operation_t(const file_operation_t &);
  file_operation_t & operator = (const file_operation_t &);

  struct file_operation_impl_t;
  file_operation_impl_t * p_;

public:
  file_operation_t();
  ~file_operation_t();

  HRESULT bind_hwnd(HWND);
  HRESULT set_flags(DWORD);
  HRESULT delete_item(const std::wstring &);
  HRESULT copy_item(const std::wstring & src, const std::wstring & dstfolder, const std::wstring & dstname);
  HRESULT move_item(const std::wstring & src, const std::wstring & dstfolder, const std::wstring & dstname);
  HRESULT rename_item(const std::wstring &, const std::wstring & name);
};

#endif //BASE_FILE_FILE_OPEARTION_H_
