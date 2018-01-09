
/*
 Have not compitable with Windows XP, Windows Vista.

 On Windows XP, use SHFileOperation()

 IFileOperation flags help:
 Default = FOF_NOCONFIRMATION|FOF_NOCONFIRMMKDIR|FOF_ALLOWUNDO,
 Rename = FOF_NOCONFIRMATION,
 Move = FOF_NOCONFIRMATION| FOF_NOCONFIRMMKDIR,
 DeleteNoRecycle=FOF_NOCONFIRMATION|FOF_NOCONFIRMMKDIR,

*/

#include "file_operation.h"
#include <new>
#include <shellapi.h> // FOF_ALLOWUNDO
#include <Shlwapi.h>
#include <atlcomcli.h>
#include <ShObjIdl.h>





#pragma comment(lib,"shlwapi") // for XP


// typedef HRESULT(WINAPI *PFNSHCreateItemFromParsingName)(__in PCWSTR pszPath, __in_opt IBindCtx *pbc, __in REFIID riid, __deref_out void **ppv);

struct file_operation_t::file_operation_impl_t
{

  // SHCreateItemFromParsingName is export from user32
  // If not have SHCreateItemFromParsingName, should LoadLibrary(), GetProcAddress()
  // base::ensure_free_library_t huser32_;
  CComPtr<IFileOperation> file_;


  file_operation_impl_t() :file_(0)
  {
    HRESULT hr;
    hr = ::CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, ::IID_IFileOperation, (void**)&file_);
    if (FAILED(hr)) {
      file_.Release();
    }
  }

  ~file_operation_impl_t()
  {
    file_.Release();
  }

  HRESULT bind_hwnd(HWND h)
  {
    if (file_) {
      return file_->SetOwnerWindow(h);
    }
    return E_NOTIMPL;
  }
  HRESULT set_flags(DWORD dw)
  {
    if (file_) {
      return file_->SetOperationFlags(dw);
    }
    return E_NOTIMPL;
  }


  HRESULT delete_item(const std::wstring & s)
  {
    HRESULT hr;
    CComPtr<IShellItem> iShellItem = NULL;
    CComPtr<IFileOperationProgressSink> iFileOperationProgressSink = NULL;


    for (;;)
    {
      if (!file_) {
        hr = E_NOTIMPL;
        break;
      }
      hr = SHCreateItemFromParsingName(s.c_str(), 0, IID_IShellItem, (void**)&iShellItem);
      if (FAILED(hr)) {
        break;
      }

      hr = file_->DeleteItem(iShellItem, iFileOperationProgressSink);
      if (FAILED(hr)) {
        break;
      }
      hr = file_->PerformOperations();

      break;
    }

    return hr;
  }


  HRESULT copy_item(const std::wstring & src, const std::wstring & dstfolder, const std::wstring & dstname)
  {

    CComPtr<IFileOperationProgressSink> iFileOperationProgressSink = NULL;
    CComPtr<IShellItem> idstfolder = NULL;
    CComPtr<IShellItem> isrc = NULL;

    HRESULT hr;

    for (;;)
    {
      if (!file_) {
        hr = E_NOTIMPL;
        break;
      }

      hr = SHCreateItemFromParsingName(src.c_str(), 0, IID_IShellItem, (void**)&isrc);
      if (FAILED(hr)) {
        break;
      }
      hr = SHCreateItemFromParsingName(dstfolder.c_str(), 0, IID_IShellItem, (void**)&idstfolder);
      if (FAILED(hr)) {
        break;
      }

      hr = file_->CopyItem(isrc, idstfolder, dstname.c_str(), iFileOperationProgressSink);
      if (FAILED(hr)) {
        break;
      }
      hr = file_->PerformOperations();
      break;
    }
    return hr;

  }
  HRESULT move_item(const std::wstring & src, const std::wstring & dstfolder, const std::wstring & dstname)
  {
    CComPtr<IFileOperationProgressSink> iFileOperationProgressSink = NULL;
    CComPtr<IShellItem> idstfolder = NULL;
    CComPtr<IShellItem> isrc = NULL;
    HRESULT hr;

    for (;;)
    {
      if (!file_) {
        hr = E_NOTIMPL;
        break;
      }
      hr = SHCreateItemFromParsingName(src.c_str(), 0, IID_IShellItem, (void**)&isrc);
      if (FAILED(hr)) {
        break;
      }
      hr = SHCreateItemFromParsingName(dstfolder.c_str(), 0, IID_IShellItem, (void**)&idstfolder);
      if (FAILED(hr)) {
        break;
      }
      hr = file_->MoveItem(isrc, idstfolder, dstname.c_str(), iFileOperationProgressSink);
      if (FAILED(hr)) {
        break;
      }
      hr = file_->PerformOperations();

      break;
    }

    return hr;
  }

  HRESULT rename_item(const std::wstring & src, const std::wstring & name)
  {
    CComPtr<IShellItem> iShellItem = NULL;
    CComPtr<IFileOperationProgressSink> iFileOperationProgressSink = NULL;
    HRESULT hr;

    for (;;)
    {
      if (!file_) {
        hr = E_NOTIMPL;
        break;
      }
      hr = SHCreateItemFromParsingName(src.c_str(), 0, IID_IShellItem, (void**)&iShellItem);
      if (FAILED(hr)) {
        break;
      }

      hr = file_->RenameItem(iShellItem, name.c_str(), iFileOperationProgressSink);
      if (FAILED(hr)) {
        break;
      }
      hr = file_->PerformOperations();
      break;
    }

    return hr;
  }
};





file_operation_t::file_operation_t() :p_(0)
{
  p_ = new(std::nothrow)file_operation_impl_t();
}

file_operation_t::~file_operation_t()
{
  delete p_;
  p_ = 0;
}


HRESULT
file_operation_t::bind_hwnd(HWND arg)
{
  return p_->bind_hwnd(arg);
}

HRESULT
file_operation_t::set_flags(DWORD arg)
{
  return p_->set_flags(arg);
}

HRESULT
file_operation_t::delete_item(const std::wstring & arg)
{
  return p_->delete_item(arg);
}

HRESULT
file_operation_t::copy_item(const std::wstring & arg0, const std::wstring & arg1, const std::wstring & arg2)
{
  return p_->copy_item(arg0, arg1, arg2);
}

HRESULT
file_operation_t::move_item(const std::wstring & arg0, const std::wstring & arg1, const std::wstring & arg2)
{
  return p_->move_item(arg0, arg1, arg2);
}

HRESULT
file_operation_t::rename_item(const std::wstring & arg0, const std::wstring & arg1)
{
  return p_->rename_item(arg0, arg1);
}


int
main()
{
    return 0;
}
