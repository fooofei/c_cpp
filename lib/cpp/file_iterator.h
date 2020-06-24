#pragma once

#include <string>


/*
  usage:

  file_iterator_t fi;

  fi.clear();
  fi.push(fullpath);
  fi.push(fullpath2);

  for (;0 == fi.next();)
  {
    printf("%s\n", fi.value().c_str());
  }

  on Linux, this will also read the soft link files, created by
  `ln -s <src> <dst>`.

*/


class file_iterator_t;
class file_iterator2_t;

class file_iterator_t
{
  struct file_iterator_impl_t;

  file_iterator_impl_t * p_;

  /* no copy */
  file_iterator_t(const file_iterator_t &);
  file_iterator_t & operator = (const file_iterator_t &);

  friend class file_iterator2_t;

public:

  file_iterator_t();
  ~file_iterator_t();


  void clear();
  /* push files or directorys, utf-8 encoding bytes.
    maybe fail if queue size up to limit.
  */
  int push(const std::string &);

  /* get current find file, utf-8 encoding */
  const std::string & value();

  /* move next, return 0 if succeed, else non-zero for error.
    if move succeed, then use value() to fetch the value.
  */
  int next();

};


/* can iter fullpath from file */
class file_iterator2_t
{
  struct file_iterator2_impl_t;
  file_iterator2_impl_t * p_;

  file_iterator2_t(const file_iterator2_t &);
  file_iterator2_t & operator = (const file_iterator2_t &);

public:
  file_iterator2_t();
  ~file_iterator2_t();

  void clear();

  /* add a file which containes fullpath list lines,
      the file is readed as iter, so it cannot be delete at the reading time.
      file is encoded as UTF-8 no BOM, \r or \n or both as line sep.
  */
  int add_file(const std::string &);

  /* same as file_iterator_t */
  int push(const std::string &);

  /* same as file_iterator_t */
  int next();

  /* same as file_iterator_t */
  const std::string & value();
};
