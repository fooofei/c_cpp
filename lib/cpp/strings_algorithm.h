#pragma once

#include <limits.h>
#include <stdint.h>
#include <cstring>
#include <cctype> // std::isalpha
#include <algorithm>


/* use this macro define, then format this file, functions will at the line begin.
    not auto insert TAB.
*/
#ifndef NAMESPACE_BASE_BEGIN
#define NAMESPACE_BASE_BEGIN namespace base { 
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END }
#endif

NAMESPACE_BASE_BEGIN

struct path_valid_characters_t
{
  enum { _size = UCHAR_MAX + 1, };
  uint8_t chr[_size];

  path_valid_characters_t()
  {
    for (size_t i = 0; i < _size; ++i) chr[i] = uint8_t(i);
#ifdef WIN32
    const char * ptr = ("\\/:*?\"<>|");
#else
    const char * ptr = ("\\/*?<>|");
#endif
    const char * end = ptr + strlen(ptr);
    for (const char * off = ptr; off != end; ++off)
    {
      /* replace the char */
      chr[uint8_t(*off)] = uint8_t('_');
    }
  }
};// struct path_valid_characters_t

/* for no warning. */
template<typename char_type>
struct tolower_agent_t {
  char_type operator()(char_type c) {
    return (char_type) ::tolower(c);
  }
};

template<typename char_type>
struct toupper_agent_t {
  char_type operator()(char_type c) {
    return (char_type) ::toupper(c);
  }
};

struct non_alpha_t {
  bool operator()(int c) {
    return !std::isalpha(c);
  }
};

struct non_digit_t {
  bool operator()(int c) {
    return !std::isdigit(c);
  }
};


//
// not assign, const
//


/* Detect a string is a valid path name, not the fullpath, not include directory,
because directory include slash ('\\')
support char, wchar_t */
template <typename forward_iterator_type>
inline
bool
path_name_is_valid(forward_iterator_type first, forward_iterator_type last)
{
  struct path_valid_characters_t chs;
  forward_iterator_type off;
  uint32_t c32;
  uint8_t c8;

  for (off = first; off != last; off ++)
  {
    c32 = *off;
    if (c32 < 0xFF)
    {
      c8 = uint8_t(*off);
      if (chs.chr[c8] != c8) {
        return false;
      }
    }
  }
  return true;
}

template<typename forward_iterator_type1, typename forward_iterator_type2>
inline
forward_iterator_type1
find_last_of(forward_iterator_type1 first1, forward_iterator_type1 last1
  , forward_iterator_type2 first2, forward_iterator_type2 last2
)
{
  size_t count1 = std::distance(first1, last1);
  size_t count2 = std::distance(first2, last2);
  if (count1 > 0 && count2 > 0)
  {
    forward_iterator_type1 off = last1;
    off --;
    for (;; --off)
    {
      forward_iterator_type2 n = std::find(first2, last2, *off);
      if (n != last2) {
        return off;
      }
      else if (off == first1) {
        break;
      }
    }
  }
  return last1;
}


/* If all are in [first2, last2), then return last1,
   If found pos not in , then return off,
   If found pos not in is at the begin, then return the begin.
*/
template<typename forward_iterator_type1, typename forward_iterator_type2>
inline
forward_iterator_type1
find_last_not_of(forward_iterator_type1 first1, forward_iterator_type1 last1
  , forward_iterator_type2 first2, forward_iterator_type2 last2
)
{
  size_t count1 = std::distance(first1, last1);
  size_t count2 = std::distance(first2, last2);
  if (count1 > 0 && count2 > 0)
  {
    forward_iterator_type1 off = last1 ;
    off --;
    for ( ;; --off)
    {
      forward_iterator_type2 n = std::find(first2, last2, *off);
      if (n == last2) {
        return off;
      }
      else if (off == first1) {
        break;
      }
    }
  }
  return last1;
}

