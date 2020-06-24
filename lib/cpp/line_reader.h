#pragma once

#include <stdio.h>
#include <stdint.h>
#include <fstream>

#ifndef NAMESPACE_BASE_BEGIN
#define NAMESPACE_BASE_BEGIN namespace base { 
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END }
#endif


NAMESPACE_BASE_BEGIN

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

struct fstream_buffer_t
{
  std::ifstream f;

  /* Windows use std::wstring, posix use std::string(utf-8 encoding) */
  template<typename string_type>
  int open(const string_type  & fullpath) {
    /* gcc not have open(std::string, ...)*/
    f.open(fullpath.c_str(), std::ios::in | std::ios::binary);
    return f.is_open() ? 0 : -1;
  }
  // MUST
  int read() {
    return f.peek();
  }
  // MUST
  void next() {
    /* get will move the cursor one byte */
    f.get();
  }
  // MUST
  uint64_t off() {
    return f.tellg();
  }
  // MUST
  bool eof() {
    return f.peek() == EOF;
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


usage 2:
base::fstream_buffer_t fb;
std::string sep("\x00\n\r",3);

#ifdef WIN32
  std::wstring ws;
  utf8_2_wstring(fullpath_listfile, ws);
  err = fb.open(ws);
#else
  err = fb.open(fullpath_listfile);
#endif

if (err) {
  return err;
}
for (std::string line;!fb.eof();)
{

base::readline_of(fb, sep, line, 400);

}

*/
template<typename buffer_t>
uint32_t
readline_of(buffer_t & src, const std::string & sep, std::string & line, uint32_t maxread)
{
  line.clear();
  /* use int type to char type, for to use EOF indicate the end of file */
  int ch = 0;
  uint64_t back_off = src.off();

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
  return (uint32_t)((uint64_t)src.off() - back_off);
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
  // fail() is when not read any valid char, such as  12\r\r234 , the middle will have fail.
  //   but the file not EOF, so not use fail() to see the file is end or not.
  //

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


  // there will have memory copy, waste, NOT recommanded
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

