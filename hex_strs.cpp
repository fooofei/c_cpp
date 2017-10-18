

#include <stdint.h>

#include "hex_strs.h"




inline
bool
is_valid_hex(char c)
{
  return (('0'<=c && c<= '9') || ('A'<=c && c<='F'));
}

/* ['0'-'9''A'-'F'] convert to [0-9A-F] */
inline
int 
upperhex_char_2_hex(char c, uint8_t * out)
{
  if (c >= '0' && c <= '9') {
    *out = (uint8_t)(c - '0');
    return 0;
  }

  if (c >= 'A' && c <= 'F') {
    *out = (uint8_t)(c - 'A' + 0x0A);
    return 0;
  }
  return -1;
}

inline
int
upperhex_hex_2_char(uint8_t c, char * out)
{
  if (c >= 0 && c <= 9) {
    *out = (char)(c+'0');
    return 0;
  }

  if (c>=0x0A && c <= 0x0F ){
    *out = (char)(c-0x0A+'A');
    return 0;
  }

  return -1;
}



struct hex_t
{
  /* total 8 bits */
  uint8_t low : 4;
  uint8_t high : 4;

  void clear() {
    memset(this, 0, sizeof(*this));
  }


  /* a -> 61 */
  void from_char(char c)
  {
    memcpy(this, &c, 1);
  }

  /* input upper hex string, tow char */
  int from_upperhex_char(const char * p)
  {
    uint8_t t;
    int err;
    const wchar_t * errs = L"Hex to Str Error , Invalid Hex , must [0-9A-F]";

    if (!is_valid_hex(*p)) {
      throw (errs);
      // return -1;
    }

    err = upperhex_char_2_hex(*p, &t);
    if (err) {
      throw(errs);
    }
    high = t;
    p += 1;

    if (!is_valid_hex(*p)) {
      throw (errs);
      // return -1;
    }

    err = upperhex_char_2_hex(*p, &t);
    if (err) {
      throw(errs);
    }
    low = t;

    return 0;
  }

  char to_char()
  {
    return *((char*)(this));
  }

  char hex_low()
  {
    char c;
    upperhex_hex_2_char(low, &c);
    return c;
  }
  char hex_high()
  {
    char c;
    upperhex_hex_2_char(high, &c);
    return c;
  }
};


int
hex2str(const std::string & hex, std::string & dst)
{
  dst.clear();
  hex_t h;
  h.clear();

  if (hex.size() % 2 != 0) {
    throw (L"not even size");
    return -1;
  }

  for (size_t i = 0; i < hex.size(); i += 2)
  {
    h.from_upperhex_char(hex.c_str() + i);
    dst.append(1,h.to_char());
  }
  return 0;
}

/* abcdefghABCDEF -> 6162636465666768414243444546 */
int
str2hex(const std::string & str, std::string & dst)
{
  
  hex_t h;
  const char * begin = str.c_str();
  const char * end = begin + str.size();
  
  dst.clear();
  h.clear();

  for ( const char * p=begin;p<end;++p)
  {
    h.from_char(*p);
    dst.append(1, h.hex_high());
    dst.append(1, h.hex_low());
  }

  return 0;
}
