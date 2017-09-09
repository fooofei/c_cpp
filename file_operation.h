
/************************************************************************/
/*                     
IFileOperation interface replace the SHFileOperation
2012_11_29 02:38 created
2014_04_19	���·�װΪDll��������ֽڴ������ķ�����������ĸСд
2014_10_19  IFileOperation��֧��XP������XP���ж�
2014_10_31	���¹��죬����XP\Win7�ֿ����룬�����쳣�������ƺ�������,ȥ��������Ϊ�����ռ�
2014_11_22  goto������»�Ϊ d
2015_01_08	��������HWND�Ͳ�����־flags��do {}  while( 0 ) ;
2015_01_10	ΪMoveFile��CopyFile����fCreateDesFolder������
2015_03_03	����x64֮����Ϊ���ǵ��ó�kernel32.dll��ĺ���������ȥ���������ռ�
2015_07_28	�޸�ΪCom���÷�ʽ���򻯵���
2015_08_22	����ʹ�� pimpl �ַ���װdll�����ǲ���ʹ��LoadLibraryʹ��class,����ʧ��
2017_09_10  00:20 ����������� pimpl �ַ�������������ģ���Ҫ���� dll �ˣ���������
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