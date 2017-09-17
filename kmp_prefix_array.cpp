/*
  KMP is too complicated, recommended use sunday
  The file shows how to write KMP step by step.
*/

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <vector>

/* the prefix table meaning is:
  if found a not match, how to move to the pattern off, meanwhile the main
 string off not move.

  try use `const char *` pointer to locate, but the sub string compare have a
 information is
  the two string length is same, so we can use that with the offset.
  if we use `const char *`, so we cannot use the string length information.
  so we cannot use std::vector<const char *>
 */
int build_prefix_table(const std::string &pattern, std::vector<size_t> &table) {
  table.clear();
  table.resize(pattern.size());

  const char *first = pattern.c_str();
  const char *last = first + pattern.size();

  if (table.size() < 1) {
    return -1;
  }
  size_t *p_prefix = &table[0];
  const char *off = first;
  for (off += 1, *p_prefix = 0, p_prefix += 1; off < last;
       off += 1, p_prefix += 1) {
    *p_prefix = 0;

    // also meaning length
    size_t off_last1 = (size_t)std::distance(first, off);

    for (; off_last1 > 0; off_last1 -= 1) {
      std::string s1(first, off_last1); // use [addr,size) to constructor
      std::string s2(off + 1 - off_last1,
                     off +
                         1); // use [first,last) to constructor // not overflow
      if (s1 == s2) {
        *p_prefix = off_last1;
        break;
      }
    }
  }

  return 0;
}

int build_prefix_table2(const std::string &pattern,
                        std::vector<size_t> &table) {
  table.clear();
  table.resize(pattern.size());

  const char *first = pattern.c_str();
  const char *last = first + pattern.size();

  if (table.size() < 1) {
    return -1;
  }
  size_t *p_prefix = &table[0];
  const char *off = first;
  for (off += 1, *p_prefix = 0, p_prefix += 1; off < last;
       off += 1, p_prefix += 1) {
    *p_prefix = 0;

    // also meaning length
    size_t off_last1 = *(p_prefix - 1) + 1;

    for (; off_last1 > 0; off_last1 -= 1) {
      std::string s1(first, off_last1); // use [addr,size) to constructor
      std::string s2(off + 1 - off_last1,
                     off +
                         1); // use [first,last) to constructor // not overflow
      if (s1 == s2) {
        *p_prefix = off_last1;
        break;
      }
    }
  }

  return 0;
}

int build_prefix_table3(const std::string &pattern,
                        std::vector<size_t> &table) {
  table.clear();
  table.resize(pattern.size());

  if (table.size() < 1) {
    return -1;
  }

  table[0] = 0;
  const char *first = pattern.c_str();
  const char *last = first + pattern.size();

  for (size_t off = 1; off < pattern.size(); off += 1) {
    table[off] = 0;

    // off is index+1
    size_t off1 = table[off - 1] + 1;
    for (; off1 > 0;) {
      std::string s1(first, off1);
      size_t length = off1;
      size_t off_end = off + 1;
      std::string s2(first + off_end - length, length);
      if (s1 == s2) {
        table[off] = off1;
        break;
      }
      if (off1) {
        off1 = table[off1 - 1];
      } else {
        break;
      }
    }
  }

  return 0;
}

int build_prefix_table4(const std::string &pattern,
                        std::vector<size_t> &table) {
  table.clear();
  table.resize(pattern.size());

  if (table.size() < 1) {
    return -1;
  }

  table[0] = 0;
  const char *first = pattern.c_str();

  for (size_t off = 1; off < pattern.size(); off += 1) {
    table[off] = 0;

    // off is index+1
    size_t off1 = off;
    for (;;) {
      // the meaning of table last value is
      // <last_value><x> == <last_value><y>
      off1 = table[off1 - 1];
      off1 += 1;

      std::string s1(first, off1);
      size_t length = off1;
      size_t off_end = off + 1;
      std::string s2(first + off_end - length, length);
      if (s1 == s2) {
        table[off] = off1;
        break;
      }
      if (off1 < 2) {
        break;
      }
    }
  }

  return 0;
}

size_t kmp_search(const std::string &pattern, const std::string &s) {
  std::vector<size_t> table;

  build_prefix_table4(pattern, table);

  for (size_t i = 0; i < table.size(); ++i) {
    printf("%zu ", table[i]);
  }
  printf("\n");

  if (pattern.size() > s.size()) {
    return s.size();
  }

  const char *first1 = s.c_str();
  const char *last1 =
      first1 + s.size(); // first1 + s.size() +1 - pattern.size();
  const char *first2 = pattern.c_str();
  const char *last2 = first2 + pattern.size();

  const char *off1 = first1;
  const char *off2 = first2;
  for (; off1 < last1;) {
    if (*off1 == *off2) {
      off1 += 1;
      off2 += 1;
      if (off2 == last2) {
        const char *p = off1 - pattern.size();
        std::ptrdiff_t v = std::distance(first1, p);
        printf("found match at %zu, %p\n", v, p);
        return (size_t)v;
      }
    } else if (off2 != first2) {
      /* a little complicated  */
      off2 = first2 + table[std::distance(first2, off2 - 1)];
    } else {
      off1 += 1;
    }
  }

  return s.size();
}

void test() {
  // 0 0 1 2 3 4 0 1
  std::string pattern("abababca");
  // 0 1
  pattern.assign("aa");

  std::string s("bacbababaabcbab");
  size_t v = kmp_search(pattern, s);
}

int main() {

  test();

  return 0;
}