template<typename forward_iterator_type1, typename forward_iterator_type2>
inline
forward_iterator_type1
find_first_not_of(forward_iterator_type1 first1, forward_iterator_type1 last1
  , forward_iterator_type2 first2, forward_iterator_type2 last2
)
{
  size_t count1 = std::distance(first1, last1);
  size_t count2 = std::distance(first2, last2);
  if (count1 > 0 && count2 > 0)
  {
    for (forward_iterator_type1 off = first1; off != last1; off ++)
    {
      forward_iterator_type2 n = std::find(first2, last2, *off);
      if (n == last2) {
        return off;
      }
    }
  }
  return last1;
}


template<typename forward_iterator_type, typename value_type>
inline
forward_iterator_type
find_first_not_of(forward_iterator_type first1, forward_iterator_type last1
  , const value_type & v
)
{
  size_t count1 = std::distance(first1, last1);
  if (count1 > 0)
  {
    for (forward_iterator_type off = first1; off != last1; off ++ )
    {
      if (value_type(*off) != v) { // warning.
        return off;
      }
    }
  }
  return last1;
}





/* If not found, return the last. If found, return the iterator off.*/
template<typename forward_iterator_type, typename value_type>
inline
forward_iterator_type
find_last_of(forward_iterator_type first1, forward_iterator_type last1
  , const value_type & v
)
{
  size_t count1 = std::distance(first1, last1);
  if (count1 > 0)
  {
    forward_iterator_type off = last1;
    off --;
    for ( ;; --off)
    {
      if (value_type(*off) == v) { // warning. 
        return off;
      }
      else if (off == first1) {
        break;
      }
    }
  }
  return last1;
}

/* copy from MSV, std::all_of is since c++11. */
template<typename _InIt, typename _Pr>
inline
bool
all_of(_InIt _First, _InIt _Last, _Pr _Pred)
{	// test if all elements satisfy _Pred
  for (; _First != _Last; ++_First)
    if (!_Pred(*_First))
      return (false);
  return (true);
}

/* copy from MSVC, std::equal has warning. */
template<typename _InIt1, typename _InIt2>
inline
bool equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2)
{	// compare [_First1, _Last1) to [First2, ...)
  for (; _First1 != _Last1; ++_First1, ++_First2)
    if (!(*_First1 == *_First2))
      return (false);
  return (true);
}


template<typename string_type>
inline
typename string_type::const_pointer
pointer_cbegin(const string_type &str)
{
  return str.c_str();
}

template<typename string_type>
inline
typename string_type::const_pointer
pointer_cend(const string_type &str)
{
  return str.c_str() + str.size();
}

template<typename string_type>
inline
bool
startswith(const string_type &str, typename string_type::const_pointer p, size_t len)
{
  return (0 == str.compare(0, len, p, len));
}

template<typename string_type>
inline
bool
startswith(const string_type &str, typename string_type::const_pointer p)
{
  size_t l = string_type::traits_type::length(p);
  return startswith(str, p, l);
}

template<typename string_type1, typename string_type2>
inline
bool
startswith(const string_type1 &str, const string_type2 & substr)
{
  return startswith(str, substr.c_str(), substr.size());
}

template<typename string_type>
inline
bool
endswith(const string_type &str, typename string_type::const_pointer p, size_t len)
{
  if (!(str.size() >= len)) return false;
  return (0 == str.compare(str.size() - len, len, p, len));
}

template<typename string_type>
inline
bool
endswith(const string_type &str, typename string_type::const_pointer p)
{
  size_t l = string_type::traits_type::length(p);
  return endswith(str, p, l);
}

template<typename string_type1, typename string_type2>
inline
bool
endswith(const string_type1 &str, const string_type2 & substr)
{
  endswith(str, substr.c_str(), substr.size());
}

template<typename string_type>
inline
bool
samewith(const string_type &str, typename string_type::const_pointer p, size_t l)
{
  return (str.size() == l && startswith(str, p, l));
}

