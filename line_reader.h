#pragma once

#include <stdio.h>
#include <stdint.h>

#define NAMESPACE_BEGIN namespace base {
#define NAMESPACE_END }

NAMESPACE_BEGIN

struct cache_buffer_t
{
    const uint8_t  * begin_;
    const uint8_t * end_;
    const uint8_t * cur_;

    cache_buffer_t(const void* p, size_t l) {
        reset(p, l);
    }


    // MUST
    int read() const {
        if (cur_ < end_) {
            return *cur_;
        }
        return EOF;
    }
    // MUST
    void next() {
        ++cur_;
    }
    // MUST
    uint32_t off() const {
        return uint32_t(cur_ - begin_);
    }
    // MUST
    bool eof() const {
        return !(cur_ < end_);
    }

    uint32_t size() const {
        return uint32_t(end_ - begin_);
    }
    void reset(const void * p, size_t l) {
        begin_ = (const uint8_t *)p;
        end_ = begin_ + l;
        cur_ = begin_;
    }
};

/*

read a line from `src` buffer to `line`.
`maxread` is the max size of `line`.
`sep` is the line end identifier, can be multi, we use one of it.
return size is the line size, maybe >= line.size(), including the `sep` size.

Why `line` is type of std::string, not const char ** ?
Because, the memory of `src` maybe not one buffer, it maybe in two buffer,
so cannot use [begin,end) to describe the `line`.

Why not use std::getline, because 1 std::getline is use elem to be sep, not seps,
cannot be multi, must call multi to implement multi sep, 2 std::getline is not have
max read, sometimes we not excepted to push all line char to line.

usage:

std::string buffer;
buffer.assign("nihaoya\nwohaone\nshide\r");

base::cache_buffer_t cache(buffer.c_str(), buffer.size());
uint32_t r;
std::string line;


for (;!cache.eof();)
{
  r = base::readline_of(cache, std::string("\r\n"), line, 100);
}

// then line will be nihaoya/nwohaone/nshide.

*/
template<typename buffer_t>
uint32_t
readline_of(buffer_t & src, const std::string & sep, std::string & line, uint32_t maxread)
{
    line.clear();
    /* use int type to char type, for to use EOF indicate the end of file */
    int ch = 0; 
    uint32_t back_off = src.off();

    for (; (!src.eof());)
    {
        /* uint8_t read */
        ch = src.read();
        if (ch == EOF) {
            break;
        }
        /* move next safely */
        src.next();
        /* this is line end */ 
        if (sep.find((char)ch) < sep.size()) {
            break;
        }
        if (line.size() < maxread) {
            line.append(1, (char)ch);
        }
    }
    return src.off() - back_off;
}


NAMESPACE_END; // namespace base 



/*
  a try to use std::getline
  template <typename char_type>
  struct basic_ostreambuf : public basic_streambuf<char_type, std::char_traits<char_type> >
  {
  basic_ostreambuf(char_type* buffer, std::streamsize bufferLength)
  {
  // set the "put" pointer the start of the buffer and record it's length.
  setp(buffer, buffer + bufferLength);
  }
  };

  typedef basic_ostreambuf<char>		ostreambuf;
  typedef basic_ostreambuf<wchar_t>	wostreambuf;


  //
  //  https://artofcode.wordpress.com/2010/12/12/deriving-from-stdstreambuf/
  //
  class char_array_buffer : public streambuf
  {
  int_type underflow()
  {
  if (current_ == end_)
  {
  return traits_type::eof();
  }
  return traits_type::to_int_type(*current_);
  }

  int_type uflow()
  {
  if (current_ == end_)
  {
  return traits_type::eof();
  }
  return traits_type::to_int_type(*current_++);   // HERE!
  }

  int_type pbackfail(int_type ch)
  {
  if (!(current_ > begin_) || (ch != traits_type::eof() && ch != current_[-1]))
  {
  return traits_type::eof();
  }
  return traits_type::to_int_type(*--current_);   // HERE!
  }

  std::streamsize showmanyc()
  {
  return end_ - current_;
  }

  const char * begin_;
  const char * end_;
  const char * current_;
  public:
  char_array_buffer(const char * p, size_t l)
  {
  reset(p, l);
  }

  char_array_buffer()
  {
  reset(NULL, 0);
  }

  void reset(const char * p, size_t l)
  {
  begin_ = p;
  end_ = p + l;
  current_ = p;
  }

  };



  //
  // getline 只能按照  \n \r 中的一个分割，但是 行读取是 两个都要过滤的，因此 写此增强版本
  //

  //
  // fail() 是当没有读取到任何字符时置的状态， 比如 12\r\r234 ，分行到中间时，也会fail，但并没有分行完毕，所以，要以 eof() 判定结束.
  //
  struct line_getter_string_t
  {
  std::char_array_buffer	stream_;
  std::istream			f_stream_;

  line_getter_string_t() :f_stream_(&stream_) {}

  void open(const std::string & src)
  {
  stream_.reset(src.c_str(), src.size());
  f_stream_.clear();
  }

  std::istream & get_stream()
  {
  return f_stream_;
  }

  bool eof() const
  {
  return f_stream_.eof();
  }

  bool fail() const
  {
  return f_stream_.fail();
  }

  };


  // 会有内存拷贝，多一份内存 不建议使用
  struct line_getter_string2_t
  {
  std::istringstream		f_stream_;

  line_getter_string2_t() {}

  void open(const std::string & src)
  {
  f_stream_.str(src);
  }

  std::istream & get_stream()
  {
  return f_stream_;
  }

  bool eof() const
  {
  return f_stream_.eof();
  }

  bool fail() const
  {
  return f_stream_.fail();
  }

  };


  struct line_getter_file_t
  {
  std::ifstream			f_;

  line_getter_file_t() {}
  ~line_getter_file_t() {}

  void open(const std::string & path_file)
  {
  f_.open(path_file, std::ios::in | std::ios::binary);
  }

  std::ifstream & get_stream()
  {
  return f_;
  }

  bool eof() const
  {
  return f_.eof();
  }

  bool fail() const
  {
  return f_.fail();
  }
  };

*/

