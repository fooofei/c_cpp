

#ifndef ENSURE_CLEAN_UP_H_
#define ENSURE_CLEAN_UP_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <functional>


#ifndef NAMESPACE_BASE_BEGIN
#define NAMESPACE_BASE_BEGIN namespace base { 
#endif


#ifndef NAMESPACE_END
#define NAMESPACE_END }
#endif


NAMESPACE_BASE_BEGIN

template< typename handle_sentry_type >
class ensure_cleanup_t
{
private:
  ensure_cleanup_t(const ensure_cleanup_t&);
  ensure_cleanup_t& operator = (const ensure_cleanup_t&);
public:
  typedef typename handle_sentry_type::handle_type      handle_type;
private:
  handle_type data_;
  handle_sentry_type oper_;
public:
  ensure_cleanup_t() { data_ = oper_.invalid_value(); }
  ensure_cleanup_t(handle_type t) : data_(t) {}

  ~ensure_cleanup_t() { close(); }
  bool is_valid() const { return(data_ != oper_.invalid_value()); }
  bool is_invalid() const { return(!is_valid()); }
  handle_type& operator=(handle_type t) { close();  data_ = t; return data_; }
  operator handle_type() { return  data_; }
  operator handle_type() const { return  data_; }
  handle_type& get() { return data_; }
  void close() { if (is_valid()) { oper_.close(data_); data_ = oper_.invalid_value(); } }
};
///////////////////////////////////////////////////////////////////////////////

#define MakeCleanupClass(className,tCloser) \
typedef ensure_cleanup_t<tCloser> className;

struct c_file_handle_sentry_t
{
  typedef  FILE*			handle_type;
  static void close(handle_type h) { fclose(h); }
  static handle_type		invalid_value() { return NULL; }
};
MakeCleanupClass(ensure_close_c_file_t, c_file_handle_sentry_t);


#ifdef WIN32
struct n_handle_sentry_t
{
  typedef HANDLE				handle_type;
  static void close(handle_type h) { CloseHandle(h); }
  static handle_type		invalid_value() { return NULL; }
};

struct hmodule_handle_sentry_t
{
  typedef HMODULE			handle_type;
  static void close(handle_type h) { FreeLibrary(h); }
  static handle_type		invalid_value() { return NULL; }
};

struct find_handle_sentry_t
{
  typedef HANDLE	handle_type;
  static void close(handle_type h) { FindClose(h); }
  static handle_type		invalid_value() { return INVALID_HANDLE_VALUE; }
};

struct file_handle_sentry_t
{
  typedef HANDLE		handle_type;
  static void close(handle_type h) { CloseHandle(h); }
  static handle_type		invalid_value() { return INVALID_HANDLE_VALUE; }
};
MakeCleanupClass(ensure_close_handle_t, n_handle_sentry_t);
MakeCleanupClass(ensure_free_library_t, hmodule_handle_sentry_t);
MakeCleanupClass(ensure_close_file_t, file_handle_sentry_t);
MakeCleanupClass(ensure_close_find_t, find_handle_sentry_t);

#endif

///////////////////////////////////////////////////////////////////////////////

//MakeCleanupClass(EnsureLocalFree,          HLOCAL,    LocalFree);
//MakeCleanupClass(EnsureGlobalFree,         HGLOBAL,   GlobalFree);
//MakeCleanupClass(EnsureRegCloseKey,        HKEY,      RegCloseKey);
//MakeCleanupClass(EnsureCloseServiceHandle, SC_HANDLE, CloseServiceHandle);
//MakeCleanupClass(EnsureCloseWindowStation, HWINSTA,   CloseWindowStation);
//MakeCleanupClass(EnsureCloseDesktop,       HDESK,     CloseDesktop);
//MakeCleanupClass(EnsureUnmapViewOfFile,    PVOID,     UnmapViewOfFile);

///////////////////////////////////////////////////////////////////////////////

NAMESPACE_END;//namespace base
#endif//ENSURE_CLEAN_UP_H_