template<typename string_type>
inline
bool
samewith(const string_type &str, typename string_type::const_pointer p)
{
  return samewith(str, p, string_type::traits_type::length(p));
}


/* If std::transform has warning, then use this one.
   std::transform's Dest iter ask for check, mostly use std::back_inserter to wrap it.
   warning C4996: 'std::transform::_Unchecked_iterators::_Deprecate': Call to 'std::transform' with parameters that may be unsafe
*/
template<typename in_iterator_type, typename out_iterator_type, typename function_type>
inline
out_iterator_type
transform(in_iterator_type _First, in_iterator_type _Last
  , out_iterator_type _Dest, function_type _Func) {
  for (; _First != _Last; ++_First, ++_Dest)
    *_Dest = _Func(*_First);
  return (_Dest);
}

template <typename iterator_type>
inline
bool
is_all_alpha(iterator_type first, iterator_type last)
{
  return std::find_if(first, last, non_alpha_t()) == last;
}

template <typename iterator_type>
inline
bool
is_all_digit(iterator_type first, iterator_type last)
{
  return std::find_if(first, last, non_digit_t()) == last;
}


template <typename forward_iterator_type>
inline
forward_iterator_type
find_last_slash(forward_iterator_type first, forward_iterator_type last)
{
#ifdef WIN32
  uint32_t c = '\\';
#else
  uint32_t c = '/';
#endif
  return find_last_of(first, last, c);
}


/* Return 'c:\\1.txt' of the '.' pos,
return 'c:\\111.222\\333.txt' of the second '.' pos.
*/
template <typename forward_iterator_type>
inline
forward_iterator_type
find_extension_dot(forward_iterator_type first, forward_iterator_type last)
{
  uint32_t c = '.';
  forward_iterator_type off;

  off = find_last_slash(first, last);
  if (off == last) {
    off = first;
  }
  return find_last_of(off, last, c);
}

template <typename string_type>
inline
size_t
find_extension_dot_pos(const string_type & s)
{
  typename string_type::const_iterator it;
  it = find_extension_dot(s.begin(), s.end());
  if (it == s.end()) {
    return size_t(-1);
  }
  else {
    return std::distance(s.begin(), it);
  }
}



//
// not assgins, over
//


//
// assign
//

/* make a path name valid, not include the directory path, only the file name.
  return iterator_type first.
*/
template <typename char_type>
inline
char_type *
normalize_path_name(char_type * first, char_type * last)
{
  struct path_valid_characters_t chs;
  char_type * off;
  uint32_t c32;
  uint8_t c8;

  for (off = first; off != last; off ++ )
  {
    c32 = *off;
    if (c32 < 0xFF)
    {
      c8 = uint8_t(c32);
      *off = (char_type)chs.chr[c8];
    }
  }
  return first;
}

/* split by path sep '\\', and normalize each substr.
  !!! WARNING, not include the root name, such as 'c:',
  because the ':' is the invalid char, it will be normalized.
*/
template <typename char_type>
inline
void
normalize_fullpath(char_type * first, char_type * last)
{
  char_type * off;
  char_type * off1;
  char_type * off2;

#ifdef WIN32
  char_type path_sep = '\\';
#else
  char_type path_sep = '/';
#endif
  off = first;
  for (;;)
  {
    off1 = find_first_not_of(off, last, path_sep);
    if (off1 == last) {
      break;
    }
    off2 = std::find(off1 + 1, last, path_sep);

    normalize_path_name(off1, off2);
    if (off2 == last) {
      break;
    }

    off = off2 + 1;
    if (off == last) {
      break;
    }
  }
}

