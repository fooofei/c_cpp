

#include "file_iterator.h"
#include "vpath.h"
#include <stdio.h>

#ifdef WIN32

#include <Windows.h>
#include <queue>
#include "encoding/encoding_std.h"
#include "strings_algorithm.h"


// static
// bool
// path_is_directory(const std::string & s)
// {
//     std::wstring ws;
//     utf8_2_wstring(s, ws);
//     return path_is_directory(ws);
// }


struct  file_iterator_t::file_iterator_impl_t
{
  std::queue<std::wstring> values_;
  std::wstring cur_w_; // use wchar_t internal, only convert with wchar_t and char
                      // when in and out.
  std::string cur_;

  struct context_t {
    HANDLE hd;
    WIN32_FIND_DATAW fd;
    std::wstring cur_search;
    void clear() {
      if (hd != INVALID_HANDLE_VALUE) {
        FindClose(hd); /* for auto close at dtor */
      }
      hd = INVALID_HANDLE_VALUE;
      std::memset(&fd, 0, sizeof(fd));
      cur_search.clear();
    }
    context_t() :hd(INVALID_HANDLE_VALUE) {} /* must init at here, otherwise clear() will error */
  }context_;



  void clear()
  {
    context_.clear();
    cur_.clear();
    while (values_.size() > 0) {
      values_.pop();
    }
  }

  int push(const std::string & s)
  {
    std::wstring ws;
    utf8_2_wstring(s, ws);
    return push_w(ws);
  }

  int push_w(const std::wstring & ws)
  {
    const size_t limit = 1000000;
    if (values_.size() > limit) {
      //fprintf(stderr, "[!] fail file_iterator_t::push, queue is up to limit %zu ,%s(%d)\n", limit, __FILE__, __LINE__);
      return -1;
    }
    values_.push(ws);
    return 0;
  }

  const std::string & value()
  {
    wstring_2_utf8(cur_w_, cur_);
    return cur_;
  }


  /* move next, return 0 if have next, return -1 if not have */
  int next()
  {
    cur_.clear();
    cur_w_.clear();

    for (;;)
    {
      if (context_.hd == INVALID_HANDLE_VALUE)
      {
        context_.clear();
        std::wstring s;
        if (values_.empty()) {
          break;
        }


        s = values_.front();
        values_.pop();

        if (!base::path_is_directory(s))
        {
          cur_w_ = s;
          return 0;
        }
        else
        {
          /* path is directory */
          std::wstring s2;
          s2.assign(s);
          base::path_join(s2, std::wstring(L"*"));
          context_.hd = FindFirstFileW(s2.c_str(), &context_.fd);
          if (context_.hd != INVALID_HANDLE_VALUE)
          {
            context_.cur_search = s;
            std::wstring name(context_.fd.cFileName);
            if (name != L"." && name != L"..")
            {

              std::wstring s3;
              s3.assign(context_.cur_search);
              base::path_join(s3, name);
              if (!base::path_is_directory(s3))
              {
                /* finded */
                cur_w_ = s3;
                return 0;
              }
              push_w(s3);

            }
          }
        }

      }

      if (context_.hd != INVALID_HANDLE_VALUE)
      {
        if (FindNextFileW(context_.hd, &context_.fd))
        {
          std::wstring name(context_.fd.cFileName);
          if (name != L"." && name != L"..")
          {

            std::wstring s3;
            s3.assign(context_.cur_search);
            base::path_join(s3, name);
            if (!base::path_is_directory(s3))
            {
              /* finded */
              cur_w_ = s3;
              return 0;
            }
            push_w(s3);
          }
        }
        else {
          //FindClose(context_.hd);
          context_.clear();
        }
      }

    }
    return -1;
  }


};

#else // !WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <queue>
#include "strings_algorithm.h"

struct file_iterator_t::file_iterator_impl_t
{
  std::queue<std::string> values_;
  std::string cur_;

  struct context_t {
    DIR * hd;
    struct dirent * fd;
    struct stat st;
    std::string cur_search;

    void clear()
    {
      if (hd != 0) {
        closedir(hd);
      }
      hd = 0;
      fd = 0;
      std::memset(&st, 0, sizeof(st));
      cur_search.clear();
    }
    context_t() :hd(0) {} /* must init at here, otherwise clear() will error */
  }context_;

  void clear()
  {
    while (values_.size() > 0) {
      values_.pop();
    }
    cur_.clear();
    context_.clear();
  }

