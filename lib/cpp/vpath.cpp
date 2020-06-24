
#include "vpath.h"

/* cannot use namespace base {}, will cause a link error */

#include <cstring>
#include "strings_algorithm.h"
#include "whereami/whereami.h"
#include <string>


#ifdef WIN32

#include "encoding/encoding_std.h"
#include <Windows.h>

int
base::path_abspath(const std::string & src, std::string & dst)
{
  std::wstring ws;
  std::wstring ws2;
  DWORD dw;
  wchar_t buf[2] = {};

  utf8_2_wstring(src, ws);
  /* small to contain the path */
  dw = GetFullPathNameW(ws.c_str(), 2, buf, NULL);
  if (dw == 0) {
    return -1;
  }
  ws2.resize(dw);
  dw = GetFullPathNameW(ws.c_str(), dw, &ws2[0], NULL);
  if (dw == 0) {
    return -1;
  }
  ws2.resize(dw);
  wstring_2_utf8(ws2, dst);
  return 0;
}


bool
base::path_is_directory(const std::wstring & ws)
{
  /* up Windows 10, there will not have MAX_PATH size limit. */
  WIN32_FILE_ATTRIBUTE_DATA wfad;
  std::memset(&wfad, 0, sizeof(wfad));

  if (!GetFileAttributesExW(ws.c_str(), GetFileExInfoStandard, &wfad)) {
    return false;// api error
  }
  return (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}


/* test a file or directory exists
  on Visual Studio 2017 
    -- <platform tool set:Visual Studio 2017 - Windows XP (v141_xp)>
    -- <c++ runtime library /MT>
  stat not work not Windows XP, but ok on Windows 7.
  on Windows XP, stat return EINVAL(22) even the file exists

  the code is:
  struct stat st;
  std::string s1;
  utf8_2_string(s, s1);
  return stat(s1.c_str(), &st) == 0;

*/

bool
base::path_exists(const std::string & s)
{
  std::wstring ws;

  utf8_2_wstring(s, ws);
  WIN32_FILE_ATTRIBUTE_DATA wfad;
  std::memset(&wfad, 0, sizeof(wfad));
  
  // return TRUE if exists
  if (!GetFileAttributesExW(ws.c_str(), GetFileExInfoStandard, &wfad)) {
    return false;// api error
  }
  // return (wfad.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ARCHIVE)) != 0;
  return true;
}



#else


#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
int
base::path_abspath(const std::string & src, std::string & dst)
{
  std::string temp;
  temp.resize(PATH_MAX + 1);

  char * p = realpath(src.c_str(), &temp[0]);
  if (p) {
    dst.assign(p); return 0;
  }
  return -1;
}

bool
base::path_is_directory(const std::string & s)
{
  int err;
  struct stat st;
  std::memset(&st, 0, sizeof(st));
  err = stat(s.c_str(), &st);
  if (err == 0)
  {
    return 0 != (st.st_mode & S_IFDIR);
  }
  return false;
}

bool
base::path_exists(const std::string & s)
{
  struct stat st;
  return stat(s.c_str(), &st) == 0;
}


#endif


/* abspath the arg*/
int
base::path_try_abspath_current(std::string & s)
{
  int err;

  std::string temp;
  err = base::path_abspath_current(s, temp);
  if (err == 0) {
    s = temp;
    return 0;
  }
  return -1;
}


int
base::get_current_directory(std::string & dst)
{
  int err;
  std::string temp;

  err = get_executable_fullpath(&temp);
  if (err) {
    return err;
  }
  base::dirname(temp);
  if (temp.empty()) {
    return -1;
  }
  dst = temp;
  return 0;
}

int
base::path_abspath_current(const std::string & src, std::string & dst)
{
  // path_exists(src)  maybe false
  if (base::startswith(src,std::string(".")))
  {
    std::string temp;
    if (0 != get_current_directory(temp)) {
      return -1;
    }

    base::path_join(temp, src);
    if (path_exists(temp) && 0 == path_abspath(std::string(temp), temp))
    {
      dst = temp;
      return 0;
    }
  }
  return -1;
}