template <typename string_type>
inline
int
string_copy(const string_type & src, typename string_type::pointer buffer, unsigned * size)
{
  if (!size) return -1;

  unsigned in_size = *size;
  unsigned len = (unsigned)src.size();
  if (buffer) {
    if (!(len < in_size)) {
      *size = len + 1;
      return -1;
    }
    string_type::traits_type::copy(buffer, src.c_str(), len);
    buffer[len] = 0;
    *size = len;
  }
  else {
    *size = len + 1;
  }
  return 0;
}

template<typename string_type>
inline
typename string_type::pointer
pointer_begin(string_type & str)
{
  return &str[0];
}

template<typename string_type>
inline
typename string_type::pointer
pointer_end(string_type & str)
{
  return pointer_begin(str) + str.size();
}


template<typename string_type, typename forward_iterator_type>
inline
void
rstrip(string_type &s, forward_iterator_type first, forward_iterator_type last)
{
  /* If found pos in sep, erase [pos_in_sep,);
  If not found, valid_pos is the last pos, erase [pos+1,) is erase nothing.
  If all in sep, pos is size_t(-1), the erase [pos+1,) is clear().
  */

  typename string_type::iterator it;
  it = find_last_not_of(s.begin(), s.end(), first, last);
  // all in
  if (it == s.end()) {
    s.clear();
  }
  else {
    s.erase(it + 1, s.end());
  }
  //  size_t valid_pos = s.find_last_not_of(first, std::string::npos, last);
  // s.erase(valid_pos + 1);
}

template<typename string_type>
inline
void
rstrip(string_type &s, const string_type & invalid_chars)
{
  return rstrip(s, invalid_chars.begin(), invalid_chars.end());
}

template<typename string_type, typename value_type>
inline
void
rstrip(string_type &s, const value_type & invalid_char)
{
  typedef typename string_type::const_pointer p_t;
  p_t b = (p_t)&invalid_char;
  p_t e = b + 1;
  rstrip(s, b, e);
}


template<typename string_type>
inline
void
trim_tail(string_type &s, const string_type &sep)
{
  return rstrip(s, sep);
}

template<typename string_type, typename forward_iterator_type>
inline
void
lstrip(string_type &s, forward_iterator_type first, forward_iterator_type last)
{
  // s.erase(0, s.find_first_not_of(first, 0, last));
  typename string_type::iterator it;
  it = find_first_not_of(s.begin(), s.end(), first, last);
  s.erase(s.begin(), it);
  /* .erase(const_iterator, const_iterator) is since c++11. */
}

template<typename string_type>
inline
void
lstrip(string_type &s, const string_type &invalid_chars)
{
  lstrip(s, invalid_chars.begin(), invalid_chars.end());
}

template<typename string_type, typename value_type>
inline
void
lstrip(string_type &s, const value_type &invalid_char)
{
  typedef typename string_type::const_pointer p_t;
  p_t b = (p_t)&invalid_char;
  p_t e = b + 1;
  lstrip(s, b, e);
}


template<typename string_type>
inline
void
trim_head(string_type &s, const string_type &invalid_chars)
{
  lstrip(s, invalid_chars);
}

template<typename string_type>
inline
void
tolower(string_type &s, size_t transformlen)
{
  transformlen = (std::min<size_t>)(s.size(), transformlen);
  std::transform(s.begin(), s.begin() + transformlen, s.begin(),
    tolower_agent_t<typename string_type::value_type>());
}

template<typename string_type>
inline
void
toupper(string_type &s, size_t transformlen)
{
  transformlen = (std::min<size_t>)(s.size(), transformlen);
  std::transform(s.begin(), s.begin() + transformlen, s.begin(),
    toupper_agent_t<typename string_type::value_type>());
}

template<typename string_type>
inline
void
tolower(string_type &s)
{
  tolower(s, s.size());
}

template<typename string_type>
inline
void
toupper(string_type &s)
{
  toupper(s, s.size());
}

template <typename value_type>
inline
value_type *
tolower(value_type * first, value_type * last)
{
  return transform(first, last, first
    , tolower_agent_t<value_type>());
}

