
/* A search file way use NTFS feature (usn log)

  Only support Windows, NEED run as Admin

  usage:
  base::file_iterator_usn_t fiu;
  int err;

  err = fiu.set_drive((L"c"));

  if (err == 0)
  {
    for (;0 == fiu.next();)
    {
      wprintf(L"%s\n", fiu.value().c_str());
    }
  }

  cost 6 seconds for iter 510143 paths

*/

#ifndef FILE_ITERATOR_USN_H
#define FILE_ITERATOR_USN_H


#include <string>


class file_iterator_usn_t
{
  file_iterator_usn_t(const file_iterator_usn_t &);
  file_iterator_usn_t & operator = (const file_iterator_usn_t &);

  struct file_iterator_usn_impl_t;
  file_iterator_usn_impl_t * p_;

public:
  
  file_iterator_usn_t();
  ~file_iterator_usn_t();
  
  void clear();

  /* iterator the drive's usn log */
  int set_drive(const std::wstring &);

  int next();

  /* contains directory and file */
  const std::wstring & value() const;

  /* if the class object is create by Python, then there is no ctor called, use this replace */
  int create();
  void release();
};


#endif //FILE_ITERATOR_USN_H