  int push(const std::string & s)
  {
    if (values_.size() > 1000000) {
      //fprintf(stderr, "[!] fail file_iterator_t::push, queue is up to limit 1000000,%s(%d)\n", __FILE__, __LINE__);
      return -1;
    }
    values_.push(s);
    return 0;
  }

  const std::string & value()
  {
    return cur_;
  }

  int next()
  {
    cur_.clear();
    for (;;)
    {
      if (context_.hd == 0)
      {
        context_.clear();
        std::string s;
        if (values_.empty()) {
          break;
        }
        s = values_.front();
        values_.pop();
        if (!base::path_is_directory(s))
        {
          cur_ = s;
          return 0;
        }
        else
        {
          context_.hd = opendir(s.c_str());
          if (context_.hd != 0) {
            context_.cur_search = s;
          }
        }
      }
      if (context_.hd != 0)
      {
        context_.fd = readdir(context_.hd);
        if (context_.fd != 0)
        {
          std::string s1;
          std::string name;
          name.assign(context_.fd->d_name);
          if (name != "." && name != "..")
          {
            s1.assign(context_.cur_search);
            base::path_join(s1, name);

            std::memset(&context_.st, 0, sizeof(context_.st));
            if (stat(s1.c_str(), &context_.st) == 0)
            {
              /* directory */
              if (0 != (context_.st.st_mode & S_IFDIR))
              //if(S_ISDIR(context_.st.st_mode))
              {
                push(s1);
              }
              else
              {
                cur_ = s1;
                return 0;
              }
            }
          }
        }
        else {
          //closedir(context_.hd);
          context_.clear();
        }
      }
    }
    return -1;
  }

};

#endif




file_iterator_t::file_iterator_t()
{
  p_ = new(std::nothrow)file_iterator_impl_t();
  clear();
}

file_iterator_t::~file_iterator_t()
{
  clear(); // If not find finish, then close it in case of resource leak.
  delete p_;
}

void file_iterator_t::clear()
{
  p_->clear();
}

int file_iterator_t::push(const std::string & s)
{
  return p_->push(s);
}

const std::string & file_iterator_t::value()
{
  return p_->value();
}

int file_iterator_t::next()
{
  return p_->next();
}


#include "fio.h"
#include "line_reader.h"

struct file_iterator2_t::file_iterator2_impl_t
{
  file_iterator_t::file_iterator_impl_t file_iter1_;
  fio::file_reader_t  fio_file_reader_;
  std::string cur_;

  /* temp values*/
  std::string temp;
  std::string linesep;
  std::wstring tempws;

  void clear()
  {
    file_iter1_.clear();
    fio_file_reader_.clear();
    linesep.assign("\r\n\x00", 3);
  }

  int add_file(const std::string & fullpath)
  {
    return fio_file_reader_.open(fullpath.c_str());
  }

  int push(const std::string & s)
  {
    return file_iter1_.push(s);
  }

  int next()
  {
    int err;
    cur_.clear();

    temp.clear();
    for (;;)
    {
      err = file_iter1_.next();
      if (err == 0) {
        cur_ = file_iter1_.value();
        break;
      }

      if (fio_file_reader_.eof()) {
        err = -1;
        break;
      }

      for (; base::readline_of(fio_file_reader_, linesep, temp, 400) > 0 && temp.empty(););
      if (temp.size() > 0)
      {
#ifdef WIN32
        tempws.clear();
        utf8_2_wstring(temp, tempws);
        if (base::path_is_directory(tempws)) {
          file_iter1_.push_w(tempws);
        }
#else
        if (base::path_is_directory(temp)) {
          file_iter1_.push(temp);
        }
#endif
        else {
          cur_ = temp;
          err = 0;
          break;
        }
      }
    }

    return err;
  }

  const std::string & value()
  {
    return cur_;
  }
};


file_iterator2_t::file_iterator2_t()
{
  p_ = new(std::nothrow)file_iterator2_impl_t();
  clear();
}

file_iterator2_t::~file_iterator2_t()
{
  clear();
  delete p_;
}

void
file_iterator2_t::clear()
{
  p_->clear();
}

int
file_iterator2_t::add_file(const std::string & fullpath)
{
  return p_->add_file(fullpath);
}

int
file_iterator2_t::push(const std::string & s)
{
  return p_->push(s);
}

int
file_iterator2_t::next()
{
  return p_->next();
}


const std::string &
file_iterator2_t::value()
{
  return p_->value();
}