template <typename value_type>
inline
value_type *
toupper(value_type * first, value_type * last)
{
  return transform(first, last, first
    , toupper_agent_t<value_type>());
}


/* delimiter is one, not one of */
template<typename container_type, typename string_type>
inline
void
split(container_type &c, const string_type &s, const string_type &delimiter)
{
  if (delimiter.empty()) return;

  size_t off = 0, off_begin = 0;

  /* not use  find_first_of(), it find a single char */
  while (off_begin < s.size() && (off = s.find(delimiter, off_begin)) < s.size()) {
    // Many implements not notice it, if off == off_begin, 
    // it will push empty value to container.
    // implement such as not notice :
    // https://stackoverflow.com/questions/5167625/splitting-a-c-stdstring-using-tokens-e-g
    // https://stackoverflow.com/questions/236129/split-a-string-in-c
    if (off > off_begin) {
      c.push_back(string_type());
      string_type & ref = c.back();
      ref.assign(s.c_str() + off_begin, off - off_begin);
    }
    off_begin = off + delimiter.size();
  }

  if (off_begin < s.size()) {
    c.push_back(string_type());
    string_type & ref = c.back();
    ref.assign(s.c_str() + off_begin, s.size() - off_begin);
  }
}

template <typename container_type, typename string_type>
inline
void
split_of(container_type & c, const string_type & s, const string_type & delimiters)
{
  if (delimiters.empty()) return;

  size_t off1 = 0;
  string_type sub;

  for (typename string_type::const_iterator it = s.begin(); it != s.end(); ++it )
  {
    off1 = delimiters.find(*it);
    if (off1 < delimiters.size()) {
      if (sub.size() > 0) {
        c.push_back(string_type());
        string_type & ref = c.back();
        ref.assign(sub);
      }
      sub.clear();
    }
    else {
      sub.push_back(*it);
    }
  }

  if (sub.size() > 0) {
    c.push_back(string_type());
    string_type & ref = c.back();
    ref.assign(sub);
  }
}


template <typename string_type>
inline
void
backslash(string_type & s)
{
  typename string_type::value_type c;
  typename string_type::const_pointer p;
#ifdef WIN32
  c = '\\';
#else
  c = '/';
#endif
  rstrip(s, c);
  p = &c;
  s.append(p, p + 1);
}


template <typename string_type>
inline
void
removebackslash(string_type & s)
{
  typename string_type::value_type c;
#ifdef WIN32
  c = '\\';
#else
  c = '/';
#endif
  rstrip(s, c);
}

/* not deal with the situation if name begin with path sep \\'. */
template <typename string_type>
inline
string_type &
path_join(string_type & prefix, const string_type & name)
{
  backslash(prefix);
  prefix.append(name);
  return prefix;
}

template <typename string_type>
inline
void
basename(string_type & s)
{
  typename string_type::iterator it;
  it = find_last_slash(s.begin(), s.end());

  /* All is treated as basename*/
  if (it == s.end()) {
    return;
  }
  /* such as 'c:\\1\\' no basename */
  if (it + 1 == s.end()) {
    s.clear();
  }
  else {
    s.assign(it + 1, s.end());
  }
}

/* With backslash at the end. */
template <typename string_type>
inline
void
dirname(string_type & s)
{
  typename string_type::iterator it;
  it = find_last_slash(s.begin(), s.end());
  /* All is basename, no dirname*/
  if (it == s.end()) {
    s.clear();
  }
  else {
    s.erase(it + 1, s.end());
  }

}

/* 1.txt -> txt*/
template <typename string_type>
inline
void
extension(string_type & s)
{
  typename string_type::iterator it;
  it = find_extension_dot(s.begin(), s.end());
  if (it == s.end() || ++it == s.end()) {
    s.clear();
  }
  else {
    s.assign(it, s.end());
  }
}

NAMESPACE_END; // namespace base

