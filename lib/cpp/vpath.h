#pragma once

#include <string>

#ifndef NAMESPACE_BASE_BEGIN
#define NAMESPACE_BASE_BEGIN namespace base { 
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END }
#endif


NAMESPACE_BASE_BEGIN
/* abspath src to dst, utf8 bytes encoding path */
int
path_abspath(const std::string & src, std::string & dst);


// get the executable located directory
int
get_current_directory(std::string & );

/* use the executable path as current path to get abspath
  if the executable path is c:\\test1\\test2\\1.exe
  and the we start at c:\\
  then the abspath(.\1.txt) will be c:\\1.txt
  this funtion will let it be c:\\test1\\test2\\1.txt
*/
int
path_abspath_current(const std::string &, std::string &);
int
path_try_abspath_current(std::string &);

/* for Windows, not for posix 
  path not exists return false
*/
bool
path_is_directory(const std::wstring & );

/* for posix, not for Windows */
bool
path_is_directory(const std::string &);

bool
path_exists(const std::string &);

NAMESPACE_END;
