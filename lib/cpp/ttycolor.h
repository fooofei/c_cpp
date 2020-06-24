
#ifndef TTY_COLOR_H
#define TTY_COLOR_H

/* Auto set the words printf in tty use color
  if detect not in terminal printf, then not use color, 
  because if redirect output to file in posix, color printf will
  add some special words to output, it was not expected.

usage:
  base::ttycolor_t ttyclr;

  if(print_red){
    ttyclr.red(stdout);
  }

  // you can call restore() either red() called or not.
  ttyclor.restore();

*/

#include <stdio.h>

#ifndef NAMESPACE_BASE_BEGIN 
#define NAMESPACE_BASE_BEGIN namespace base{
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END }
#endif

NAMESPACE_BASE_BEGIN

/* wrap isatty() for cross platform, return value samewith isatty() */
int
isatty(FILE *);

class ttycolor_t
{
  ttycolor_t(const ttycolor_t &);
  ttycolor_t & operator = (const ttycolor_t &);

  struct ttycolor_impl_t;
  ttycolor_impl_t * p_;
  
public:
  ttycolor_t();
  ~ttycolor_t();

  /* set which handle to color, stdout or stderr */
  void red(FILE * );
  void restore();
};

NAMESPACE_END; // namespace base

#endif //TTY_COLOR_H
