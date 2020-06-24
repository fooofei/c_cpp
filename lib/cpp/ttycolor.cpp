
#include "ttycolor.h"

#include <new>


#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <io.h> // for _isatty



#define ConsoleColor_WhiteFont_RedBack  (BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_INTENSITY|FOREGROUND_INTENSITY)


int
base::isatty(FILE * f)
{
  return _isatty(_fileno(f));
}

struct base::ttycolor_t::ttycolor_impl_t
{
  HANDLE hd;
  WORD backup_color;

  void clear() {
    hd = INVALID_HANDLE_VALUE;
    backup_color = 0;
  }

  bool valid() const {
    return hd != INVALID_HANDLE_VALUE && hd != 0;
  }

  /*  GetStdHandle()
  no need to close
  If the function fails, the return value is INVALID_HANDLE_VALUE. To get extended error information, call GetLastError.+
  If an application does not have associated standard handles, such as a service running on an interactive desktop,
  and has not redirected them, the return value is NULL.
  
  */

  void red(FILE * f) {
    clear();
    if (f == stdout)
    {
      hd = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    else if (f == stderr)
    {
      hd = GetStdHandle(STD_ERROR_HANDLE);
    }
    if (valid()) {
      CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
      GetConsoleScreenBufferInfo(hd, &csbi);
      backup_color = csbi.wAttributes;
      SetConsoleTextAttribute(hd, ConsoleColor_WhiteFont_RedBack);
    }
  }

  void restore() {
    if (valid()) {
      SetConsoleTextAttribute(hd, backup_color);
    }
    clear();
  }
};


#else

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#include <unistd.h>

int
base::isatty(FILE * f)
{
  /* not use ::isatty() */
  return ::isatty(fileno(f));
}

struct base::ttycolor_t::ttycolor_impl_t
{
  FILE * f_;
  void clear() {
    f_ = 0;
    fflush(stdout);
    fflush(stderr);
  }
  void red(FILE * f) {
    clear();
    fprintf(f, ANSI_COLOR_RED);
    f_ = f;
  }
  void restore() {
    if (f_) {
      fprintf(f_, ANSI_COLOR_RESET);
    }
    clear();
  }
};

#endif

base::ttycolor_t::ttycolor_t()
{
  p_ = new(std::nothrow)ttycolor_impl_t();
  p_->clear();
}

base::ttycolor_t::~ttycolor_t()
{
  restore();
  delete p_;
}

void 
base::ttycolor_t::red(FILE * arg)
{
  /* if not in terminal then no color */
  if(isatty(arg)) {
    p_->red(arg);
  }
}

void 
base::ttycolor_t::restore()
{
  p_->restore();
}


